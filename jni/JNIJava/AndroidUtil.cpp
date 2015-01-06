#include "stdafx.h"
#include <JNIReflect.h>
#include <AndroidUtil.h>

jint JLog::xxx(const char* methodName, JString& tag, JString& msg) {
	JNIEnvGetter jniEnvGetter;
	return CallStaticIntMethod_(jniEnvGetter.get(), "android/util/Log", methodName, "(Ljava/lang/String;Ljava/lang/String;)I", tag.GetJObject(), msg.GetJObject());
}

jint JLog::v(JString& tag, JString& msg) {
	return xxx("v", tag, msg);
}

jint JLog::d(JString& tag, JString& msg) {
	return xxx("d", tag, msg);
}

jint JLog::i(JString& tag, JString& msg) {
	return xxx("i", tag, msg);
}

jint JLog::w(JString& tag, JString& msg) {
	return xxx("w", tag, msg);
}

jint JLog::e(JString& tag, JString& msg) {
	return xxx("e", tag, msg);
}
