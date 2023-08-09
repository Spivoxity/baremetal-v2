@ x06-arrays/func.s
@ Copyright (c) 2018-20 J. M. Spivey

        .syntax unified
        .global func

        .bss                    @ In the uninitialised data segment
        .align 2                @ Aligned on a 4-byte boundary
account:        
            .space 40               @ Reserve space for 10 integers

        .text
        .thumb_func
func:
@@@ Increment element of static array
        ldr r3, =account        @ Base of array in r3
        lsls r2, r0, #2         @ Offset of element x
        ldr r0, [r3, r2]        @ Fetch the element
        adds r0, r1             @ Increment by y
        str r0, [r3, r2]        @ Save the new value
        bx lr                   @ New value is also result
