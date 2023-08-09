# microbian/Makefile
# Copyright (c) 2020 J. M. Spivey

all: microbian.a startup.o

CPU = -mcpu=cortex-m4 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar

DRIVERS = timer.o serial.o i2c.o radio.o display.o

MICROBIAN = microbian.o mpx-m4.o $(DRIVERS) lib.o

microbian.a: $(MICROBIAN)
	$(AR) cr $@ $^

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) -c $< -o $@ 

%.o: %.s
	$(AS) $(CPU) $< -o $@

clean: force
	rm -f microbian.a *.o

force:

###

$(MICROBIAN) startup.o: microbian.h hardware.h lib.h
