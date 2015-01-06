#ifndef _JAVA_BASE_H_
#define _JAVA_BASE_H_

#include <jni.h>
#include <stddef.h>
#include <typeinfo>
#include "CommDef.h"
#include "JNIReflect.h"

//////////////////////////////////////////////////////////////////////////
// 

class JObject;
class JClass;
class JString;
template<typename T>
class JArray;

//////////////////////////////////////////////////////////////////////////
// JNIEnv获取器。

class JNIEnvGetter {
public:
	JNIEnvGetter();
	~JNIEnvGetter();

	JNIEnv* get();

private:
	JNIEnv* mJNIEnv;
	JavaVM* mJavaVM;
};

//////////////////////////////////////////////////////////////////////////
// Object

class JObject {
public:
	JObject();
	virtual ~JObject();

	void InitJObject(IN jobject obj);
	jobject GetJObject();

	JClass* getClass();

	JString* toString();

protected:
	JNIEnvGetter mJNIEnvGetter;
	jobject mJObject;

	// 释放JObject对象。
	void ReleaseJObject();

	/**
	 * 调用构造方法。
	 * @param[in] classDesc 类名。
	 */
	void CallConstructorMethod(IN const char* classDesc);

	/**
	 * 调用构造方法。
	 * @param[in] classDesc 类名。
	 * @param[in] sig 方法签名。
	 * @param[in] ... 方法参数。
	 */
	void CallConstructorMethod(IN const char* classDesc, IN const char* sig, IN ...);

	void CallVoidMethod(IN const char* methodName, IN const char* sig, IN ...);
	jboolean CallBooleanMethod(IN const char* methodName, IN const char* sig, IN ...);
	jbyte CallByteMethod(IN const char* methodName, IN const char* sig, IN ...);
	jshort CallShortMethod(IN const char* methodName, IN const char* sig, IN ...);
	jchar CallCharMethod(IN const char* methodName, IN const char* sig, IN ...);
	jint CallIntMethod(IN const char* methodName, IN const char* sig, IN ...);
	jlong CallLongMethod(IN const char* methodName, IN const char* sig, IN ...);
	jfloat CallFloatMethod(IN const char* methodName, IN const char* sig, IN ...);
	jdouble CallDoubleMethod(IN const char* methodName, IN const char* sig, IN ...);

	/**
	 * 调用返回对象的方法。
	 * @param[in] methodName 方法名。
	 * @param[in] sig 方法签名。
	 * @param[in] ... 方法参数。
	 * @note 此函数不需要方法的返回值。
	 */
	void CallObjectMethod(const char* methodName, const char* sig, ...);

	/**
	 * 调用返回对象的方法。
	 * @param[out] obj 返回对象。此对象需要释放内存。
	 * @param[in] methodName 方法名。
	 * @param[in] sig 方法签名。
	 * @param[in] ... 方法参数。
	 */
	template<typename T>
	void CallObjectMethod(T** obj, const char* methodName, const char* sig, ...);

	void CallObjectMethod(JObject* obj, const char* methodName, const char* sig, ...);

	template<typename T>
	//void CallArrayObjectMethod(T** obj, const char* methodName, const char* sig, ...);
	void CallArrayObjectMethod(JArray<T>** obj, const char* methodName, const char* sig, ...) {
		*obj = NULL;
		JNIEnv* env = mJNIEnvGetter.get();

		jmethodID id = GetMethodID_(env, mJObject, methodName, sig);

		va_list ap;
		va_start(ap, sig);
		jobject result = env->CallObjectMethodV(mJObject, id, ap);
		va_end(ap);

		*obj = new JArray<T>();
		(*obj)->InitJObject((jarray)result);
		env->DeleteLocalRef(result);
	}

// 	template<typename T>
// 	void CallObjectMethod(JArray<T>** obj, const char* methodName, const char* sig, ...)  {
// 		*obj = NULL;
// 		JNIEnv* env = mJNIEnvGetter.get();
// 
// 		jmethodID id = GetMethodID_(env, mJObject, methodName, sig);
// 
// 		va_list ap;
// 		va_start(ap, sig);
// 		jobject result = env->CallObjectMethodV(mJObject, id, ap);
// 		va_end(ap);
// 
// // 		*obj = new T();
// // 		(*obj)->InitJObject(result);
// // 		env->DeleteLocalRef(result);
// 	}
};

//////////////////////////////////////////////////////////////////////////

#define JClassSuper JObject

class JClass : public JClassSuper {
public:
	JString* getName();
};

//////////////////////////////////////////////////////////////////////////
// String

#define JStringSuper JObject

class JString : public JStringSuper {
public:
	/**
	 * 构造方法。
	 * @param[in] data 字符串。
	 */
	void New(const char* data);

	JArray<jchar>* toCharArray();
};

//////////////////////////////////////////////////////////////////////////
// Java数组。

#define JArraySuper JObject

template<typename T>
class JArray : public JArraySuper {
public:
	JArray() : JArraySuper(), mArray(NULL) {}

	virtual ~JArray() {
		ReleaseJArray();
	}

	/**
	 * 获得数组长度。
	 * @return 返回数组长度。
	 */
	int GetFlength() {
		return mJNIEnvGetter.get()->GetArrayLength((jarray)mJObject);
	}

	/**
	 * 初始化数组对象。
	 * @param[in] arr 数组对象。 
	 * @param[in] type 数组的类型。
	 *				Z: boolean. B: byte. S: short. C: char. I: int. J: lang. F: float. double: D. L: Object.
	 */
	void InitJObject(IN jarray arr) {
		ReleaseJArray();
		JArraySuper::InitJObject(arr);
		if (NULL == arr) {
			return;
		}

		JNIEnv* env = mJNIEnvGetter.get();

		if (typeid(T) == typeid(jboolean)) {
			mArray = (T*)env->GetBooleanArrayElements((jbooleanArray)arr, NULL);
		} else if (typeid(T) == typeid(jbyte)) {
			mArray = (T*)env->GetByteArrayElements((jbyteArray)arr, NULL);
		} else if (typeid(T) == typeid(jshort)) {
			mArray = (T*)env->GetShortArrayElements((jshortArray)arr, NULL);
		} else if (typeid(T) == typeid(jchar)) {
			mArray = (T*)env->GetCharArrayElements((jcharArray)arr, NULL);
		} else if (typeid(T) == typeid(jint)) {
			mArray = (T*)env->GetIntArrayElements((jintArray)arr, NULL);
		} else if (typeid(T) == typeid(jlong)) {
			mArray = (T*)env->GetLongArrayElements((jlongArray)arr, NULL);
		} else if (typeid(T) == typeid(jfloat)) {
			mArray = (T*)env->GetFloatArrayElements((jfloatArray)arr, NULL);
		} else if (typeid(T) == typeid(jdouble)) {
			mArray = (T*)env->GetDoubleArrayElements((jdoubleArray)arr, NULL);
		} else {
			// Object对象。
			// 这个函数不管Object对象的事儿。
		}
	}

	/**
	 * 获得数组元素。
	 * @param[in] index 索引。
	 * @return 返回数组元素。
	 * @note 无法获得对象类型的数组元素，如果要获得对象类型的数组元素，需要调用T* GetObject(int index)函数。
	 */
	T &operator[] (int index) {
// 		if ('L' != mArrayType) {
// 			return mArray[index];
// 		}
		return mArray[index];
	}

	/**
	 * 获得对象类型的数组元素。
	 * @param[in] index 索引。
	 * @return 返回对象类型的数组元素。
	 * @note 专用于获得对象类型的数组元素。
	 */
	T* GetObjectElement(int index) {
		JNIEnv* env = mJNIEnvGetter.get();
		jobject obj = env->GetObjectArrayElement((jobjectArray)mJObject, index);
		T* t = new T;
		t->InitJObject(obj);
		env->DeleteLocalRef(obj);
		return t;
	}

	/**
	 * 创建一个数组。
	 * @param[in] len 数组长度。
	 */
	void New(IN jsize len) {
		ReleaseJArray();
		jarray arr = NULL;
		JNIEnv* env = mJNIEnvGetter.get();
		if (typeid(T) == typeid(jboolean)) {
			arr = env->NewBooleanArray(len);
			mArray = (T*)env->GetBooleanArrayElements((jbooleanArray)arr, NULL);
		} else if (typeid(T) == typeid(jbyte)) {
			arr = env->NewByteArray(len);
			mArray = (T*)env->GetByteArrayElements((jbyteArray)arr, NULL);
		} else if (typeid(T) == typeid(jshort)) {
			arr = env->NewShortArray(len);
			mArray = (T*)env->GetShortArrayElements((jshortArray)arr, NULL);
		} else if (typeid(T) == typeid(jchar)) {
			arr = env->NewCharArray(len);
			mArray = (T*)env->GetCharArrayElements((jcharArray)arr, NULL);
		} else if (typeid(T) == typeid(jint)) {
			arr = env->NewIntArray(len);
			mArray = (T*)env->GetIntArrayElements((jintArray)arr, NULL);
		} else if (typeid(T) == typeid(jlong)) {
			arr = env->NewLongArray(len);
			mArray = (T*)env->GetLongArrayElements((jlongArray)arr, NULL);
		} else if (typeid(T) == typeid(jfloat)) {
			arr = env->NewFloatArray(len);
			mArray = (T*)env->GetFloatArrayElements((jfloatArray)arr, NULL);
		} else if (typeid(T) == typeid(jdouble)) {
			arr = env->NewDoubleArray(len);
			mArray = (T*)env->GetDoubleArrayElements((jdoubleArray)arr, NULL);
		} else {
			// Object对象。
			// 这个函数不管Object对象的事儿。
		}

		JArraySuper::InitJObject(arr);
		env->DeleteLocalRef(arr);
	}

	void New(IN const T arr[], IN jsize len) {
		JNIEnv* env = mJNIEnvGetter.get();
		New(len);
		env->SetBooleanArrayRegion((jbooleanArray)mJObject, 0, len, (const jboolean*)arr);
	}

	void NewObjArray(IN jsize len, IN const char* className) {
		ReleaseJArray();
		JNIEnv* env = mJNIEnvGetter.get();
		jclass clazz = env->FindClass(className);
		jobjectArray objArray = env->NewObjectArray(len, clazz, NULL);
		JArraySuper::InitJObject(objArray);
		env->DeleteLocalRef(objArray);
	}

protected:
	T* mArray;

	void ReleaseJArray() {
		if (NULL != mArray) {
			JNIEnv* env = mJNIEnvGetter.get();

			if (typeid(T) == typeid(jboolean)) {
				env->ReleaseBooleanArrayElements((jbooleanArray)mJObject, (jboolean*)mArray, 0);
			} else if (typeid(T) == typeid(jbyte)) {
				env->ReleaseByteArrayElements((jbyteArray)mJObject, (jbyte*)mArray, 0);
			} else if (typeid(T) == typeid(jshort)) {
				env->ReleaseShortArrayElements((jshortArray)mJObject, (jshort*)mArray, 0);
			} else if (typeid(T) == typeid(jchar)) {
				env->ReleaseCharArrayElements((jcharArray)mJObject, (jchar*)mArray, 0);
			} else if (typeid(T) == typeid(jint)) {
				env->ReleaseIntArrayElements((jintArray)mJObject, (jint*)mArray, 0);
			} else if (typeid(T) == typeid(jlong)) {
				env->ReleaseLongArrayElements((jlongArray)mJObject, (jlong*)mArray, 0);
			} else if (typeid(T) == typeid(jfloat)) {
				env->ReleaseFloatArrayElements((jfloatArray)mJObject, (jfloat*)mArray, 0);
			} else if (typeid(T) == typeid(jdouble)) {
				env->ReleaseDoubleArrayElements((jdoubleArray)mJObject, (jdouble*)mArray, 0);
			} else {
				// Object这个不需要释放。
			}

			mArray = NULL;

			JArraySuper::ReleaseJObject();
		}
	}
};

#endif