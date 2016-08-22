
MODDIR = ../modules
BUILDDIR = ../build2.6

include ../Makefile.common

MODULE = ct.o

SMOBJECTS = $(shell grep -l "^EXPORT_SYMBOL" *.c)

MOBJECTSC = ct_hardware.o put_c.o get_c.o ../flash/flash_proc.o
MOBJECTSA = ct_hardware.o put.o get.o ../flash/flash_proc.o
ifneq ($(AS), )
MOBJECTS = $(MOBJECTSA)	
else
MOBJECTS = $(MOBJECTSC)	
endif

all:	$(MODULE)

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
	rm -f $(MOBJECTSC)  $(MOBJECTSA)
	rm -f $(MODDIR)/$(MODULE)
	rm -f $(BUILDDIR)/$(MODULE)
