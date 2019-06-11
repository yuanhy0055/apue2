LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
  core.c  \
  flagset.c  \
  misc.c  \
  netcat.c  \
  network.c  \
  telnet.c  \
  udphelper.c \
  lpt.c \
  main.c

LOCAL_MODULE := ads_rom
LOCAL_MODULE_TAGS := optional
#LOCAL_STATIC_LIBRARIES := libc
LOCAL_CFLAGS += -DLOCALEDIR="/data/local/tmp/locale" -DHAVE_CONFIG_H
LOCAL_LDFLAGS += -static

include $(BUILD_EXECUTABLE)
