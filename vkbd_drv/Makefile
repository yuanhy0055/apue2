KERNEL_BLD = /lib/modules/`uname -r`/build

TARGET	= vv_kbd.o

obj-m              := $(TARGET)

all:
	KBUILD_NOPEDANTIC=1 make -C $(KERNEL_BLD) M=`pwd`

clean:
	KBUILD_NOPEDANTIC=1 make -C $(KERNEL_BLD) M=`pwd` clean

inst:
	cp -f nandsim.ko ../run_root/
