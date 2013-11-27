LOCAL_PATH:= $(call my-dir)

define PREBUILT_APP_template
include $(CLEAR_VARS)
LOCAL_MODULE := $(1)
LOCAL_SRC_FILES := $$(LOCAL_MODULE)
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := eng
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)
include $(BUILD_PREBUILT)
endef

prebuilt_apps := \
	bash \
	busybox

$(foreach app,$(prebuilt_apps), \
	$(eval $(call PREBUILT_APP_template, $(app))) \
)


#####################
#include $(CLEAR_VARS)
#LOCAL_MODULE := bash
#LOCAL_SRC_FILES := $(LOCAL_MODULE)
#LOCAL_MODULE_CLASS := EXECUTABLES
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/bin
#include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE := busybox
#LOCAL_SRC_FILES := $(LOCAL_MODULE)
#LOCAL_MODULE_CLASS := EXECUTABLES
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_EXECUTABLES)
#include $(BUILD_PREBUILT)
