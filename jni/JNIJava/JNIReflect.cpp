#include "stdafx.h"
#include <JNIReflect.h>

// 获得方法ID。
jmethodID GetMethodID_(IN JNIEnv* env, IN jobject obj, IN const char* methodName, IN const char* sig) {
	jclass clazz = env->GetObjectClass(obj);
	if (NULL == clazz) {
		return NULL;
	}
	jmethodID id = env->GetMethodID(clazz, methodName, sig);
	env->DeleteLocalRef(clazz);
	return id;
}

// 获得方法ID。
jmethodID GetMethodID_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig) {
	jclass clazz = env->FindClass(className);
	if (NULL == clazz) {
		return NULL;
	}
	jmethodID id = env->GetMethodID(clazz, methodName, sig);
	env->DeleteLocalRef(clazz);
	return id;
}

// 获得静态方法ID。
// jmethodID GetStaticMethodID_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig) {
// 	jclass clazz = env->FindClass(className);
// 	if (NULL == clazz) {
// 		return NULL;
// 	}
// 	jmethodID id = env->GetStaticMethodID(clazz, methodName, sig);
// 	env->DeleteLocalRef(clazz);
// 	return id;
// }

//////////////////////////////////////////////////////////////////////////

// 调用Java方法。
jobject CallObjectMethod_(IN JNIEnv* env, IN jobject obj, IN const char* methodName, IN const char* sig, IN ...) {
	jmethodID id = GetMethodID_(env, obj, methodName, sig);
	if (NULL == id) return NULL;

	va_list ap;
	va_start(ap, sig);
	jobject result = env->CallObjectMethodV(obj, id, ap);
	va_end(ap);

	return result;
}

void CallStaticVoidMethod_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig, IN ...) {
	jclass clazz = env->FindClass(className);
	if (NULL == clazz) {
		return;
	}

	jmethodID id = env->GetStaticMethodID(clazz, methodName, sig);
	if (NULL == id) return;

	va_list ap;
	va_start(ap, sig);
	env->CallStaticVoidMethodV(clazz, id, ap);
	va_end(ap);

	env->DeleteLocalRef(clazz);
}

jint CallStaticIntMethod_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig, IN ...) {
	jclass clazz = env->FindClass(className);
	if (NULL == clazz) {
		return 0;
	}

	jmethodID id = env->GetStaticMethodID(clazz, methodName, sig);
	if (NULL == id) return 0;

	va_list ap;
	va_start(ap, sig);
	jint result = env->CallStaticIntMethodV(clazz, id, ap);
	va_end(ap);

	env->DeleteLocalRef(clazz);
	return result;
}

// 调用Java方法。
jobject CallStaticObjectMethod_(IN JNIEnv* env, IN const char* className, IN const char* methodName, IN const char* sig, IN ...) {
	jclass clazz = env->FindClass(className);
	if (NULL == clazz) {
		return NULL;
	}

	jmethodID id = env->GetStaticMethodID(clazz, methodName, sig);
	if (NULL == id) return NULL;

	va_list ap;
	va_start(ap, sig);
	jobject result = env->CallStaticObjectMethodV(clazz, id, ap);
	va_end(ap);

	return result;
}
