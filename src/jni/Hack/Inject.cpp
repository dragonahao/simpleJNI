#include "stdafx.h"
#include <nativelog.h>
#include "Inject.h"

bool Inject(pid_t pid, const char* soPath) {
	RemoteProcess remoteProcess(pid);
	if (!remoteProcess.Attach()) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*) - 附加进程失败。Pid: %d。", pid);
		return false;
	}

	bool bRet = false;
	int i_close;

	void* handle = remoteProcess.remote_dlopen(soPath, RTLD_LAZY);
	if (NULL == handle) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*) - 在远程进程中打开so失败。");
		goto _ret;
	}
	
	i_close = remoteProcess.remote_dlclose(handle);
	if (0 != i_close) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*) - 关闭so句柄失败。");
	} else {
		//MY_LOG_INFO("[*] Inject(pid_t, const char*) - 注入成功。");
		bRet = true;
	}
_ret:
	remoteProcess.Detach();
	return bRet;
}

bool Inject(pid_t pid, const char* soPath, const char* funName, void* params, uint32_t paramSize, struct pt_regs* regs) {
	RemoteProcess remoteProcess(pid);
	if (!remoteProcess.Attach()) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*, const char*, long*, uint32_t, struct pt_regs*) - 附加进程失败。Pid: %d。", pid);
		return false;
	}

	bool bRet = false;
	void* funAddr;
	int i_close;
	RemoteMemory rm_param;

	// dlopen
	void* handle = remoteProcess.remote_dlopen(soPath, RTLD_LAZY);
	if (NULL == handle) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*, const char*, long*, uint32_t, struct pt_regs*) - 在远程进程中打开so失败。");
		goto _ret;
	}

	// dlsym
	funAddr = remoteProcess.remote_dlsym(handle, funName);
	if (NULL == funAddr) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*, const char*, long*, uint32_t, struct pt_regs*) - 获得符号地址失败。");
		goto _release;
	}

	// 向远程进程中写入参数。
	if (!remoteProcess.WriteBytes((uint8_t*)params, paramSize, &rm_param)) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*, const char*, long*, uint32_t, struct pt_regs*) - 向远程进程中写入参数失败。");
		goto _release;
	}

	// 调用函数。
	long p[1];
	p[0] = (long) rm_param.addr;
	if (!remoteProcess.Call(funAddr, p, array_size(p), regs)) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*, const char*, long*, uint32_t, struct pt_regs*) - 调用函数'%s'失败。", funName);
	}

_release:
	if (NULL != rm_param.addr) {
		if (!remoteProcess.FreeData(&rm_param)) {
			MY_LOG_WARNING("[-] Inject(pid_t, const char*, const char*, long*, uint32_t, struct pt_regs*) - 释放参数内存失败。");
		}
	}

	// dlclose
	i_close = remoteProcess.remote_dlclose(handle);
	if (0 != i_close) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*, const char*, long*, uint32_t, struct pt_regs*) - 关闭so句柄失败。ret=%d", i_close);
	} else {
		//MY_LOG_INFO("[*] Inject(pid_t, const char*) - 注入成功。");
		bRet = true;
	}
_ret:
	remoteProcess.Detach();
	return bRet;
}

#define REMOTE_ADDR( addr, local_base, remote_base ) ( (uint32_t)(addr) + (uint32_t)(remote_base) - (uint32_t)(local_base) )
#define  MAX_PATH 0x100

int inject_remote_process( pid_t target_pid, const char *library_path, const char *function_name, void *param, size_t param_size )
{
	int ret = -1;
	void *mmap_addr, *dlopen_addr, *dlsym_addr, *dlclose_addr;
	void *local_handle, *remote_handle, *dlhandle;
	uint8_t *map_base;
	uint8_t *dlopen_param1_ptr, *dlsym_param2_ptr, *saved_r0_pc_ptr, *inject_param_ptr, *remote_code_ptr, *local_code_ptr;

	struct pt_regs regs, original_regs;
	extern uint32_t _dlopen_addr_s, _dlopen_param1_s, _dlopen_param2_s, _dlsym_addr_s, \
		_dlsym_param2_s, _dlclose_addr_s, _inject_start_s, _inject_end_s, _inject_function_param_s, \
		_saved_cpsr_s, _saved_r0_pc_s;

	uint32_t code_length;


	long parameters[10];


	MY_LOG_INFO( "[+] Injecting process: %d", target_pid );

	if ( ptrace_attach( target_pid ) == -1 )
		return EXIT_SUCCESS;


	if ( ptrace_getregs( target_pid, &regs ) == -1 )
		goto exit;

	/* save original registers */
	memcpy( &original_regs, &regs, sizeof(regs) );

	// 获得远程进程中的函数地址。
	mmap_addr = GetRemoteAddress( target_pid, "/system/lib/libc.so", "mmap" );

	MY_LOG_INFO( "[+] Remote mmap address: %x", mmap_addr );

	/* call mmap */
	parameters[0] = 0;	// addr
	parameters[1] = 0x4000; // size
	parameters[2] = PROT_READ | PROT_WRITE | PROT_EXEC;  // prot
	parameters[3] =  MAP_ANONYMOUS | MAP_PRIVATE; // flags
	parameters[4] = 0; //fd
	parameters[5] = 0; //offset

	MY_LOG_INFO( "[+] Calling mmap in target process." );

	// 远程调用mmap，在远程进程中分配内存。
	if ( ptrace_call( target_pid, (uint32_t)mmap_addr, parameters, 6, &regs ) == -1 ) {
		MY_LOG_WARNING("[-] inject_remote_process - 在远程进程中分配内存失败。");
		goto exit;
	}

	// 获得寄存器，为了获得mmap的返回值。
	if ( ptrace_getregs( target_pid, &regs ) == -1 ) {
		MY_LOG_WARNING("[-] inject_remote_process - 获得远程进程中分配的内存地址失败。");
		goto exit;
	}


	MY_LOG_INFO( "[+] Target process returned from mmap, return value=0x%08X, pc=0x%08X", regs.ARM_r0, regs.ARM_pc );

	// mmap的返回值。
	map_base = (uint8_t *)regs.ARM_r0;

	// 获得远程进程中的：dlopen、dlsym、dlclose的地址。
	dlopen_addr = GetRemoteAddress( target_pid, (void *)dlopen );
	dlsym_addr = GetRemoteAddress( target_pid, (void *)dlsym );
	dlclose_addr = GetRemoteAddress( target_pid, (void *)dlclose );

	MY_LOG_INFO( "[+] Get imports: dlopen: %x, dlsym: %x, dlclose: %x", dlopen_addr, dlsym_addr, dlclose_addr );


	remote_code_ptr = map_base + 0x3C00;
	local_code_ptr = (uint8_t *)&_inject_start_s;


	_dlopen_addr_s = (uint32_t)dlopen_addr;
	_dlsym_addr_s = (uint32_t)dlsym_addr;
	_dlclose_addr_s = (uint32_t)dlclose_addr;

	MY_LOG_INFO( "[+] Inject code start: %x, end: %x", local_code_ptr, &_inject_end_s );

	// 代码长度。
	code_length = (uint32_t)&_inject_end_s - (uint32_t)&_inject_start_s;
	dlopen_param1_ptr = local_code_ptr + code_length + 0x20;
	dlsym_param2_ptr = dlopen_param1_ptr + MAX_PATH;
	saved_r0_pc_ptr = dlsym_param2_ptr + MAX_PATH;
	inject_param_ptr = saved_r0_pc_ptr + MAX_PATH;


	/* dlopen parameter 1: library name */
	strcpy( (char*)dlopen_param1_ptr, library_path );
	// 远程进程中的dlopen第一个参数的地址。
	_dlopen_param1_s = REMOTE_ADDR( dlopen_param1_ptr, local_code_ptr, remote_code_ptr );
	MY_LOG_INFO( "[+] _dlopen_param1_s: %x", _dlopen_param1_s );

	/* dlsym parameter 2: function name */
	strcpy( (char*)dlsym_param2_ptr, function_name );
	// 远程进程中的dlsym第二个参数的地址。
	_dlsym_param2_s = REMOTE_ADDR( dlsym_param2_ptr, local_code_ptr, remote_code_ptr );
	MY_LOG_INFO( "[+] _dlsym_param2_s: %x", _dlsym_param2_s );

	/* saved cpsr */
	_saved_cpsr_s = original_regs.ARM_cpsr;

	/* saved r0-pc */
	memcpy( saved_r0_pc_ptr, &(original_regs.ARM_r0), 16 * 4 ); // r0 ~ r15
	_saved_r0_pc_s = REMOTE_ADDR( saved_r0_pc_ptr, local_code_ptr, remote_code_ptr );
	MY_LOG_INFO( "[+] _saved_r0_pc_s: %x", _saved_r0_pc_s );

	/* Inject function parameter */
	memcpy( inject_param_ptr, param, param_size );
	_inject_function_param_s = REMOTE_ADDR( inject_param_ptr, local_code_ptr, remote_code_ptr );
	MY_LOG_INFO( "[+] _inject_function_param_s: %x", _inject_function_param_s );

	MY_LOG_INFO( "[+] Remote shellcode address: %x", remote_code_ptr );

	// 将注入代码写到远程进程。
	ptrace_writedata( target_pid, remote_code_ptr, local_code_ptr, 0x400 );

	memcpy( &regs, &original_regs, sizeof(regs) );

	// 设置寄存器，准备执行。
	regs.ARM_sp = (long)remote_code_ptr;
	regs.ARM_pc = (long)remote_code_ptr;
	ptrace_setregs( target_pid, &regs );
	ptrace_detach( target_pid );

	// inject succeeded
	ret = 0;

exit:
	return ret;
}
