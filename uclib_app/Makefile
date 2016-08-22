GCC_INC		= -I/usr/lib/gcc/i486-linux-gnu/4.3.3/include -I/usr/lib/gcc/i486-linux-gnu/4.3.3/include-fixed
UCLIB_INC	= -I/KU/kfa/include -I/usr/include

EXTRA_CFLAG = -std=c99 -fno-stack-protector

#/usr/lib/gcc/i486-linux-gnu/4.3.2/collect2 --eh-frame-hdr -m elf_i386 --hash-style=both -dynamic-linker /lib/ld-linux.so.2 -o a
#crt1.o crti.o crtbegin.o -L/usr/lib/gcc/i486-linux-gnu/4.3.2
#-L/lib -L/usr/lib -L/usr/lib/gcc/i486-linux-gnu/4.3.2/.... /tmp/ccaB6Qrc.o -lgcc --as-needed -lgcc_s --no-as-needed
#-lc  crtend.o crtn.o

#init: init.o
fbv: fbv.o
	/usr/bin/ld -nostdlib -m elf_i386 -dynamic-linker /KU/lib/ld-uClibc.so.0 -o $@ \
	/KU/kfa/lib/crt1.o $< \
	-lgcc --as-needed -lgcc_s --no-as-needed \
	-lc \
	-L/usr/lib/gcc/i486-linux-gnu/4.3.3 \
	-L/KU/lib \
	-L/KU/kfa/lib \
	/KU/kfa/lib/crti.o \
	/KU/kfa/lib/crtn.o

#./usr/lib/Scrt1.o

clean:
	rm -f fbv fbv.o init init.o

%.o: %.c
	/usr/bin/gcc-4.3 -g -nostdinc ${EXTRA_CFLAG} ${GCC_INC} ${UCLIB_INC} -c -o $@ $<
