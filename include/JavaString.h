#ifndef _JAVA_STRING_H_
#define _JAVA_STRING_H_

#include "JavaContainer.h"

class JObject;

//namespace java_lang {

//}

//////////////////////////////////////////////////////////////////////////
// AbstractStringBuilder

#define JAbstractStringBuilderSuper JObject

class AbstractStringBuilder : public JAbstractStringBuilderSuper {
public:
};

//////////////////////////////////////////////////////////////////////////
// StringBuilder(这个类不是线程安全的)

#define JStringBuilderSuper AbstractStringBuilder

class JStringBuilder : public JStringBuilderSuper {
public:
	void New();

	JStringBuilder* append(jboolean b);
	JStringBuilder* append(jchar b);
	JStringBuilder* append(jint b);
	JStringBuilder* append(jlong l);
	JStringBuilder* append(jfloat l);
	JStringBuilder* append(jdouble l);
	JStringBuilder* append(jobject obj);

	/**
	 * 追加字符串。
	 * @param[in] str 字符串。
	 * @return 返回this，所以返回值不需要释放。
	 */
	JStringBuilder* append(const char* str);
	JStringBuilder* append(JString* str);
	JStringBuilder* append(JString& str);
	JStringBuilder* append(JStringBuilder* str);
	JStringBuilder* append(JStringBuilder& str);
};

#endif