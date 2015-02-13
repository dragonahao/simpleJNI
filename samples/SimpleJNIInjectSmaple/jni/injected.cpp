#include <jni.h>
#include <stdio.h>
#include <android/log.h>

__attribute__((constructor))
void test() {
	printf("injected - test");
	__android_log_print(ANDROID_LOG_INFO, "debug", "injected - test");
}
