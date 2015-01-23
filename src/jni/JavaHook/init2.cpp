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
#include <system/AndroidRuntime.h>

#include "JavaHook.h"
#include "JavaDump.h"
#include "JniUtil.h"
#include "hlog.h"
#include "localSocket.h"

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

jint getPid(JNIEnv *env){
	jclass clazz = env->FindClass("android/os/Binder");
	if( clazz != NULL ){
		jmethodID getCallingPidMethodId = env->GetStaticMethodID(clazz, "getCallingPid", "()I" );
		if( getCallingPidMethodId != NULL ){
			jint pid = env->CallStaticIntMethod(clazz, getCallingPidMethodId);
			//HLOGI("pid=:%d\n", pid);
			return pid;
		}else{
			HLOGI("[-]getCallingPid not found");
			return -1;
		}
	}else{
		HLOGI("[-]Binder class not found");
	}
	return -1;
}

jint getUid(JNIEnv *env){
	jclass clazz = env->FindClass("android/os/Binder");
	if( clazz != NULL ){
		jmethodID getCallingPidMethodId = env->GetStaticMethodID(clazz, "getCallingUid", "()I" );
		if( getCallingPidMethodId != NULL ){
			jint uid = env->CallStaticIntMethod(clazz, getCallingPidMethodId);
			//HLOGI("uid=:%d\n", uid);
			return uid;
		}else{
			HLOGI("[-]getCallingUid not found");
			return -1;
		}
	}else{
		HLOGI("[-]Binder class not found");
	}
	return -1;
}

jint getType(JNIEnv *env, jobject attrs){
	jclass clsP = env->GetObjectClass(attrs);
	if( clsP != NULL ){
		jfieldID dataFieldType = env->GetFieldID(clsP, "type", "I");

		jint val = env->GetIntField(attrs, dataFieldType);
		if( dataFieldType != NULL ){
			//HLOGI("dataFieldType=:%d\n", val);
			return val;
		}else{
			HLOGI("[-]params type not found");
		}
		return -1;
	}else{
		HLOGI("[-]params class not found");
	}
	return -1;
}

static Method* orgWMSAddWindowMethod = NULL;
static Method* bakWMSAddWindowMethod = NULL;
JNICALL jint JNI_WMS_AddWindow(JNIEnv *env, jobject thiz, jobject session,jobject iwindow, int seq, jobject attrs, int viewVisibility, int xx, jobject outContentInsets, jobject outInputChannel) {
//	HLOGI("[*] enter WMS AddWindow\n");

	if(orgWMSAddWindowMethod && bakWMSAddWindowMethod) {
//		HLOGI("[*] enter WMS AddWindow2\n");

		int pid = getPid(env);
		HLOGI("[*] pid1: %d", pid);
		char data[128];
		sprintf(data, "2__jj__%d", pid);
		HLOGI("[*] pid: %s",data);


		SwapMethod(orgWMSAddWindowMethod, bakWMSAddWindowMethod);
		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "addWindow","(Lcom/android/server/wm/Session;Landroid/view/IWindow;ILandroid/view/WindowManager$LayoutParams;IILandroid/graphics/Rect;Landroid/view/InputChannel;)I");
		jint val = env->CallIntMethod(thiz, this_meth, session, iwindow, seq, attrs, viewVisibility, xx, outContentInsets, outInputChannel);

		SwapMethod(orgWMSAddWindowMethod, bakWMSAddWindowMethod);



		int execVal = sendmsgtojava(data);
		HLOGI("[*]exec:%d\n", execVal);

		return val;
	} else {
		HLOGI("[-] Error: WMS AddWindow null");
	}

	return 0;
}

JNICALL jint JNI_WMS_AddWindow_4_0_3(JNIEnv *env, jobject thiz, jobject session,jobject iwindow, int seq, jobject attrs, int viewVisibility, jobject outContentInsets, jobject outInputChannel) {
//	HLOGI("[*] enter WMS AddWindow\n");

	if(orgWMSAddWindowMethod && bakWMSAddWindowMethod) {
//		HLOGI("[*] enter WMS AddWindow2\n");

		int pid = getPid(env);
		HLOGI("[*] pid1: %d", pid);
		char data[128];
		sprintf(data, "2__jj__%d", pid);
		HLOGI("[*] pid: %s",data);


		SwapMethod(orgWMSAddWindowMethod, bakWMSAddWindowMethod);
		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "addWindow","(Lcom/android/server/wm/Session;Landroid/view/IWindow;ILandroid/view/WindowManager$LayoutParams;ILandroid/graphics/Rect;Landroid/view/InputChannel;)I");
		jint val = env->CallIntMethod(thiz, this_meth, session, iwindow, seq, attrs, viewVisibility, outContentInsets, outInputChannel);

		SwapMethod(orgWMSAddWindowMethod, bakWMSAddWindowMethod);

		int execVal = sendmsgtojava(data);
		HLOGI("[*]exec:%d\n", execVal);

		return val;
	} else {
		HLOGI("[-] Error: WMS AddWindow null");
	}

	return 0;
}


static Method* orgAmsSetFocusedActivityLockedMethod = NULL;
static Method* bakAmsSetFocusedActivityLockedMethod = NULL;
JNICALL void JNI_AMS_setFocusedActivityLocked(JNIEnv *env, jobject thiz, jobject obj1) {
	HLOGI("[*] enter AMS setFocusedActivityLocked beg\n");

	if(orgAmsSetFocusedActivityLockedMethod && bakAmsSetFocusedActivityLockedMethod) {
		jint pid = getPid(env);
		jint uid = getUid(env);
		char buffer[128];
		sprintf(buffer,"call setFocusedActivityLocked app pid:%d  uid:%d \n", pid, uid);
		HLOGI("%s", buffer);

		SwapMethod(orgAmsSetFocusedActivityLockedMethod, bakAmsSetFocusedActivityLockedMethod);
		jclass cls = env->GetObjectClass(thiz);
		jmethodID this_meth = env->GetMethodID(cls, "setFocusedActivityLocked",
				"(Lcom/android/server/am/ActivityRecord;)V");
		env->CallVoidMethod(thiz, this_meth, obj1);

		SwapMethod(orgAmsSetFocusedActivityLockedMethod, bakAmsSetFocusedActivityLockedMethod);

		jclass cls1 = env->GetObjectClass(obj1);
		if(cls1 == NULL){
			HLOGI("[-] obj1 is null");
		}
		jfieldID proName = env->GetFieldID(cls1, "packageName", "Ljava/lang/String;");

		jstring str1 = (jstring)env->GetObjectField(obj1, proName);
		jboolean isCopy;
		const char *stringPtr1 = env->GetStringUTFChars(str1, &isCopy);
		char data[128];
		sprintf(data, "1__jj__%s", stringPtr1);
		HLOGI("[*] package name: %s",data);

		int execVal = sendmsgtojava(data);
		HLOGI("[*]exec:%d\n", execVal);

		HLOGI("[*] enter AMS setFocusedActivityLocked end\n");
	} else {
		HLOGI("[-] Error: AMS setFocusedActivityLocked null");
	}
}

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
		env->CallVoidMethod(thiz, this_meth, url);

		SwapMethod(orgURLinitMethod, bakURLinitMethod);
	} else {
		HLOGI("[-] Error: url init method null");
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
	HLOGI("[+] jni version=0x%08x\n", jniVersion);


	/*
	HookMethod(jni_env, "com/android/server/am/ActivityManagerService",
			"setFocusedActivityLocked",
			"(Lcom/android/server/am/ActivityRecord;)V",
			(void*)JNI_AMS_setFocusedActivityLocked, &orgAmsSetFocusedActivityLockedMethod, &bakAmsSetFocusedActivityLockedMethod);


	int version = 42;
	//监听AddWindow
	if(version > 41){
		HookMethod(jni_env, "com/android/server/wm/WindowManagerService",
				"addWindow",
				"(Lcom/android/server/wm/Session;Landroid/view/IWindow;ILandroid/view/WindowManager$LayoutParams;IILandroid/graphics/Rect;Landroid/view/InputChannel;)I",
				(void*)JNI_WMS_AddWindow, &orgWMSAddWindowMethod, &bakWMSAddWindowMethod);
	}else{
		HookMethod(jni_env, "com/android/server/wm/WindowManagerService",
				"addWindow",
				"(Lcom/android/server/wm/Session;Landroid/view/IWindow;ILandroid/view/WindowManager$LayoutParams;ILandroid/graphics/Rect;Landroid/view/InputChannel;)I",
				(void*)JNI_WMS_AddWindow_4_0_3, &orgWMSAddWindowMethod, &bakWMSAddWindowMethod);
	}
	*/

	HookMethod(jni_env, "java/net/URL",
					"<init>",
					"(Ljava/lang/String;)V",
					(void*)JNI_URL_init, &orgURLinitMethod, &bakURLinitMethod);


	//产生的异常需要手工清除，异常产生原因是找不到累的静态方法
	jni_env->ExceptionClear();
	javaVM->DetachCurrentThread();
	HLOGI("[*] Hook end\n");
}


extern "C"{

// int main(int argc, char* argv[]) {
//__attribute__ ((visibility ("default"))) int init(int argc, char* argv[]) {
//	HLOGI("[*] hook main\n");
//
//	pthread_t tid;
//	pthread_create(&tid, NULL, hook_entry, NULL);
//	return 0;
//}

void Java_com_hookjava_MainActivity_init(JNIEnv* env, jobject thiz) {
	__android_log_print(ANDROID_LOG_INFO, "HookJava", "Java_com_hookjava_MainActivity_init");
	pthread_t tid;
	pthread_create(&tid, NULL, hook_entry, NULL);
}

}
// end of file

