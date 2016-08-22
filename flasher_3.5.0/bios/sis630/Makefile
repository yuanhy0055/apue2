
MODDIR = ../../modules

include ../../Makefile.common

MODULE = sis630_flash.o

MOBJECTS = sis630.o ../../flash/flash_proc.o

all:	$(MODULE)


$(MODULE): $(MOBJECTS)
	$(LD) -r $(MOBJECTS) -o $(MODDIR)/$(MODULE)

.c.o:
	$(CC) $(MFLAGS) -DEXPORT_SYMTAB -c -o $*.o $<

.s.o:
	$(AS) $(ASFLAGS) -o $*.o $<

.c.s:
	$(CC) $(MFLAGS) -DMODULE -D__KERNEL__ -S -o $*.s $<

clean:
	rm -f $(MOBJECTS)
	rm -f $(MODDIR)/$(MODULE)
