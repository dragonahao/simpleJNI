LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Hack
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../include/hack

LOCAL_SRC_FILES := ./Inject.cpp \
				   ./Hook.cpp

# Á´½Ó¾²Ì¬¿â¡£
#LOCAL_LDLIBS := -llog -L$(SIMPLE_LIBRARIES) -lBaseUtils

include $(BUILD_STATIC_LIBRARY)
