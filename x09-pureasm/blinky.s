@@@ x09-pureasm/blinky.s
@@@ Copyright (c) 2019 J. M. Spivey

    .thumb                      @ Blinky for Micro:bit V2
    .syntax unified
        .global __reset

    .section .vectors
    @@ Vector table at address 0
    .word __stack               @ Initial stack pointer
    .word __reset               @ Reset vector
    @@ Rest of the vectors are unused

    .text
    .thumb_func
delay:                          @ Delay in microseconds (64MHz clock)
    lsls r0, r0, #4             @ 62.5ns per iteration
repeat: 
    subs r0, #1                 @ One iteration takes 4 cycles
    nop
    bhi repeat
    bx lr

    .thumb_func
__reset:
    ldr r0, =0x50000514         @ GPIO0_DIR register
    ldr r1, =0xd1688800         @ LED pins as outputs
    str r1, [r0]
    ldr r0, =0x50000814         @ Same for GPIO1_DIR
    ldr r1, =0x00000020
    str r1, [r0]    

    ldr r4, =0x50000504         @ Address of GPIO0_OUT register
    ldr r5, =0x50000804         @ Address of GPIO1_OUT register
    ldr r6, =0x50008800         @ Bit pattern 0 for dot
    ldr r7, =0x00000020         @ Bit pattern 1 for dot

again:
    str r6, [r4]                @ Light an LED
    str r7, [r5]
    ldr r0, =500000             @ Wait 500ms
    bl delay
    mov r3, #0
    str r3, [r4]                @ All LEDs off
    str r3, [r5]
    ldr r0, =500000             @ Wait 500ms
    bl delay
    b again                     @ and repeat
