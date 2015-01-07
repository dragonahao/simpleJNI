#pragma once

#include <system/Object_4.h>

class JavaHook {
private:
	JNIEnv* mEnv;
	void* mFunc;
	Method* mOrigMethod;
	Method* mNewMethod;
	//char mReturnShorty;
	jclass mClass;
	char* mMethodSig;

public:
	/**
	 * @param[in] 
	 * @param[in] 
	 * @param[in] 
	 * @param[in] 
	 */
	JavaHook(JNIEnv* env, const char* classDesc, const char* methodName, const char* methodSig);
	~JavaHook();

	void Hook(void* func);

	/**
	 * @param[in] 
	 * @param[in] 
	 */
	void CallOldVoidMethod(jobject thiz, ...);

	/**
	 * @param[in] 
	 * @param[in] 
	 */
	jboolean CallOldBooleanMethod(jobject thiz, ...);

	jbyte CallOldByteMethod(jobject thiz, ...);

	jshort CallOldShortMethod(jobject thiz, ...);

	jchar CallOldCharMethod(jobject thiz, ...);

	jint CallOldIntMethod(jobject thiz, ...);

	jlong CallOldLongMethod(jobject thiz, ...);

	jfloat CallOldFloatMethod(jobject thiz, ...);

	jdouble CallOldDoubleMethod(jobject thiz, ...);

	//////////////////////////////////////////////////////////////////////////

	void CallOldStaticVoidMethod(...);

	jboolean CallOldStaticBooleanMethod(...);

	jbyte CallOldStaticByteMethod(...);

	jshort CallOldStaticShortMethod(...);

	jchar CallOldStaticCharMethod(...);

	jint CallOldStaticIntMethod(...);

	jlong CallOldStaticLongMethod(...);

	jfloat CallOldStaticFloatMethod(...);

	jdouble CallOldStaticDoubleMethod(...);

private:
	/**
	 * 克隆方法结构体。
	 */
	void CloneMethod();

	void ModifyMethod();
};
