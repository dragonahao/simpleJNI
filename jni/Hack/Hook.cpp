#include "stdafx.h"
#include "Hook.h"

//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////

int CalcMethodArgsSize(const char* shorty) {
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

int ComputeJniArgInfo(const char* shorty) {
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

//////////////////////////////////////////////////////////////////////////

JavaHook::JavaHook(JNIEnv* env, const char* classDesc, const char* methodName, const char* methodSig)
	: mEnv(env), mNewMethod(NULL)
{
	// 获得返回值的短表示方式。
// 	char* tmp = strrchr(methodSig, ')') + 1;
// 	mReturnShorty = *tmp;

	// 获得类对象和方法ID。
	mClass = env->FindClass(classDesc);
	mOrigMethod = (Method*)env->GetMethodID(mClass, methodName, methodSig);

	// 拷贝方法签名。
	mMethodSig = (char*)calloc(strlen(methodSig) + 1, sizeof(char));
	strcpy(mMethodSig, methodSig);
}

JavaHook::~JavaHook() {
	if (NULL != mNewMethod) {
		free(mNewMethod);
	}
	if (NULL != mClass) {
		mEnv->DeleteLocalRef(mClass);
	}
	if (NULL != mMethodSig) {
		free(mMethodSig);
	}
}

void JavaHook::Hook(void* func) {
	this->mFunc = func;
	CloneMethod();
	ModifyMethod();
}

void JavaHook::CallOldVoidMethod(jobject thiz, ...) {
	va_list ap;
	va_start(ap, thiz);
	mEnv->CallVoidMethodV(thiz, (jmethodID)mOrigMethod, ap);
	va_end(ap);
}

jboolean JavaHook::CallOldBooleanMethod(jobject thiz, ...) {
	va_list ap;
	va_start(ap, thiz);
	jboolean result = mEnv->CallBooleanMethodV(thiz, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jbyte JavaHook::CallOldByteMethod(jobject thiz, ...) {
	va_list ap;
	va_start(ap, thiz);
	jbyte result = mEnv->CallByteMethodV(thiz, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jshort JavaHook::CallOldShortMethod(jobject thiz, ...) {
	va_list ap;
	va_start(ap, thiz);
	jshort result = mEnv->CallShortMethodV(thiz, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jchar JavaHook::CallOldCharMethod(jobject thiz, ...) {
	va_list ap;
	va_start(ap, thiz);
	jchar result = mEnv->CallCharMethodV(thiz, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jint JavaHook::CallOldIntMethod(jobject thiz, ...) {
	va_list ap;
	va_start(ap, thiz);
	jint result = mEnv->CallIntMethodV(thiz, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jlong JavaHook::CallOldLongMethod(jobject thiz, ...) {
	va_list ap;
	va_start(ap, thiz);
	jlong result = mEnv->CallLongMethodV(thiz, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jfloat JavaHook::CallOldFloatMethod(jobject thiz, ...) {
	va_list ap;
	va_start(ap, thiz);
	jfloat result = mEnv->CallFloatMethodV(thiz, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jdouble JavaHook::CallOldDoubleMethod(jobject thiz, ...) {
	va_list ap;
	va_start(ap, thiz);
	jdouble result = mEnv->CallDoubleMethodV(thiz, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

void JavaHook::CallOldStaticVoidMethod(...) {
	va_list ap;
	va_start(ap, this);
	mEnv->CallStaticVoidMethodV(mClass, (jmethodID)mOrigMethod, ap);
	va_end(ap);
}

jboolean JavaHook::CallOldStaticBooleanMethod(...) {
	va_list ap;
	va_start(ap, this);
	jboolean result = mEnv->CallStaticBooleanMethodV(mClass, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jbyte JavaHook::CallOldStaticByteMethod(...) {
	va_list ap;
	va_start(ap, this);
	jbyte result = mEnv->CallStaticByteMethodV(mClass, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jshort JavaHook::CallOldStaticShortMethod(...) {
	va_list ap;
	va_start(ap, this);
	jshort result = mEnv->CallStaticShortMethodV(mClass, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jchar JavaHook::CallOldStaticCharMethod(...) {
	va_list ap;
	va_start(ap, this);
	jchar result = mEnv->CallStaticCharMethodV(mClass, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jint JavaHook::CallOldStaticIntMethod(...) {
	va_list ap;
	va_start(ap, this);
	jint result = mEnv->CallStaticIntMethodV(mClass, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jlong JavaHook::CallOldStaticLongMethod(...) {
	va_list ap;
	va_start(ap, this);
	jlong result = mEnv->CallStaticLongMethodV(mClass, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jfloat JavaHook::CallOldStaticFloatMethod(...) {
	va_list ap;
	va_start(ap, this);
	jfloat result = mEnv->CallStaticFloatMethodV(mClass, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

jdouble JavaHook::CallOldStaticDoubleMethod(...) {
	va_list ap;
	va_start(ap, this);
	jdouble result = mEnv->CallStaticDoubleMethodV(mClass, (jmethodID)mOrigMethod, ap);
	va_end(ap);
	return result;
}

// 克隆方法结构体。
void JavaHook::CloneMethod() {
	mNewMethod = (Method*) calloc(1, sizeof(Method));
	memcpy(mNewMethod, mOrigMethod, sizeof(Method));
}

void JavaHook::ModifyMethod() {
	int argsSize = CalcMethodArgsSize(mNewMethod->shorty);
	if (!dvmIsStaticMethod(mNewMethod))
		argsSize++;

	mNewMethod->registersSize = mNewMethod->insSize = argsSize;
	mNewMethod->nativeFunc = (DalvikBridgeFunc)mFunc;
	mNewMethod->insns = 0;
	mNewMethod->outsSize = 0;
	mNewMethod->registerMap = 0;
	mNewMethod->jniArgInfo = ComputeJniArgInfo(mNewMethod->shorty); // 1073741824;

	SET_METHOD_FLAG(mNewMethod, ACC_NATIVE);

	dvmUseJNIBridge(mNewMethod, mFunc);

	hs_clearcache((u4*)mNewMethod, (u4*)((char*)mNewMethod + sizeof(Method)));
}
