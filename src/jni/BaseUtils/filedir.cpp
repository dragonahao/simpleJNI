#include "stdafx.h"
#include "filedir.h"

////////////////////////////////////////////////////////////////////////////
// 文件。

// 拷贝文件。
bool CopyFile(FILE* src, FILE* dest) {
	size_t rc;
	unsigned char buf[BUF_SIZE_4096];

	while (true) {
		rc = fread(buf, sizeof(unsigned char), BUF_SIZE_4096, src);
		if (0 == rc) {
			break;
		}

		fwrite(buf, sizeof(unsigned char), rc, dest);
		if (BUF_SIZE_4096 != rc) {
			break;
		}
	}

	return true;
}

// 拷贝文件。
bool CopyFile(const char* srcFile, const char* outputFile) {
	FILE *in_file = NULL, *out_file = NULL;
	bool bRet = false;

	if ((in_file = fopen(srcFile, "rb")) == NULL) {
		MY_LOG_WARNING("打开源文件失败。路径：%s", srcFile);
		return false;
	}
	if ((out_file = fopen(outputFile, "wb")) == NULL) {
		MY_LOG_WARNING("打开目标件失败。路径：%s。%s。", outputFile, strerror(errno));
		fclose(in_file);
		return false;
	}

	bRet = CopyFile(in_file, out_file);

	fclose(out_file);
	fclose(in_file);

	return bRet;
}

// 通过文件描述符获得文件名。
char* GetFilePath(const int fd) {
	if (0 >= fd) {
		return NULL;
	}

	char buf[1024] = { '\0' };
	char *file_path = (char*) calloc(PATH_MAX, sizeof(char)); // PATH_MAX in limits.h
	snprintf(buf, sizeof(buf), "/proc/self/fd/%d", fd);
	if (readlink(buf, file_path, PATH_MAX - sizeof(char)) != -1) {
		return file_path;
	}

	free(file_path);
	return NULL;
}

// 获得文件长度。
ssize_t GetFileSize(const int fd) {
	off_t start, end;

	start = lseek(fd, 0L, SEEK_CUR);
	end = lseek(fd, 0L, SEEK_END);
	//MY_LOG_INFO("GetFileSize(const int) - start=%ld, end=%ld, code=%d,%s。", start, end, errno, strerror(errno));
	(void) lseek(fd, start, SEEK_SET);

	if (start == (off_t) -1 || end == (off_t) -1) {
		return -1;
	}

	if (end == 0) {
		return -1;
	} else {
		return end;
	}
}

// 获得文件长度。
long GetFileSize(const char* filePath) {
	FILE* fi = fopen(filePath, "r");
	if (NULL == fi) {
		return -1;
	}
	long size = GetFileSize(fi);
	fclose(fi);
	return size;
}

// 获得文件长度。
long GetFileSize(FILE* fi) {
	int start, end;
	long filesize;

	start = fseek(fi, 0L, SEEK_CUR);
	end = fseek(fi, 0L, SEEK_END);
	filesize = ftell(fi);
	(void) fseek(fi, start, SEEK_SET);
	return filesize;
}

////////////////////////////////////////////////////////////////////////////
// 目录。

// 目录是否存在。
bool IsDirExist(const char* path) {
	struct stat fileStat;
	if ((stat(path, &fileStat) == 0) && S_ISDIR(fileStat.st_mode)) {
		return true;
	} else {
		return false;
	}
}
