Bare metal micro:bit &ndash; code for V2
----

This repository contains code for the experiments that are described
in the book,
[_Bare metal micro:bit_](https://spivey.oriel.ox.ac.uk/baremetal),
configured for the V2 micro:bit with an nRF52833
microcontroller chip.

Note than in August, 2024, I published a new revision of the code that
uses a changed convention `UART.TXD` for referring to device
registers, in place of the convention `UART_TXD` that was used before.
This means changes to almost every source file, and will break any
code that others have added to a fork of the repository.  This won't
happen again.

Each experiment has a separate directory containing all the files it needs:
| Directory | Description |
|:----------|:------------|
| `x01-echo` | Building a program: _Check that you can build and upload a simple program_. |
| `x02-instrs` | Machine instructions: _Investigate the effect of single machine instructions_. |
| `x03-loops` | Loops: _Write programs for multiplication and division that contain loops_. |
| `x04-numbers` | Numbers: _Explore number representations and conditional branches_. |
| `x05-subrs` | Subroutines: _Learn how to define and call subroutines to give structure to a larger program_. |
| `x06-arrays` | Memory and arrays: _Exploit instructions that load and store data in RAM_. |
| `x07-hack` | A buffer overrun attack: _Build a working (but harmless) model of a computer virus_. |
| `x08-heart` | Digital input/output: _Use device registers to control I/O pins and light LEDs_. |
| `x09-pureasm` | Pure assembly language: _Flash an LED with a minimal program written in assembly language_. |
| `x10-serial` | Serial communication: _Use a serial device to transmit characters_. |
| `x11-interrupts` | Interrupts for I/O: _Control the serial device with interrupts to free the processor_. |
| `x12-intrmech` | Interrupt mechanism: _Plot gaps in a waveform to measure the time needed to handle interrupts_. |
| `x13-neopixels` | Neopixels: _Use assembly language to make a bit-banged implementation of the protocol for WS2812 'NeoPixel' LEDs_. |
| `x14-processes` | Processes: _Use micro:bian processes to perform multiple tasks concurrently_. |
| `x15-messages` | Messages: _Use messages to communicate between processes_. |
| `x16-sync` | Synchronisation: _Synchronise the actions of multiple processes by making them exchange messages_. |
| `x17-drivers` | Device drivers: _Manage I/O devices using driver processes that receive interrupts as messages_. |
| `x18-level` | I2C-based spirit level: _Access the micro:bit accelerometer over the I2C bus to make a 2D spirit level_. |
| `x19-servos` | Servo motors: _Use a timer to generate the signals needed to control servo motors_. |
| `x20-radio` | Radio: _Communicate between multiple micro:bits using the in-built radio_. |
| `x21-car` | Remote-controlled car: _Use servos and radio to make a remote-controlled car_. |
| `x31-adc` | Analog to digital converter: _A device driver for the analog to digital converter_. |
| `x32-infrared` | Infrared remote control: _A program that uses GPIO interrupts to decode signals from an infrared remote control_. |
| `x33-clock` | Neopixel clock: _Software for a Neopixel-based clock_. |
