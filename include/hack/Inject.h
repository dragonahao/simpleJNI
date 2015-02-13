#pragma once

#include <sys/types.h>

/**
 * 注入so到进程。
 * @param[in] pid 进程ID。
 * @param[in] soPath so文件路径。
 * @return 注入成功，则返回true。注入失败，则返回false。
 */
bool Inject(pid_t pid, const char* soPath);
