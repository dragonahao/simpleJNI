#include "stdafx.h"
#include "module.h"

//////////////////////////////////////////////////////////////////////////
// ModuleList。

ModuleList::Iterator* ModuleList::iterator(pid_t pid) {
	if (mIterator.setPid(pid)) {
		return &mIterator;
	} else {
		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// ModuleList::Entry。

ModuleList::Entry::Entry() {}
ModuleList::Entry::Entry(ModuleList::Entry& entry) {
	*this = entry;
}

ModuleList::Entry& ModuleList::Entry::operator=(ModuleList::Entry& entry) {
	this->start = entry.start;
	this->end = entry.end;
	this->pagePerms = entry.pagePerms;
	this->offset = entry.offset;
	this->dev = entry.dev;
	this->inode = entry.inode;
	this->name = entry.name;
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// ModuleList::Iterator。

ModuleList::Iterator::Iterator()
: mFILE(NULL)
{}

ModuleList::Iterator::~Iterator() {clear();}

// bool ModuleList::Iterator::hasNext() {
// 	if (filePoint > fileEnd) {
// 		return false;
// 	} else {
// 		return true;
// 	}
// }

ModuleList::Entry* ModuleList::Iterator::next(OUT ModuleList::Entry* entry) {
	char line[1024];

	if (NULL == fgets(line, sizeof(line), mFILE)) {
		return NULL;
	} else {
		uint8_t* filePoint = (uint8_t*)line;
		uint8_t* fileEnd = filePoint + (strlen(line) - 1);
		//MY_LOG_INFO("line=%s", line);
		// start。
		uint8_t* cursor = filePoint;
		while (filePoint <= fileEnd) {
			char ch = *((char*)filePoint);
			filePoint++;
			if ('-' == ch) {
				char *endptr = (char*)((uint8_t*)filePoint - 1);
				entry->start = strtoul((char*)cursor, &endptr, 16);
				break;
			}
		}

		// end。
		cursor = filePoint;
		while (filePoint <= fileEnd) {
			char ch = *((char*)filePoint);
			filePoint++;
			if (' ' == ch) {
				char *endptr = (char*)((uint8_t*)filePoint - 1);
				entry->end = strtoul((char*)cursor, &endptr, 16);
				break;
			}
		}

		// pagePerms
		cursor = filePoint;
		while (filePoint <= fileEnd) {
			char ch = *((char*)filePoint);
			filePoint++;
			if (' ' == ch) {
				char *endptr = (char*)((uint8_t*)filePoint - 1);
				*endptr = '\0';

				char *tmp = (char*) calloc(strlen((const char*)cursor) + 1, sizeof(char));
				strcpy(tmp, (const char*)cursor);
				entry->pagePerms = tmp;

				*endptr = ' ';
				break;
			}
		}

		// offset
		cursor = filePoint;
		while (filePoint <= fileEnd) {
			char ch = *((char*)filePoint);
			filePoint++;
			if (' ' == ch) {
				char *endptr = (char*)((uint8_t*)filePoint - 1);
				entry->offset = strtoul((char*)cursor, &endptr, 16);
				break;
			}
		}

		// dev
		cursor = filePoint;
		while (filePoint <= fileEnd) {
			char ch = *((char*)filePoint);
			filePoint++;
			if (' ' == ch) {
				char *endptr = (char*)((uint8_t*)filePoint - 1);
				*endptr = '\0';
				//entry->dev = (char*)cursor;

				char *tmp = (char*) calloc(strlen((const char*)cursor) + 1, sizeof(char));
				strcpy(tmp, (const char*)cursor);
				entry->dev = tmp;

				*endptr = ' ';
				break;
			}
		}

		// inode
		cursor = filePoint;
		while (filePoint <= fileEnd) {
			char ch = *((char*)filePoint);
			filePoint++;
			if (' ' == ch) {
				char *endptr = (char*)((uint8_t*)filePoint - 1);
				entry->inode = strtoul((char*)cursor, &endptr, 10);
				break;
			}
		}

		// name
		cursor = filePoint;
		uint8_t* nameStart = cursor;
		while (true) {
			char ch = *((char*)filePoint);
			filePoint++;
			if ('\n' == ch) {
				*((char*)filePoint - 1) = '\0';
				break;
			} else if (' ' != ch) {
				if (nameStart == cursor) {
					nameStart = filePoint;
				}
			}
		}
		if ('\0' == *(char*)nameStart) {
			//entry->name = "";
		} else {
			//entry->name = (char*)nameStart;
			char *tmp = (char*) calloc(strlen((const char*)nameStart) + 1, sizeof(char));
			strcpy(tmp, (const char*)nameStart);
			entry->name = tmp;
		}
	}

	return entry;
}

void ModuleList::Iterator::clear() {
	if (NULL != mFILE) {
		if (-1 == fclose(mFILE)) {
			MY_LOG_WARNING("[-] ModuleList::Iterator::Close() - 关闭文件失败。code=%d,%s。", errno, strerror(errno));
		}
		mFILE = NULL;
	}
}

bool ModuleList::Iterator::setPid(pid_t pid) {
	clear();

	char filename[32];
	if (pid < 0) {
		/* self process */
		snprintf(filename, sizeof(filename), "/proc/self/maps");
	} else {
		snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
	}

	mFILE = fopen(filename, "r");
	if (NULL == mFILE) {
		MY_LOG_WARNING("[-] ModuleList::Iterator::setPid(pid_t) - 打开文件失败。code=%d,%s。", errno, strerror(errno));
		return false;
	} else {
		return true;
	}
}

//////////////////////////////////////////////////////////////////////////

// 获得地址所在的模块名。
lsp<char> GetModuleName (pid_t pid, void* addr) {
	ModuleList moduleList;
	ModuleList::Iterator* iterator = moduleList.iterator(pid);
	ModuleList::Entry entry;
	while(NULL != iterator->next(&entry)) {
		if (((size_t)addr >= entry.start) && ((size_t)addr <= entry.end)) {
			return entry.name;
		}
		// 		MY_LOG_INFO("my = %08x-%08x %s %08x %s %d |%s|", 
		// 			entry.start, entry.end, entry.pagePerms.c_str(), entry.offset, entry.dev.c_str(), entry.inode, entry.name.c_str());
	}
	return lsp<char>();
}

// 通过模块名获得模块基址。
void* GetModuleBaseByModuleName(pid_t pid, const char* moduleName) {
	FILE *fp;
	long addr = 0;
	char *pch;
	char filename[32];
	char line[1024];

	if (pid < 0) {
		/* self process */
		snprintf(filename, sizeof(filename), "/proc/self/maps");
	} else {
		snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
	}

	fp = fopen(filename, "r");

	if (fp != NULL) {
		while (fgets(line, sizeof(line), fp)) {
			if (strstr(line, moduleName)) {
				pch = strtok(line, "-");
				addr = strtoul(pch, NULL, 16);

				if (0x8000 == addr)
					addr = 0;

				break;
			}
		}
		if (0 == addr) {
			MY_LOG_WARNING("[-] GetModuleBase(pid_t, const char*) - 获得模块'%s'的地址失败。", moduleName);
		}
		//MY_LOG_INFO("GetModuleBase(pid_t, const char*) - 模块名：%s，模块基址：%p", moduleName, addr);
		fclose(fp);
	} else {
		MY_LOG_WARNING("[-] GetModuleBase(pid_t, const char*) - 打开文件'%s'失败。", filename);
	}

	return (void *) addr;
}

// 获得模块基址。
void* GetModuleBase(pid_t pid, const char* modulePath) {
	FILE *fp;
	long addr = 0;
	char *pch;
	char filename[32];
	char line[1024];

	if (pid < 0) {
		/* self process */
		snprintf(filename, sizeof(filename), "/proc/self/maps");
	} else {
		snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
	}

	fp = fopen(filename, "r");

	if (fp != NULL) {
		while (fgets(line, sizeof(line), fp)) {
			if (strstr(line, modulePath)) {
				pch = strtok(line, "-");
				addr = strtoul(pch, NULL, 16);

				if (0x8000 == addr)
					addr = 0;

				break;
			}
		}
		if (0 == addr) {
			MY_LOG_WARNING("[-] GetModuleBase(pid_t, const char*) - 获得模块'%s'的地址失败。", modulePath);
		}
		//MY_LOG_INFO("GetModuleBase(pid_t, const char*) - 模块名：%s，模块基址：%p", moduleName, addr);
		fclose(fp);
	} else {
		MY_LOG_WARNING("[-] GetModuleBase(pid_t, const char*) - 打开文件'%s'失败。", filename);
	}

	return (void *) addr;
}
