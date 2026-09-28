; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08026c14  70b5      push	{r4, r5, r6, lr}
08026c16  90f82c30  ldrb.w	r3, [r0, #44]
08026c1a  0546      mov	r5, r0
08026c1c  022b      cmp	r3, #2
08026c1e  d8b2      uxtb	r0, r3
08026c20  04d0      beq	#8 ; -> 0x08026c2c ; branch_target=0x08026c2c
08026c22  00f0fb04  and	r4, r0, #251
08026c26  012c      cmp	r4, #1
08026c28  01d0      beq	#2 ; -> 0x08026c2e ; branch_target=0x08026c2e
08026c2a  0120      movs	r0, #1
08026c2c  70bd      pop	{r4, r5, r6, pc}
08026c2e  0223      movs	r3, #2
08026c30  0e46      mov	r6, r1
08026c32  2868      ldr	r0, [r5]
08026c34  85f82c30  strb.w	r3, [r5, #44]
08026c38  01f0eefb  bl	#6108 ; -> 0x08028418 ; branch_target=0x08028418
08026c3c  3368      ldr	r3, [r6]
08026c3e  022b      cmp	r3, #2
08026c40  03d0      beq	#6 ; -> 0x08026c4a ; branch_target=0x08026c4a
08026c42  85f82c40  strb.w	r4, [r5, #44]
08026c46  0020      movs	r0, #0
08026c48  70bd      pop	{r4, r5, r6, pc}
08026c4a  0523      movs	r3, #5
08026c4c  85f82c30  strb.w	r3, [r5, #44]
08026c50  f9e7      b	#-14 ; -> 0x08026c46 ; branch_target=0x08026c46
