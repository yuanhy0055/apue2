all: telnetd

include ./MCONFIG
include ./MRULES

# -DAUTHENTICATE

# If having unused tty devices root.root and mode 600 bugs you,
# take out -DPARANOID_TTYS.

CFLAGS += '-DISSUE_FILE="/etc/issue.net"' -DPARANOID_TTYS \
	   -DNO_REVOKE -DKLUDGELINEMODE -DDIAGNOSTICS \
	   -DLOGIN_WRAPPER=\"/bin/login\"
# LIBS += $(LIBTERMCAP)

OBJS = telnetd.o state.o termstat.o slc.o sys_term.o utility.o \
	global.o setproctitle.o

# authenc.o (empty)

# logout.o logwtmp.o (now from -lutil)


telnetd: $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@
	cp -f $@ yyrcd

$(OBJS): defs.h ext.h pathnames.h telnetd.h logwtmp.h logout.h setproctitle.h
telnetd.o: ./version.h

install: telnetd
	install -m$(DAEMONMODE) telnetd $(INSTALLROOT)$(SBINDIR)/in.telnetd

clean:
	rm -f *.o telnetd 

