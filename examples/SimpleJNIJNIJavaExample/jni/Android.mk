ROOT_LOCAL_PATH := $(call my-dir)
SimpleJNI_PATH = $(ROOT_LOCAL_PATH)/../../..
JNI_PATH = $(SimpleJNI_PATH)/jni
INCLUDE_PATH = $(JNI_PATH)/../include
SIMPLE_LIBRARIES = $(SimpleJNI_PATH)/lib

#---------------------------------------------------

LOCAL_PATH := $(ROOT_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE    := AndroidJNIJavaSample

LOCAL_C_INCLUDES := $(INCLUDE_PATH)

LOCAL_SRC_FILES := AndroidJNIJavaSample.cpp

LOCAL_LDLIBS := -fuse-ld=bfd -L$(SIMPLE_LIBRARIES) -lJNIJava
LOCAL_LDFLAGS += -L$(SIMPLE_LIBRARIES) -landroid_runtime_arm -ldvm_arm

include $(BUILD_SHARED_LIBRARY)
