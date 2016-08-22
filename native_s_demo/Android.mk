ifeq ($(HAVE_YYUAN_STUDY),true)

else

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= yyjsvc.cpp edfw.cpp
LOCAL_SHARED_LIBRARIES:= libutils libbinder
#LOCAL_SHARED_LIBRARIES:= libutils libnicosvc
LOCAL_MODULE:= mynsv
include $(BUILD_EXECUTABLE)

endif
