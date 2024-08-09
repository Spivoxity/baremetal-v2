# x17-drivers/Makefile
# Copyright (c) 2020-21 J. M. Spivey
# genmake x17-drivers myprimes.hex part3 {} myserial.o

all: myprimes.hex

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
INCLUDE = -I ../microbian
DEPS = myserial.o
STARTUP = ../microbian/microbian.a ../microbian/startup.o
LIBS = -lgcc -lc
LDSCRIPT = ../microbian/nRF52833.ld

vpath %.h ../microbian

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

../microbian/microbian.a ../microbian/startup.o:
	$(MAKE) -C $(@D) all

# Disable the default rules for building executables
SORRY = echo "Please say 'make $@.hex' to compile '$@'"
%: %.s; @$(SORRY)
%: %.o; @$(SORRY)

clean:
	rm -f *.hex *.elf *.bin *.map *.o

# Don't delete intermediate files
.SECONDARY:

###

myprimes.o myserial.o: microbian.h
myprimes.o: lib.h
