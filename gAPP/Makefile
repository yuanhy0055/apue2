CC=gcc

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

EXP_CF := -DTEST -g
CFLAGS += $(EXP_CF)

TARGET = utst
 
$(TARGET): $(OBJS)
	$(CC) -g -o $@ $(OBJS) -lAjpeg -L./jpeg

#	$(CC) -g -o $@ $(OBJS) -ljpeg

clean:
	rm -f $(TARGET) *.o

%.o: %.c
	$(CC) -c -o $@ $< -Ijpeg
