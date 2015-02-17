#include "stdafx.h"
#include "ClassLoader.h"

JClassLoader* JClassLoader::getSystemClassLoader() {
	JClassLoader* classLoader = new JClassLoader;
	JNIEnvGetter jniEnvGetter;
	classLoader->InitJObject(CallStaticObjectMethod_(jniEnvGetter.get(), "java/lang/ClassLoader", "getSystemClassLoader", "()Ljava/lang/ClassLoader;"));
	return classLoader;
}
