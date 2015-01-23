#include "stdafx.h"
#include "MyJavaHook.h"

static int g_JniVersion = 0;

JavaHook* g_HookURLInit1 = NULL;
void JNICALL JNI_URL_init(JNIEnv *env, jobject thiz, jstring url) {
	MY_LOG_INFO("[*] enter url init");

	jboolean isCopy;
	const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
	MY_LOG_INFO("[*] url init: %s", stringPtr);

	g_HookURLInit1->CallOldVoidMethod(thiz, url);
}

//////////////////////////////////////////////////////////////////////////
// DexFile

JavaHook* g_HookDexFileInit1 = NULL;
void JNICALL JNI_DexFile_init(JNIEnv *env, jobject thiz, jobject file) {
	MY_LOG_INFO("[*] enter JNI_DexFile_init");
	g_HookDexFileInit1->CallOldVoidMethod(thiz, file);
}

JavaHook* g_HookDexFileInit2 = NULL;
void JNICALL JNI_DexFile_init2(JNIEnv *env, jobject thiz, jstring fileName) {
	MY_LOG_INFO("[*] enter JNI_DexFile_init2");

	jboolean isCopy;
	const char *stringPtr = env->GetStringUTFChars(fileName, &isCopy);
	MY_LOG_INFO("[*] DexFile sourceName=%s", stringPtr);
	env->ReleaseStringUTFChars(fileName, stringPtr);

	g_HookDexFileInit2->CallOldVoidMethod(thiz, fileName);
}

JavaHook* g_HookDexFileInit3 = NULL;
void JNICALL JNI_DexFile_init3(JNIEnv *env, jobject thiz, jstring sourceName,
							   jstring outputName, jint flags) {
								   MY_LOG_INFO("[*] enter JNI_DexFile_init3");

								   jboolean isCopy;
								   const char *strSourceName = env->GetStringUTFChars(sourceName, &isCopy);
								   MY_LOG_INFO("[*] DexFile sourceName=%s", strSourceName);
								   env->ReleaseStringUTFChars(sourceName, strSourceName);

								   g_HookDexFileInit3->CallOldVoidMethod(thiz, sourceName, outputName, flags);
}

//////////////////////////////////////////////////////////////////////////
// ZipFile

JavaHook* g_HookZipFileInit1 = NULL;
void JNICALL JNI_ZipFile_init(JNIEnv *env, jobject thiz, jobject file) {
	MY_LOG_INFO("[*] enter JNI_ZipFile_init");

	jboolean isCopy;

	g_HookZipFileInit1->CallOldVoidMethod(thiz, file);

	// 获得file中的文件路径。
	jclass FileClazz = env->GetObjectClass(file);
	jmethodID getAbsolutePath = env->GetMethodID(FileClazz,
		"getAbsolutePath", "()Ljava/lang/String;");
	jstring filePath = (jstring) env->CallObjectMethod(file,
		getAbsolutePath);
	const char *strFilePath = env->GetStringUTFChars(filePath, &isCopy);
	MY_LOG_INFO("[*] ZipFile strFilePath=%s", strFilePath);
	env->DeleteLocalRef(FileClazz);
	// 拷贝文件。
	//		jclass YoungUtil = g_UserEnv->FindClass("young/android/util/YoungUtil");
	//		if (NULL == YoungUtil) {
	//			MY_LOG_WARNING("未找到young.android.util.YoungUtil类。");
	//		}
	//		jmethodID copyFile = env->GetStaticMethodID(YoungUtil, "copyFile",
	//				"(Ljava/lang/String;Ljava/lang/String;)V");
	//		jstring outputPath = stoJstring(env, "/data/local/tmp/x.apk");
	//		env->CallStaticObjectMethod(YoungUtil, copyFile, filePath, outputPath);
	//
	//		env->DeleteLocalRef(outputPath);
	//		g_UserEnv->DeleteLocalRef(YoungUtil);

	//		char outputPath[PATH_MAX] = {'\0'};

	//		sprintf(cmd, "cp %s %s", strFilePath, "/data/local/tmp/x.apk");
	//		MY_LOG_INFO("cmd = %s", cmd);
	//		system(cmd);

	//if (CopyFile(strFilePath, "/data/data/com.example.hookself/files/x.apk")) {
	//		if (CopyFile(strFilePath, "/data/data/com.example.hookself/x.apk")) {
	//			MY_LOG_INFO("拷贝zip文件成功。");
	//		} else {
	//			MY_LOG_WARNING("拷贝zip文件失败。");
	//		}

	env->ReleaseStringUTFChars(filePath, strFilePath);
}

JavaHook* g_HookZipFileInit2 = NULL;
void JNICALL JNI_ZipFile_init2(JNIEnv *env, jobject thiz, jstring name) {
	MY_LOG_INFO("[*] enter JNI_ZipFile_init2");

	jboolean isCopy;
	const char *strName = env->GetStringUTFChars(name, &isCopy);
	MY_LOG_INFO("[*] ZipFile name=%s", strName);
	env->ReleaseStringUTFChars(name, strName);

	g_HookZipFileInit2->CallOldVoidMethod(thiz, name);
}

JavaHook* g_HookZipFileInit3 = NULL;
void JNICALL JNI_ZipFile_init3(JNIEnv *env, jobject thiz, jstring file,
							   jint mode) {
								   MY_LOG_INFO("[*] enter JNI_ZipFile_init3");
								   g_HookZipFileInit3->CallOldVoidMethod(thiz, file, mode);
}

//////////////////////////////////////////////////////////////////////////

void hook_entry() {
	MY_LOG_INFO("[*] Hook begin");

	JNIEnvGetter envGetter;
	JNIEnv* env = envGetter.get();

	g_JniVersion = env->GetVersion();
	MY_LOG_INFO("[+] jni version=0x%08x", g_JniVersion);

	// hook DexFile
	g_HookDexFileInit1 = new JavaHook(env, "dalvik/system/DexFile", "<init>", "(Ljava/io/File;)V",
		(void*) JNI_DexFile_init);
	g_HookDexFileInit2 = new JavaHook(env, "dalvik/system/DexFile", "<init>",
		"(Ljava/lang/String;)V", (void*) JNI_DexFile_init2);
	g_HookDexFileInit3 = new JavaHook(env, "dalvik/system/DexFile", "<init>", "(Ljava/lang/String;Ljava/lang/String;I)V",
		(void*) JNI_DexFile_init3);

	// hook ZipFile
	g_HookZipFileInit1 = new JavaHook(env, "java/util/zip/ZipFile", "<init>", "(Ljava/io/File;)V",
		(void*) JNI_ZipFile_init);
	g_HookZipFileInit2 = new JavaHook(env, "java/util/zip/ZipFile", "<init>",
		"(Ljava/lang/String;)V", (void*) JNI_ZipFile_init2);
	g_HookZipFileInit3 = new JavaHook(env, "java/util/zip/ZipFile", "<init>", "(Ljava/io/File;I)V",
		(void*) JNI_ZipFile_init3);

	//产生的异常需要手工清除，异常产生原因是找不到类的静态方法

	MY_LOG_INFO("[*] Hook end");
}

void MyJavaHook() {
	hook_entry();
}
