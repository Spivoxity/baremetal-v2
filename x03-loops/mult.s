@ x03-loops/mult.s
@ Copyright (c) 2018-20 J. M. Spivey

        .syntax unified
        .global func

        .text
        .thumb_func
func:
@ ----------------
@ Two parameters a, b are in registers r0 and r1

@@@ Keep x and y in r0 and r1; compute the result z in r2, 
@@@ maintaining the relationship a * b = x * y + z.

        movs r2, #0             @ Initially z = 0
loop:   
        cmp r1, #0              @ Is y = 0?
        beq done                @ If so, finished

        subs r1, r1, #1         @ Decrease y by 1
        adds r2, r2, r0         @ Increase z by x
        b loop                  @ Repeat
done:
        movs r0, r2             @ Put z in r0

@ Result is now in register r0
@ ----------------
        bx lr                   @ Return to the caller

