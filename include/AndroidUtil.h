#ifndef _ANDROID_UTIL_H_
#define _ANDROID_UTIL_H_

#include "JavaString.h"

//#define JLogSuper JObject

class JLog {
public:
	static jint v(JString& tag, JString& msg);
	static jint d(JString& tag, JString& msg);
	static jint i(JString& tag, JString& msg);
	static jint w(JString& tag, JString& msg);
	static jint e(JString& tag, JString& msg);

private:
	static jint xxx(const char* methodName, JString& tag, JString& msg);
};

#endif