#include "stdafx.h"
#include "MyJavaHook.h"

#define HOOK_FUNCTION(funName) MSHookFunction((void*) funName, (void*) my##funName, (void**) (&old##funName))

FILE * (*oldfopen)(const char * fileName, const char * mode) = NULL;

FILE * myfopen(const char * fileName, const char * mode) {
	MY_LOG_INFO("[*] native hook - fopen函数：文件路径：%s，模式：%s", fileName, mode);
	return oldfopen(fileName, mode);
}

// Hook Native层。
void nativeHooks() {
	MY_LOG_INFO("#---------- Start nativeHooks ---------------#");
 	HOOK_FUNCTION(fopen);
	MY_LOG_INFO("#---------- End nativeHooks ---------------#");
}

/**
 * hook。
 */
void hooks(JNIEnv *env, jobject thiz) {
	MyJavaHook();
	nativeHooks();
}

/**
 * 调用fopen。
 * @param[in] fpath 文件路径。
 */
void call_fopen(JNIEnv *env, jobject thiz, jstring fpath) {
	jstring_ j_fpath;
	j_fpath.SetString(fpath);
	FILE* pf = fopen(j_fpath.GetString(), "r");
	if (NULL == pf) {
		MY_LOG_INFO("打开文件失败：%s。");
	} else {
		fclose(pf);
	}
}

const char* g_ClassName = "buwai/sample/simplejni/hook/MainActivity";

static const JNINativeMethod gMethods[] = {
	{ "hooks", "()V", (void*)hooks }, 
	{ "call_fopen", "(Ljava/lang/String;)V", (void*)call_fopen }
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
