; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08032ba0  4ff40051  mov.w	r1, #8192
08032ba4  0348      ldr	r0, [pc, #12] ; [0x08032bb4] = 0x58020800
08032ba6  08b5      push	{r3, lr}
08032ba8  f0f72afc  bl	#-63404 ; -> 0x08023400 ; branch_target=0x08023400
08032bac  b0fa80f0  clz	r0, r0
08032bb0  4009      lsrs	r0, r0, #5
08032bb2  08bd      pop	{r3, pc}
