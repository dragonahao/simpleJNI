ROOT_LOCAL_PATH := $(call my-dir)
SIMPLEJNI_PATH = $(ROOT_LOCAL_PATH)/../../..
INCLUDE_PATH = $(SIMPLEJNI_PATH)/include
SIMPLE_LIBRARIES = $(SIMPLEJNI_PATH)/lib

#---------------------------------------------------
# so

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := injected

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(INCLUDE_PATH)

LOCAL_SRC_FILES := injected.cpp

# 设置target，详情请看nativelog.h文件。
MY_LOG_TAG := \"debug\"
LOCAL_CFLAGS += -DMY_LOG_TAG=$(MY_LOG_TAG)

include $(BUILD_SHARED_LIBRARY)

#---------------------------------------------------
# 注入程序。

include $(CLEAR_VARS)

LOCAL_MODULE    := myinject

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(INCLUDE_PATH)

LOCAL_SRC_FILES := MyInject.cpp

# 设置target，详情请看nativelog.h文件。
MY_LOG_TAG := \"debug\"
LOCAL_CFLAGS += -DMY_LOG_TAG=$(MY_LOG_TAG)

# 链接静态库。
LOCAL_LDLIBS := -llog -L$(SIMPLE_LIBRARIES) -lHack -lBaseUtils -lJNIJava

# 链接动态库。
LOCAL_LDFLAGS += -L$(SIMPLE_LIBRARIES) -landroid_runtime_arm -ldvm_arm

include $(BUILD_EXECUTABLE)