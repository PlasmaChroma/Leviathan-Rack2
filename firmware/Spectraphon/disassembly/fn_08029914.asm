; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08029914  30b4      push	{r4, r5}
08029916  054c      ldr	r4, [pc, #20] ; [0x0802992c] = 0x20002094
08029918  04eb8005  add.w	r5, r4, r0, lsl #2
0802991c  0444      add	r4, r0
0802991e  6d68      ldr	r5, [r5, #4]
08029920  207a      ldrb	r0, [r4, #8]
08029922  ec68      ldr	r4, [r5, #12]
08029924  a446      mov	r12, r4
08029926  30bc      pop	{r4, r5}
08029928  6047      bx	r12
