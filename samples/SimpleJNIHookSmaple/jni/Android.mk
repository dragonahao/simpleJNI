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

LOCAL_LDLIBS := -llog -L$(SIMPLE_LIBRARIES) -lHack -lJNIJava

LOCAL_LDFLAGS += -L$(SIMPLE_LIBRARIES) -landroid_runtime_arm -ldvm_arm

include $(BUILD_SHARED_LIBRARY)
