LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := JNIJava
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../include

LOCAL_SRC_FILES := ./JavaBase.cpp \
				   ./JavaNumber.cpp \
				   ./JavaString.cpp \
				   ./JavaIO.cpp \
				   ./JNIReflect.cpp \
				   ./AndroidUtil.cpp \
				   ./AndroidContent.cpp \
				   ./jstring_.cpp \
				   ./JavaException.cpp

include $(BUILD_STATIC_LIBRARY)
