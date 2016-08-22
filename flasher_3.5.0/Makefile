# dirty, short, i hate it....

KVERSION = $(shell uname -r|grep --count '^2.6') 

all:
	mkdir -p modules
ifeq ($(KVERSION),1 )
	cd build2.6;make ; cd - 
	cp build2.6/*.ko modules
else
	cd flash; make all; cd -
	cd ct; make all; cd -
	cd ide; make all; cd -
	cd nics/e100; make all; cd -
	cd nics/3com; make all; cd -
	cd nics/realtek; make all; cd -
	cd nics/via; make all; cd -
	cd bios/sis630; make all; cd -
endif
	
clean:
	rm -f modules/*.o modules/*.ko
	cd build2.6; make clean; cd -
	cd flash; make clean; cd -
	cd ct; make clean; cd -
	cd ide; make clean; cd -
	cd nics/e100; make clean; cd -
	cd nics/3com; make clean; cd -
	cd nics/realtek; make clean; cd -
	cd nics/via; make clean; cd -
	cd bios/sis630; make clean; cd -
