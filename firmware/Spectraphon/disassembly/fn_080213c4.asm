; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080213c4  90f85020  ldrb.w	r2, [r0, #80]
080213c8  012a      cmp	r2, #1
080213ca  44d0      beq	#136 ; -> 0x08021456 ; branch_target=0x08021456
080213cc  0346      mov	r3, r0
080213ce  0022      movs	r2, #0
080213d0  0120      movs	r0, #1
080213d2  70b4      push	{r4, r5, r6}
080213d4  344d      ldr	r5, [pc, #208] ; [0x080214a8] = 0x40022000 / f32_bits_interpretation=2.033203125
080213d6  9bb0      sub	sp, #108
080213d8  1c68      ldr	r4, [r3]
080213da  1692      str	r2, [sp, #88]
080213dc  ac42      cmp	r4, r5
080213de  83f85000  strb.w	r0, [r3, #80]
080213e2  1792      str	r2, [sp, #92]
080213e4  08d0      beq	#16 ; -> 0x080213f8 ; branch_target=0x080213f8
080213e6  596d      ldr	r1, [r3, #84]
080213e8  83f85020  strb.w	r2, [r3, #80]
080213ec  41f02001  orr	r1, r1, #32
080213f0  5965      str	r1, [r3, #84]
080213f2  1bb0      add	sp, #108
080213f4  70bc      pop	{r4, r5, r6}
080213f6  7047      bx	lr
080213f8  2c4a      ldr	r2, [pc, #176] ; [0x080214ac] = 0x40022100 / f32_bits_interpretation=2.03326416
080213fa  9068      ldr	r0, [r2, #8]
080213fc  4007      lsls	r0, r0, #29
080213fe  0bd5      bpl	#22 ; -> 0x08021418 ; branch_target=0x08021418
08021400  a268      ldr	r2, [r4, #8]
08021402  5a6d      ldr	r2, [r3, #84]
08021404  0120      movs	r0, #1
08021406  42f02002  orr	r2, r2, #32
0802140a  5a65      str	r2, [r3, #84]
0802140c  0022      movs	r2, #0
0802140e  83f85020  strb.w	r2, [r3, #80]
08021412  1bb0      add	sp, #108
08021414  70bc      pop	{r4, r5, r6}
08021416  7047      bx	lr
08021418  a068      ldr	r0, [r4, #8]
0802141a  4507      lsls	r5, r0, #29
0802141c  f1d4      bmi	#-30 ; -> 0x08021402 ; branch_target=0x08021402
0802141e  0868      ldr	r0, [r1]
08021420  d8b1      cbz	r0, #54 ; -> 0x0802145a ; branch_target=0x0802145a
08021422  234d      ldr	r5, [pc, #140] ; [0x080214b0] = 0x40022300 / f32_bits_interpretation=2.03338623
08021424  4e68      ldr	r6, [r1, #4]
08021426  a868      ldr	r0, [r5, #8]
08021428  20f44040  bic	r0, r0, #49152
0802142c  3043      orrs	r0, r6
0802142e  a860      str	r0, [r5, #8]
08021430  1868      ldr	r0, [r3]
08021432  a042      cmp	r0, r4
08021434  2fd0      beq	#94 ; -> 0x08021496 ; branch_target=0x08021496
08021436  9042      cmp	r0, r2
08021438  2dd0      beq	#90 ; -> 0x08021496 ; branch_target=0x08021496
0802143a  1e4a      ldr	r2, [pc, #120] ; [0x080214b4] = 0x58026000
0802143c  9268      ldr	r2, [r2, #8]
0802143e  d207      lsls	r2, r2, #31
08021440  1ed4      bmi	#60 ; -> 0x08021480 ; branch_target=0x08021480
08021442  0a68      ldr	r2, [r1]
08021444  1a48      ldr	r0, [pc, #104] ; [0x080214b0] = 0x40022300 / f32_bits_interpretation=2.03338623
08021446  8968      ldr	r1, [r1, #8]
08021448  8468      ldr	r4, [r0, #8]
0802144a  0a43      orrs	r2, r1
0802144c  1a49      ldr	r1, [pc, #104] ; [0x080214b8] = 0xfffff0e0
0802144e  2140      ands	r1, r4
08021450  0a43      orrs	r2, r1
08021452  8260      str	r2, [r0, #8]
08021454  14e0      b	#40 ; -> 0x08021480 ; branch_target=0x08021480
08021456  0220      movs	r0, #2
08021458  7047      bx	lr
0802145a  1548      ldr	r0, [pc, #84] ; [0x080214b0] = 0x40022300 / f32_bits_interpretation=2.03338623
0802145c  8168      ldr	r1, [r0, #8]
0802145e  21f44041  bic	r1, r1, #49152
08021462  8160      str	r1, [r0, #8]
08021464  1968      ldr	r1, [r3]
08021466  a142      cmp	r1, r4
08021468  0cd0      beq	#24 ; -> 0x08021484 ; branch_target=0x08021484
0802146a  9142      cmp	r1, r2
0802146c  0ad0      beq	#20 ; -> 0x08021484 ; branch_target=0x08021484
0802146e  114a      ldr	r2, [pc, #68] ; [0x080214b4] = 0x58026000
08021470  9268      ldr	r2, [r2, #8]
08021472  d407      lsls	r4, r2, #31
08021474  04d4      bmi	#8 ; -> 0x08021480 ; branch_target=0x08021480
08021476  0e49      ldr	r1, [pc, #56] ; [0x080214b0] = 0x40022300 / f32_bits_interpretation=2.03338623
08021478  0f4a      ldr	r2, [pc, #60] ; [0x080214b8] = 0xfffff0e0
0802147a  8868      ldr	r0, [r1, #8]
0802147c  0240      ands	r2, r0
0802147e  8a60      str	r2, [r1, #8]
08021480  0020      movs	r0, #0
08021482  c3e7      b	#-122 ; -> 0x0802140c ; branch_target=0x0802140c
08021484  0849      ldr	r1, [pc, #32] ; [0x080214a8] = 0x40022000 / f32_bits_interpretation=2.033203125
08021486  094a      ldr	r2, [pc, #36] ; [0x080214ac] = 0x40022100 / f32_bits_interpretation=2.03326416
08021488  8968      ldr	r1, [r1, #8]
0802148a  9268      ldr	r2, [r2, #8]
0802148c  d607      lsls	r6, r2, #31
0802148e  f7d4      bmi	#-18 ; -> 0x08021480 ; branch_target=0x08021480
08021490  cd07      lsls	r5, r1, #31
08021492  f0d5      bpl	#-32 ; -> 0x08021476 ; branch_target=0x08021476
08021494  f4e7      b	#-24 ; -> 0x08021480 ; branch_target=0x08021480
08021496  0448      ldr	r0, [pc, #16] ; [0x080214a8] = 0x40022000 / f32_bits_interpretation=2.033203125
08021498  044a      ldr	r2, [pc, #16] ; [0x080214ac] = 0x40022100 / f32_bits_interpretation=2.03326416
0802149a  8068      ldr	r0, [r0, #8]
0802149c  9268      ldr	r2, [r2, #8]
0802149e  d407      lsls	r4, r2, #31
080214a0  eed4      bmi	#-36 ; -> 0x08021480 ; branch_target=0x08021480
080214a2  c007      lsls	r0, r0, #31
080214a4  cdd5      bpl	#-102 ; -> 0x08021442 ; branch_target=0x08021442
080214a6  ebe7      b	#-42 ; -> 0x08021480 ; branch_target=0x08021480
