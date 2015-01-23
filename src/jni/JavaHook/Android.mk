LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

SO_LIB_PATH := $(LOCAL_PATH)/../../lib

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../include/JavaHook

LOCAL_LDLIBS += -llog
LOCAL_LDFLAGS += -L$(SO_LIB_PATH) -landroid_runtime_arm -ldvm_arm

#LOCAL_SRC_FILES += libmynet.cpp
LOCAL_SRC_FILES += localSocket.c
LOCAL_SRC_FILES += init2.cpp
LOCAL_SRC_FILES += JniUtil.cpp
LOCAL_SRC_FILES += JavaHook.cpp
LOCAL_SRC_FILES += JavaDump.cpp


LOCAL_MODULE := JavaHook
LOCAL_MODULE_TAGS := optional 

#LOCAL_ARM_MODE := arm
 
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)


