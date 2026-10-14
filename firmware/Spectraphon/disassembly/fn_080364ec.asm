; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
080364ec  10b5      push	{r4, lr}
080364ee  11f8014b  ldrb	r4, [r1], #1
080364f2  03f8014f  strb	r4, [r3, #1]!
080364f6  9142      cmp	r1, r2
080364f8  f9d1      bne	#-14 ; -> 0x080364ee ; branch_target=0x080364ee
080364fa  10bd      pop	{r4, pc}
