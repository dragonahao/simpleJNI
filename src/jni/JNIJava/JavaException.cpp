#include "stdafx.h"
#include "JavaException.h"

// 用给定的异常类和异常消息抛出新的异常。
int ThrowException(
	JNIEnv *env,
	const char* expClassName,
	const char* message ) {
	// 获取异常类。
	jclass clazz = env->FindClass(expClassName);
	// 异常类是否找到？
	if ( NULL == clazz ) {
		return 0;
	} else {
		//char buffer[MAX_EXP_MESSAGE_LENGTH];
		const char* format = "{%s:%s} %s: [%s]";
		const char* file = __FILE__;
		char line[25];
		snprintf(line, sizeof(line), "%d", __LINE__);
		const char* prettyFunction = __PRETTY_FUNCTION__;
		// 加20是因为下面的格式中的符号也要占用字节：{%s:%s} %s: [%s]
		int bufferLen = strlen(file) + strlen(line) + strlen(prettyFunction) + strlen(message) + strlen(format) + 1;
		bufferLen *= 2;
		char *buffer = new char[bufferLen];
		//snprintf(buffer, sizeof(buffer), "(%s:%u) %s: %s", __FILE__, __LINE__, __PRETTY_FUNCTION__, message);
		snprintf(buffer, bufferLen, format, file, line, prettyFunction, message);
		// 抛出异常。
		env->ThrowNew(clazz, buffer);
		delete[] buffer;
		// 释放原生类引用。
		env->DeleteLocalRef(clazz);
		return 1;
	}
}

void ThrowErrnoException(
	JNIEnv *env,
	const char* expClassName,
	int errnum ) {
	int ret = 1;
	char errorInfo[512];
	// 获取错误号消息。
	if ( -1 == ( ret = strerror_r(errnum, errorInfo, 512) ) ) {
		ret = strerror_r(errno, errorInfo, errnum);
	}

	// 抛出异常。
	ThrowException(env, expClassName, errorInfo);

	return;
}
