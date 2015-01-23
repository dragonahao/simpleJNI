/*
 * libmynet.c
 *
 *  Created on: 2013-1-17
 *      Author: d
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
#include "hlog.h"
/*
#include "android_runtime/AndroidRuntime.h"


#include "JavaHook.h"
#include "JavaDump.h"
#include "JniUtil.h"


#ifndef PATH_MAX
#define PATH_MAX 255
#endif
*/

__attribute__ ((visibility ("default"))) int initGo(){
	HLOGI("hello my pid1");
	printf("hello my pid2\n");
	return getpid();
}

