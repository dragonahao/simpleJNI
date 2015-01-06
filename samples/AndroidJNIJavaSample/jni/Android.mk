ROOT_LOCAL_PATH := $(call my-dir)
SimpleJNI_PATH = $(ROOT_LOCAL_PATH)/../../..
JNI_PATH = $(SimpleJNI_PATH)/jni
INCLUDE_PATH = $(JNI_PATH)/include
JNIJava_PATH = $(JNI_PATH)/JNIJava
SYSTEM_LIBRARIES = $(SimpleJNI_PATH)/lib

#---------------------------------------------------
# JNIJava
include $(JNIJava_PATH)/Android.mk

#---------------------------------------------------

LOCAL_PATH := $(ROOT_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE    := AndroidJNIJavaSample

LOCAL_C_INCLUDES := $(INCLUDE_PATH)

LOCAL_STATIC_LIBRARIES := JNIJava

LOCAL_SRC_FILES := AndroidJNIJavaSample.cpp

LOCAL_LDLIBS := -fuse-ld=bfd
LOCAL_LDFLAGS += -L$(SYSTEM_LIBRARIES) -landroid_runtime_arm -ldvm_arm

include $(BUILD_SHARED_LIBRARY)
