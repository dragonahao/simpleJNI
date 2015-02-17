#pragma once

#include <limits.h>
#include <asm/ptrace.h>

/**
 *	通过进程名找到进程ID。
 *	@param[in] processName 进程名。
 *	@return 如果找到进程，则返回进程ID。如果未找到进程，则返回-1。
 */
pid_t GetPid(const char *processName);

/**
 * 获得父进程ID。
 * @param[in] pid 父进程ID。传入-1将返回当前进程的父进程ID。
 * @return 成功，则返回父进程ID。失败，则返回-1。
 */
pid_t GetPPid(pid_t pid);

/**
 * 通过PID获得进程绝对路径。
 * @param[in] pid 进程id。
 * @param[out] processPath 进程路径。
 * @return true: 获得成功。false: 获得失败。
 */
bool GetProcessPath(pid_t pid, char processPath[PATH_MAX]);

/**
 * 通过Pid获得进程名。
 * @param[in] pid 进程id。
 * @param[out] processName 进程名。
 */
void GetProcessName(pid_t pid, char processName[PATH_MAX]);

//////////////////////////////////////////////////////////////////////////

/**
 * 获得远程函数地址。
 * @param[in] target_pid 目标进程ID。
 * @param[in] local_addr 函数在当前模块中的地址。
 * @return 获得成功，则返回函数在远程模块中的地址。获得失败，则返回NULL。
 * @note 为了适配性，不传入模块名称。比如：小米的某些系统中dlopen在/data/data/com.lbe.security.miui/app_lib/libfwclient.so中，
 *       而在标准的android系统中dlopen在/system/lib/libdl.so。
 */
void* GetRemoteAddress(pid_t target_pid, void* local_addr);

/**
 *	获得远程函数地址。
 *	@param[in] target_pid 目标进程ID。
 *	@param[in] modulePath 模块路径。
 *	@param[in] funcName 函数名。
 *	@return 获得成功，则返回函数在远程模块中的地址。获得失败，则返回NULL。
 */
void* GetRemoteAddress(pid_t target_pid, const char* modulePath, const char* funcName);

/**
 * 获得远程函数地址。
 * @param[in] target_pid 目标进程ID。
 * @param[in] modulePath 模块路径。
 * @param[in] moduleName 模块名。
 * @param[in] funcName 函数名。
 * @return 获得成功，则返回函数在远程模块中的地址。获得失败，则返回NULL。
 */
void* GetRemoteAddress(pid_t target_pid, const char* modulePath, const char* moduleName, const char* funcName);

//////////////////////////////////////////////////////////////////////////

int ptrace_readdata( pid_t pid,  uint8_t *src, uint8_t *buf, size_t size );

int ptrace_writedata( pid_t pid, uint8_t *dest, uint8_t *data, size_t size );

int ptrace_writestring( pid_t pid, uint8_t *dest, char *str  );

int ptrace_call( pid_t pid, uint32_t addr, long *params, uint32_t num_params, struct pt_regs* regs );

int ptrace_getregs( pid_t pid, struct pt_regs* regs );

int ptrace_setregs( pid_t pid, struct pt_regs* regs );

int ptrace_continue( pid_t pid );

int ptrace_attach( pid_t pid );

int ptrace_detach( pid_t pid );

//////////////////////////////////////////////////////////////////////////

/**
 * 远程内存。
 */
class RemoteMemory {
public:
	size_t length;	// 实际分配的内存大小。
	void*  addr;		// 分配空间的起始地址。

public:
	RemoteMemory();

	RemoteMemory& operator=(RemoteMemory& ra);

	/**
	 * 清理类中的数据，并不回收所占用的内存。
	 */
	void clear();
};

//////////////////////////////////////////////////////////////////////////

#define CPSR_T_MASK		( 1u << 5 )

typedef void*(*start_routine)(void*);

#if defined(__arm__)

/**
 * 远程进程操作。
 */
class RemoteProcess {
protected:
	pid_t mPid;
	struct pt_regs mOrigReg;

public:
	/**
	 * 构造函数。
	 * @param[in] pid 进程id。
	 */
	RemoteProcess(pid_t pid);
	virtual ~RemoteProcess();

	/**
	 * 获得Pid。
	 * @return 返回Pid。
	 */
	//pid_t GetPid();

	/**
	 * 附加进程。
	 * @return 附加成功，返回true。附加失败，返回false。
	 * @note 如果附加失败，将输出log。
	 */
	bool Attach();

	/**
	 * 取消附加进程。
	 * @return 取消附加成功，返回true。取消附加失败，返回false。
	 * @note 如果取消附加失败，将输出log。
	 */
	bool Detach();

	/**
	 * 暂停的进程继续运行。
	 * @param[in] pid 进程ID。
	 * @return 函数执行成功，则返回true。函数执行失败，则返回false。
	 */
	bool Cont();

	/**
	 * 暂停进程。
	 * @return true: 暂停成功。false: 暂停失败。
	 */
	//bool Pause();

	/**
	 * 读取远程进程中的数据。
	 * @param[in] dest 目标地址。
	 * @param[in] data 输出缓冲区数据。
	 * @param[in] size 缓冲区的长度。
	 */
	//bool ReadData(uint8_t *dest, uint8_t *data, size_t size);

	/**
	 * 向进程中写入数据。
	 * @param[in] data 写入的数据地址。
	 * @param[in] size 数据的长度。
	 * @param[out] out 输出在远程进程中分配内存的信息。
	 * @return 写入成功，则返回true，当不再使用out时，需要通过RemoteProcess::FreeData函数释放远程内存。写入失败，则返回false，并且不设置out的值。
	 * @note 默认内存保护类型是PROT_READ | PROT_WRITE。
	 */
	bool WriteBytes(uint8_t *data, size_t size, RemoteMemory* out);

	/**
	 * 向进程中写入数据。
	 * @param[in] data 写入的数据地址。
	 * @param[in] size 数据的长度。
	 * @param[in] prot 内存的保护类型。
	 *                 是 PROT_NONE 或是下面值的“位或”运算结果：
	 *                 PROT_NONE：内存根本不能访问。
	 *                 PROT_READ：内存可读。
	 *                 PROT_WRITE：内存可写。
	 *                 PROT_EXEC：内存可执行。
	 * @param[out] out 输出在远程进程中分配内存的信息。
	 * @return 写入成功，则返回true，当不再使用out时，需要通过RemoteProcess::FreeData函数释放远程内存。写入失败，则返回false，并且不设置out的值。
	 */
	bool WriteBytes(uint8_t *data, size_t size, int prot, RemoteMemory* out);

	/**
	 * 向进程中写入函数。
	 * @param[in] fun 写入的函数地址。
	 * @param[in] size 函数长度。
	 * @param[out] out 输出在远程进程中分配内存的信息。
	 * @return 写入成功，则返回true，当不再使用out时，需要通过RemoteProcess::FreeCode函数释放远程内存。写入失败，则返回false，并且不设置out的值。
	 * @note 函数设置的内存保护属性为：PROT_READ | PROT_WRITE。
	 *       函数内部会根据fun参数，判断是arm指令还是thumb指令，如果是thumb指令，out.addr的地址的末位是1。
	 */
	//bool WriteCode(void* fun, size_t size, RemoteMemory* out);

	/**
	 * 在远程进程中调用函数。
	 * @param[in] addr 远程进程中的函数地址。
	 * @param[in] params 参数列表。参数列表中的参数数据必须存在于远程进程中。
	 * @param[in] num_params 参数个数。
	 * @param[out] regs 返回远程函数执行后各个寄存器的值。
	 * @return 调用成功，则返回true。调用失败，则返回false。
	 */
	bool Call(void *addr, long *params, uint32_t num_params, struct pt_regs* regs);

	/**
	 * 分配远程内存。
	 * @param[in] length 分配的内存大小。
	 * @param[in] prot 内存的保护类型。
	 *                 是 PROT_NONE 或是下面值的“位或”运算结果：
	 *                 PROT_NONE：内存根本不能访问。
	 *                 PROT_READ：内存可读。
	 *                 PROT_WRITE：内存可写。
	 *                 PROT_EXEC：内存可执行。
	 * @param[out] remoteAlloc 返回RemoteMemory类型。
	 * @return 分配成功，则返回true。分配失败，则返回false，并不设置remoteAlloc的值。
	 * @note 分配的内存空间初始化为0。
	 */
	bool Alloc(size_t length, int prot, RemoteMemory* remoteAlloc);

	/**
	 * 释放远程内存中的数据。
	 * @param[in] remoteAlloc RemoteMemory类型的值。
	 * @return true：释放成功。false：释放失败。false：释放失败。
	 * @note true：释放成功，并清理remoteAlloc。
	 */
	bool FreeData(RemoteMemory* remoteAlloc);

	/**
	 * 释放远程内存中的代码。
	 * @param[in] remoteAlloc RemoteMemory类型的值。
	 * @return true：释放成功，并清理remoteAlloc。false：释放失败。
	 * @note 函数内部会根据remoteAlloc参数，判断是arm指令还是thumb指令。
	 */
	//bool FreeCode(RemoteMemory* remoteAlloc);

	/**
	 * 获得远程错误号。
	 * @return 返回错误号。
	 */
	//int remote_errno();

	/**
	 * 远程调用dlopen函数。
	 * @param[in] pathname 
	 * @param[in] mode 
	 * @return 
	 */
	void* remote_dlopen(const char * pathname, int mode);

	/**
	 * 远程调用dlclose函数。
	 * @param[in] handle 动态库句柄。
	 * @return 如果调用成功，则返回0。如果调用失败，则返回一个非零值。
	 */
	int remote_dlclose (void *handle);

	/**
	 * 远程调用dlsym函数。
	 * @param[in] handle 动态库句柄。
	 * @param[in] symbol 符号名。
	 * @return 如果调用成功，则返回函数地址。如果调用失败，则返回NULL。
	 */
	void* remote_dlsym(void* handle, const char* symbol);

	/**
	 * 同步创建远程线程。
	 * @param[out] pThread 
	 * @param[in] routine 
	 * @param[in] routine_size 
	 * @param[in] arg 
	 * @param[in] arg_size 
	 * @param[out] 【可选】 retval 输出线程函数的返回值。
	 * @return 
	 * @note 如果参数attr不为NULL，则在远程进程中调用pthread_attr_setdetachstate，设置属性标志PTHREAD_CREATE_JOINABLE。
	 */
// 	bool CreateRemoteThreadSync(pthread_t *pThread, /*const pthread_attr_t* attr, */
// 		const start_routine routine, const size_t routine_size, 
// 		void* arg, const size_t arg_size, 
// 		void **retval);

	/**
	 * 异步创建远程线程。
	 * @param[out] pThread 
	 * @param[in] routine 
	 * @param[in] routine_size 
	 * @param[in] arg 【可选】 
	 * @param[in] arg_size 参数长度。
	 * @param[out] outRoutine 
	 * @param[out] outArg 【可选】
	 * @return 。
	 * @note 此函数在远程进程中调用了pthread_detach函数，设置创建的远程线程为分离线程，所以不要在线程函数中自行调用pthread_detach函数。
	 */
// 	bool CreateRemoteThreadAsync(
// 		pthread_t *pThread, 
// 		/*const pthread_attr_t* attr, */
// 		const start_routine routine, 
// 		const size_t routine_size, 
// 		void* arg, 
// 		const size_t arg_size, 
// 		RemoteMemory* outRoutine, 
// 		RemoteMemory* outArg);

	/**
	 * 在远程进程中加载动态库并同步执行库中的函数。
	 * @param[in] dynLibPath 动态库路径。
	 * @param[in] 
	 * @param[in] 
	 * @param[in] 
	 * @param[out] 【可选】
	 * @return 加载成功，则返回动态库句柄。加载失败，则返回NULL。
	 * @note 在远程进程中新创建一个线程运行函数。
	 */
	//bool RemoteExecution(const char* dynLibPath, const char* funcName, void* arg, const size_t arg_size, void** retval);


	//bool RemoteExecution(const char* dynLibPath, const char* funcName, long *params, uint32_t num_params, void** retval);

protected:
	/**
	 * 获得寄存器信息。
	 * @param[out] regs 返回寄存器信息。
	 * @return 获得成功，则返回true。获得失败，则返回false。
	 */
	bool GetRegs(struct pt_regs* regs);

	/**
	 * 设置寄存器信息。
	 * @param[in] pid 进程ID。
	 * @param[in] regs 寄存器信息。
	 * @return 获得成功，则返回true。获得失败，则返回false。
	 */
	bool SetRegs(struct pt_regs* regs);

	/**
	 * 向进程中写入数据。
	 * @param[in] dest 目标地址。
	 * @param[in] data 写入的数据地址。
	 * @param[in] size 数据的长度。
	 * @return 写入成功，则返回true。写入失败，则返回false。
	 */
	bool WriteBytes(uint8_t *dest, uint8_t *data, size_t size);

	//////////////////////////////////////////////////////////////////////////

	/**
	 * 远程调用pthread_attr_init函数。
	 * @param[out] attr 输出初始化的线程属性（pthread_attr_t）。
	 * @return 如果成功，则返回0。如果失败，则返回一个非零的错误号，并将参数attr清零。
	 * @note 参数attr使用完毕后，需要调用FreeData函数释放远程内存。
	 */
	//int remote_pthread_attr_init(RemoteMemory *attr);

	/**
	 * 远程调用pthread_attr_destroy函数。
	 * @param[in] attr 
	 * @return 如果成功，则返回0。如果失败，则返回一个非零的错误号。
	 */
	//int remote_pthread_attr_destroy(RemoteMemory *attr);

	/**
	 * 远程调用pthread_attr_setdetachstate函数。
	 * @param[in] attr 
	 * @param[in] detachstate 
	 * @return 如果成功，则返回0。如果失败，则返回一个非零的错误号。
	 */
	//int remote_pthread_attr_setdetachstate(RemoteMemory *attr, int detachstate);

	/**
	 * 远程调用pthread_create函数。
	 * @param[out] thread
	 * @param[in] attr 【可选】
	 * @param[in] start_routine 
	 * @param[in] arg 【可选】
	 * @return 如果成功，则返回0。如果失败，则返回一个非零的错误号，并不定义参数thread。
	 */
	//int remote_pthread_create(pthread_t* thread, RemoteMemory* attr, RemoteMemory* start_routine, RemoteMemory* arg);

	/**
	 * 等待远程线程运行结束。
	 * @param[in] thread 
	 * @param[out] retval 【可选】
	 */
	//int remote_pthread_join(pthread_t thread, void **retval);

	/**
	 * 设置线程为可分离的。
	 * @param[in] 
	 * @return 如果成功，则返回0。如果失败，则返回一个非零的错误号。
	 */
	//int remote_pthread_detach(pthread_t thread);

	/**
	 * 异步创建远程线程。
	 * @param[out] pThread 
	 * @param[in] routine 
	 * @param[in] routine_size 
	 * @param[in] arg 【可选】
	 * @param[in] arg_size 
	 * @return 。
	 * @note 此函数在远程进程中调用了pthread_detach函数，设置创建的远程线程为分离线程，所以不要在线程函数中自行调用pthread_detach函数。
	 */
// 	bool CreateRemoteThreadAsync(
// 		pthread_t *pThread, 
// 		/*const pthread_attr_t* attr, */
// 		const start_routine routine, 
// 		const size_t routine_size, 
// 		void* arg, 
// 		const size_t arg_size);
};

// class RemoteProcess {
// protected:
// 	pid_t mPid;
// 	struct pt_regs mOrigReg;
// 
// public:
// 	/**
// 	 * 构造函数。
// 	 * @param[in] pid 进程id。
// 	 */
// 	RemoteProcess(pid_t pid);
// 	~RemoteProcess();
// 
// 	/**
// 	 * 附加进程。
// 	 * @return 附加成功，返回true。附加失败，返回false。
// 	 * @note 如果附加失败，将输出log。
// 	 */
// 	bool Attach();
// 
// 	/**
// 	 * 取消附加进程。
// 	 * @return 取消附加成功，返回true。取消附加失败，返回false。
// 	 * @note 如果取消附加失败，将输出log。
// 	 */
// 	bool Detach();
// 
// protected:
// 	/**
// 	 * 获得寄存器信息。
// 	 * @param[out] regs 返回寄存器信息。
// 	 * @return 获得成功，则返回true。获得失败，则返回false。
// 	 */
// 	bool GetRegs(struct pt_regs* regs);
// 
// 	/**
// 	 * 设置寄存器信息。
// 	 * @param[in] pid 进程ID。
// 	 * @param[in] regs 寄存器信息。
// 	 * @return 获得成功，则返回true。获得失败，则返回false。
// 	 */
// 	bool SetRegs(struct pt_regs* regs);
// };

#endif
