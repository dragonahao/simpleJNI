#pragma once

#include <string>

class ModuleList {
public:
	class Entry {
	public:
		unsigned long start;
		unsigned long end;
		//PagePermissions pagePerms;
		std::string pagePerms;
		unsigned long offset;
		//unsigned long dev;
		std::string dev;
		size_t inode;
		std::string name;

		Entry();

		/**
		 * @param[in] pid 
		 */
		Entry(ModuleList::Entry& entry);

		/**
		 * @param[in] pid 
		 */
		ModuleList::Entry& operator=(ModuleList::Entry& entry);
	};

	class Iterator {
	public:
		Iterator();
		~Iterator();
		//bool hasNext();
		/**
		 * @param[out] entry 
		 */
		ModuleList::Entry* next(ModuleList::Entry* entry);

		friend class ModuleList;

	protected:
		void clear();
		/**
		 * @param[in] pid 
		 */
		bool setPid(pid_t pid);

		FILE *mFILE;
	};

public:
	// ModuleList(IN pid_t pid);
	/**
	 * @param[in] 
	 */
	ModuleList::Iterator* iterator(pid_t pid);
protected:
	ModuleList::Iterator mIterator;
};

/**
 * 获得地址所在的模块名。
 * @param[in] pid 进程id。如果传入-1则表示当前进程。
 * @param[in] addr 地址。
 * @return 返回模块名。
 */
std::string GetModuleName (pid_t pid, void* addr);

/**
 * 通过模块名获得模块基址。
 * @param[in] pid 进程ID。
 * @param[in] moduleName 模块名。如：libc.so。
 * @return 获得成功，则返回模块基址。获得失败，则返回0。
 */
void* GetModuleBaseByModuleName(pid_t pid, const char* moduleName);

/**
 * 获得模块基址。
 * @param[in] pid 进程ID。
 * @param[in] modulePath 模块完整路径。如：/system/lib/libc.so。
 * @return 获得成功，则返回模块基址。获得失败，则返回0。
 */
void* GetModuleBase(pid_t pid, const char* modulePath);
