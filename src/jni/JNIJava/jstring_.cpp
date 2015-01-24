#include "stdafx.h"
#include "jstring_.h"

#include <system/AndroidRuntime.h>
#include <JavaException.h>
#include <JNIReflect.h>

#include <assert.h>

using namespace android;

jstring_::jstring_() {
	init();
}

jstring_::jstring_(const char* strUtf) {
	init();
	if (NULL != strUtf) {
		bool bRet = GetJString(strUtf);
		if (false == bRet) {
			ThrowRuntime(mJNIEnvGetter.get(), "convert char* to jstring failed");
		}
	}
}

// jstring_::jstring_(const jstring jstr) {
// 	MY_LOG_INFO("jstring_::jstring_(jstring jstr)");
// 	init();
// 	if (NULL != jstr) {
// 		SetJString(jstr);
// 	}
// }

// jstring_::jstring_(jstring_& jstr_) {
//  	MY_LOG_INFO("jstring_::jstring_(jstring_* jstr_)");
// // 	init();
// // 	jstring_& js_ = (jstring_&) jstr_;
// // 	jstring jstr = (jstring)(js_.GetJString());
// // 	SetJString(jstr);
// // 	SetJString(jstr);
// }

jstring_::~jstring_() {
	//MY_LOG_INFO("jstring_::~jstring_() - this=%p", this);
	if (mIsInitSuc) {
		reset();
	}
}

// 重载赋值运算符。
jstring_& jstring_::operator=(const char* s) {
	reset();
	if (NULL != s) {
		bool bRet = GetJString(s);
		if (false == bRet) {
			ThrowRuntime(mJNIEnvGetter.get(), "convert char* to jstring failed");
		}
	}
	
	return *this;
}
// 
// // 重载赋值运算符。
// jstring_& jstring_::operator=(const jstring s) {
// 	MY_LOG_INFO("jstring_& jstring_::operator=(const jstring& s)");
// 	reset();
// 	SetJString(s);
// 	return *this;
// }

// jstring_& jstring_::operator=(const jstring_&  s_) {
// 	MY_LOG_INFO("jstring_& jstring_::operator=(const jstring_& s)");
// 	reset();
// 	jstring_& js_ = (jstring_&) s_;
// 	jstring jstr = (jstring)(js_.GetJString());
// 	SetJString(jstr);
// 	return *this;
// }

// 初始化。
void jstring_::init() {
	//MY_LOG_INFO("jstring_::init()");
	mIsInitSuc = false;

	ZeroMemory(&mJString, sizeof(mJString));

	mChars.charsUtf = NULL;
	mChars.chars = NULL;

	mIsInitSuc = true;
}

// 重置。
void jstring_::reset() {
	//MY_LOG_INFO("jstring_::reset0, mJString.jstr=%p, mChars.charsUtf=%p", mJString.jstr, mChars.charsUtf);
	if (NULL != mChars.charsUtf) {
		mJNIEnvGetter.get()->ReleaseStringUTFChars(mJString.jstr, mChars.charsUtf);
		mChars.charsUtf = NULL;
	}
	//MY_LOG_INFO("jstring_::reset1, mChars.chars=%p", mChars.chars);
	if (NULL != mChars.chars) {
		mJNIEnvGetter.get()->ReleaseStringChars(mJString.jstr, mChars.chars);
		mChars.chars = NULL;
	}

	//MY_LOG_INFO("jstring_::reset2");
	if (mJString.isAlloc) {
		//MY_LOG_INFO("jstring_::reset2.1");
		mJNIEnvGetter.get()->DeleteGlobalRef(mJString.jstr);
	}

	//MY_LOG_INFO("jstring_::reset() - Length=%d, mJString.jstr=%p, mJString.isAlloc=%d", Length(), mJString.jstr, mJString.isAlloc);
	ZeroMemory(&mJString, sizeof(mJString));
	//MY_LOG_INFO("jstring_::reset() - mJString.jstr=%p, mJString.isAlloc=%d", mJString.jstr, mJString.isAlloc);
}

// 初始化是否成功。
bool jstring_::IsInitSuccess() {
	return mIsInitSuc;
}

const jstring jstring_::GetJString() {
	if (false == mIsInitSuc)
		return NULL;

	return mJString.jstr;
}

// 获得jstring类型的字符串。
bool jstring_::GetJString(const char* s) {
	if (false == mIsInitSuc)
		return false;

	if (NULL != mJString.jstr)
		return true;

	JNIEnv* env = mJNIEnvGetter.get();
	bool bRet = false;

	jclass clazz = env->FindClass("java/lang/String");
	if (NULL == clazz) {
		return false;
	}

	jmethodID methodID;
	jobject jstr;

	int len = strlen(s);
	if (0 == len) {
		methodID = GetMethodID_(env, "java/lang/String", "<init>", "()V");
		jstr = env->NewObject(clazz, methodID);
		if (NULL == jstr) {
			return false;
		} else {
			mJString.jstr = (jstring) env->NewGlobalRef(jstr);
			env->DeleteLocalRef(jstr);
			if (NULL != mJString.jstr) {
				bRet = true;
			}
		}
	} else {
		jbyteArray bytes = NULL;
		jstring encoding = NULL;

		methodID = GetMethodID_(env, "java/lang/String", "<init>", "([BLjava/lang/String;)V");
		bytes = env->NewByteArray(len);
		if (NULL == bytes)
			goto _ret;

		env->SetByteArrayRegion(bytes, 0, len, (jbyte*) s);
		encoding = env->NewStringUTF("utf-8");
		if (NULL == encoding)
			goto _ret;

//		mJString.jstr = (jstring) mJNIEnvGetter.get()->NewObject(clazz, methodID, bytes,
//				encoding);
		jstr = env->NewObject(clazz, methodID, bytes, encoding);
		if (NULL != jstr) {
			if (SetJString((jstring)jstr)) {
				bRet = true;
			} else {
				MY_LOG_WARNING("[-]jstring_::GetJString - SetJString()失败。");
			}
			env->DeleteLocalRef(jstr);
		}

	_ret:
		if (NULL != bytes)
			env->DeleteLocalRef(bytes);
		if (NULL != encoding)
			env->DeleteLocalRef(encoding);
	}

	env->DeleteLocalRef(clazz);
	return bRet;
}

// 设置字符串。
bool jstring_::SetJString(jstring jstr) {
	mJString.jstr = (jstring) mJNIEnvGetter.get()->NewGlobalRef(jstr);
	//MY_LOG_INFO("jstring_::SetJString - mJString.jstr=%p, jstr=%p", mJString.jstr, jstr);
	if (NULL != mJString.jstr) {
		mJString.isAlloc = true;
		return true;
	} else {
		return false;
	}
}

// 获得char类型的字符串。
const char* jstring_::GetString() {
	if (false == mIsInitSuc)
		return NULL;

	if (NULL != mChars.charsUtf)
		return mChars.charsUtf;

	if (NULL == mJString.jstr) {
		return NULL;
	}

	//MY_LOG_INFO("jstring_::GetString()1, mJString.jstr=%p", mJString.jstr);
	mChars.charsUtf = mJNIEnvGetter.get()->GetStringUTFChars(mJString.jstr, NULL);
	if (NULL == mChars.charsUtf) {
		return NULL;
	}
	return mChars.charsUtf;
}

void jstring_::SetString(const jstring jstr) {
	reset();
	SetJString(jstr);
}

// 获得字符串的字符个数。
int jstring_::Length() {
	//MY_LOG_INFO("jstring_::Length - this=%p", this);
	const char* str = GetString();
	if (NULL == str) {
		return 0;
	} else {
		return strlen(str);
	}
}
