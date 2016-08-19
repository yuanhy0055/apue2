commands.o: commands.cpp ring.h externs.h defines.h types.h genget.h \
 environ.h proto.h ptrarray.h netlink.h
main.o: main.cpp ./version.h ring.h externs.h defines.h proto.h
network.o: network.cpp ring.h defines.h externs.h proto.h netlink.h
ring.o: ring.cpp ring.h
sys_bsd.o: sys_bsd.cpp ring.h defines.h externs.h types.h proto.h \
 netlink.h terminal.h
telnet.o: telnet.cpp ring.h defines.h externs.h types.h environ.h \
 proto.h ptrarray.h netlink.h terminal.h
terminal.o: terminal.cpp ring.h defines.h externs.h types.h proto.h \
 terminal.h
tn3270.o: tn3270.cpp defines.h ring.h externs.h proto.h
utilities.o: utilities.cpp ring.h defines.h externs.h proto.h \
 terminal.h
genget.o: genget.cpp genget.h
environ.o: environ.cpp ring.h defines.h externs.h environ.h array.h
netlink.o: netlink.cpp netlink.h proto.h ring.h
