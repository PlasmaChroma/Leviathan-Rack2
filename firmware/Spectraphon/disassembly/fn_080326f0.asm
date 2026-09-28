; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080326f0  10b5      push	{r4, lr}
080326f2  9446      mov	r12, r2
080326f4  82b0      sub	sp, #8
080326f6  1c46      mov	r4, r3
080326f8  0a46      mov	r2, r1
080326fa  6346      mov	r3, r12
080326fc  0146      mov	r1, r0
080326fe  0094      str	r4, [sp]
08032700  0348      ldr	r0, [pc, #12] ; [0x08032710] = 0x20014a48
08032702  f3f771fc  bl	#-50974 ; -> 0x08025fe8 ; branch_target=0x08025fe8
08032706  0038      subs	r0, #0
08032708  18bf      it	ne
0803270a  0120      movne	r0, #1
0803270c  02b0      add	sp, #8
0803270e  10bd      pop	{r4, pc}
