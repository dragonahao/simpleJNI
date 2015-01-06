#include "stdafx.h"
#include <JavaContainer.h>
#include <string.h>
#include <JavaString.h>

//////////////////////////////////////////////////////////////////////////
// StringBuilder(这个类不是线程安全的)

void JStringBuilder::New() {
	CallConstructorMethod("java/lang/StringBuilder");
}

JStringBuilder* JStringBuilder::append(jboolean b) {
	CallObjectMethod("append", "(Z)Ljava/lang/StringBuilder;", b);
	return this;
}

JStringBuilder* JStringBuilder::append(jchar c) {
	CallObjectMethod("append", "(C)Ljava/lang/StringBuilder;", c);
	return this;
}

JStringBuilder* JStringBuilder::append(jint i) {
	CallObjectMethod("append", "(I)Ljava/lang/StringBuilder;", i);
	return this;
}

JStringBuilder* JStringBuilder::append(jlong l) {
	CallObjectMethod("append", "(J)Ljava/lang/StringBuilder;", l);
	return this;
}

JStringBuilder* JStringBuilder::append(jfloat f) {
	CallObjectMethod("append", "(F)Ljava/lang/StringBuilder;", f);
	return this;
}

JStringBuilder* JStringBuilder::append(jdouble d) {
	CallObjectMethod("append", "(D)Ljava/lang/StringBuilder;", d);
	return this;
}

JStringBuilder* JStringBuilder::append(jobject obj) {
	CallObjectMethod("append", "(Ljava/lang/Object;)Ljava/lang/StringBuilder;", obj);
	return this;
}

// 追加字符串。
JStringBuilder* JStringBuilder::append(const char* str) {
	JNIEnv* env = mJNIEnvGetter.get();
	jstring j_str = env->NewStringUTF(str);
	CallObjectMethod("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;", j_str);
	env->DeleteLocalRef(j_str);
	return this;
}

JStringBuilder* JStringBuilder::append(JString* str) {
	CallObjectMethod("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;", str->GetJObject());
	return this;
}

JStringBuilder* JStringBuilder::append(JString& str) {
	CallObjectMethod("append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;", str.GetJObject());
	return this;
}

JStringBuilder* JStringBuilder::append(JStringBuilder* str) {
	JString* tmp = str->toString();
	append(tmp);
	delete tmp;
	return this;
}

JStringBuilder* JStringBuilder::append(JStringBuilder& str) {
	JString* tmp = str.toString();
	append(tmp);
	delete tmp;
	return this;
}
