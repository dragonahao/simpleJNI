#include "stdafx.h"
#include <process-thread.h>
#include <dlfcn.h>
#include <nativelog.h>
#include "Inject.h"

bool Inject(pid_t pid, const char* soPath) {
	RemoteProcess remoteProcess(pid);
	if (!remoteProcess.Attach()) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*) - 附加进程失败。Pid: %d。", pid);
		return false;
	}

	bool bRet = false;
	void* handle = remoteProcess.remote_dlopen(soPath, RTLD_LAZY);
	if (NULL == handle) {
		MY_LOG_WARNING("[-] Inject(pid_t, const char*) - 在远程进程中打开so失败。");
		goto _ret;
	}
	
	//MY_LOG_INFO("[*] Inject(pid_t, const char*) - 注入成功。");
	bRet = true;
_ret:
	remoteProcess.Detach();
	return bRet;
}
