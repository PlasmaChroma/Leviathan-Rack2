; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802b264  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802b268  1f46      mov	r7, r3
0802b26a  0023      movs	r3, #0
0802b26c  83b0      sub	sp, #12
0802b26e  3b60      str	r3, [r7]
0802b270  50b1      cbz	r0, #20 ; -> 0x0802b288 ; branch_target=0x0802b288
0802b272  0368      ldr	r3, [r0]
0802b274  0446      mov	r4, r0
0802b276  3bb1      cbz	r3, #14 ; -> 0x0802b288 ; branch_target=0x0802b288
0802b278  9146      mov	r9, r2
0802b27a  1a78      ldrb	r2, [r3]
0802b27c  22b1      cbz	r2, #8 ; -> 0x0802b288 ; branch_target=0x0802b288
0802b27e  0e46      mov	r6, r1
0802b280  da88      ldrh	r2, [r3, #6]
0802b282  8188      ldrh	r1, [r0, #4]
0802b284  9142      cmp	r1, r2
0802b286  03d0      beq	#6 ; -> 0x0802b290 ; branch_target=0x0802b290
0802b288  0920      movs	r0, #9
0802b28a  03b0      add	sp, #12
0802b28c  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802b290  5878      ldrb	r0, [r3, #1]
0802b292  fef715fb  bl	#-6614 ; -> 0x080298c0 ; branch_target=0x080298c0
0802b296  c207      lsls	r2, r0, #31
0802b298  f6d4      bmi	#-20 ; -> 0x0802b288 ; branch_target=0x0802b288
0802b29a  607d      ldrb	r0, [r4, #21]
0802b29c  0028      cmp	r0, #0
0802b29e  f4d1      bne	#-24 ; -> 0x0802b28a ; branch_target=0x0802b28a
0802b2a0  237d      ldrb	r3, [r4, #20]
0802b2a2  13f0010b  ands	r11, r3, #1
0802b2a6  00f0d680  beq.w	#428 ; -> 0x0802b456 ; branch_target=0x0802b456
0802b2aa  a169      ldr	r1, [r4, #24]
0802b2ac  e368      ldr	r3, [r4, #12]
0802b2ae  d4f800a0  ldr.w	r10, [r4]
0802b2b2  a3eb0108  sub.w	r8, r3, r1
0802b2b6  c845      cmp	r8, r9
0802b2b8  28bf      it	hs
0802b2ba  c846      movhs	r8, r9
0802b2bc  b8f1000f  cmp.w	r8, #0
0802b2c0  e3d0      beq	#-58 ; -> 0x0802b28a ; branch_target=0x0802b28a
0802b2c2  0090      str	r0, [sp]
0802b2c4  c1f30800  ubfx	r0, r1, #0, #9
0802b2c8  0028      cmp	r0, #0
0802b2ca  42d1      bne	#132 ; -> 0x0802b352 ; branch_target=0x0802b352
0802b2cc  baf80a20  ldrh.w	r2, [r10, #10]
0802b2d0  4b0a      lsrs	r3, r1, #9
0802b2d2  013a      subs	r2, #1
0802b2d4  12ea5122  ands.w	r2, r2, r1, lsr #9
0802b2d8  55d0      beq	#170 ; -> 0x0802b386 ; branch_target=0x0802b386
0802b2da  e069      ldr	r0, [r4, #28]
0802b2dc  daf81430  ldr.w	r3, [r10, #20]
0802b2e0  0238      subs	r0, #2
0802b2e2  023b      subs	r3, #2
0802b2e4  9842      cmp	r0, r3
0802b2e6  5cd2      bhs	#184 ; -> 0x0802b3a2 ; branch_target=0x0802b3a2
0802b2e8  baf80a10  ldrh.w	r1, [r10, #10]
0802b2ec  daf82850  ldr.w	r5, [r10, #40]
0802b2f0  01fb0055  mla	r5, r1, r0, r5
0802b2f4  002d      cmp	r5, #0
0802b2f6  54d0      beq	#168 ; -> 0x0802b3a2 ; branch_target=0x0802b3a2
0802b2f8  b8f5007f  cmp.w	r8, #512
0802b2fc  1544      add	r5, r2
0802b2fe  79d3      blo	#242 ; -> 0x0802b3f4 ; branch_target=0x0802b3f4
0802b300  02eb5820  add.w	r0, r2, r8, lsr #9
0802b304  4fea5829  lsr.w	r9, r8, #9
0802b308  8842      cmp	r0, r1
0802b30a  9af80100  ldrb.w	r0, [r10, #1]
0802b30e  88bf      it	hi
0802b310  a1eb0209  subhi.w	r9, r1, r2
0802b314  2a46      mov	r2, r5
0802b316  3146      mov	r1, r6
0802b318  4b46      mov	r3, r9
0802b31a  fef7edfa  bl	#-6694 ; -> 0x080298f8 ; branch_target=0x080298f8
0802b31e  0028      cmp	r0, #0
0802b320  40f09380  bne.w	#294 ; -> 0x0802b44a ; branch_target=0x0802b44a
0802b324  94f91420  ldrsb.w	r2, [r4, #20]
0802b328  002a      cmp	r2, #0
0802b32a  c0f2aa80  blt.w	#340 ; -> 0x0802b482 ; branch_target=0x0802b482
0802b32e  4fea4922  lsl.w	r2, r9, #9
0802b332  a369      ldr	r3, [r4, #24]
0802b334  b8eb0208  subs.w	r8, r8, r2
0802b338  1644      add	r6, r2
0802b33a  1344      add	r3, r2
0802b33c  a361      str	r3, [r4, #24]
0802b33e  3b68      ldr	r3, [r7]
0802b340  1344      add	r3, r2
0802b342  3b60      str	r3, [r7]
0802b344  00f0b480  beq.w	#360 ; -> 0x0802b4b0 ; branch_target=0x0802b4b0
0802b348  a169      ldr	r1, [r4, #24]
0802b34a  c1f30800  ubfx	r0, r1, #0, #9
0802b34e  0028      cmp	r0, #0
0802b350  bcd0      beq	#-136 ; -> 0x0802b2cc ; branch_target=0x0802b2cc
0802b352  04f13001  add.w	r1, r4, #48
0802b356  c0f50073  rsb.w	r3, r0, #512
0802b35a  4345      cmp	r3, r8
0802b35c  28bf      it	hs
0802b35e  4346      movhs	r3, r8
0802b360  1a46      mov	r2, r3
0802b362  0b18      adds	r3, r1, r0
0802b364  551e      subs	r5, r2, #1
0802b366  052d      cmp	r5, #5
0802b368  04d9      bls	#8 ; -> 0x0802b374 ; branch_target=0x0802b374
0802b36a  0130      adds	r0, #1
0802b36c  0144      add	r1, r0
0802b36e  711a      subs	r1, r6, r1
0802b370  0229      cmp	r1, #2
0802b372  1cd8      bhi	#56 ; -> 0x0802b3ae ; branch_target=0x0802b3ae
0802b374  711e      subs	r1, r6, #1
0802b376  9d18      adds	r5, r3, r2
0802b378  13f8010b  ldrb	r0, [r3], #1
0802b37c  ab42      cmp	r3, r5
0802b37e  01f8010f  strb	r0, [r1, #1]!
0802b382  f9d1      bne	#-14 ; -> 0x0802b378 ; branch_target=0x0802b378
0802b384  d5e7      b	#-86 ; -> 0x0802b332 ; branch_target=0x0802b332
0802b386  0029      cmp	r1, #0
0802b388  4bd1      bne	#150 ; -> 0x0802b422 ; branch_target=0x0802b422
0802b38a  a068      ldr	r0, [r4, #8]
0802b38c  0128      cmp	r0, #1
0802b38e  08d9      bls	#16 ; -> 0x0802b3a2 ; branch_target=0x0802b3a2
0802b390  431c      adds	r3, r0, #1
0802b392  5ad0      beq	#180 ; -> 0x0802b44a ; branch_target=0x0802b44a
0802b394  e061      str	r0, [r4, #28]
0802b396  0238      subs	r0, #2
0802b398  daf81430  ldr.w	r3, [r10, #20]
0802b39c  023b      subs	r3, #2
0802b39e  9842      cmp	r0, r3
0802b3a0  a2d3      blo	#-188 ; -> 0x0802b2e8 ; branch_target=0x0802b2e8
0802b3a2  0223      movs	r3, #2
0802b3a4  1846      mov	r0, r3
0802b3a6  6375      strb	r3, [r4, #21]
0802b3a8  03b0      add	sp, #12
0802b3aa  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802b3ae  22f0030c  bic	r12, r2, #3
0802b3b2  3146      mov	r1, r6
0802b3b4  1846      mov	r0, r3
0802b3b6  b444      add	r12, r6
0802b3b8  50f8045b  ldr	r5, [r0], #4
0802b3bc  41f8045b  str	r5, [r1], #4
0802b3c0  6145      cmp	r1, r12
0802b3c2  f9d1      bne	#-14 ; -> 0x0802b3b8 ; branch_target=0x0802b3b8
0802b3c4  22f00301  bic	r1, r2, #3
0802b3c8  12f0030f  tst.w	r2, #3
0802b3cc  02f00300  and	r0, r2, #3
0802b3d0  06eb0105  add.w	r5, r6, r1
0802b3d4  03eb010c  add.w	r12, r3, r1
0802b3d8  abd0      beq	#-170 ; -> 0x0802b332 ; branch_target=0x0802b332
0802b3da  5b5c      ldrb	r3, [r3, r1]
0802b3dc  0128      cmp	r0, #1
0802b3de  7354      strb	r3, [r6, r1]
0802b3e0  a7d0      beq	#-178 ; -> 0x0802b332 ; branch_target=0x0802b332
0802b3e2  9cf80130  ldrb.w	r3, [r12, #1]
0802b3e6  0228      cmp	r0, #2
0802b3e8  6b70      strb	r3, [r5, #1]
0802b3ea  a2d0      beq	#-188 ; -> 0x0802b332 ; branch_target=0x0802b332
0802b3ec  9cf80230  ldrb.w	r3, [r12, #2]
0802b3f0  ab70      strb	r3, [r5, #2]
0802b3f2  9ee7      b	#-196 ; -> 0x0802b332 ; branch_target=0x0802b332
0802b3f4  226a      ldr	r2, [r4, #32]
0802b3f6  04f13001  add.w	r1, r4, #48
0802b3fa  aa42      cmp	r2, r5
0802b3fc  0cd0      beq	#24 ; -> 0x0802b418 ; branch_target=0x0802b418
0802b3fe  94f91430  ldrsb.w	r3, [r4, #20]
0802b402  9af80100  ldrb.w	r0, [r10, #1]
0802b406  002b      cmp	r3, #0
0802b408  2ddb      blt	#90 ; -> 0x0802b466 ; branch_target=0x0802b466
0802b40a  0123      movs	r3, #1
0802b40c  2a46      mov	r2, r5
0802b40e  0191      str	r1, [sp, #4]
0802b410  fef772fa  bl	#-6940 ; -> 0x080298f8 ; branch_target=0x080298f8
0802b414  0199      ldr	r1, [sp, #4]
0802b416  c0b9      cbnz	r0, #48 ; -> 0x0802b44a ; branch_target=0x0802b44a
0802b418  a069      ldr	r0, [r4, #24]
0802b41a  2562      str	r5, [r4, #32]
0802b41c  c0f30800  ubfx	r0, r0, #0, #9
0802b420  99e7      b	#-206 ; -> 0x0802b356 ; branch_target=0x0802b356
0802b422  e56a      ldr	r5, [r4, #44]
0802b424  2068      ldr	r0, [r4]
0802b426  c5b1      cbz	r5, #48 ; -> 0x0802b45a ; branch_target=0x0802b45a
0802b428  4189      ldrh	r1, [r0, #10]
0802b42a  281d      adds	r0, r5, #4
0802b42c  b3fbf1f3  udiv	r3, r3, r1
0802b430  6968      ldr	r1, [r5, #4]
0802b432  29b9      cbnz	r1, #10 ; -> 0x0802b440 ; branch_target=0x0802b440
0802b434  b5e7      b	#-150 ; -> 0x0802b3a2 ; branch_target=0x0802b3a2
0802b436  5b1a      subs	r3, r3, r1
0802b438  50f8081f  ldr	r1, [r0, #8]!
0802b43c  0029      cmp	r1, #0
0802b43e  b0d0      beq	#-160 ; -> 0x0802b3a2 ; branch_target=0x0802b3a2
0802b440  8b42      cmp	r3, r1
0802b442  f8d2      bhs	#-16 ; -> 0x0802b436 ; branch_target=0x0802b436
0802b444  4068      ldr	r0, [r0, #4]
0802b446  1844      add	r0, r3
0802b448  a0e7      b	#-192 ; -> 0x0802b38c ; branch_target=0x0802b38c
0802b44a  0123      movs	r3, #1
0802b44c  5846      mov	r0, r11
0802b44e  6375      strb	r3, [r4, #21]
0802b450  03b0      add	sp, #12
0802b452  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802b456  0720      movs	r0, #7
0802b458  17e7      b	#-466 ; -> 0x0802b28a ; branch_target=0x0802b28a
0802b45a  e169      ldr	r1, [r4, #28]
0802b45c  0192      str	r2, [sp, #4]
0802b45e  fef7d5fe  bl	#-4694 ; -> 0x0802a20c ; branch_target=0x0802a20c
0802b462  019a      ldr	r2, [sp, #4]
0802b464  92e7      b	#-220 ; -> 0x0802b38c ; branch_target=0x0802b38c
0802b466  0123      movs	r3, #1
0802b468  0191      str	r1, [sp, #4]
0802b46a  fef753fa  bl	#-7002 ; -> 0x08029914 ; branch_target=0x08029914
0802b46e  0028      cmp	r0, #0
0802b470  ebd1      bne	#-42 ; -> 0x0802b44a ; branch_target=0x0802b44a
0802b472  237d      ldrb	r3, [r4, #20]
0802b474  0199      ldr	r1, [sp, #4]
0802b476  03f07f03  and	r3, r3, #127
0802b47a  2375      strb	r3, [r4, #20]
0802b47c  9af80100  ldrb.w	r0, [r10, #1]
0802b480  c3e7      b	#-122 ; -> 0x0802b40a ; branch_target=0x0802b40a
0802b482  226a      ldr	r2, [r4, #32]
0802b484  521b      subs	r2, r2, r5
0802b486  4a45      cmp	r2, r9
0802b488  bff451af  bhs.w	#-350 ; -> 0x0802b32e ; branch_target=0x0802b32e
0802b48c  06eb4222  add.w	r2, r6, r2, lsl #9
0802b490  04f13101  add.w	r1, r4, #49
0802b494  511a      subs	r1, r2, r1
0802b496  0229      cmp	r1, #2
0802b498  0cd9      bls	#24 ; -> 0x0802b4b4 ; branch_target=0x0802b4b4
0802b49a  04f13001  add.w	r1, r4, #48
0802b49e  02f50075  add.w	r5, r2, #512
0802b4a2  51f8043b  ldr	r3, [r1], #4
0802b4a6  42f8043b  str	r3, [r2], #4
0802b4aa  aa42      cmp	r2, r5
0802b4ac  f9d1      bne	#-14 ; -> 0x0802b4a2 ; branch_target=0x0802b4a2
0802b4ae  3ee7      b	#-388 ; -> 0x0802b32e ; branch_target=0x0802b32e
0802b4b0  0098      ldr	r0, [sp]
0802b4b2  eae6      b	#-556 ; -> 0x0802b28a ; branch_target=0x0802b28a
0802b4b4  013a      subs	r2, #1
0802b4b6  04f13001  add.w	r1, r4, #48
0802b4ba  04f50c75  add.w	r5, r4, #560
0802b4be  11f8013b  ldrb	r3, [r1], #1
0802b4c2  8d42      cmp	r5, r1
0802b4c4  02f8013f  strb	r3, [r2, #1]!
0802b4c8  f9d1      bne	#-14 ; -> 0x0802b4be ; branch_target=0x0802b4be
0802b4ca  30e7      b	#-416 ; -> 0x0802b32e ; branch_target=0x0802b32e
