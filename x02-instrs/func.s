@ x02-instrs/func.s
@ Copyright (c) 2018-20 J. M. Spivey

        .syntax unified
        .global func

        .text
        .thumb_func
func:
@ ----------------
@ Two parameters are in registers r0 and r1

        adds r0, r0, r1         @ Add r0 and r1, result in r0

@ Result is now in register r0
@ ----------------
        bx lr                   @ Return to the caller

