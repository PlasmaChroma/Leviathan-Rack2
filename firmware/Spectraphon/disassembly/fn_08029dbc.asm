; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08029dbc  2de9f041  push.w	{r4, r5, r6, r7, r8, lr}
08029dc0  c66a      ldr	r6, [r0, #44]
08029dc2  8e42      cmp	r6, r1
08029dc4  02d1      bne	#4 ; -> 0x08029dcc ; branch_target=0x08029dcc
08029dc6  0020      movs	r0, #0
08029dc8  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08029dcc  0446      mov	r4, r0
08029dce  0d46      mov	r5, r1
08029dd0  4078      ldrb	r0, [r0, #1]
08029dd2  e378      ldrb	r3, [r4, #3]
08029dd4  04f13007  add.w	r7, r4, #48
08029dd8  5bb9      cbnz	r3, #22 ; -> 0x08029df2 ; branch_target=0x08029df2
08029dda  0123      movs	r3, #1
08029ddc  3946      mov	r1, r7
08029dde  2a46      mov	r2, r5
08029de0  fff78afd  bl	#-1260 ; -> 0x080298f8 ; branch_target=0x080298f8
08029de4  10b1      cbz	r0, #4 ; -> 0x08029dec ; branch_target=0x08029dec
08029de6  0120      movs	r0, #1
08029de8  4ff0ff35  mov.w	r5, #4294967295
08029dec  e562      str	r5, [r4, #44]
08029dee  bde8f081  pop.w	{r4, r5, r6, r7, r8, pc}
08029df2  0123      movs	r3, #1
08029df4  3246      mov	r2, r6
08029df6  3946      mov	r1, r7
08029df8  fff78cfd  bl	#-1256 ; -> 0x08029914 ; branch_target=0x08029914
08029dfc  d0b9      cbnz	r0, #52 ; -> 0x08029e34 ; branch_target=0x08029e34
08029dfe  226a      ldr	r2, [r4, #32]
08029e00  a369      ldr	r3, [r4, #24]
08029e02  b21a      subs	r2, r6, r2
08029e04  e070      strb	r0, [r4, #3]
08029e06  9a42      cmp	r2, r3
08029e08  12d2      bhs	#36 ; -> 0x08029e30 ; branch_target=0x08029e30
08029e0a  94f80280  ldrb.w	r8, [r4, #2]
08029e0e  b8f1010f  cmp.w	r8, #1
08029e12  01d8      bhi	#2 ; -> 0x08029e18 ; branch_target=0x08029e18
08029e14  0ce0      b	#24 ; -> 0x08029e30 ; branch_target=0x08029e30
08029e16  a369      ldr	r3, [r4, #24]
08029e18  1e44      add	r6, r3
08029e1a  08f1ff38  add.w	r8, r8, #4294967295
08029e1e  0123      movs	r3, #1
08029e20  3946      mov	r1, r7
08029e22  3246      mov	r2, r6
08029e24  6078      ldrb	r0, [r4, #1]
08029e26  fff775fd  bl	#-1302 ; -> 0x08029914 ; branch_target=0x08029914
08029e2a  b8f1010f  cmp.w	r8, #1
08029e2e  f2d1      bne	#-28 ; -> 0x08029e16 ; branch_target=0x08029e16
08029e30  6078      ldrb	r0, [r4, #1]
08029e32  d2e7      b	#-92 ; -> 0x08029dda ; branch_target=0x08029dda
08029e34  0120      movs	r0, #1
08029e36  c7e7      b	#-114 ; -> 0x08029dc8 ; branch_target=0x08029dc8
