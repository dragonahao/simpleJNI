#include <jni.h>
#include <nativelog.h>
#include <SmartPoint.h>
#include <string.h>
#include <stdlib.h>
#include <process-thread.h>
#include <hack/Inject.h>

/**
 * 程序接收两个参数：
 * 1. 进程名。
 * 2. so路径。
 */
int main(int argc, char *argv[]) {
	pid_t pid = GetPid(argv[1]);
	if (-1 == pid) {
		MY_LOG_INFO("获得进程ID失败，进程名：%s", argv[1]);
		return -1;
	}

	MY_LOG_INFO("注入程序 - 目标PID：%d，so路径：%s", pid, argv[2]);
	Inject(pid, argv[2]);
	return 0;
}
