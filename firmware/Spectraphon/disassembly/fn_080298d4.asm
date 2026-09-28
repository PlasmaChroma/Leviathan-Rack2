; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080298d4  074b      ldr	r3, [pc, #28] ; [0x080298f4] = 0x20002094
080298d6  1a5c      ldrb	r2, [r3, r0]
080298d8  52b9      cbnz	r2, #20 ; -> 0x080298f0 ; branch_target=0x080298f0
080298da  03eb8002  add.w	r2, r3, r0, lsl #2
080298de  1918      adds	r1, r3, r0
080298e0  4ff0010c  mov.w	r12, #1
080298e4  5268      ldr	r2, [r2, #4]
080298e6  03f800c0  strb.w	r12, [r3, r0]
080298ea  087a      ldrb	r0, [r1, #8]
080298ec  1368      ldr	r3, [r2]
080298ee  1847      bx	r3
080298f0  0020      movs	r0, #0
080298f2  7047      bx	lr
