; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080333fc  70b5      push	{r4, r5, r6, lr}
080333fe  0a4c      ldr	r4, [pc, #40] ; [0x08033428] = 0x20000920
08033400  0601      lsls	r6, r0, #4
08033402  0a4b      ldr	r3, [pc, #40] ; [0x0803342c] = 0x60001000
08033404  4ff48042  mov.w	r2, #16384
08033408  04eb0015  add.w	r5, r4, r0, lsl #4
0803340c  a059      ldr	r0, [r4, r6]
0803340e  0434      adds	r4, #4
08033410  0749      ldr	r1, [pc, #28] ; [0x08033430] = 0x08045a98
08033412  03eb8000  add.w	r0, r3, r0, lsl #2
08033416  03f063f8  bl	#12486 ; -> 0x080364e0 ; branch_target=0x080364e0
0803341a  0823      movs	r3, #8
0803341c  0022      movs	r2, #0
0803341e  3119      adds	r1, r6, r4
08033420  a351      str	r3, [r4, r6]
08033422  4b60      str	r3, [r1, #4]
08033424  ea60      str	r2, [r5, #12]
08033426  70bd      pop	{r4, r5, r6, pc}
