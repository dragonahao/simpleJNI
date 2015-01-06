/*
 * Copyright (C) 2009 Tomken Corp.
 *
 * Author: Tomken
 * Date:   2011-04-02
 * 
 */
 
#ifndef __JAVA_HOOK_H__
#define __JAVA_HOOK_H__

#include "Object_4.h"

int HookMethod(JNIEnv* env, const char* classDesc,
		const char* methodName,
		const char* methodDesc,
		void* func,
		Method** orgMethod,
		Method** bakMethod);
		
void SwapMethod(Method* method1, Method* method2);

#endif // __JAVA_HOOK_H__
