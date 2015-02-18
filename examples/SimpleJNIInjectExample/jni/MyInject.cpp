#include <nativelog.h>
#include <stdlib.h>
#include <process-thread.h>
#include <hack/Inject.h>
#include <sys/ptrace.h>

/**
 * 程序接收两个参数：
 * 1. 进程名。
 * 2. so路径。
 */
int main(int argc, char *argv[]) {
	pid_t pid = atoi(argv[1]);

	char processName[PATH_MAX] = {'\0'};
	GetProcessName(pid, processName);

 	MY_LOG_INFO("[*] 注入程序 - 目标PID：%d，进程名：%s，so路径：%s", pid, processName, argv[2]);

	if (0 == strlen(processName)) {
		MY_LOG_WARNING("[-] 注入程序 - 获得进程名失败，请确认输入的是有效的PID！");
		return -1;
	}

// 	if (!Inject(pid, argv[2])) {
// 		MY_LOG_WARNING("[-] 注入程序 - 注入失败！");
// 		return -1;
// 	}

	struct pt_regs regs;
	if (!Inject(pid, argv[2], "hook_entry", (void*)"I'm parameter!", strlen("I'm parameter!") + 1, &regs)) {
		MY_LOG_WARNING("[-] 注入程序 - 注入失败！");
		return -1;
	}

	return 0;
}
