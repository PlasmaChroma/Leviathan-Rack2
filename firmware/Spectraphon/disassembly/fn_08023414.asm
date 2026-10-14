; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08023414  4ff0b042  mov.w	r2, #1476395008
08023418  d2f88810  ldr.w	r1, [r2, #136]
0802341c  0142      tst	r1, r0
0802341e  00d1      bne	#0 ; -> 0x08023422 ; branch_target=0x08023422
08023420  7047      bx	lr
08023422  08b5      push	{r3, lr}
08023424  c2f88800  str.w	r0, [r2, #136]
08023428  0ff02ef9  bl	#62044 ; -> 0x08032688 ; branch_target=0x08032688
0802342c  08bd      pop	{r3, pc}
