/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h> 
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <elf.h>
#include <assert.h>
#include <jni.h>
#include <pthread.h>

#include <android/log.h>
#include "../buwaindk/android_runtime/AndroidRuntime.h"

#include "JavaHook.h"
#include "JavaDump.h"
#include "JniUtil.h"
#include "hlog.h"

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

using namespace android;

static int jniVersion = 0;
static JavaVM* javaVM = NULL;

static char processName[256];
static char* processNamePtr = NULL;




static const char JSTRING[] = "Ljava/lang/String;";
static const char JCLASS_LOADER[] = "Ljava/lang/ClassLoader;";
static const char JCLASS[] = "Ljava/lang/Class;";

static const int PROCESS_SYSTEM_SERVER = 0;
static const int PROCESS_PHONE_SERVER = 1;
static const int PROCESS_MEDIA_SERVER = 2;




static Method* orgURLinitMethod = NULL;
static Method* bakURLinitMethod = NULL;
void JNICALL JNI_URL_init(JNIEnv *env, jobject thiz, jstring url) {
	HLOGI("[*] enter url init\n");

	if(orgURLinitMethod && bakURLinitMethod) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
		HLOGI("[*] url init: %s",stringPtr);

		SwapMethod(orgURLinitMethod, bakURLinitMethod);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");


	//	jstring url2 = env->NewStringUTF("http://www.baidu.com");
	//	const char *stringPtr2 = env->GetStringUTFChars(url2, &isCopy);
	//	HLOGI("[*] modify url: %s",stringPtr2);
		env->CallObjectMethod(thiz, this_meth, url);

		SwapMethod(orgURLinitMethod, bakURLinitMethod);
	} else {
		HLOGI("[-] Error: url init method null");
	}
}


static Method* orgURLinit2Method = NULL;
static Method* bakURLinit2Method = NULL;
void JNICALL JNI_URL_init2(JNIEnv *env, jobject thiz,jobject obj, jstring url) {
	HLOGI("[*] enter url ini2t\n");

	if(orgURLinit2Method && bakURLinit2Method) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
		HLOGI("[*] url init2: %s",stringPtr);

		SwapMethod(orgURLinit2Method, bakURLinit2Method);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "<init>", "(Ljava/net/URL;Ljava/lang/String;)V");


	//	jstring url2 = env->NewStringUTF("http://www.baidu.com");
	//	const char *stringPtr2 = env->GetStringUTFChars(url2, &isCopy);
	//	HLOGI("[*] modify url: %s",stringPtr2);
		env->CallObjectMethod(thiz, this_meth, obj, url);

		SwapMethod(orgURLinit2Method, bakURLinit2Method);
	} else {
		HLOGI("[-] Error: url init2 method null");
	}
}

static Method* orgURIinitMethod = NULL;
static Method* bakURIinitMethod = NULL;
void JNICALL JNI_URI_init(JNIEnv *env, jobject thiz, jstring url) {
	HLOGI("[*] enter uri init\n");

	if(orgURIinitMethod && bakURIinitMethod) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
		HLOGI("[*] uri init: %s",stringPtr);

		SwapMethod(orgURIinitMethod, bakURIinitMethod);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");

		env->CallObjectMethod(thiz, this_meth, url);

		SwapMethod(orgURIinitMethod, bakURIinitMethod);
	} else {
		HLOGI("[-] Error: uri init method null");
	}
}

//--------------------
static Method* orgUriParseMethod = NULL;
static Method* bakUriParseMethod = NULL;
jobject JNICALL JNI_Uri_parse(JNIEnv *env, jobject thiz, jstring url) {
	HLOGI("[*] enter uri parse\n");

	if(orgUriParseMethod && bakUriParseMethod) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
		HLOGI("[*] uri parse: %s",stringPtr);

		SwapMethod(orgUriParseMethod, bakUriParseMethod);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetStaticMethodID(cls, "parse", "(Ljava/lang/String;)Landroid/net/Uri;");

		jobject val = env->CallStaticObjectMethod(cls, this_meth, url);

		SwapMethod(orgUriParseMethod, bakUriParseMethod);
		return val;
	} else {
		HLOGI("[-] Error: parse parse method null");
	}
	return NULL;
}


static Method* orgHttpGetMethod = NULL;
static Method* bakHttpGetMethod = NULL;
void JNICALL JNI_HttpGet_init(JNIEnv *env, jobject thiz, jstring url) {
	HLOGI("[*] enter httpGet init\n");

	if(orgHttpGetMethod && bakHttpGetMethod) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
		HLOGI("[*] http get: %s",stringPtr);

		SwapMethod(orgHttpGetMethod, bakHttpGetMethod);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");


	//	jstring url2 = env->NewStringUTF("http://jeeble.cn/test2.apk");
	//	const char *stringPtr2 = env->GetStringUTFChars(url2, &isCopy);
	//	HLOGI("[*] modify url: %s",stringPtr2);
		env->CallObjectMethod(thiz, this_meth, url);

		SwapMethod(orgHttpGetMethod, bakHttpGetMethod);
	} else {
		HLOGI("[-] Error: http get method null");
	}
}

static Method* orgHttpPostMethod = NULL;
static Method* bakHttpPostMethod = NULL;
void JNICALL JNI_HttpPost_init(JNIEnv *env, jobject thiz, jstring url) {
	HLOGI("[*] enter httpPost init\n");

	if(orgHttpPostMethod && bakHttpPostMethod) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
		HLOGI("[*] http post: %s",stringPtr);

		SwapMethod(orgHttpPostMethod, bakHttpPostMethod);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
		env->CallObjectMethod(thiz, this_meth, url);

		SwapMethod(orgHttpPostMethod, bakHttpPostMethod);
	} else {
		HLOGI("[-] Error: http get method null");
	}
}

static Method* orgWebViewLoadUrlMethod = NULL;
static Method* bakWebViewLoadUrlMethod = NULL;
void JNICALL JNI_WebView_LoadUrl(JNIEnv *env, jobject thiz, jstring url) {
	HLOGI("[*] enter webView loadUrl\n");

	if(orgWebViewLoadUrlMethod && bakWebViewLoadUrlMethod) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
		HLOGI("[*] webView loadurl: %s",stringPtr);

		SwapMethod(orgWebViewLoadUrlMethod, bakWebViewLoadUrlMethod);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "loadUrl", "(Ljava/lang/String;)V");
		env->CallObjectMethod(thiz, this_meth, url);

		SwapMethod(orgWebViewLoadUrlMethod, bakWebViewLoadUrlMethod);
	} else {
		HLOGI("[-] Error: http get method null");
	}
}


static Method* orgWebViewLoadUrlMethod2 = NULL;
static Method* bakWebViewLoadUrlMethod2 = NULL;
void JNICALL JNI_WebView_LoadUrl2(JNIEnv *env, jobject thiz, jstring url, jobject mymap) {
	HLOGI("[*] enter webView loadUrl2\n");

	if(orgWebViewLoadUrlMethod2 && bakWebViewLoadUrlMethod2) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
		HLOGI("[*] webView loadurl2: %s",stringPtr);

		SwapMethod(orgWebViewLoadUrlMethod2, bakWebViewLoadUrlMethod2);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "loadUrl", "(Ljava/lang/String;Ljava/util/Map;)V");
		env->CallObjectMethod(thiz, this_meth, url, mymap);

		SwapMethod(orgWebViewLoadUrlMethod2, bakWebViewLoadUrlMethod2);
	} else {
		HLOGI("[-] Error: http get method null");
	}
}


static Method* orgWebViewLoadDataMethod = NULL;
static Method* bakWebViewLoadDataMethod = NULL;
void JNICALL JNI_WebView_LoadData(JNIEnv *env, jobject thiz, jstring data, jstring type, jstring encode) {
	HLOGI("[*] enter webView loadData\n");

	if(orgWebViewLoadDataMethod && bakWebViewLoadDataMethod) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(data, &isCopy);
		HLOGI("[*] webView loaddata: %s",stringPtr);

		SwapMethod(orgWebViewLoadDataMethod, bakWebViewLoadDataMethod);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "loadData", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
		env->CallObjectMethod(thiz, this_meth, data, type, encode );

		SwapMethod(orgWebViewLoadDataMethod, bakWebViewLoadDataMethod);
	} else {
		HLOGI("[-] Error: http get method null");
	}
}


static Method* orgWebViewloadDataWithBaseURLMethod = NULL;
static Method* bakWebViewloadDataWithBaseURLMethod = NULL;
void JNICALL JNI_WebView_loadDataWithBaseURL(JNIEnv *env, jobject thiz, jstring url,jstring data, jstring type, jstring encode, jstring history) {
	HLOGI("[*] enter webView loadDataWithBaseURL\n");

	if(orgWebViewloadDataWithBaseURLMethod && bakWebViewloadDataWithBaseURLMethod) {
		jboolean isCopy;
		const char *stringPtr = env->GetStringUTFChars(url, &isCopy);
		HLOGI("[*] webView loadDataWithBaseURL: %s",stringPtr);

		SwapMethod(orgWebViewloadDataWithBaseURLMethod, bakWebViewloadDataWithBaseURLMethod);

		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "loadDataWithBaseURL",
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
		env->CallObjectMethod(thiz, this_meth, url, data, type, encode, history );

		SwapMethod(orgWebViewloadDataWithBaseURLMethod, bakWebViewloadDataWithBaseURLMethod);
	} else {
		HLOGI("[-] Error: http get method null");
	}
}

void* hook_entry(void* arg) {
	HLOGI("[*] Hook begin\n");
	
	javaVM = AndroidRuntime::getJavaVM();
	if (javaVM == NULL) {
		HLOGI("[-] Can't get jvm\n");
		return 0;
	}
	
	JNIEnv* jni_env;
	javaVM->AttachCurrentThread(&jni_env, arg);
	if (jni_env == NULL) {
		HLOGI("[-] Can't get jni env\n");
		return 0;
	}
	
	jniVersion = jni_env->GetVersion(); 
	HLOGI("[-] jni version=0x%08x\n", jniVersion);
	

	HookMethod(jni_env, "java/net/URL",
				"<init>",
				"(Ljava/lang/String;)V",
				(void*)JNI_URL_init, &orgURLinitMethod, &bakURLinitMethod);

	HookMethod(jni_env, "java/net/URL",
				"<init>",
				"(Ljava/net/URL;Ljava/lang/String;)V",
				(void*)JNI_URL_init2, &orgURLinit2Method, &bakURLinit2Method);

	HookMethod(jni_env, "android/net/Uri",
				"parse",
				"(Ljava/lang/String;)Landroid/net/Uri;",
				(void*)JNI_Uri_parse, &orgUriParseMethod, &bakUriParseMethod);

	HookMethod(jni_env, "java/net/URI",
				"<init>",
				"(Ljava/lang/String;)V",
				(void*)JNI_URI_init, &orgURIinitMethod, &bakURIinitMethod);

	HookMethod(jni_env, "org/apache/http/client/methods/HttpGet",
				"<init>",
				"(Ljava/lang/String;)V",
				(void*)JNI_HttpGet_init, &orgHttpGetMethod, &bakHttpGetMethod);

	HookMethod(jni_env, "org/apache/http/client/methods/HttpPost",
				"<init>",
				"(Ljava/lang/String;)V",
				(void*)JNI_HttpPost_init, &orgHttpPostMethod, &bakHttpPostMethod);

	HookMethod(jni_env, "android/webkit/WebView",
				"loadUrl",
				"(Ljava/lang/String;)V",
				(void*)JNI_WebView_LoadUrl, &orgWebViewLoadUrlMethod, &bakWebViewLoadUrlMethod);

	HookMethod(jni_env, "android/webkit/WebView",
				"loadUrl",
				"(Ljava/lang/String;Ljava/util/Map;)V",
				(void*)JNI_WebView_LoadUrl2, &orgWebViewLoadUrlMethod2, &bakWebViewLoadUrlMethod2);

	HookMethod(jni_env, "android/webkit/WebView",
				"loadData",
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
				(void*)JNI_WebView_LoadData, &orgWebViewLoadDataMethod, &bakWebViewLoadDataMethod);

	HookMethod(jni_env, "android/webkit/WebView",
				"loadDataWithBaseURL",
				"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
				(void*)JNI_WebView_loadDataWithBaseURL, &orgWebViewloadDataWithBaseURLMethod, &bakWebViewloadDataWithBaseURLMethod);



	//产生的异常需要手工清除，异常产生原因是找不到累的静态方法
	jni_env->ExceptionClear();
	javaVM->DetachCurrentThread();
	HLOGI("[*] Hook end\n");
}


extern "C"{

// int main(int argc, char* argv[]) {
__attribute__ ((visibility ("default"))) int init(int argc, char* argv[]) {
	HLOGI("[*] hook main\n");

	pthread_t tid;
	pthread_create(&tid, NULL, hook_entry, NULL);
	return 0;
}

}
// end of file

