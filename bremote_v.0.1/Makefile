# This is the makefile of the bremote application

progs = bremote 

bremoteobjs = bremote.o
libs = -lbluetooth 
CC = gcc

CFLAGS = -Wall -O2 -g 
LDFLAGS = 

all: $(progs)

bremote: $(bremoteobjs)
	$(CC) $(CFLAGS) $(LDFLAGS) $(libs) -o $@ $(bremoteobjs)
$(bremoteobjs): bremote.h

clean:
	rm -f core *.o $(progs)
realclean: clean
	rm -f .depend

depend .depend:
	$(CC) $(CFLAGS) -MM *.c > .depend
include .depend

install: 
	cp -f $(PWD)/bremote /usr/bin/bremote
	cp -f $(PWD)/btxine /usr/bin/btxine
	chmod 755 /usr/bin/bremote
	chmod 755 /usr/bin/btxine

