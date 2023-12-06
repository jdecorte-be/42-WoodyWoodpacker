.LC0:
        .ascii  "....WOODY....\012\000"
foo:
        push    {r7, lr}
        sub     sp, sp, #16
        add     r7, sp, #0
        str     r0, [r7, #12]
        str     r1, [r7, #8]
        str     r2, [r7, #4]
        str     r3, [r7]
        movs    r2, #14
        movw    r1, #:lower16:.LC0
        movt    r1, #:upper16:.LC0
        movs    r0, #1
        bl      write
        nop
        adds    r7, r7, #16
        mov     sp, r7
        pop     {r7, pc}