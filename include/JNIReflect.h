#ifndef _JNI_REFLECT_H_
#define _JNI_REFLECT_H_

/**
 * 获得方法ID。
 * @param[in] env JNI环境指针。
 * @param[in] obj Java对象。
 * @param[in] methodName 方法名。
 * @param[in] sig 方法签名。
 * @return 获得成功，返回方法ID。获得失败，返回NULL。
 */
jmethodID GetMethodID_(IN JNIEnv* env, IN jobject obj, IN const char* methodName, IN const char* sig);

/**
 * 获得方法ID。
 * @param[in] env JNI环境指针。
 * @param[in] className 类名。如：com/androidtest/Test。
 * @param[in] methodName 方法名。
 * @param[in] sig 方法签名。
 * @return 获得成功，返回方法ID。获得失败，返回NULL。
 */
jmethodID GetMethodID_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig);

//jmethodID GetStaticMethodID_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig);

jobject CallObjectMethod_(IN JNIEnv* env, IN jobject obj, IN const char* methodName, IN const char* sig, IN ...);

void CallStaticVoidMethod_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig, IN ...);

jint CallStaticIntMethod_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig, IN ...);

jobject CallStaticObjectMethod_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig, IN ...);

#endif