
MODDIR = ../../modules

include ../../Makefile.common

MODULE = via-rhine_flash.o

MOBJECTS = via.o ../../flash/flash_proc.o
COBJECTS = via.c ../../flash/flash_proc.c

all:	$(MODULE)


$(MODULE): $(MOBJECTS) 
	$(LD) -r $(MOBJECTS) -o $(MODDIR)/$(MODULE)

.c.o: $(COBJECTS)
	$(CC) $(MFLAGS) -DEXPORT_SYMTAB -c -o $*.o $<

.s.o:
	$(AS) $(ASFLAGS) -o $*.o $<

.c.s:
	$(CC) $(MFLAGS) -DMODULE -D__KERNEL__ -S -o $*.s $<

clean:
	rm -f $(MOBJECTS)
	rm -f $(MODDIR)/$(MODULE)
