@ x05-subrs/fact.s
@ Copyright (c) 2018-20 J. M. Spivey

        .syntax unified
        .global func

        .text
        .thumb_func
func:
@@@ Factorials using a loop: keep x in r4, y in r5 and maintain the
@@@ relationship fac(a) = fac(x) * y
    push {r4, r5, lr}           @ Save registers and return address
    movs r4, r0                 @ Set k to n
    movs r5, #1                 @ Set r to 1

again:
    cmp r4, #2          @ Test whether k < 2
    blo exit            @ If so, exit the loop

        @@ Call mult to multiply r by k
    movs r0, r4         @ Prepare the call: first argument is k
    movs r1, r5         @ Second argument is r
    bl mult             @ Call the function
    movs r5, r0         @ Put result in r

    subs r4, r4, #1     @ Decrease k by 1
    b again             @ Repeat the loop

exit:
    movs r0, r5         @ Put result in r0
    pop {r4, r5, pc}    @ Restore regs and return

    .thumb_func
mult:   
@@@ Keep x and y in r0 and r1; compute the result z in r2, maintaining
@@@ the relationship a * b = x * y + z

    movs r2, #0
loop:   
    cmp r0, #0
    beq done

    subs r0, r0, #1
    adds r2, r2, r1
    b loop

done:
    movs r0, r2
    bx lr

