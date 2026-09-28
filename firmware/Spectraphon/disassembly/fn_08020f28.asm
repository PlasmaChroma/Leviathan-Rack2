; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08020f28  30b5      push	{r4, r5, lr}
08020f2a  0023      movs	r3, #0
08020f2c  83b0      sub	sp, #12
08020f2e  0193      str	r3, [sp, #4]
08020f30  0028      cmp	r0, #0
08020f32  3bd0      beq	#118 ; -> 0x08020fac ; branch_target=0x08020fac
08020f34  456d      ldr	r5, [r0, #84]
08020f36  0446      mov	r4, r0
08020f38  002d      cmp	r5, #0
08020f3a  00f0af80  beq.w	#350 ; -> 0x0802109c ; branch_target=0x0802109c
08020f3e  2368      ldr	r3, [r4]
08020f40  9a68      ldr	r2, [r3, #8]
08020f42  9100      lsls	r1, r2, #2
08020f44  04d5      bpl	#8 ; -> 0x08020f50 ; branch_target=0x08020f50
08020f46  9968      ldr	r1, [r3, #8]
08020f48  734a      ldr	r2, [pc, #460] ; [0x08021118] = 0x5fffffc0
08020f4a  0a40      ands	r2, r1
08020f4c  9a60      str	r2, [r3, #8]
08020f4e  2368      ldr	r3, [r4]
08020f50  9a68      ldr	r2, [r3, #8]
08020f52  d200      lsls	r2, r2, #3
08020f54  17d4      bmi	#46 ; -> 0x08020f86 ; branch_target=0x08020f86
08020f56  714a      ldr	r2, [pc, #452] ; [0x0802111c] = 0x20000014
08020f58  7149      ldr	r1, [pc, #452] ; [0x08021120] = 0x053e2d63
08020f5a  1268      ldr	r2, [r2]
08020f5c  9868      ldr	r0, [r3, #8]
08020f5e  9209      lsrs	r2, r2, #6
08020f60  a1fb0212  umull	r1, r2, r1, r2
08020f64  6f49      ldr	r1, [pc, #444] ; [0x08021124] = 0x6fffffc0
08020f66  9209      lsrs	r2, r2, #6
08020f68  0140      ands	r1, r0
08020f6a  0132      adds	r2, #1
08020f6c  41f08051  orr	r1, r1, #268435456
08020f70  9960      str	r1, [r3, #8]
08020f72  0192      str	r2, [sp, #4]
08020f74  019b      ldr	r3, [sp, #4]
08020f76  2bb1      cbz	r3, #10 ; -> 0x08020f84 ; branch_target=0x08020f84
08020f78  019b      ldr	r3, [sp, #4]
08020f7a  013b      subs	r3, #1
08020f7c  0193      str	r3, [sp, #4]
08020f7e  019b      ldr	r3, [sp, #4]
08020f80  002b      cmp	r3, #0
08020f82  f9d1      bne	#-14 ; -> 0x08020f78 ; branch_target=0x08020f78
08020f84  2368      ldr	r3, [r4]
08020f86  9a68      ldr	r2, [r3, #8]
08020f88  d500      lsls	r5, r2, #3
08020f8a  13d4      bmi	#38 ; -> 0x08020fb4 ; branch_target=0x08020fb4
08020f8c  626d      ldr	r2, [r4, #84]
08020f8e  0125      movs	r5, #1
08020f90  42f01002  orr	r2, r2, #16
08020f94  6265      str	r2, [r4, #84]
08020f96  a26d      ldr	r2, [r4, #88]
08020f98  2a43      orrs	r2, r5
08020f9a  a265      str	r2, [r4, #88]
08020f9c  9a68      ldr	r2, [r3, #8]
08020f9e  5007      lsls	r0, r2, #29
08020fa0  0cd5      bpl	#24 ; -> 0x08020fbc ; branch_target=0x08020fbc
08020fa2  636d      ldr	r3, [r4, #84]
08020fa4  636d      ldr	r3, [r4, #84]
08020fa6  43f01003  orr	r3, r3, #16
08020faa  6365      str	r3, [r4, #84]
08020fac  0125      movs	r5, #1
08020fae  2846      mov	r0, r5
08020fb0  03b0      add	sp, #12
08020fb2  30bd      pop	{r4, r5, pc}
08020fb4  9a68      ldr	r2, [r3, #8]
08020fb6  0025      movs	r5, #0
08020fb8  5007      lsls	r0, r2, #29
08020fba  f2d4      bmi	#-28 ; -> 0x08020fa2 ; branch_target=0x08020fa2
08020fbc  626d      ldr	r2, [r4, #84]
08020fbe  d106      lsls	r1, r2, #27
08020fc0  f0d4      bmi	#-32 ; -> 0x08020fa4 ; branch_target=0x08020fa4
08020fc2  626d      ldr	r2, [r4, #84]
08020fc4  22f48172  bic	r2, r2, #258
08020fc8  42f00202  orr	r2, r2, #2
08020fcc  6265      str	r2, [r4, #84]
08020fce  9a68      ldr	r2, [r3, #8]
08020fd0  d207      lsls	r2, r2, #31
08020fd2  0bd4      bmi	#22 ; -> 0x08020fec ; branch_target=0x08020fec
08020fd4  544a      ldr	r2, [pc, #336] ; [0x08021128] = 0x40022000 / f32_bits_interpretation=2.033203125
08020fd6  9342      cmp	r3, r2
08020fd8  79d0      beq	#242 ; -> 0x080210ce ; branch_target=0x080210ce
08020fda  02f58072  add.w	r2, r2, #256
08020fde  9342      cmp	r3, r2
08020fe0  75d0      beq	#234 ; -> 0x080210ce ; branch_target=0x080210ce
08020fe2  524b      ldr	r3, [pc, #328] ; [0x0802112c] = 0x58026000
08020fe4  9b68      ldr	r3, [r3, #8]
08020fe6  d907      lsls	r1, r3, #31
08020fe8  40f18180  bpl.w	#258 ; -> 0x080210ee ; branch_target=0x080210ee
08020fec  fff7eef9  bl	#-3108 ; -> 0x080203cc ; branch_target=0x080203cc
08020ff0  41f20303  movw	r3, #4099
08020ff4  a168      ldr	r1, [r4, #8]
08020ff6  9842      cmp	r0, r3
08020ff8  227f      ldrb	r2, [r4, #28]
08020ffa  55d8      bhi	#170 ; -> 0x080210a8 ; branch_target=0x080210a8
08020ffc  94f815c0  ldrb.w	r12, [r4, #21]
08021000  1304      lsls	r3, r2, #16
08021002  206b      ldr	r0, [r4, #48]
08021004  43ea4c33  orr.w	r3, r3, r12, lsl #13
08021008  0343      orrs	r3, r0
0802100a  0b43      orrs	r3, r1
0802100c  012a      cmp	r2, #1
0802100e  03d1      bne	#6 ; -> 0x08021018 ; branch_target=0x08021018
08021010  226a      ldr	r2, [r4, #32]
08021012  013a      subs	r2, #1
08021014  43ea4243  orr.w	r3, r3, r2, lsl #17
08021018  626a      ldr	r2, [r4, #36]
0802101a  22b1      cbz	r2, #8 ; -> 0x08021026 ; branch_target=0x08021026
0802101c  02f47872  and	r2, r2, #992
08021020  a16a      ldr	r1, [r4, #40]
08021022  0a43      orrs	r2, r1
08021024  1343      orrs	r3, r2
08021026  2168      ldr	r1, [r4]
08021028  414a      ldr	r2, [pc, #260] ; [0x08021130] = 0xfff0c003
0802102a  c868      ldr	r0, [r1, #12]
0802102c  0240      ands	r2, r0
0802102e  1a43      orrs	r2, r3
08021030  ca60      str	r2, [r1, #12]
08021032  2368      ldr	r3, [r4]
08021034  9a68      ldr	r2, [r3, #8]
08021036  12f0040f  tst.w	r2, #4
0802103a  9a68      ldr	r2, [r3, #8]
0802103c  1ed1      bne	#60 ; -> 0x0802107c ; branch_target=0x0802107c
0802103e  1207      lsls	r2, r2, #28
08021040  1cd4      bmi	#56 ; -> 0x0802107c ; branch_target=0x0802107c
08021042  d868      ldr	r0, [r3, #12]
08021044  3b4a      ldr	r2, [pc, #236] ; [0x08021134] = 0xffffbffc
08021046  217d      ldrb	r1, [r4, #20]
08021048  0240      ands	r2, r0
0802104a  42ea8132  orr.w	r2, r2, r1, lsl #14
0802104e  e16a      ldr	r1, [r4, #44]
08021050  0a43      orrs	r2, r1
08021052  da60      str	r2, [r3, #12]
08021054  94f83830  ldrb.w	r3, [r4, #56]
08021058  012b      cmp	r3, #1
0802105a  4ad0      beq	#148 ; -> 0x080210f2 ; branch_target=0x080210f2
0802105c  2268      ldr	r2, [r4]
0802105e  1369      ldr	r3, [r2, #16]
08021060  23f00103  bic	r3, r3, #1
08021064  1361      str	r3, [r2, #16]
08021066  2268      ldr	r2, [r4]
08021068  2046      mov	r0, r4
0802106a  616b      ldr	r1, [r4, #52]
0802106c  1369      ldr	r3, [r2, #16]
0802106e  23f07043  bic	r3, r3, #4026531840
08021072  0b43      orrs	r3, r1
08021074  1361      str	r3, [r2, #16]
08021076  fff79dfe  bl	#-710 ; -> 0x08020db4 ; branch_target=0x08020db4
0802107a  2368      ldr	r3, [r4]
0802107c  e268      ldr	r2, [r4, #12]
0802107e  012a      cmp	r2, #1
08021080  1dd0      beq	#58 ; -> 0x080210be ; branch_target=0x080210be
08021082  1a6b      ldr	r2, [r3, #48]
08021084  22f00f02  bic	r2, r2, #15
08021088  1a63      str	r2, [r3, #48]
0802108a  636d      ldr	r3, [r4, #84]
0802108c  2846      mov	r0, r5
0802108e  23f00303  bic	r3, r3, #3
08021092  43f00103  orr	r3, r3, #1
08021096  6365      str	r3, [r4, #84]
08021098  03b0      add	sp, #12
0802109a  30bd      pop	{r4, r5, pc}
0802109c  0bf046f9  bl	#45708 ; -> 0x0802c32c ; branch_target=0x0802c32c
080210a0  a565      str	r5, [r4, #88]
080210a2  84f85050  strb.w	r5, [r4, #80]
080210a6  4ae7      b	#-364 ; -> 0x08020f3e ; branch_target=0x08020f3e
080210a8  1029      cmp	r1, #16
080210aa  a7d1      bne	#-178 ; -> 0x08020ffc ; branch_target=0x08020ffc
080210ac  617d      ldrb	r1, [r4, #21]
080210ae  1304      lsls	r3, r2, #16
080210b0  43ea4133  orr.w	r3, r3, r1, lsl #13
080210b4  216b      ldr	r1, [r4, #48]
080210b6  0b43      orrs	r3, r1
080210b8  43f01c03  orr	r3, r3, #28
080210bc  a6e7      b	#-180 ; -> 0x0802100c ; branch_target=0x0802100c
080210be  196b      ldr	r1, [r3, #48]
080210c0  a269      ldr	r2, [r4, #24]
080210c2  21f00f01  bic	r1, r1, #15
080210c6  013a      subs	r2, #1
080210c8  0a43      orrs	r2, r1
080210ca  1a63      str	r2, [r3, #48]
080210cc  dde7      b	#-70 ; -> 0x0802108a ; branch_target=0x0802108a
080210ce  164a      ldr	r2, [pc, #88] ; [0x08021128] = 0x40022000 / f32_bits_interpretation=2.033203125
080210d0  194b      ldr	r3, [pc, #100] ; [0x08021138] = 0x40022100 / f32_bits_interpretation=2.03326416
080210d2  9268      ldr	r2, [r2, #8]
080210d4  9b68      ldr	r3, [r3, #8]
080210d6  db07      lsls	r3, r3, #31
080210d8  88d4      bmi	#-240 ; -> 0x08020fec ; branch_target=0x08020fec
080210da  d007      lsls	r0, r2, #31
080210dc  86d4      bmi	#-244 ; -> 0x08020fec ; branch_target=0x08020fec
080210de  174a      ldr	r2, [pc, #92] ; [0x0802113c] = 0x40022300 / f32_bits_interpretation=2.03338623
080210e0  9368      ldr	r3, [r2, #8]
080210e2  6168      ldr	r1, [r4, #4]
080210e4  23f47c13  bic	r3, r3, #4128768
080210e8  0b43      orrs	r3, r1
080210ea  9360      str	r3, [r2, #8]
080210ec  7ee7      b	#-260 ; -> 0x08020fec ; branch_target=0x08020fec
080210ee  144a      ldr	r2, [pc, #80] ; [0x08021140] = 0x58026300
080210f0  f6e7      b	#-20 ; -> 0x080210e0 ; branch_target=0x080210e0
080210f2  d4e90f23  ldrd	r2, r3, [r4, #60]
080210f6  606c      ldr	r0, [r4, #68]
080210f8  013a      subs	r2, #1
080210fa  2168      ldr	r1, [r4]
080210fc  0343      orrs	r3, r0
080210fe  0869      ldr	r0, [r1, #16]
08021100  43ea0243  orr.w	r3, r3, r2, lsl #16
08021104  a26c      ldr	r2, [r4, #72]
08021106  1343      orrs	r3, r2
08021108  0e4a      ldr	r2, [pc, #56] ; [0x08021144] = 0xfc00f81e
0802110a  0240      ands	r2, r0
0802110c  1343      orrs	r3, r2
0802110e  43f00103  orr	r3, r3, #1
08021112  0b61      str	r3, [r1, #16]
08021114  a7e7      b	#-178 ; -> 0x08021066 ; branch_target=0x08021066
