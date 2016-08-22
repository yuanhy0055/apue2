CFLAGS := -Wall -Wstrict-prototypes
LDFLAGS :=

TARGET	= U_DATA
OBJS	= us.o daemon.o

CC		= gcc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
