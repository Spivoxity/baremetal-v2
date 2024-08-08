# x11-interrupt/Makefile
# Copyright (c) 2020-21 J. M. Spivey
# genmake x11-interrupt primes2.hex part2 {} lib.o

all: primes2.hex

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
DEPS = lib.o
STARTUP = startup.o
LIBS = -lgcc
LDSCRIPT = nRF52833.ld

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

%.elf: %.o $(DEPS) $(STARTUP)
	$(CC) $(CPU) $(CFLAGS) -T $(LDSCRIPT) \
		$^ -nostdlib $(LIBS) -o $@ -Wl,-Map,$*.map
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

primes2.o: hardware.h lib.h
lib.o: lib.h
startup.o: hardware.h
