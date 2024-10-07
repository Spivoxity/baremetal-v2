# microbian/Makefile
# Copyright (c) 2020-24 J. M. Spivey

all: microbian.a startup.o

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
AR = arm-none-eabi-ar
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy

CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding

%.a:
	$(AR) cr $@ $^

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(CPU) $< -o $@

clean:
	rm -f *.o microbian.a

###

DRIVERS = timer.o serial.o i2c.o radio.o display.o

MICROBIAN = microbian.o mpx.o $(DRIVERS) lib.o

microbian.a: $(MICROBIAN)

$(MICROBIAN) startup.o: microbian.h hardware.h lib.h
