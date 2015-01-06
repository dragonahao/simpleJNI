#include "stdafx.h"
#include <system/AndroidRuntime.h>
#include <JavaString.h>

using namespace android;

//////////////////////////////////////////////////////////////////////////
// JNIEnv获取器。

JNIEnvGetter::JNIEnvGetter() : mJNIEnv(NULL), mJavaVM(NULL) {}
JNIEnvGetter::~JNIEnvGetter() {
	if (NULL != mJavaVM) {
		mJavaVM->DetachCurrentThread();
	}
}

JNIEnv* JNIEnvGetter::get() {
	if (NULL != mJNIEnv) {
		return mJNIEnv;
	}

	mJNIEnv = AndroidRuntime::getJNIEnv();
	if (NULL != mJNIEnv) {
		return mJNIEnv;
	}

	mJavaVM = AndroidRuntime::getJavaVM();
	if (NULL == mJavaVM) {
		return NULL;
	}

	if (JNI_OK != mJavaVM->AttachCurrentThread(&mJNIEnv, NULL)) {
		return NULL;
	}

	if (NULL == mJNIEnv) {
		mJavaVM->DetachCurrentThread();
		mJavaVM = NULL;
		return NULL;
	} else {
		return mJNIEnv;
	}
}

//////////////////////////////////////////////////////////////////////////
// Object

JObject::JObject() : mJObject(NULL) {}

void JObject::InitJObject(IN jobject obj) {
	ReleaseJObject();
	JNIEnv* env = mJNIEnvGetter.get();
	if (NULL != obj) mJObject = env->NewGlobalRef(obj);
}

// 释放JObject对象。
void JObject::ReleaseJObject() {
	if (NULL != mJObject) {
		JNIEnv* env = mJNIEnvGetter.get();
		env->DeleteGlobalRef(mJObject);
		mJObject = NULL;
	}
}

jobject JObject::GetJObject() {
	return mJObject;
}

JObject::~JObject() {
	if (NULL != mJObject) mJNIEnvGetter.get()->DeleteGlobalRef(mJObject);
}

JClass* JObject::getClass() {
	JClass* obj;
	CallObjectMethod(&obj, "getClass", "()Ljava/lang/Class;");
	return obj;
}

JString* JObject::toString() {
	JString* obj;
	CallObjectMethod(&obj, "toString", "()Ljava/lang/String;");
	return obj;
}

// 调用构造方法。
void JObject::CallConstructorMethod(IN const char* classDesc) {
	CallConstructorMethod(classDesc, "()V");
}

// 调用构造方法。
void JObject::CallConstructorMethod(IN const char* classDesc, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();
	jclass clazz = env->FindClass(classDesc);
	jmethodID id = env->GetMethodID(clazz, "<init>", sig);

	va_list ap;
	va_start(ap, sig);
	jobject result = env->NewObjectV(clazz, id, ap);
	va_end(ap);

	env->DeleteLocalRef(clazz);
	InitJObject(result);
	env->DeleteLocalRef(result);
}

void JObject::CallVoidMethod(IN const char* methodName, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();

	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);

	va_list ap;
	va_start(ap, sig);
	env->CallVoidMethodV(mJObject, id, ap);
	va_end(ap);
}

jboolean JObject::CallBooleanMethod(IN const char* methodName, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();

	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);

	va_list ap;
	va_start(ap, sig);
	jboolean result = env->CallBooleanMethodV(mJObject, id, ap);
	va_end(ap);
	return result;
}

jbyte JObject::CallByteMethod(IN const char* methodName, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();

	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);

	va_list ap;
	va_start(ap, sig);
	jbyte result = env->CallByteMethodV(mJObject, id, ap);
	va_end(ap);
	return result;
}

jshort JObject::CallShortMethod(IN const char* methodName, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();

	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);

	va_list ap;
	va_start(ap, sig);
	jshort result = env->CallShortMethodV(mJObject, id, ap);
	va_end(ap);
	return result;
}

jchar JObject::CallCharMethod(IN const char* methodName, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();
	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);
	va_list ap;
	va_start(ap, sig);
	jchar result = env->CallCharMethodV(mJObject, id, ap);
	va_end(ap);
	return result;
}

jint JObject::CallIntMethod(IN const char* methodName, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();
	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);
	va_list ap;
	va_start(ap, sig);
	jint result = env->CallIntMethodV(mJObject, id, ap);
	va_end(ap);
	return result;
}

jlong JObject::CallLongMethod(IN const char* methodName, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();
	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);
	va_list ap;
	va_start(ap, sig);
	jlong result = env->CallLongMethodV(mJObject, id, ap);
	va_end(ap);
	return result;
}

jfloat JObject::CallFloatMethod(IN const char* methodName, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();
	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);
	va_list ap;
	va_start(ap, sig);
	jfloat result = env->CallFloatMethodV(mJObject, id, ap);
	va_end(ap);
	return result;
}

jdouble JObject::CallDoubleMethod(IN const char* methodName, IN const char* sig, IN ...) {
	JNIEnv* env = mJNIEnvGetter.get();
	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);
	va_list ap;
	va_start(ap, sig);
	jdouble result = env->CallDoubleMethodV(mJObject, id, ap);
	va_end(ap);
	return result;
}

void JObject::CallObjectMethod(const char* methodName, const char* sig, ...) {
	JNIEnv* env = mJNIEnvGetter.get();
	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);

	va_list ap;
	va_start(ap, sig);
	jobject result = env->CallObjectMethodV(mJObject, id, ap);
	va_end(ap);

	env->DeleteLocalRef(result);
}

template<typename T>
void JObject::CallObjectMethod(T** obj, const char* methodName, const char* sig, ...) {
	*obj = NULL;
	JNIEnv* env = mJNIEnvGetter.get();

	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);

	va_list ap;
	va_start(ap, sig);
	jobject result = env->CallObjectMethodV(mJObject, id, ap);
	va_end(ap);

	*obj = new T();
	(*obj)->InitJObject(result);
	env->DeleteLocalRef(result);
}

void JObject::CallObjectMethod(JObject* obj, const char* methodName, const char* sig, ...) {
	JNIEnv* env = mJNIEnvGetter.get();

	jmethodID id = GetMethodID_(env, mJObject, methodName, sig);

	va_list ap;
	va_start(ap, sig);
	jobject result = env->CallObjectMethodV(mJObject, id, ap);
	va_end(ap);

	obj->InitJObject(result);
	env->DeleteLocalRef(result);
}

//////////////////////////////////////////////////////////////////////////
// 

JString* JClass::getName() {
 	JString* obj;
 	CallObjectMethod(&obj, "getName", "()Ljava/lang/String;");
 	return obj;
}

//////////////////////////////////////////////////////////////////////////
// String

// 构造方法。
void JString::New(const char* data) {
	JNIEnv* env = mJNIEnvGetter.get();
	jstring j_str = env->NewStringUTF(data);
	CallConstructorMethod("java/lang/String", "(Ljava/lang/String;)V", j_str);
	env->DeleteLocalRef(j_str);
}

JArray<jchar>* JString::toCharArray() {
	JArray<jchar>* obj;
	CallArrayObjectMethod(&obj, "toCharArray", "()[C");
	return obj;
}
