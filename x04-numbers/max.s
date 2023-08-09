@ x04-numbers/max.s
@ Copyright (c) 2018-20 J. M. Spivey

        .syntax unified
        .global func

        .text
        .thumb_func
func:
@ ----------------
@ Two parameters a, b are in registers r0 and r1

@@@ Move larger of a and b to r0

        cmp r0, r1              @ Compare a and b
        bgt skip                @ Skip next instruction if a is larger
        movs r0, r1             @ Replace a with b
skip:   

@ Result is now in register r0
@ ----------------
        bx lr                   @ Return to the caller

