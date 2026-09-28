; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802a10c  2de9f843  push.w	{r3, r4, r5, r6, r7, r8, r9, lr}
0802a110  90f80090  ldrb.w	r9, [r0]
0802a114  0546      mov	r5, r0
0802a116  0e46      mov	r6, r1
0802a118  1446      mov	r4, r2
0802a11a  b9f1020f  cmp.w	r9, #2
0802a11e  08d0      beq	#16 ; -> 0x0802a132 ; branch_target=0x0802a132
0802a120  b9f1030f  cmp.w	r9, #3
0802a124  42d0      beq	#132 ; -> 0x0802a1ac ; branch_target=0x0802a1ac
0802a126  b9f1010f  cmp.w	r9, #1
0802a12a  14d0      beq	#40 ; -> 0x0802a156 ; branch_target=0x0802a156
0802a12c  0220      movs	r0, #2
0802a12e  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
0802a132  016a      ldr	r1, [r0, #32]
0802a134  01eb1621  add.w	r1, r1, r6, lsr #8
0802a138  fff740fe  bl	#-896 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a13c  0028      cmp	r0, #0
0802a13e  f6d1      bne	#-20 ; -> 0x0802a12e ; branch_target=0x0802a12e
0802a140  7600      lsls	r6, r6, #1
0802a142  05f13003  add.w	r3, r5, #48
0802a146  06f4ff76  and	r6, r6, #510
0802a14a  9c55      strb	r4, [r3, r6]
0802a14c  9a19      adds	r2, r3, r6
0802a14e  c4f30724  ubfx	r4, r4, #8, #8
0802a152  5470      strb	r4, [r2, #1]
0802a154  3de0      b	#122 ; -> 0x0802a1d2 ; branch_target=0x0802a1d2
0802a156  016a      ldr	r1, [r0, #32]
0802a158  06eb5608  add.w	r8, r6, r6, lsr #1
0802a15c  01eb5821  add.w	r1, r1, r8, lsr #9
0802a160  fff72cfe  bl	#-936 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a164  0028      cmp	r0, #0
0802a166  e2d1      bne	#-60 ; -> 0x0802a12e ; branch_target=0x0802a12e
0802a168  08f10107  add.w	r7, r8, #1
0802a16c  f207      lsls	r2, r6, #31
0802a16e  c8f30803  ubfx	r3, r8, #0, #9
0802a172  05f13008  add.w	r8, r5, #48
0802a176  4fea5721  lsr.w	r1, r7, #9
0802a17a  2fd5      bpl	#94 ; -> 0x0802a1dc ; branch_target=0x0802a1dc
0802a17c  18f80320  ldrb.w	r2, [r8, r3]
0802a180  2846      mov	r0, r5
0802a182  02f00f02  and	r2, r2, #15
0802a186  42ea0412  orr.w	r2, r2, r4, lsl #4
0802a18a  08f80320  strb.w	r2, [r8, r3]
0802a18e  2b6a      ldr	r3, [r5, #32]
0802a190  85f80390  strb.w	r9, [r5, #3]
0802a194  1944      add	r1, r3
0802a196  fff711fe  bl	#-990 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a19a  0028      cmp	r0, #0
0802a19c  c7d1      bne	#-114 ; -> 0x0802a12e ; branch_target=0x0802a12e
0802a19e  c7f30802  ubfx	r2, r7, #0, #9
0802a1a2  c4f30713  ubfx	r3, r4, #4, #8
0802a1a6  4244      add	r2, r8
0802a1a8  1370      strb	r3, [r2]
0802a1aa  12e0      b	#36 ; -> 0x0802a1d2 ; branch_target=0x0802a1d2
0802a1ac  016a      ldr	r1, [r0, #32]
0802a1ae  01ebd611  add.w	r1, r1, r6, lsr #7
0802a1b2  fff703fe  bl	#-1018 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a1b6  0028      cmp	r0, #0
0802a1b8  b9d1      bne	#-142 ; -> 0x0802a12e ; branch_target=0x0802a12e
0802a1ba  b600      lsls	r6, r6, #2
0802a1bc  05f13002  add.w	r2, r5, #48
0802a1c0  24f07044  bic	r4, r4, #4026531840
0802a1c4  06f4fe76  and	r6, r6, #508
0802a1c8  9359      ldr	r3, [r2, r6]
0802a1ca  03f07043  and	r3, r3, #4026531840
0802a1ce  2343      orrs	r3, r4
0802a1d0  9351      str	r3, [r2, r6]
0802a1d2  0123      movs	r3, #1
0802a1d4  0020      movs	r0, #0
0802a1d6  eb70      strb	r3, [r5, #3]
0802a1d8  bde8f883  pop.w	{r3, r4, r5, r6, r7, r8, r9, pc}
0802a1dc  08f80340  strb.w	r4, [r8, r3]
0802a1e0  2846      mov	r0, r5
0802a1e2  2b6a      ldr	r3, [r5, #32]
0802a1e4  85f80390  strb.w	r9, [r5, #3]
0802a1e8  1944      add	r1, r3
0802a1ea  fff7e7fd  bl	#-1074 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802a1ee  0028      cmp	r0, #0
0802a1f0  9dd1      bne	#-198 ; -> 0x0802a12e ; branch_target=0x0802a12e
0802a1f2  c7f30807  ubfx	r7, r7, #0, #9
0802a1f6  c4f30324  ubfx	r4, r4, #8, #4
0802a1fa  18f80730  ldrb.w	r3, [r8, r7]
0802a1fe  08eb0702  add.w	r2, r8, r7
0802a202  23f00f03  bic	r3, r3, #15
0802a206  2343      orrs	r3, r4
0802a208  1370      strb	r3, [r2]
0802a20a  e2e7      b	#-60 ; -> 0x0802a1d2 ; branch_target=0x0802a1d2
