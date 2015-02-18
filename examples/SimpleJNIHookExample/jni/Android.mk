ROOT_LOCAL_PATH := $(call my-dir)
SIMPLEJNI_PATH = $(ROOT_LOCAL_PATH)/../../..
INCLUDE_PATH = $(SIMPLEJNI_PATH)/include
SIMPLE_LIBRARIES = $(SIMPLEJNI_PATH)/lib

#---------------------------------------------------

LOCAL_PATH := $(ROOT_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE    := SimpleJNIHookSmaple

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(INCLUDE_PATH)

#VisualGDBAndroid: AutoUpdateSourcesInNextLine
LOCAL_SRC_FILES := MyJavaHook.cpp SimpleJNIHookSmaple.cpp

# 设置target，详情请看nativelog.h文件。
MY_LOG_TAG := \"debug\"
LOCAL_CFLAGS += -DMY_LOG_TAG=$(MY_LOG_TAG)

# 链接静态库。
LOCAL_LDLIBS := -llog -L$(SIMPLE_LIBRARIES) -lHack -lJNIJava -lSubstrate

# 链接动态库。
LOCAL_LDFLAGS += -L$(SIMPLE_LIBRARIES) -landroid_runtime_arm -ldvm_arm

include $(BUILD_SHARED_LIBRARY)
