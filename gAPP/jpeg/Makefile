CC := gcc
LOCAL_SRC_FILES := \
	jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c \
	jcinit.c jcmainct.c jcmarker.c jcmaster.c jcomapi.c jcparam.c \
	jcphuff.c jcprepct.c jcsample.c jctrans.c jdapimin.c jdapistd.c \
	jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c \
	jdinput.c jdmainct.c jdmarker.c jdmaster.c jdmerge.c jdphuff.c \
	jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c jfdctfst.c \
	jfdctint.c jidctflt.c jidctred.c jquant1.c \
	jquant2.c jutils.c jmemmgr.c \
	jmem-android.c

ANDROID_JPEG_NO_ASSEMBLER := true

ifeq ($(strip $(ANDROID_JPEG_NO_ASSEMBLER)),true)
LOCAL_SRC_FILES += jidctint.c jidctfst.c
else
LOCAL_SRC_FILES += jidctint.c jidctfst.S
endif

OBJS := $(patsubst %.c, %.o, $(LOCAL_SRC_FILES))

LOCAL_CFLAGS += -DAVOID_TABLES 
LOCAL_CFLAGS += -O3 -fstrict-aliasing -fprefetch-loop-arrays
#LOCAL_CFLAGS += -march=armv6j

LOCAL_MODULE:= libAjpeg.so

$(LOCAL_MODULE): $(OBJS)
	$(CC) -nostdlib -Wl,-soname,libAjpeg.so -Wl,--gc-sections -Wl,-shared,-Bsymbolic -lc -o $@ -Wl,--whole-archive $(OBJS)

clean:
	rm -f $(OBJS) $(LOCAL_MODULE)
