/*
 * Copyright (C) 2009 Tomken Corp.
 *
 * Author: Tomken
 * Date:   2011-04-02
 * 
 */

#include "JavaHook.h"
#include "hlog.h"
#include "libdvm.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void _clearcache(char* begin, char *end)
{	
	const int syscall = 0xf0002;
	__asm __volatile (
		"mov	 r0, %0\n"			
		"mov	 r1, %1\n"
		"mov	 r7, %2\n"
		"mov     r2, #0x0\n"
		"svc     0x00000000\n"
		:
		:	"r" (begin), "r" (end), "r" (syscall)
		:	"r0", "r1", "r7"
		);
}

static void hs_clearcache(u4* begin, u4* end) {
	_clearcache((char*)begin, (char*)end);
}

static void cloneMethod(Method* dst, const Method* src) {
    // if (src->registerMap != NULL) {
    //    return;
    // }
	
    memcpy(dst, src, sizeof(Method));
}

void SwapMethod(Method* method1, Method* method2) {
	if(method1 && method2) {
		Method* tmpMeghod = (Method*)malloc(sizeof(struct Method));
		cloneMethod(tmpMeghod, method1);
		cloneMethod(method1, method2);
		cloneMethod(method2, tmpMeghod);
		free(tmpMeghod);
	}
}

static int calcMethodArgsSize(const char* shorty) {
    int count = 0;

    /* Skip the return type. */
    shorty++;

    for (;;) {
        switch (*(shorty++)) {
            case '\0': {
                return count;
            }
            case 'D':
            case 'J': {
                count += 2;
                break;
            }
            default: {
                count++;
                break;
            }
        }
    }
}


u4 dvmPlatformInvokeHints(const char* shorty) {
    const char* sig = shorty;
    int padFlags, jniHints;
    char sigByte;
    int stackOffset, padMask;

    stackOffset = padFlags = 0;
    padMask = 0x00000001;

    /* Skip past the return type */
    sig++;

    while (true) {
        sigByte = *(sig++);

        if (sigByte == '\0')
            break;

        if (sigByte == 'D' || sigByte == 'J') {
            if ((stackOffset & 1) != 0) {
                padFlags |= padMask;
                stackOffset++;
                padMask <<= 1;
            }
            stackOffset += 2;
            padMask <<= 2;
        } else {
            stackOffset++;
            padMask <<= 1;
        }
    }

    jniHints = 0;

    if (stackOffset > DALVIK_JNI_COUNT_SHIFT) {
        /* too big for "fast" version */
        jniHints = DALVIK_JNI_NO_ARG_INFO;
    } else {
        assert((padFlags & (0xffffffff << DALVIK_JNI_COUNT_SHIFT)) == 0);
        stackOffset -= 2;           // r2/r3 holds first two items
        if (stackOffset < 0)
            stackOffset = 0;
        jniHints |= ((stackOffset+1) / 2) << DALVIK_JNI_COUNT_SHIFT;
        jniHints |= padFlags;
    }

    return jniHints;
}

static int computeJniArgInfo(const char* shorty) {
    const char* sig = shorty;
    int returnType, jniArgInfo;
    u4 hints;

    /* The first shorty character is the return type. */
    switch (*(sig++)) {
    case 'V':
        returnType = DALVIK_JNI_RETURN_VOID;
        break;
    case 'F':
        returnType = DALVIK_JNI_RETURN_FLOAT;
        break;
    case 'D':
        returnType = DALVIK_JNI_RETURN_DOUBLE;
        break;
    case 'J':
        returnType = DALVIK_JNI_RETURN_S8;
        break;
    case 'Z':
    case 'B':
        returnType = DALVIK_JNI_RETURN_S1;
        break;
    case 'C':
        returnType = DALVIK_JNI_RETURN_U2;
        break;
    case 'S':
        returnType = DALVIK_JNI_RETURN_S2;
        break;
    default:
        returnType = DALVIK_JNI_RETURN_S4;
        break;
    }

    jniArgInfo = returnType << DALVIK_JNI_RETURN_SHIFT;

    hints = dvmPlatformInvokeHints(shorty);

    if (hints & DALVIK_JNI_NO_ARG_INFO) {
        jniArgInfo |= DALVIK_JNI_NO_ARG_INFO;
    } else {
        assert((hints & DALVIK_JNI_RETURN_MASK) == 0);
        jniArgInfo |= hints;
    }

    return jniArgInfo;
}

/**
 * 修改Java方法的指针。
 * @param[in] env
 * @param[in] method 方法结构体。
 * @param[in] funPtr
 */
static void ModifyMethod(JNIEnv* env, Method* method, void* funPtr) {
	Method* newMethod = (Method*) method;
	
	int argsSize = calcMethodArgsSize(method->shorty);
	if (!dvmIsStaticMethod(method))
		argsSize++;
		
	newMethod->registersSize = newMethod->insSize = argsSize;
	newMethod->nativeFunc = (DalvikBridgeFunc)funPtr;
	newMethod->insns = 0;
	newMethod->outsSize = 0;
	newMethod->registerMap = 0;
	newMethod->jniArgInfo = computeJniArgInfo(method->shorty); // 1073741824;
	
	SET_METHOD_FLAG(newMethod, ACC_NATIVE);
	
	dvmUseJNIBridge(newMethod, funPtr);
	
	hs_clearcache((u4*)newMethod, (u4*)((char*)newMethod + sizeof(Method)));
}

int HookMethod(JNIEnv* env, const char* classDesc, const char* methodName, const char* methodDesc, 
		void* func, Method** orgMethod, Method** bakMethod) {
	
	// 找到要Hook的类。
	jclass classObj = env->FindClass(classDesc);
	if(classObj == NULL) {
		HLOGI("[-] %s class not found!", classDesc);
		return -1;
	}

	// 找到要Hook的方法的方法ID。
	// 先尝试实例方法，如果未找到实例方法，再去找静态方法。
	jmethodID methodId = env->GetMethodID(classObj, methodName, methodDesc);
	if(methodId == NULL) {
		HLOGI("[-] %s method not found!", methodName);
		methodId = env->GetStaticMethodID(classObj, methodName, methodDesc);
		if(methodId == NULL){
			HLOGI("[-] %s static method not found!", methodName);
			return -2;
		}else{
			HLOGI("[+] %s static method is found!", methodName);
		}
	}
	
	Method* method = (Method*)methodId;

	// 备份方法到hash表。(backup method to hash table)
	Method* newMeth = (Method*)malloc(sizeof(Method));
	cloneMethod(newMeth, method); // 拷贝。
	// 备份原方法的结构。
	*bakMethod = newMeth;

	// hook method
	ModifyMethod(env, method, func);
	*orgMethod = method;

	HLOGI("[*] %s has hook!\n", methodName);
	return 0;
}

// end of file
