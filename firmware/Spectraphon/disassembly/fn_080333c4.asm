; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
080333c4  70b5      push	{r4, r5, r6, lr}
080333c6  0a4c      ldr	r4, [pc, #40] ; [0x080333f0] = 0x20000a20
080333c8  0601      lsls	r6, r0, #4
080333ca  0a4b      ldr	r3, [pc, #40] ; [0x080333f4] = 0x60c01000
080333cc  4ff48042  mov.w	r2, #16384
080333d0  04eb0015  add.w	r5, r4, r0, lsl #4
080333d4  a059      ldr	r0, [r4, r6]
080333d6  0434      adds	r4, #4
080333d8  0749      ldr	r1, [pc, #28] ; [0x080333f8] = 0x08045a98
080333da  03eb8000  add.w	r0, r3, r0, lsl #2
080333de  03f07ff8  bl	#12542 ; -> 0x080364e0 ; branch_target=0x080364e0
080333e2  0823      movs	r3, #8
080333e4  0022      movs	r2, #0
080333e6  3119      adds	r1, r6, r4
080333e8  a351      str	r3, [r4, r6]
080333ea  4b60      str	r3, [r1, #4]
080333ec  ea60      str	r2, [r5, #12]
080333ee  70bd      pop	{r4, r5, r6, pc}
