
MODDIR = ../modules

include ../Makefile.common

MODULE = flash.o

SMOBJECTS = $(shell grep -l "^EXPORT_SYMBOL" *.c)

MOBJECTS = main.o am29.o at29.o am28a.o eon29F00x.o eon29F0x0.o v29.o cat28.o i28.o i28a.o sst39.o sst39a.o w29.o unknown.o 


#all:	SYM $(MODULE)
all:	$(MODULE)

flashcalls: 
	sh -c "awk -f mkcallarray.awk flash.h >flashcalls.h|| ( rm flashcalls.h;exit -1 ) "   

SYM:
	$(CC) $(MFLAGS) -DEXPORT_SYMTAB  -c $(SMOBJECTS)


$(MODULE): $(MOBJECTS) 
	$(LD) -r $(MOBJECTS) -o $(MODDIR)/$(MODULE)

.c.o:
	$(CC) $(MFLAGS) -c -o $*.o $<

.s.o:
	$(AS) $(ASFLAGS) -o $*.o $<

.c.s:
	$(CC) $(MFLAGS) -DMODULE -D__KERNEL__ -S -o $*.s $<

clean:
	rm -f $(MOBJECTS) 
	rm -f $(MODDIR)/$(MODULE)
