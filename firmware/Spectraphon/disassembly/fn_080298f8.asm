; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080298f8  30b4      push	{r4, r5}
080298fa  054c      ldr	r4, [pc, #20] ; [0x08029910] = 0x20002094
080298fc  04eb8005  add.w	r5, r4, r0, lsl #2
08029900  0444      add	r4, r0
08029902  6d68      ldr	r5, [r5, #4]
08029904  207a      ldrb	r0, [r4, #8]
08029906  ac68      ldr	r4, [r5, #8]
08029908  a446      mov	r12, r4
0802990a  30bc      pop	{r4, r5}
0802990c  6047      bx	r12
