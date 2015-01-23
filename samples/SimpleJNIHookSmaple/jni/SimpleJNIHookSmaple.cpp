#include "stdafx.h"
#include "MyJavaHook.h"

// Hook Native²ã¡£
void nativeHooks() {
	MY_LOG_INFO("#---------- Start nativeHooks ---------------#");
// 	HOOK_FUNCTION(dvmJarFileOpen);
// 	HOOK_FUNCTION(dvmOpenCachedDexFile);
// 	HOOK_FUNCTION(dvmOptimizeDexFile);


	MY_LOG_INFO("#---------- End nativeHooks ---------------#");
}

void hooks() {
	MyJavaHook();
	nativeHooks();
}
