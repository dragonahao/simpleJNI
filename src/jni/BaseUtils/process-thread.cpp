#include "stdafx.h"
#include "module.h"
#include "process-thread.h"

// 通过进程名找到进程ID。
pid_t GetPid(const char *processName) {
	pid_t id;
	pid_t pid = -1;
	DIR* dir;
	FILE *fp;
	char filename[32];
	char cmdline[256];

	struct dirent * entry;

	if (processName == NULL)
		return -1;

	dir = opendir("/proc");
	if (dir == NULL)
		return -1;

	while ((entry = readdir(dir)) != NULL) {
		id = atoi(entry->d_name);
		if (id != 0) {
			sprintf(filename, "/proc/%d/cmdline", id);
			fp = fopen(filename, "r");
			if (fp) {
				fgets(cmdline, sizeof(cmdline), fp);
				fclose(fp);

				if (strcmp(processName, cmdline) == 0) {
					/* 找到进程。 */
					pid = id;
					break;
				}
			}
		}
	}

	closedir(dir);

	return pid;
}

// 获得父进程ID。
pid_t GetPPid(pid_t pid) {
	char buf_ps[1024];
	const char *ps = "ps";
	FILE *ptr;
	size_t count = 0;
	pid_t ppid = -1;
	if (pid < 0) {
		pid = getpid();
	}
	if((ptr=popen(ps, "r"))!=NULL) {
		while(fgets(buf_ps, 1024, ptr) != NULL) {
			if (0 != count) {
				const char *delim = " ";
				strtok(buf_ps, delim);
				pid_t pid_tmp = atoi(strtok(NULL, delim));
				if (pid_tmp != pid) {
					continue;
				}
				ppid = atoi(strtok(NULL, delim));
				break;
			}
			count++;
			//MY_LOG_INFO("%s", buf_ps);
		}
		pclose(ptr);
		ptr = NULL;
		return ppid;
	} else {
		MY_LOG_INFO("popen %s 错误。code=%d,%s。", ps, errno, strerror(errno));
		return -1;
	}
}

// 通过PID获得进程绝对路径。
bool GetProcessPath(pid_t pid, char processPath[PATH_MAX]) {
	FILE *fptr;
	bool bret = false;
	char cmd[PATH_MAX] = { '\0' };
	sprintf(cmd, "readlink /proc/%d/exe", pid);
	if ((fptr = popen(cmd, "r")) != NULL) {
		if (fgets(processPath, PATH_MAX - 1, fptr) != NULL) {
			//printf("the path name is %s.\n", name);
			bret = true;
		}
	}

	pclose(fptr);
	return bret;
}

// 通过Pid获得进程名。
void GetProcessName(pid_t pid, char processName[PATH_MAX]) {
	int fd, r;
	char cmd[PATH_MAX] = { '\0' };
	sprintf(cmd, "/proc/%d/cmdline", pid);
	fd = open(cmd, O_RDONLY);
	if (-1 == fd) {
		MY_LOG_WARNING("[-] GetProcessNameByPid(pid_t, char[]) - open失败，pid=%d, cmd=%s。code=%d: %s。", pid, cmd, errno, strerror(errno));
		r = 0;
	} else {
		errno = 0;
		r = read(fd, processName, PATH_MAX - 1);
		close(fd);
		if (r < 0) {
			MY_LOG_WARNING("[-] GetProcessNameByPid(pid_t, char[]) - read失败，pid=%d, cmd=%s。code=%d: %s。", pid, cmd, errno, strerror(errno));
			r = 0;
		}
	}
	processName[r] = '\0';

	if (-1 != fd)
		close(fd);
}

//////////////////////////////////////////////////////////////////////////

// 获得远程函数地址。
void* GetRemoteAddress(pid_t target_pid, void* local_addr) {
	//MY_LOG_INFO("GetRemoteAddress进入。");
	void* local_base, *remote_base;

	lsp<char> moduleName = GetModuleName(-1, local_addr);
	if (NULL == moduleName.get()) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, void*) - 获得模块名失败。");
		return NULL;
	}

	// 获得模块在当前进程中的基址。
	if (NULL == (local_base = GetModuleBase(-1, moduleName.get()))) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, void*) - 获得模块在当前进程中的基址失败。");
		return NULL;
	}
	// 获得模块在远程进程中的基址。
	if (NULL == (remote_base = GetModuleBase(target_pid, moduleName.get()))) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, void*) - 获得模块在远程进程中的基址失败。");
		return NULL;
	}

	// 	MY_LOG_INFO("GetRemoteAddress(pid_t, void*) - target_pid=%d, module_name=%s, local_addr=%p, local_base=%p, remote_handle=%p", 
	// 		target_pid, moduleName.c_str(), local_addr, local_base, remote_base);
	//MY_LOG_VERBOSE( "[+] get_remote_addr: 本地模块基址[%p], 远程模块基址[%p]\n", local_base, remote_handle );

	// 通过计算得出远程函数的地址。
	return (void *) (((size_t) local_addr - (size_t) local_base) + (size_t) remote_base);
}

// 获得远程函数地址。
void* GetRemoteAddress(pid_t target_pid, const char* modulePath, const char* funcName) {
	void *remote_base, *remote_addr = NULL;

	// 判断动态库中是否有相应的函数。
	void* handle = dlopen(modulePath, RTLD_LAZY);
	if (NULL == handle) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 打开动态库失败。%s。", dlerror());
		return NULL;
	}
	//MY_LOG_INFO("GetRemoteAddress(pid_t, const char*, void*) - handle=%p", handle);
	void* funAddr = dlsym(handle, funcName);
	if (NULL == funAddr) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 获得函数指针失败。%s。", dlerror());
		goto _ret;
	}

	void* local_handle;
	// 获得模块在当前进程中的基址。
	if (NULL == (local_handle = GetModuleBase(-1, modulePath))) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 获得当前进程中模块'%s'的基址失败。", modulePath);
		goto _ret;
	}
	//MY_LOG_INFO("GetRemoteAddress(pid_t, const char*, void*) - funAddr=%p", funAddr);
	// 获得模块在远程进程中的基址。
	if (NULL == (remote_base = GetModuleBase(target_pid, modulePath))) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 获得模块在远程进程'%d'中的基址失败。", target_pid);
		goto _ret;
	}

	// 通过计算得出远程函数的地址。
	remote_addr = (void *) (((size_t) funAddr - (size_t) local_handle) + (size_t) remote_base);
	// 	MY_LOG_INFO("GetRemoteAddress(pid_t, const char*, void*) - target_pid=%d, module_name=%s, local_addr=%p, local_base=%p, remote_handle=%p", 
	// 		target_pid, module_name, local_addr, local_base, remote_base);
	//MY_LOG_VERBOSE( "[+] get_remote_addr: 本地模块基址[%p], 远程模块基址[%p]\n", local_base, remote_handle );

_ret:
	if (0 != dlclose(handle)) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 关闭动态库句柄失败。%s。", dlerror());
	}
	return remote_addr;
}

// 获得远程函数地址。
void* GetRemoteAddress(pid_t target_pid, const char* modulePath, const char* moduleName, const char* funcName) {
	void *remote_base, *remote_addr = NULL;

	// 判断动态库中是否有相应的函数。
	void* handle = dlopen(modulePath, RTLD_LAZY);
	if (NULL == handle) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 打开动态库失败。%s。", dlerror());
		return NULL;
	}
	//MY_LOG_INFO("GetRemoteAddress(pid_t, const char*, void*) - handle=%p", handle);
	void* funAddr = dlsym(handle, funcName);
	if (NULL == funAddr) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 获得函数指针失败。%s。", dlerror());
		goto _ret;
	}

	void* local_handle;
	// 获得模块在当前进程中的基址。
	if (NULL == (local_handle = GetModuleBase(-1, modulePath))) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 获得当前进程中模块'%s'的基址失败。", modulePath);
		goto _ret;
	}
	//MY_LOG_INFO("GetRemoteAddress(pid_t, const char*, void*) - funAddr=%p", funAddr);
	// 获得模块在远程进程中的基址。
	if (NULL == (remote_base = GetModuleBaseByModuleName(target_pid, moduleName))) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 获得模块在远程进程'%d'中的基址失败。", target_pid);
		goto _ret;
	}

	// 通过计算得出远程函数的地址。
	remote_addr = (void *) (((size_t) funAddr - (size_t) local_handle) + (size_t) remote_base);
	// 	MY_LOG_INFO("GetRemoteAddress(pid_t, const char*, void*) - target_pid=%d, module_name=%s, local_addr=%p, local_base=%p, remote_handle=%p", 
	// 		target_pid, module_name, local_addr, local_base, remote_base);
	//MY_LOG_VERBOSE( "[+] get_remote_addr: 本地模块基址[%p], 远程模块基址[%p]\n", local_base, remote_handle );

_ret:
	if (0 != dlclose(handle)) {
		MY_LOG_WARNING("[-] GetRemoteAddress(pid_t, const char*, void*) - 关闭动态库句柄失败。%s。", dlerror());
	}
	return remote_addr;
}

//////////////////////////////////////////////////////////////////////////
// 远程内存。

RemoteMemory::RemoteMemory() : length(0), addr(NULL) {}
RemoteMemory& RemoteMemory::operator=(RemoteMemory& ra) {
	this->length = ra.length;
	this->addr = ra.addr;
	return *this;
}

// 清理类中的数据，并不回收所占用的内存。
void RemoteMemory::clear() {length = 0; addr = NULL;}

//////////////////////////////////////////////////////////////////////////
// 远程进程操作。

#if defined(__arm__)

RemoteProcess::RemoteProcess(pid_t pid)
	: mPid(pid)
{
	ZeroMemory(&mOrigReg, sizeof(struct pt_regs));
}

RemoteProcess::~RemoteProcess(){}

// 附加进程。
bool RemoteProcess::Attach() {
	if ( ptrace( PTRACE_ATTACH, mPid, NULL, 0  ) < 0 ) {
		MY_LOG_WARNING("[-] RemoteProcess::Attach() - PTRACE_ATTACH。code=%d: %s。", errno, strerror(errno));
		return false;
	}

	waitpid( mPid, NULL, WUNTRACED );

	//DEBUG_PRINT("attached\n");

	if ( ptrace( PTRACE_SYSCALL, mPid, NULL, 0  ) < 0 ) {
		MY_LOG_WARNING("[-] RemoteProcess::Attach() - PTRACE_SYSCALL。code=%d: %s。", errno, strerror(errno));
		return false;
	}

	waitpid( mPid, NULL, WUNTRACED );	// 等待目标进程暂停。

	// 获得原寄存器信息，以供结束时恢复用。
	if (false == GetRegs(&mOrigReg)) {
		MY_LOG_WARNING("[-] RemoteProcess::Attach() - 获得寄存器失败。code=%d: %s。", errno, strerror(errno));
		return false;
	}

	return true;
}

// 取消附加进程。
bool RemoteProcess::Detach() {
	if (!SetRegs(&mOrigReg)) {
		MY_LOG_WARNING("[-] RemoteProcess::Detach() - 设置寄存器失败。");
	}
	//MY_LOG_INFO("[*] Detach - 将要取消附加进程。");
	if (ptrace(PTRACE_DETACH, mPid, NULL, 0) < 0) {
		MY_LOG_WARNING("[-] RemoteProcess::Detach() - pid=%d。code=%d: %s。", mPid, errno, strerror(errno));
		return false;
	}

	return true;
}

// 暂停的进程继续运行。
bool RemoteProcess::Cont() {
	if (ptrace(PTRACE_CONT, mPid, NULL, 0) < 0) {
		MY_LOG_WARNING("[-] RemoteProcess::Cont() - code=%d: %s", errno, strerror(errno));
		return false;
	}

	return true;
}

// 获得寄存器信息。
bool RemoteProcess::GetRegs(struct pt_regs* regs) {
	if ( ptrace( PTRACE_GETREGS, mPid, NULL, regs ) < 0 )
	{
		MY_LOG_WARNING("[-] RemoteProcess::getRegs(pt_regs*) - 获得寄存器值失败。code=%d: %s。", errno, strerror(errno));
		return -1;
	}

	return true;
}

// 设置寄存器信息。
bool RemoteProcess::SetRegs(struct pt_regs* regs) {
	if (ptrace(PTRACE_SETREGS, mPid, NULL, regs) < 0) {
		MY_LOG_WARNING("[-] RemoteProcess::SetRegs(pt_regs*) - 设置寄存器值失败。code=%d: %s。", errno, strerror(errno));
		return false;
	}

	return true;
}

bool RemoteProcess::Call(void *addr, long *params, uint32_t num_params, struct pt_regs* regs) {
	bool bRet = false;
	uint32_t i;
	struct pt_regs destRegs;

	// 需要在原寄存器基础上操作，如果不这样，ptrace将会出问题。
	// 原来的代码是：struct pt_regs destRegs = {0}; 没有在原寄存器的基础上操作，所以ptrace出错了。
	// 这里对原寄存器拷贝一份是因为要保护原寄存器中的内容，以供以后恢复。
	memcpy(&destRegs, &mOrigReg, sizeof(struct pt_regs));

	// 将参数依次放入寄存器中。
	// 由于arm cpu的特性，寄存器的前4个可以放入4个参数。
	for (i = 0; i < num_params && i < 4; i++) {
		destRegs.uregs[i] = params[i];
	}
	//MY_LOG_INFO("0. origRegs.ARM_sp=%p", origRegs.ARM_sp);
	// 将超过4个的参数放入堆栈中。
	if (i < num_params) {
		//MY_LOG_INFO("0. destRegs.ARM_sp=%p", destRegs.ARM_sp);
		destRegs.ARM_sp -= (num_params - i) * sizeof(long); // 分配栈空间。
		//MY_LOG_INFO("1. destRegs.ARM_sp=%p", destRegs.ARM_sp);
		// 将参数写入栈空间。
		if (!WriteBytes((uint8_t *) destRegs.ARM_sp, (uint8_t *) &params[i], (num_params - i) * sizeof(long))) {
			MY_LOG_WARNING("[-] Call(void*, long*, uint32_t, struct pt_regs*) - 向远程进程写参数失败。");
		}
	}

	destRegs.ARM_pc = (long) addr; // 设置代码执行的起始地址。

	// 判断是thumb指令还是arm指令。
	// CPSR_T_MASK解释：
	// 对于ARM体系结构v5及以上的版本的T系列处理器，当该位为1时，程序运行于Thumb状态，否则运行于ARM状态。
	// 对于ARM体系结构v5及以上的版本的非T系列处理器，当该位为1时，执行下一条指令以引起为定义的指令异常；当该位为0时，表示运行于ARM状态。
	if (destRegs.ARM_pc & 1) {
		/* thumb */
		destRegs.ARM_pc &= (~1u); // thumb指令是以2字节对齐的。
		destRegs.ARM_cpsr |= CPSR_T_MASK;
	} else {
		/* arm */
		destRegs.ARM_cpsr &= ~CPSR_T_MASK;
	}

	destRegs.ARM_lr = 0;

	// 设置寄存器，并执行函数。
	if (false == SetRegs(&destRegs)) {
		MY_LOG_WARNING("[-] Call(void*, long*, uint32_t, struct pt_regs*) - 设置寄存器失败。");
		goto _ret;
	}
	if (false == Cont()) {
		MY_LOG_WARNING("[-] Call(void*, long*, uint32_t, struct pt_regs*) - 继续失败。");
		goto _ret;
	}
	waitpid(mPid, NULL, WUNTRACED);
	if (false == GetRegs(regs)) {
		MY_LOG_WARNING("[-] Call(void*, long*, uint32_t, struct pt_regs*) - 获得寄存信息失败。");
		goto _ret;
	}

	// 恢复寄存器的值。
	if (false == SetRegs(&mOrigReg)) {
		MY_LOG_WARNING("[-] Call(void*, long*, uint32_t, struct pt_regs*) - 恢复寄存器的值失败。");
		goto _ret;
	}
	bRet = true;

_ret:
	//MY_LOG_INFO("1. origRegs.ARM_sp=%p", origRegs.ARM_sp);
	//SetRegs(&origRegs);

	//struct pt_regs checkRegs;
	//GetRegs(&checkRegs);
	//MY_LOG_INFO("checkRegs.ARM_sp=%p", checkRegs.ARM_sp);
	return bRet;
}

// 分配远程内存。
bool RemoteProcess::Alloc(size_t length, int prot, RemoteMemory* remoteMemory) {
	if (NULL == remoteMemory) {
		MY_LOG_WARNING("[-] Alloc(size_t, int, RemoteMemory*) - 参数remoteMemory为NULL。");
		return false;
	}

	void* result_mmap = NULL;
	void* p_mmap;
	p_mmap = GetRemoteAddress(mPid, "/system/lib/libc.so", "mmap");
	if (NULL == p_mmap) {
		p_mmap = GetRemoteAddress(mPid, (void*) mmap);
	}

	if (NULL == p_mmap) {
		MY_LOG_WARNING("[-] Alloc(size_t, int, RemoteMemory*) - 获得进程'%d'中函数mmap的地址失败。", mPid);
		return false;
	}

	struct pt_regs regs;
	long parameters_mmap[6];

	parameters_mmap[0] = (long)NULL;
	parameters_mmap[1] = length;
	parameters_mmap[2] = prot;
	parameters_mmap[3] = MAP_ANONYMOUS | MAP_PRIVATE;
	parameters_mmap[4] = -1;
	parameters_mmap[5] = 0;

	// 远程调用mmap，在远程进程中分配内存。
	if (false == Call(p_mmap, parameters_mmap, array_size(parameters_mmap), &regs)) {
		MY_LOG_WARNING("[-] Alloc(size_t, int, RemoteMemory*) - 远程调用mmap失败。");
	} else {
		result_mmap = (void*) regs.ARM_r0;
	}

	if (NULL == result_mmap) {
		MY_LOG_WARNING("[-] Alloc(size_t, int, RemoteMemory*) - mmap函数执行失败。");
		return false;
	} else {
		remoteMemory->length = length;
		remoteMemory->addr = result_mmap;
		return true;
	}
}

// 释放远程内存。
bool RemoteProcess::FreeData(RemoteMemory* remoteMemory) {
	if (NULL == remoteMemory || NULL == remoteMemory->addr || 0 == remoteMemory->length) {
		MY_LOG_WARNING("[-] Free(RemoteMemory*) - 参数remoteMemory不正确。");
		return false;
	}

	bool bRet = false;
	//void* p_munmap = GetRemoteAddress(mPid, "/system/lib/libc.so", (void*) munmap);
	void* p_munmap = GetRemoteAddress(mPid, (void*) munmap);
	if (NULL == p_munmap) {
		MY_LOG_WARNING("[-] Free(RemoteMemory*) - 获得进程'%d'中函数munmap的地址失败。", mPid);
		return false;
	}
	struct pt_regs regs;
	long parameters_munmap[2];

	parameters_munmap[0] = (long)remoteMemory->addr;
	parameters_munmap[1] = remoteMemory->length;

	// 远程调用munmap，在远程进程中分配内存。
	if (false == Call(p_munmap, parameters_munmap, 2, &regs)) {
		MY_LOG_WARNING("[-] Free(RemoteMemory*) - 远程调用munmap失败。addr = %p。", remoteMemory->addr);
	} else {
		int ret = regs.ARM_r0;
		if (0 == ret) {
			bRet = true;
			remoteMemory->clear();
		} else {
			MY_LOG_WARNING("[-] Free(RemoteMemory*) - munmap执行失败。parameters_munmap[0]=%p。", (void*)(parameters_munmap[0]));
		}
	}

	return bRet;
}

// 向进程中写入数据。
bool RemoteProcess::WriteBytes(uint8_t *data, size_t size, RemoteMemory* out) {
	return WriteBytes(data, size, PROT_READ | PROT_WRITE, out);
}

// 向进程中写入数据。
bool RemoteProcess::WriteBytes(uint8_t *data, size_t size, int prot, RemoteMemory* out) {
	RemoteMemory rm;
	if (false == Alloc(size, prot, &rm)) {
		MY_LOG_WARNING("[-] RemoteProcess::WriteBytes(uint8_t*, size_t, RemoteMemory*) - 分配远程内存失败。");
		return false;
	}
	// 将数据写入远程进程。
	if (WriteBytes((uint8_t *)rm.addr, data, size)) {
		out->length = rm.length;
		out->addr = rm.addr;
		return true;
	} else {
		return false;
	}
}

// 向进程中写入数据。
bool RemoteProcess::WriteBytes(uint8_t *dest, uint8_t *data, size_t size) {
	uint32_t i, j, remain;
	uint8_t *laddr;

	union u {
		long val;
		char chars[sizeof(long)];
	} d;

	j = size / sizeof(long);
	remain = size % sizeof(long);

	laddr = data;

	// 一次写入四字节。
	for (i = 0; i < j; i++) {
		memcpy(d.chars, laddr, sizeof(long));
		if (-1 == ptrace(PTRACE_POKETEXT, mPid, dest, (void*) d.val)) {
			MY_LOG_WARNING("[-] RemoteProcess::WriteBytes(uint8_t*, uint8_t*, size_t) - 0.写入long类型数据失败。code=%d: %s。", errno, strerror(errno));
			return false;
		}

		dest += sizeof(long);
		laddr += sizeof(long);
	}

	if (remain > 0) {
		errno = 0;
		// 读取数据。
		d.val = ptrace(PTRACE_PEEKTEXT, mPid, dest, 0);
		if (errno > 0) {
			MY_LOG_WARNING("[-] RemoteProcess::WriteBytes(uint8_t*, uint8_t*, size_t) - 读取long类型数据失败。code=%d: %s。", errno, strerror(errno));
			return false;
		} else {
			// 将剩下的字节存入
			for (i = 0; i < remain; i++) {
				d.chars[i] = *laddr;
				laddr++;
			}

			if (-1 == ptrace(PTRACE_POKETEXT, mPid, dest, (void*) d.val)) {
				MY_LOG_WARNING("[-] RemoteProcess::WriteBytes(uint8_t*, uint8_t*, size_t) - 1.写入long类型数据失败。code=%d: %s。", errno, strerror(errno));
				return false;
			}
		}
	}

	return true;
}

// 远程调用dlopen函数。
void* RemoteProcess::remote_dlopen(const char * pathname, int mode) {
	void* p_remote_dlopen = GetRemoteAddress(mPid, (void*) dlopen);
	if (NULL == p_remote_dlopen) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlopen(const char *, int) - 获得远程进程'%d'中的dlopen函数地址失败。", mPid);
		return NULL;
	}

	// 	std::string moduleName = AddressModuleName(-1, (void*)dlopen);
	// 	MY_LOG_INFO("RemoteProcess::remote_dlopen(const char*, int) - p_remote_dlopen=%p, moduleName=%s", p_remote_dlopen, moduleName.c_str());

	RemoteMemory rm_pathname;
	if (false == WriteBytes((uint8_t*)pathname, strlen(pathname) + 1, &rm_pathname)) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlopen(const char *, int) - 将动态库地址写入目标进程失败。");
		return NULL;
	}

	// 	char* pStr = (char*)calloc(strlen(pathname) + 1, sizeof(char));
	// 	ReadData((uint8_t*)rm_pathname.addr, (uint8_t*)pStr, strlen(pathname) + 1);
	// 	MY_LOG_INFO("RemoteProcess::remote_dlopen(const char *, int) - pStr=|%s|", pStr);
	// 	free(pStr);

	void* dl_ret = NULL;
	struct pt_regs regs;
	long params[2];
	params[0] = (long) (rm_pathname.addr);
	params[1] = mode;

	// 调用dlopen。
	if (false == Call(p_remote_dlopen, params, array_size(params), &regs)) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlopen(const char *, int) - 远程调用dlopen失败。");
	}

	dl_ret = (void*)regs.ARM_r0;

	if (false == FreeData(&rm_pathname)) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlopen(const char *, int) - 释放远程内存失败。");
	}
	return dl_ret;
}

// 远程调用dlclose函数。
int RemoteProcess::remote_dlclose (void *handle) {
	void* p_remote_dlclose = GetRemoteAddress(mPid, (void*) dlclose);
	if (NULL == p_remote_dlclose) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlclose (void*) - 获得远程进程'%d'中的dlclose函数地址失败。", mPid);
		return -1;
	}

	int dl_ret = -1;
	struct pt_regs regs;
	long params[1];

	params[0] = (long)handle;
	// 调用dlclose。
	if (false == Call(p_remote_dlclose, params, array_size(params), &regs)) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlopen(const char *, int) - 远程调用dlclose失败。");
	}
	dl_ret = (int)regs.ARM_r0;

	return dl_ret;
}

// 远程调用dlsym函数。
void* RemoteProcess::remote_dlsym(void* handle, const char* symbol) {
	void* p_remote_dlsym = GetRemoteAddress(mPid, (void*) dlsym);
	if (NULL == p_remote_dlsym) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlsym (void*, const char*) - 获得远程进程'%d'中的dlsym函数地址失败。", mPid);
		return NULL;
	}

	RemoteMemory rm_symbolname;
	if (false == WriteBytes((uint8_t*)symbol, strlen(symbol) + 1, &rm_symbolname)) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlsym (void*, const char*) - 将符号名写入目标进程失败。");
		return NULL;
	}

	void* dl_ret = NULL;
	struct pt_regs regs;
	long params[2];
	params[0] = (long) handle;
	params[1] = (long) (rm_symbolname.addr);

	// 调用dlsym。
	if (false == Call(p_remote_dlsym, params, array_size(params), &regs)) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlsym (void*, const char*) - 远程调用dlsym失败。");
	}

	dl_ret = (void*)regs.ARM_r0;

	if (false == FreeData(&rm_symbolname)) {
		MY_LOG_WARNING("[-] RemoteProcess::remote_dlsym (void*, const char*) - 释放远程内存失败。");
	}

	return dl_ret;
}

// RemoteProcess::RemoteProcess(pid_t pid)
// 	: mPid(pid)
// {
// 	ZeroMemory(&mOrigReg, sizeof(struct pt_regs));
// }
// 
// RemoteProcess::~RemoteProcess(){}
// 
// // 附加进程。
// bool RemoteProcess::Attach() {
// 	if ( ptrace( PTRACE_ATTACH, mPid, NULL, 0  ) < 0 ) {
// 		MY_LOG_WARNING("[-] RemoteProcess::Attach(), PTRACE_ATTACH: %s", strerror(errno));
// 		return false;
// 	}
// 
// 	waitpid( mPid, NULL, WUNTRACED );
// 
// 	//DEBUG_PRINT("attached\n");
// 
// 	if ( ptrace( PTRACE_SYSCALL, mPid, NULL, 0  ) < 0 ) {
// 		MY_LOG_WARNING("[-] RemoteProcess::Attach(), PTRACE_SYSCALL: %s", strerror(errno));
// 		return false;
// 	}
// 
// 	waitpid( mPid, NULL, WUNTRACED );	// 等待目标进程暂停。
// 
// 	// 获得原寄存器信息，以供结束时恢复用。
// 	if (false == GetRegs(&mOrigReg)) {
// 		MY_LOG_WARNING("[-] RemoteProcess::Attach() - 获得寄存器失败。code=%d: %s。", errno, strerror(errno));
// 		return false;
// 	}
// 
// 	return true;
// }
// 
// // 取消附加进程。
// bool RemoteProcess::Detach() {
// 	SetRegs(&mOrigReg);
// 	if (ptrace(PTRACE_DETACH, mPid, NULL, 0) < 0) {
// 		//perror( "ptrace_detach" );
// 		MY_LOG_WARNING("[-] RemoteProcess::detach(pid_t): pid=%d, code=%d: %s\n",
// 			mPid, errno, strerror(errno));
// 		return false;
// 	}
// 
// 	return true;
// }
// 
// // 获得寄存器信息。
// bool RemoteProcess::GetRegs(struct pt_regs* regs) {
// 	if ( ptrace( PTRACE_GETREGS, mPid, NULL, regs ) < 0 )
// 	{
// 		MY_LOG_WARNING("[-] RemoteProcess::getRegs(pid_t,pt_regs*), 无法获取寄存器的值: %s",
// 			strerror(errno));
// 		return -1;
// 	}
// 
// 	return true;
// }
// 
// // 设置寄存器信息。
// bool RemoteProcess::SetRegs(struct pt_regs* regs) {
// 	if (ptrace(PTRACE_SETREGS, mPid, NULL, regs) < 0) {
// 		//perror( "ptrace_setregs: Can not set register values" );
// 		MY_LOG_WARNING("[-] RemoteProcess::setRegs(pid_t,pt_regs*), 未能设置寄存器的值: %s", strerror(errno));
// 		return false;
// 	}
// 
// 	return true;
// }

#endif
