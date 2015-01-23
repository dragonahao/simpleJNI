#pragma once

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////
// 文件。
/**
 * 拷贝文件。
 * @param[in] src 源文件的文件指针。
 * @param[in] dest 输出文件的文件指针。
 * @return true: 拷贝成功。false: 拷贝失败。
 */
bool CopyFile(IN FILE* src, IN FILE* dest);

/**
 * 拷贝文件。
 * @param[in] srcFile 源文件。
 * @param[in] outputFile 输出文件。
 * @return true: 拷贝成功。false: 拷贝失败。
 */
bool CopyFile(IN const char* srcFile, IN const char* outputFile);

/**
 * 通过文件描述符获得文件名。
 * @param[in] 文件描述符。
 * @return 获得成功，则返回文件名指针，需要使用free函数手动释放所占内存。获得失败，则返回NULL。
 */
char* GetFilePath(IN const int fd);

/**
 * 获得文件长度。
 * @param[in] 文件描述符。
 * @return 获得成功，则返回文件长度。获得失败，则返回-1。
 */
ssize_t GetFileSize(IN const int fd);

/**
 * 获得文件长度。
 * @param[in] filePath 文件路径。
 * @return 获得成功，则返回文件长度。获得失败，则返回-1。
 */
long GetFileSize(IN const char* filePath);

/**
 * 获得文件长度。
 * @param[in] 文件指针。
 * @return 获得成功，则返回文件长度。获得失败，则返回-1。
 */
long GetFileSize(IN FILE* fi);

////////////////////////////////////////////////////////////////////////////
// 目录。

/**
 * 目录是否存在。
 * @param[in] path 目录路径。
 * @return true：目录存在。false：目录不存在。
 */
bool IsDirExist(IN const char* path);
