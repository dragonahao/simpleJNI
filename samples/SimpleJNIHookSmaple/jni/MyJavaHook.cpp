#include "stdafx.h"
#include "MyJavaHook.h"

//////////////////////////////////////////////////////////////////////////
// DexClassLoader

JavaHook* g_HookDexClassLoaderInit = NULL;
void JNICALL JNI_DexClassLoader_init(JNIEnv *env, jobject thiz, jstring dexPath, jstring optimizedDirectory, jstring libraryPath, jobject parent) {
	MY_LOG_INFO("[*] Java Hook - DexClassLoader的构造函数。");
	g_HookDexClassLoaderInit->CallOldVoidMethod(thiz, dexPath, optimizedDirectory, libraryPath, parent);
}

//////////////////////////////////////////////////////////////////////////

void MyJavaHook() {
	MY_LOG_INFO("[*] Hook begin");

	JNIEnvGetter envGetter;
	JNIEnv* env = envGetter.get();

	// hook DexFile
	g_HookDexClassLoaderInit = new JavaHook(env, "dalvik/system/DexClassLoader", "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
	g_HookDexClassLoaderInit->SetHookMethod((void*) JNI_DexClassLoader_init);
	g_HookDexClassLoaderInit->Hook();

	MY_LOG_INFO("[*] Hook end");
}
