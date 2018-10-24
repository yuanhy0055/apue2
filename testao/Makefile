TARGET = go
CC     = gcc

SRCS   := $(wildcard *.c)
OBJS   := $(patsubst %.c, out/obj/%.o, $(SRCS))

CFLAGS   +=
LDFLAGS += -lutil

$(TARGET): $(OBJS)
	gcc -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET) omxplayer.log omxplayer.old.log

out/obj/%.o: %.c
	@echo "\t\tCC->$<"
	@$(CC) $(CFLAGS) $< -c -o $@

#%.o: %.cc
#	$(CXX) $(CXXFLAGS) $< -c

TST_TGT := getfile
test: $(TST_TGT)

clrt:
	rm -f $(TST_TGT)

getfile: getfile.c
	gcc -o $@ -DTEST_SELF $^	
