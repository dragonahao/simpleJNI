#include <jni.h>
#include <JavaBase.h>
#include <AndroidContent.h>
#include <SmartPoint.h>

jstring stringFromJNI( JNIEnv* env, jobject thiz )
{
#if defined(__arm__)
  #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a/NEON (hard-float)"
      #else
        #define ABI "armeabi-v7a/NEON"
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a (hard-float)"
      #else
        #define ABI "armeabi-v7a"
      #endif
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__i386__)
   #define ABI "x86"
#elif defined(__x86_64__)
   #define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
   #define ABI "mips64"
#elif defined(__mips__)
   #define ABI "mips"
#elif defined(__aarch64__)
   #define ABI "arm64-v8a"
#else
   #define ABI "unknown"
#endif

    return env->NewStringUTF("Hello from JNI !  Compiled with ABI " ABI ".");
}

/**
 * 将assets中的文件拷贝出来。
 * 
 */
void copyFormAssets(JNIEnv *env, jobject thiz, jstring jfileName, jstring joutPath) {
	// 上下文。
	JContext context;
	context.InitJObject(thiz);

	// assets目录中的文件名。
	JString fileName;
	fileName.InitJObject(jfileName);

	// 输出目录。
	JString outPath;
	outPath.InitJObject(joutPath);

	JFile fileOutPath;
	fileOutPath.New(outPath);

	// AssetManager。
	lsp<JAssetManager> am = context.getAssets();
	lsp<JInputStream> is = am->open(fileName);

	JFile dir;
	dir.New(*(lsp<JString>(fileOutPath.getParent()).get()));
	// 如果目录不存在，则创建。
	if (false == dir.exists()) {
		dir.mkdirs();
	}

	JFileOutputStream os;
	os.New(outPath);

	JArray<jbyte> buffer;
	buffer.New(1024);

	// 写文件。
	jint length;
	while ((length = is->read(buffer)) > 0) {
		os.write(buffer, 0, length);
	}

	is->close();
	os.close();
}

const char* g_ClassName = "com/buwai/androidjnilibrarysample/MainActivity";

static const JNINativeMethod gMethods[] = {
	{ "copyFormAssets", "(Ljava/lang/String;Ljava/lang/String;)V", (void*)copyFormAssets },
	{ "stringFromJNI", "()Ljava/lang/String;", (void*)stringFromJNI}
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
