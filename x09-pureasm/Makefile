# x09-pureasm/Makefile
# Copyright (c) 2020-21 J. M. Spivey
# genmake x09-pureasm blinky.hex pureasm {no-startup use-ld no-gcclib}

all: blinky.hex

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
LIBS = -lgcc
LDSCRIPT = nRF52833.ld

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

# Use ld for linking to help with the explanation
%.elf: %.o $(DEPS) $(STARTUP)
	$(LD) -T $(LDSCRIPT) $^ -o $@ -Map $*.map
	$(SIZE) $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

# Disable the default rules for building executables
SORRY = echo "Please say 'make $@.hex' to compile '$@'"
%: %.s; @$(SORRY)
%: %.o; @$(SORRY)

clean:
	rm -f *.hex *.elf *.bin *.map *.o

# Don't delete intermediate files
.SECONDARY:

###

# empty
