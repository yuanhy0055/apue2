
MODDIR = ../modules

include ../Makefile.common

MODULE = ide_flash.o

MOBJECTS = ide_hardware.o ../flash/flash_proc.o

all:	$(MODULE)


$(MODULE): $(MOBJECTS)
	$(LD) -r $(MOBJECTS) -o $(MODDIR)/$(MODULE)

SYM: 
	$(CC) $(MFLAGS) -DEXPORT_SYMTAB  -c $(SMOBJECTS)

.c.o:
	$(CC) $(MFLAGS) -c -o $*.o $<

.s.o:
	$(AS) $(ASFLAGS) -o $*.o $<

.c.s:
	$(CC) $(MFLAGS) -DMODULE -D__KERNEL__ -S -o $*.s $<

clean:
	rm -f $(MOBJECTS)
	rm -f $(MODDIR)/$(MODULE)
