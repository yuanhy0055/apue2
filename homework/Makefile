#CC=arm-none-linux-gnueabi-gcc
CC=gcc
CXX=g++

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

EXP_CF := -DYUAN -g -Wall
EXP_LD := -g
#XP_LD := -v -g -Wl,--section-start=__hwkmains=0x8050000
CXXFLAGS += $(EXP_CF)
LDFLAGS += $(EXP_LD)
HEAD_DEP := homework.h

TARGET = h
 
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

install: h
	adb push h /mnt/asec/

clean:
	rm -f $(TARGET) *.o

##~~~~~~~~~~~~~~~~~~~~~
%.o: %.cpp $(HEAD_DEP)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

xusj.E:
	g++ -E -DYUAN -o xusj.E xusj.cpp
