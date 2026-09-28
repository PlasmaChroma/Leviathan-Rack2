; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: prologue_heuristic
08035d98  30b5      push	{r4, r5, lr}
08035d9a  d3f8f020  ldr.w	r2, [r3, #240]
08035d9e  0025      movs	r5, #0
08035da0  124c      ldr	r4, [pc, #72] ; [0x08035dec] = 0x20014c90
08035da2  85b0      sub	sp, #20
08035da4  42f00202  orr	r2, r2, #2
08035da8  2561      str	r5, [r4, #16]
08035daa  2046      mov	r0, r4
08035dac  c4e90155  strd	r5, r5, [r4, #4]
08035db0  c3f8f020  str.w	r2, [r3, #240]
08035db4  44f24f62  movw	r2, #17999
08035db8  d3f8f030  ldr.w	r3, [r3, #240]
08035dbc  e260      str	r2, [r4, #12]
08035dbe  03f00203  and	r3, r3, #2
08035dc2  2160      str	r1, [r4]
08035dc4  0093      str	r3, [sp]
08035dc6  009b      ldr	r3, [sp]
08035dc8  f1f77cfd  bl	#-58632 ; -> 0x080278c4 ; branch_target=0x080278c4
08035dcc  2023      movs	r3, #32
08035dce  01a9      add	r1, sp, #4
08035dd0  2046      mov	r0, r4
08035dd2  0395      str	r5, [sp, #12]
08035dd4  0193      str	r3, [sp, #4]
08035dd6  f2f7f9f9  bl	#-56334 ; -> 0x080281cc ; branch_target=0x080281cc
08035dda  2046      mov	r0, r4
08035ddc  f1f70efe  bl	#-58340 ; -> 0x080279fc ; branch_target=0x080279fc
08035de0  05b0      add	sp, #20
08035de2  30bd      pop	{r4, r5, pc}
