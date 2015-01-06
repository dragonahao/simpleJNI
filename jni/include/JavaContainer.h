#ifndef _JAVA_CONTAINER_H_
#define _JAVA_CONTAINER_H_

#include <jni.h>
#include "JavaBase.h"

//#include <typeinfo>

//////////////////////////////////////////////////////////////////////////
// List

#define JListSuper JObject

template<typename T>
class JList : public JListSuper {
public:
	void add(jint location, T& object) {
		return CallVoidMethod("add", "(ILjava/lang/Object;)V", location, object.GetJObject());
	}

	jboolean add(T& object) {
		return CallBooleanMethod("add", "(Ljava/lang/Object;)Z", object.GetJObject());
	}

	void clear() {
		return CallVoidMethod("clear", "()V");
	}

	jboolean contains(JObject& object) {
		return CallBooleanMethod("contains", "(Ljava/lang/Object;)Z", object.GetJObject());
	}

	/**
	 * 获得列表中的元素。
	 * @param[in] location 位置。
	 * @return 返回列表中的元素，此元素使用完成后需要释放内存。
	 */
	T* get(jint location) {
		T* t = new T;
		CallObjectMethod(t, "get", "(I)Ljava/lang/Object;", location);
		return t;
	}

	jint indexOf(JObject& object) {
		return CallIntMethod("indexOf", "(Ljava/lang/Object;)I", object.GetJObject());
	}

	jboolean isEmpty() {
		return CallBooleanMethod("isEmpty", "()Z");
	}

	/**
	 * 删除列表中的元素。
	 * @param[in] location 位置。
	 * @return 返回删除的元素，此元素使用完成后需要释放内存。
	 */
	T* remove(jint location) {
		T* t = new T;
		CallObjectMethod(t, "remove", "(I)Ljava/lang/Object;", location);
		return t;
	}

	jboolean remove(JObject& object) {
		return CallBooleanMethod("remove", "(Ljava/lang/Object;)Z", object.GetJObject());
	}

	/**
	 * @return 返回的对象需要释放内存。
	 */
	T* set(jint location, T& object) {
		T* t = new T;
		CallObjectMethod(t, "set", "(ILjava/lang/Object;)Ljava/lang/Object;", location, object.GetJObject());
		return t;
	}

	jint size() {
		return CallIntMethod("size", "()I");
	}
};

//////////////////////////////////////////////////////////////////////////
// ArrayList

#define JArrayListSuper JList<T>

template<typename T>
class JArrayList : public JArrayListSuper {
public:
	void New() {
		JArrayListSuper::CallConstructorMethod("java/util/ArrayList");
	}
};

#endif