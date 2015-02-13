LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := BaseUtils
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../include

LOCAL_SRC_FILES := ./filedir.cpp \
				   ./process-thread.cpp \
				   ./module.cpp

include $(BUILD_STATIC_LIBRARY)
