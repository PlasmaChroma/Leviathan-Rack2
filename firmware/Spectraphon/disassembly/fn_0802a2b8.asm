; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802a2b8  2de9f047  push.w	{r4, r5, r6, r7, r8, r9, r10, lr}
0802a2bc  c769      ldr	r7, [r0, #28]
0802a2be  82b0      sub	sp, #8
0802a2c0  0446      mov	r4, r0
0802a2c2  0568      ldr	r5, [r0]
0802a2c4  002f      cmp	r7, #0
0802a2c6  00f09f80  beq.w	#318 ; -> 0x0802a408 ; branch_target=0x0802a408
0802a2ca  8846      mov	r8, r1
0802a2cc  05f13006  add.w	r6, r5, #48
0802a2d0  d5f82c90  ldr.w	r9, [r5, #44]
0802a2d4  b945      cmp	r9, r7
0802a2d6  13d0      beq	#38 ; -> 0x0802a300 ; branch_target=0x0802a300
0802a2d8  eb78      ldrb	r3, [r5, #3]
0802a2da  6878      ldrb	r0, [r5, #1]
0802a2dc  002b      cmp	r3, #0
0802a2de  48d1      bne	#144 ; -> 0x0802a372 ; branch_target=0x0802a372
0802a2e0  0123      movs	r3, #1
0802a2e2  3a46      mov	r2, r7
0802a2e4  3146      mov	r1, r6
0802a2e6  fff707fb  bl	#-2546 ; -> 0x080298f8 ; branch_target=0x080298f8
0802a2ea  40b1      cbz	r0, #16 ; -> 0x0802a2fe ; branch_target=0x0802a2fe
0802a2ec  4ff0ff33  mov.w	r3, #4294967295
0802a2f0  eb62      str	r3, [r5, #44]
0802a2f2  0120      movs	r0, #1
0802a2f4  0023      movs	r3, #0
0802a2f6  e361      str	r3, [r4, #28]
0802a2f8  02b0      add	sp, #8
0802a2fa  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
0802a2fe  ef62      str	r7, [r5, #44]
0802a300  236a      ldr	r3, [r4, #32]
0802a302  1a78      ldrb	r2, [r3]
0802a304  002a      cmp	r2, #0
0802a306  7fd0      beq	#254 ; -> 0x0802a408 ; branch_target=0x0802a408
0802a308  db7a      ldrb	r3, [r3, #11]
0802a30a  e52a      cmp	r2, #229
0802a30c  18bf      it	ne
0802a30e  2e2a      cmpne	r2, #46
0802a310  03f03f03  and	r3, r3, #63
0802a314  a371      strb	r3, [r4, #6]
0802a316  0ad0      beq	#20 ; -> 0x0802a32e ; branch_target=0x0802a32e
0802a318  0f2b      cmp	r3, #15
0802a31a  08d0      beq	#16 ; -> 0x0802a32e ; branch_target=0x0802a32e
0802a31c  23f02003  bic	r3, r3, #32
0802a320  a3f10803  sub.w	r3, r3, #8
0802a324  b3fa83f3  clz	r3, r3
0802a328  5b09      lsrs	r3, r3, #5
0802a32a  4345      cmp	r3, r8
0802a32c  1dd0      beq	#58 ; -> 0x0802a36a ; branch_target=0x0802a36a
0802a32e  6369      ldr	r3, [r4, #20]
0802a330  2268      ldr	r2, [r4]
0802a332  2033      adds	r3, #32
0802a334  e769      ldr	r7, [r4, #28]
0802a336  b3f5001f  cmp.w	r3, #2097152
0802a33a  65d2      bhs	#202 ; -> 0x0802a408 ; branch_target=0x0802a408
0802a33c  002f      cmp	r7, #0
0802a33e  63d0      beq	#198 ; -> 0x0802a408 ; branch_target=0x0802a408
0802a340  c3f30801  ubfx	r1, r3, #0, #9
0802a344  21b1      cbz	r1, #8 ; -> 0x0802a350 ; branch_target=0x0802a350
0802a346  3032      adds	r2, #48
0802a348  6361      str	r3, [r4, #20]
0802a34a  0a44      add	r2, r1
0802a34c  2262      str	r2, [r4, #32]
0802a34e  bfe7      b	#-130 ; -> 0x0802a2d0 ; branch_target=0x0802a2d0
0802a350  0137      adds	r7, #1
0802a352  a169      ldr	r1, [r4, #24]
0802a354  e761      str	r7, [r4, #28]
0802a356  79bb      cbnz	r1, #94 ; -> 0x0802a3b8 ; branch_target=0x0802a3b8
0802a358  1189      ldrh	r1, [r2, #8]
0802a35a  b1eb531f  cmp.w	r1, r3, lsr #5
0802a35e  53d9      bls	#166 ; -> 0x0802a408 ; branch_target=0x0802a408
0802a360  3032      adds	r2, #48
0802a362  6361      str	r3, [r4, #20]
0802a364  2262      str	r2, [r4, #32]
0802a366  002f      cmp	r7, #0
0802a368  b2d1      bne	#-156 ; -> 0x0802a2d0 ; branch_target=0x0802a2d0
0802a36a  0020      movs	r0, #0
0802a36c  02b0      add	sp, #8
0802a36e  bde8f087  pop.w	{r4, r5, r6, r7, r8, r9, r10, pc}
0802a372  0123      movs	r3, #1
0802a374  4a46      mov	r2, r9
0802a376  3146      mov	r1, r6
0802a378  fff7ccfa  bl	#-2664 ; -> 0x08029914 ; branch_target=0x08029914
0802a37c  0028      cmp	r0, #0
0802a37e  b8d1      bne	#-144 ; -> 0x0802a2f2 ; branch_target=0x0802a2f2
0802a380  2b6a      ldr	r3, [r5, #32]
0802a382  e870      strb	r0, [r5, #3]
0802a384  a9eb0302  sub.w	r2, r9, r3
0802a388  ab69      ldr	r3, [r5, #24]
0802a38a  9a42      cmp	r2, r3
0802a38c  12d2      bhs	#36 ; -> 0x0802a3b4 ; branch_target=0x0802a3b4
0802a38e  95f802a0  ldrb.w	r10, [r5, #2]
0802a392  baf1010f  cmp.w	r10, #1
0802a396  01d8      bhi	#2 ; -> 0x0802a39c ; branch_target=0x0802a39c
0802a398  0ce0      b	#24 ; -> 0x0802a3b4 ; branch_target=0x0802a3b4
0802a39a  ab69      ldr	r3, [r5, #24]
0802a39c  9944      add	r9, r3
0802a39e  0af1ff3a  add.w	r10, r10, #4294967295
0802a3a2  0123      movs	r3, #1
0802a3a4  3146      mov	r1, r6
0802a3a6  4a46      mov	r2, r9
0802a3a8  6878      ldrb	r0, [r5, #1]
0802a3aa  fff7b3fa  bl	#-2714 ; -> 0x08029914 ; branch_target=0x08029914
0802a3ae  baf1010f  cmp.w	r10, #1
0802a3b2  f2d1      bne	#-28 ; -> 0x0802a39a ; branch_target=0x0802a39a
0802a3b4  6878      ldrb	r0, [r5, #1]
0802a3b6  93e7      b	#-218 ; -> 0x0802a2e0 ; branch_target=0x0802a2e0
0802a3b8  5089      ldrh	r0, [r2, #10]
0802a3ba  0138      subs	r0, #1
0802a3bc  10ea5329  ands.w	r9, r0, r3, lsr #9
0802a3c0  ced1      bne	#-100 ; -> 0x0802a360 ; branch_target=0x0802a360
0802a3c2  1046      mov	r0, r2
0802a3c4  0193      str	r3, [sp, #4]
0802a3c6  0092      str	r2, [sp]
0802a3c8  fff720ff  bl	#-448 ; -> 0x0802a20c ; branch_target=0x0802a20c
0802a3cc  0128      cmp	r0, #1
0802a3ce  13d9      bls	#38 ; -> 0x0802a3f8 ; branch_target=0x0802a3f8
0802a3d0  431c      adds	r3, r0, #1
0802a3d2  8ed0      beq	#-228 ; -> 0x0802a2f2 ; branch_target=0x0802a2f2
0802a3d4  009a      ldr	r2, [sp]
0802a3d6  5169      ldr	r1, [r2, #20]
0802a3d8  8842      cmp	r0, r1
0802a3da  15d2      bhs	#42 ; -> 0x0802a408 ; branch_target=0x0802a408
0802a3dc  a061      str	r0, [r4, #24]
0802a3de  0238      subs	r0, #2
0802a3e0  5169      ldr	r1, [r2, #20]
0802a3e2  019b      ldr	r3, [sp, #4]
0802a3e4  0239      subs	r1, #2
0802a3e6  8842      cmp	r0, r1
0802a3e8  08d2      bhs	#16 ; -> 0x0802a3fc ; branch_target=0x0802a3fc
0802a3ea  b2f80ac0  ldrh.w	r12, [r2, #10]
0802a3ee  916a      ldr	r1, [r2, #40]
0802a3f0  00fb0c17  mla	r7, r0, r12, r1
0802a3f4  e761      str	r7, [r4, #28]
0802a3f6  b3e7      b	#-154 ; -> 0x0802a360 ; branch_target=0x0802a360
0802a3f8  0220      movs	r0, #2
0802a3fa  7be7      b	#-266 ; -> 0x0802a2f4 ; branch_target=0x0802a2f4
0802a3fc  3032      adds	r2, #48
0802a3fe  c4f81c90  str.w	r9, [r4, #28]
0802a402  6361      str	r3, [r4, #20]
0802a404  2262      str	r2, [r4, #32]
0802a406  b0e7      b	#-160 ; -> 0x0802a36a ; branch_target=0x0802a36a
0802a408  0420      movs	r0, #4
0802a40a  73e7      b	#-282 ; -> 0x0802a2f4 ; branch_target=0x0802a2f4
