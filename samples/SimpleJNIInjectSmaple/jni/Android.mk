ROOT_LOCAL_PATH := $(call my-dir)
SIMPLEJNI_PATH = $(ROOT_LOCAL_PATH)/../../..
INCLUDE_PATH = $(SIMPLEJNI_PATH)/include
SIMPLE_LIBRARIES = $(SIMPLEJNI_PATH)/lib

#---------------------------------------------------

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := SimpleJNIInjectSmaple

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(INCLUDE_PATH)

#VisualGDBAndroid: AutoUpdateSourcesInNextLine
LOCAL_SRC_FILES := SimpleJNIInjectSmaple.cpp

# ����target�������뿴nativelog.h�ļ���
MY_LOG_TAG := \"debug\"
LOCAL_CFLAGS += -DMY_LOG_TAG=$(MY_LOG_TAG)

# ���Ӿ�̬�⡣
LOCAL_LDLIBS := -llog -L$(SIMPLE_LIBRARIES) -lHack -lBaseUtils -lJNIJava

# ���Ӷ�̬�⡣
LOCAL_LDFLAGS += -L$(SIMPLE_LIBRARIES) -landroid_runtime_arm -ldvm_arm

include $(BUILD_SHARED_LIBRARY)
