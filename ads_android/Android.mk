LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  lpt.c \
  main.c

LOCAL_MODULE := ads_rom
LOCAL_MODULE_TAGS := optional
#LOCAL_STATIC_LIBRARIES := libc
#OCAL_STATIC_LIBRARIES += libkallsyms
LOCAL_LDFLAGS += -static

TOP_SRCDIR := $(abspath $(LOCAL_PATH))
TARGET_C_INCLUDES += \
  $(TOP_SRCDIR)/device_database

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
