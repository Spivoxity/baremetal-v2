# x02-instrs/Makefile
# Copyright (c) 2020-21 J. M. Spivey

# genmake x02-instrs func.hex part1 {fmain.o lib.o}

all: func.hex

CC = arm-none-eabi-gcc
CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

# Use ld for linking to help with the explanation
%.elf: %.o fmain.o lib.o startup.o libgcc.a
	$(LD) -T nRF52833.ld $^ -o $@ -Map $*.map
	$(SIZE) $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

# Nuke the default rules for building executables
SORRY = echo "Please say 'make $@.hex' to compile '$@'"
%: %.s; @$(SORRY)
%: %.o; @$(SORRY)

# Find libgcc: find the first part of the gcc search path.
GCCLIB := $(shell $(CC) --print-search-dirs \
		| sed -n '/^libraries: =\([^:]*\)\/:.*/s//\1/p')
# Then try a couple of likely subdirectories for the library.
vpath libgcc.a $(GCCLIB)/thumb/v6-m \
		$(GCCLIB)/armv6-m $(GCCLIB)/thumb/v6-m/nofp

clean:
	rm -f *.hex *.elf *.bin *.map *.o

# Don't delete intermediate files
.SECONDARY:

###

fmain.o: lib.h hardware.h
lib.o: lib.h
startup.o: hardware.h
