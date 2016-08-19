all: yyrcd

include ./MCONFIG
include ./MRULES

# -DAUTHENTICATE

# If having unused tty devices root.root and mode 600 bugs you,
# take out -DPARANOID_TTYS.

CFLAGS += '-DISSUE_FILE="/etc/issue.net"' -DPARANOID_TTYS \
	   -DNO_REVOKE -DKLUDGELINEMODE -DDIAGNOSTICS \
	   -DLOGIN_WRAPPER=\"/bin/login\"
# LIBS += $(LIBTERMCAP)

OBJS = main.o state.o termstat.o slc.o sys_term.o utility.o \
	global.o setproctitle.o

# authenc.o (empty)

# logout.o logwtmp.o (now from -lutil)


yyrcd: $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(OBJS): defs.h ext.h pathnames.h telnetd.h logwtmp.h logout.h setproctitle.h
#telnetd.o: ./version.h

install: yyrcd
	install -m$(DAEMONMODE) yyrcd $(INSTALLROOT)$(SBINDIR)/in.telnetd

clean:
	rm -f *.o yyrcd
