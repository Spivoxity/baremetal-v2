# x07-hack/Makefile
# Copyright (c) 2020-21 J. M. Spivey
# genmake x07-hack {total.hex attack squirt} hack hack lib.o

all: total.hex attack squirt

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

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

%.elf: %.o $(DEPS) $(STARTUP)
	$(CC) $(CPU) $(CFLAGS) -T device.ld \
		$^ -nostdlib $(LIBS) -o $@ -Wl,-Map,$*.map
	$(SIZE) $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

attack: attack.o
	$(OBJCOPY) -O binary attack.o attack.bin
	hexdump -v -e '/4 "%d\n"' attack.bin >attack

squirt: squirt.c
	gcc $< -o $@

# Disable the default rules for building executables
SORRY = echo "Please say 'make $@.hex' to compile '$@'"
%: %.s; @$(SORRY)
%: %.o; @$(SORRY)

clean:
	rm -f *.hex *.elf *.bin *.map *.o attack squirt

# Don't delete intermediate files
.SECONDARY:

###

total.o: lib.h hardware.h
startup.o: hardware.h
lib.o: lib.h
