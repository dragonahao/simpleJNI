LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Substrate

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../include/substrate

LOCAL_SRC_FILES := hde64.c \
				   SubstrateDebug.cpp \
				   SubstrateHook.cpp \
				   SubstratePosixMemory.cpp
include $(BUILD_STATIC_LIBRARY)
