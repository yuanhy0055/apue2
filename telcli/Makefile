all: telnet

include ./MCONFIG
include ./MRULES

#CXXFLAGS:=$(patsubst -O2, -g, $(CXXFLAGS))

# -DAUTHENTICATE
CXXFLAGS += -DUSE_TERMIO -DKLUDGELINEMODE
LIBS = $(LIBTERMCAP)

SRCS = commands.cpp main.cpp network.cpp ring.cpp sys_bsd.cpp telnet.cpp \
	terminal.cpp utilities.cpp genget.cpp environ.cpp netlink.cpp

OBJS = $(patsubst %.cpp, %.o, $(SRCS))

telnet: $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LIBS) -o $@

include depend.mk
depend:
	$(CXX) $(CXXFLAGS) -MM $(SRCS) >depend.mk

install: telnet
	echo install -m$(BINMODE) telnet $(INSTALLROOT)$(BINDIR)

clean:
	rm -f *.o telnet

