#pragma once

// 最大异常消息长度。
#define MAX_EXP_MESSAGE_LENGTH 1000

/**
 * 用给定的异常类和异常消息抛出新的异常。
 * @param[in] env JNIEnv引用。
 * @param[in] expClassName 要抛出的异常类的类名。如：java/lang/RuntimeException。
 * @param[in] message 异常消息。
 * @return 返回1，抛出成功。返回0，抛出失败。
 * @note 如果抛出失败，请检查异常类是否正确，如果异常类无法找到，将返回false。
 */
int ThrowException(
	JNIEnv *env,
	const char* expClassName,
	const char* message);

/**
 * 用给定异常类和基于错误号的错误消息抛出新的异常。
 * @param[in] env JNIEnv引用。
 * @param[in] expClassName 要抛出的异常类的类名。如：java/lang/RuntimeException。
 * @param[in] errnum 错误号。
 * @return
 * @note
 */
void ThrowErrnoException(
	JNIEnv *env,
	const char* expClassName,
	int errnum );

#define CatchException(env) { \
	jthrowable ex = env->ExceptionOccurred(); \
	if (0 != ex) { \
		mJNIEnvAdaptor.getJNIEnv()->ExceptionClear(); \
	} \
}

/**
 * 抛出空指针异常。
 */
#define ThrowNullPointer(env, message) (ThrowException(env, "java/lang/NullPointerException", message))
/**
 * 抛出类定义未找到异常。
 */
#define ThrowNoClassDefFoundError(env, message) (ThrowException(env, "java/lang/NoClassDefFoundError", message))
/**
 * 抛出运行时异常。
 */
#define ThrowRuntime(env, message) (ThrowException(env, "java/lang/RuntimeException", message))
