; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802c060  104b      ldr	r3, [pc, #64] ; [0x0802c0a4] = 0x20002094
0802c062  10b4      push	{r4}
0802c064  5c7a      ldrb	r4, [r3, #9]
0802c066  ccb9      cbnz	r4, #50 ; -> 0x0802c09c ; branch_target=0x0802c09c
0802c068  0246      mov	r2, r0
0802c06a  04f0ff00  and	r0, r4, #255
0802c06e  5c7a      ldrb	r4, [r3, #9]
0802c070  1855      strb	r0, [r3, r4]
0802c072  5c7a      ldrb	r4, [r3, #9]
0802c074  03eb8404  add.w	r4, r3, r4, lsl #2
0802c078  6260      str	r2, [r4, #4]
0802c07a  5c7a      ldrb	r4, [r3, #9]
0802c07c  5a7a      ldrb	r2, [r3, #9]
0802c07e  1c44      add	r4, r3
0802c080  2072      strb	r0, [r4, #8]
0802c082  541c      adds	r4, r2, #1
0802c084  3032      adds	r2, #48
0802c086  e4b2      uxtb	r4, r4
0802c088  5c72      strb	r4, [r3, #9]
0802c08a  3a24      movs	r4, #58
0802c08c  2f23      movs	r3, #47
0802c08e  0a70      strb	r2, [r1]
0802c090  4c70      strb	r4, [r1, #1]
0802c092  c870      strb	r0, [r1, #3]
0802c094  5df8044b  ldr	r4, [sp], #4
0802c098  8b70      strb	r3, [r1, #2]
0802c09a  7047      bx	lr
0802c09c  0120      movs	r0, #1
0802c09e  5df8044b  ldr	r4, [sp], #4
0802c0a2  7047      bx	lr
