#pragma once

#include <sys/types.h>

/**
 * 注入so到进程。
 * @param[in] pid 进程ID。
 * @param[in] soPath so文件路径。
 * @return 注入成功，则返回true。注入失败，则返回false。
 */
bool Inject(pid_t pid, const char* soPath);

/**
 * 注入so到进程。
 * @param[in] pid 进程ID。
 * @param[in] soPath so文件路径。
 * @param[in] funName 函数名。
 * @param[in] params 参数列表。参数列表中的参数数据必须存在于远程进程中。
 * @param[in] paramSize 参数个数。
 * @param[out] regs 返回远程函数执行后各个寄存器的值。
 * @return 注入成功，则返回true。注入失败，则返回false。
 */
bool Inject(pid_t pid, const char* soPath, const char* funName, void* params, uint32_t paramSize, struct pt_regs* regs);

int inject_remote_process( pid_t target_pid, const char *library_path, const char *function_name, void *param, size_t param_size );
