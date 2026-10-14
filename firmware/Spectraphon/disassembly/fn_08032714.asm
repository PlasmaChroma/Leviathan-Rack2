; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08032714  10b5      push	{r4, lr}
08032716  9446      mov	r12, r2
08032718  82b0      sub	sp, #8
0803271a  1c46      mov	r4, r3
0803271c  0a46      mov	r2, r1
0803271e  6346      mov	r3, r12
08032720  0146      mov	r1, r0
08032722  0094      str	r4, [sp]
08032724  0348      ldr	r0, [pc, #12] ; [0x08032734] = 0x20014a48
08032726  f3f749fd  bl	#-50542 ; -> 0x080261bc ; branch_target=0x080261bc
0803272a  0038      subs	r0, #0
0803272c  18bf      it	ne
0803272e  0120      movne	r0, #1
08032730  02b0      add	sp, #8
08032732  10bd      pop	{r4, pc}
