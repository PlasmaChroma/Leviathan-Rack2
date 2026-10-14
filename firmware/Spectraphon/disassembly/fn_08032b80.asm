; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08032b80  08b5      push	{r3, lr}
08032b82  0349      ldr	r1, [pc, #12] ; [0x08032b90] = 0x200033ec
08032b84  0348      ldr	r0, [pc, #12] ; [0x08032b94] = 0x08049a98
08032b86  f9f76bfa  bl	#-27434 ; -> 0x0802c060 ; branch_target=0x0802c060
08032b8a  034b      ldr	r3, [pc, #12] ; [0x08032b98] = 0x200033f0
08032b8c  1870      strb	r0, [r3]
08032b8e  08bd      pop	{r3, pc}
