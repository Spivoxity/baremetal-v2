@@@ ubit-v2/neopixel.s                         
@@@ Copyright (c) 2020 J. M. Spivey
        
        .syntax unified
        .section .xram, "ax"    @ Copy the code to RAM

        .set GPIO_BASE, 0x50000500 @ Base address of GPIO registers
        .set SET, 0x8           @ Offset of GPIO_OUTSET
        .set CLR, 0xc           @ Offset of GPIO_OUTCLR

@@@ The signal for a neopixel string consists of a sequence of pixels.
@@@ Each pixel contains 24 bits, and a bit lasts 1.25 +/- 0.6 usec.
@@@ A zero bit is high for 0.4 +/- 0.15 usec then low for 0.85 usec.
@@@ A one bit is high for 0.8 +/- 0.15 usec then low for 0.45 usec
@@@ One frame is separated from the next by a low period lasting
@@@ 50 usec or longer.
@@@ 
@@@ Each bit is transmitted in three phases.
@@@   Phase 1: High for 25 cycles
@@@   Phase 2: High/Low for 26 cycles
@@@   Phase 3: Low for 29 cycles
@@@ The cycles are counted out below in comments like [2.4],
@@@ meaning cycle 4 of phase 2.
        
        .global neoframe
        .thumb_func
neoframe:       
        @@  args: r0 = pin, r1 = buffer, r2 = npixels
        push {r4, r5}

        @@ r0 -- scratch
        @@ r1 -- limit of buffer
        @@ r2 -- buffer index (-ve)
        @@ r3 -- pixel index
        @@ r4 -- bit for GPIO pin
        @@ r5 -- pointer to GPIO regs

        lsls r2, #2             @ Multiply pixel count by 4
        beq done                @ Finished if it was zero
        adds r1, r1, r2         @ Adjust r1 to end of buffer
        negs r2, r2             @ Set r2 to (-ve) offset of first pixel
        movs r3, #24            @ Set r3 to bit index in pixel
        movs r4, #1             @ Set r4 to GPIO bit
        lsls r4, r4, r0
        ldr r5, =GPIO_BASE      @ Set r5 to GPIO base address

        .align 4
nextbit:
        @@ Phase 1
        str r4, [r5, #SET]      @ [1.1] Output high
        ldr r0, [r1, r2]        @ [1.2] Fetch pixel
        nop; nop; nop; nop; nop @ [1.3]
        nop; nop; nop; nop; nop @ [1.8]
        nop; nop; nop; nop; nop @ [1.13]
        nop; nop; nop; nop; nop @ [1.18]
        nop                     @ [1.23]
        lsrs r0, r0, r3         @ [1.24] Put relevant bit in C flag
        bcs high                @ [1.25] Branch if bit is 1

        @@ Phase 2
        str r4, [r5, #CLR]      @ [2.1] Output low if bit is 0
        b cont                  @ [2.2]
high:   
        nop; nop                @ [2.3]
cont:   
        nop; nop; nop; nop; nop @ [2.5]
        nop; nop; nop; nop; nop @ [2.10]
        nop; nop; nop; nop; nop @ [2.15]
        nop; nop; nop; nop; nop @ [2.20]
        nop; nop                @ [2.25]

        @@ Phase 3
        str r4, [r5, #CLR]      @ [3.1] Output low if not already done
        nop; nop; nop; nop; nop @ [3.3]
        nop; nop; nop; nop; nop @ [3.8]
        nop; nop; nop; nop; nop @ [3.13]
        nop; nop; nop; nop; nop @ [3.18]
        nop; nop
        subs r3, r3, #1         @ [3.23] Update bit index
        beq lastbit             @ [3.24] Branch if last bit
        nop; nop; nop           @ [3.25]
        b nextbit               @ [3.28]
lastbit:        
        movs r3, #24            @ [3.26] Reset bit index
        adds r2, r2, #4         @ [3.27] Update pixel index     
        blt nextbit             @ [3.28] Go for first bit of next pixel
done:   
        pop {r4, r5}
        bx lr
