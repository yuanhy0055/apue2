ASFLAGS	:= -gstabs

TARGET	:= bz.img
OBJS	:= yyrun.o

all: $(TARGET)

$(TARGET): bz.coff
	objcopy -O binary $< $@

bz.coff: $(OBJS)
	ld -Map bz.map -r $< -o $@


clean:
	rm -f $(TARGET) $(OBJS) bz.coff bz.map

# DEfault Rule
%.o: %.S
	gcc -E $< | as $(ASFLAGS) -o $@

%.o: %.c
	gcc -c -Wall -Wstrict-prototypes -o $@ $<

