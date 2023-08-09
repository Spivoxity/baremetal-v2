# x31-adc/Makefile
# Copyright (c) 2020-21 J. M. Spivey

# genmake x31-adc lights.hex part3 adc.o

all: lights.hex

CC = arm-none-eabi-gcc
CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
INCLUDE = -I ../microbian
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

vpath %.h ../microbian

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

%.elf: %.o adc.o ../microbian/microbian.a ../microbian/startup.o
	$(CC) $(CPU) $(CFLAGS) -T ../microbian/nRF52833.ld \
		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map
	$(SIZE) $@

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

../microbian/microbian.a:
	$(MAKE) -C $(@D) all

# Nuke the default rules for building executables
SORRY = echo "Please say 'make $@.hex' to compile '$@'"
%: %.s; @$(SORRY)
%: %.o; @$(SORRY)

clean:
	rm -f *.hex *.elf *.bin *.map *.o

# Don't delete intermediate files
.SECONDARY:

###

lights.o adc.o: microbian.h lib.h hardware.h
