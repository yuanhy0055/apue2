
ide_flash-objs := ../flash/flash_proc.o ../ide/ide_hardware.o 
ct_flash-objs := ../flash/flash_proc.o ../ct/ct_hardware.o ../ct/get.o ../ct/put.o
sis630_flash-objs := ../flash/flash_proc.o ../bios/sis630/sis630.o

3c90xc_flash-objs := ../flash/flash_proc.o ../nics/3com/3c90xc.o
e100_flash-objs := ../flash/flash_proc.o ../nics/eeprom/eeprom_proc.o ../nics/e100/e100.o
via_flash-objs :=  ../flash/flash_proc.o ../nics/via/via.o
rtl8139_flash-objs :=  ../flash/flash_proc.o ../nics/realtek/rtl8139.o

flash-objs := ../flash/main.o ../flash/am29.o ../flash/at29.o ../flash/am28a.o ../flash/eon29F00x.o ../flash/eon29F0x0.o ../flash/v29.o ../flash/cat28.o ../flash/i28.o ../flash/i28a.o ../flash/sst39.o ../flash/sst39a.o ../flash/w29.o ../flash/unknown.o 


ifneq ($(KERNELRELEASE),) 
obj-m   := e100_flash.o flash.o ide_flash.o ct_flash.o sis630_flash.o 3c90xc_flash.o via_flash.o rtl8139_flash.o


else
KDIR    := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR)  SUBDIRS=$(PWD)  modules
endif

clean:
	rm -f *.o *.ko *.mod.*
