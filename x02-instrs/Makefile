# x02-instrs/Makefile
# Copyright (c) 2020-21 J. M. Spivey
# genmake x02-instrs func.hex part1 use-ld fmain.o lib.o

all: func.hex

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
DEPS = fmain.o lib.o
STARTUP = startup.o
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

# Find libgcc: use the first component of the gcc search path.
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
