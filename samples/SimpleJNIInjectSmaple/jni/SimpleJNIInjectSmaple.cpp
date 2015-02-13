#include <jni.h>
#include <SmartPoint.h>
#include <nativelog.h>
#include <string.h>
//#include <hack/Inject.h>

const char* g_ClassName = "buwai/sample/simplejni/inject/MainActivity";

class Test {
public:
	lsp<char*> str;
	lsp<int> p_i;
};

jboolean inject(JNIEnv *env, jobject thiz, jint pid) {
	//return Inject(pid, NULL);
	Test test;

	MY_LOG_INFO("count: %d", test.str.getCount());

// 	char* str = (char*) calloc(strlen("123456") + 1, sizeof(char));
// 	strcpy(str, "123456");
// 	test.str = str;
// 
// 	MY_LOG_INFO("str: %s, count: %d", test.str.get(), test.str.getCount());

	//test.p_i = 1;
	lsp<int> a(1);
	return false;
}

static const JNINativeMethod gMethods[] = {
	{ "inject", "(I)Z", (void*)inject }
};

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
	jint result = -1;
	JNIEnv* env;
	result = vm->GetEnv((void **) &env, JNI_VERSION_1_4);
	if (result != JNI_OK) {
		return -1;
	}

	jclass clazz;
	clazz = env->FindClass(g_ClassName);
	if (clazz == NULL) {
		return -1;
	}

	if (env->RegisterNatives(clazz, gMethods,
		sizeof(gMethods) / sizeof(gMethods[0])) != JNI_OK) {
			return -1;
	}
	result = JNI_VERSION_1_4;
	return result;
}
