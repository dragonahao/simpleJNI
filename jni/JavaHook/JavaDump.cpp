/*
 * Copyright (C) 2009 Tomken Corp.
 *
 * Author: Tomken
 * Date:   2011-04-02
 * 
 */

#include "JavaDump.h"
#include "hlog.h"

void DumpMethod(int index, const Method* method) {
	if(method == NULL) {
		HLOGI("method is null\n");
		return;
	}

	if(method->name)
		HLOGI("method: [%03d][%s][0x%08x]\n", index, method->name, method); // 0x%08x %s
	else
		HLOGI("method: [%03d][NULL][0x%08x]\n", index, method);

	HLOGI("-----------------------------------------------------\n");
	HLOGI("accessFlags=%x", method->accessFlags);
	HLOGI("methodIndex=%d", method->methodIndex);
	HLOGI("registersSize=%d", method->registersSize);
	HLOGI("outsSize=%d", method->outsSize);
	HLOGI("insSize=%d", method->insSize);
	HLOGI("prototype.dexFile=0x%08x", method->prototype.dexFile);
	HLOGI("prototype.protoIdx=%d", method->prototype.protoIdx);
	
	if(method->shorty)
		HLOGI("shorty: [%s]\n", method->shorty);
	else
		HLOGI("shorty: [NULL]\n");
		
	HLOGI("insns=0x%08x", method->insns);
	HLOGI("jniArgInfo=%d", method->jniArgInfo);
	HLOGI("nativeFunc=0x%08x", method->nativeFunc);
	HLOGI("registerMap=0x%08x", method->registerMap);
	HLOGI("fastJni=%d", method->fastJni);
	HLOGI("noRef=%d", method->noRef);
	HLOGI("shouldTrace=%d", method->shouldTrace);
	HLOGI("inProfile=%d", method->inProfile);
	HLOGI("-----------------------------------------------------\n");
}

void DumpClassObject(ClassObject* clazz) {
	int i;
	Method* method;
	
	HLOGI("=====================================================\n");
	if(clazz->descriptor)
		HLOGI("descriptor: [%s]\n", clazz->descriptor);
	else
		HLOGI("descriptor: [NULL]\n");

	HLOGI("accessFlags=%lu", clazz->accessFlags);
	HLOGI("serialNumber=%lu", clazz->serialNumber);
	HLOGI("status=%d", clazz->status);
	HLOGI("verifyErrorClass=0x%08x", clazz->verifyErrorClass);
	HLOGI("initThreadId=%u", clazz->initThreadId);
	HLOGI("objectSize=%d", clazz->objectSize);
	HLOGI("elementClass=0x%08x", clazz->elementClass);
//	HLOGI("arrayClass=0x%08x", clazz->arrayClass);
	HLOGI("arrayDim=%d", clazz->arrayDim);
	HLOGI("primitiveType=%d", clazz->primitiveType);
	HLOGI("super=0x%08x", clazz->super);
	HLOGI("classLoader=0x%08x", clazz->classLoader);
	HLOGI("interfaceCount=%d", clazz->interfaceCount);
	HLOGI("interfaces=0x%08x", clazz->interfaces);
	HLOGI("directMethodCount=%d\n", clazz->directMethodCount);
	HLOGI("virtualMethodCount=%d\n", clazz->virtualMethodCount);
	HLOGI("vtableCount=%d\n", clazz->vtableCount);
	HLOGI("iftableCount=%d\n", clazz->iftableCount);
	HLOGI("ifviPoolCount=%d\n", clazz->ifviPoolCount);
	HLOGI("sfieldCount=%d\n", clazz->sfieldCount);
	
	HLOGI("ifieldCount=%d\n", clazz->ifieldCount);
	HLOGI("ifieldRefCount=%d\n", clazz->ifieldRefCount);
	
	HLOGI("refOffsets=%u\n", clazz->refOffsets);
	
	if(clazz->sourceFile)
		HLOGI("sourceFile: [%x]\n", clazz->sourceFile);
	else
		HLOGI("sourceFile: [NULL]\n");
	
	if(clazz->directMethods) {
		HLOGI("directMethodCount=%d\n", clazz->directMethodCount);
		for(i=0; i<clazz->directMethodCount; i++) {
			method = &(clazz->directMethods[i]);
			DumpMethod(i+1, method);
		}
	}
	
	if(clazz->virtualMethods) {
		HLOGI("virtualMethodCount=%d\n", clazz->virtualMethodCount);
		for(i=0; i<clazz->virtualMethodCount; i++) {
			method = &(clazz->virtualMethods[i]);
			DumpMethod(i+1, method);
		}
	}
	
	if(clazz->vtable) {
		HLOGI("vtableCount=%d\n", clazz->vtableCount);
		for(i=0; i<clazz->vtableCount; i++) {
			method = clazz->vtable[i];
			DumpMethod(i+1, method);
		}
	}
	
	HLOGI("=====================================================\n");
}

// end of file

