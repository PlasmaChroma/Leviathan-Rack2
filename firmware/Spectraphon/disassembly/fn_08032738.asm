; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08032738  08b5      push	{r3, lr}
0803273a  0348      ldr	r0, [pc, #12] ; [0x08032748] = 0x20014a48
0803273c  f4f72cfa  bl	#-48040 ; -> 0x08026b98 ; branch_target=0x08026b98
08032740  0438      subs	r0, #4
08032742  18bf      it	ne
08032744  0120      movne	r0, #1
08032746  08bd      pop	{r3, pc}
