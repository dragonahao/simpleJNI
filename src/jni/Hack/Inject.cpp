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
