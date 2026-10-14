; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
0802b944  2de9f04f  push.w	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802b948  85b0      sub	sp, #20
0802b94a  48b1      cbz	r0, #18 ; -> 0x0802b960 ; branch_target=0x0802b960
0802b94c  0368      ldr	r3, [r0]
0802b94e  0446      mov	r4, r0
0802b950  33b1      cbz	r3, #12 ; -> 0x0802b960 ; branch_target=0x0802b960
0802b952  1a78      ldrb	r2, [r3]
0802b954  22b1      cbz	r2, #8 ; -> 0x0802b960 ; branch_target=0x0802b960
0802b956  0e46      mov	r6, r1
0802b958  da88      ldrh	r2, [r3, #6]
0802b95a  8188      ldrh	r1, [r0, #4]
0802b95c  9142      cmp	r1, r2
0802b95e  04d0      beq	#8 ; -> 0x0802b96a ; branch_target=0x0802b96a
0802b960  0925      movs	r5, #9
0802b962  2846      mov	r0, r5
0802b964  05b0      add	sp, #20
0802b966  bde8f08f  pop.w	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802b96a  5878      ldrb	r0, [r3, #1]
0802b96c  fdf7a8ff  bl	#-8368 ; -> 0x080298c0 ; branch_target=0x080298c0
0802b970  c307      lsls	r3, r0, #31
0802b972  f5d4      bmi	#-22 ; -> 0x0802b960 ; branch_target=0x0802b960
0802b974  657d      ldrb	r5, [r4, #21]
0802b976  002d      cmp	r5, #0
0802b978  f3d1      bne	#-26 ; -> 0x0802b962 ; branch_target=0x0802b962
0802b97a  e06a      ldr	r0, [r4, #44]
0802b97c  d4f80090  ldr.w	r9, [r4]
0802b980  e8b1      cbz	r0, #58 ; -> 0x0802b9be ; branch_target=0x0802b9be
0802b982  771c      adds	r7, r6, #1
0802b984  5bd0      beq	#182 ; -> 0x0802ba3e ; branch_target=0x0802ba3e
0802b986  e368      ldr	r3, [r4, #12]
0802b988  9e42      cmp	r6, r3
0802b98a  28bf      it	hs
0802b98c  1e46      movhs	r6, r3
0802b98e  a661      str	r6, [r4, #24]
0802b990  002e      cmp	r6, #0
0802b992  e6d0      beq	#-52 ; -> 0x0802b962 ; branch_target=0x0802b962
0802b994  771e      subs	r7, r6, #1
0802b996  b9f80a10  ldrh.w	r1, [r9, #10]
0802b99a  4368      ldr	r3, [r0, #4]
0802b99c  021d      adds	r2, r0, #4
0802b99e  7f0a      lsrs	r7, r7, #9
0802b9a0  b7fbf1f1  udiv	r1, r7, r1
0802b9a4  2bb9      cbnz	r3, #10 ; -> 0x0802b9b2 ; branch_target=0x0802b9b2
0802b9a6  5fe0      b	#190 ; -> 0x0802ba68 ; branch_target=0x0802ba68
0802b9a8  c91a      subs	r1, r1, r3
0802b9aa  52f8083f  ldr	r3, [r2, #8]!
0802b9ae  002b      cmp	r3, #0
0802b9b0  5ad0      beq	#180 ; -> 0x0802ba68 ; branch_target=0x0802ba68
0802b9b2  8b42      cmp	r3, r1
0802b9b4  f8d9      bls	#-16 ; -> 0x0802b9a8 ; branch_target=0x0802b9a8
0802b9b6  5368      ldr	r3, [r2, #4]
0802b9b8  0b44      add	r3, r1
0802b9ba  991e      subs	r1, r3, #2
0802b9bc  56e0      b	#172 ; -> 0x0802ba6c ; branch_target=0x0802ba6c
0802b9be  e268      ldr	r2, [r4, #12]
0802b9c0  a369      ldr	r3, [r4, #24]
0802b9c2  b242      cmp	r2, r6
0802b9c4  04d2      bhs	#8 ; -> 0x0802b9d0 ; branch_target=0x0802b9d0
0802b9c6  217d      ldrb	r1, [r4, #20]
0802b9c8  8807      lsls	r0, r1, #30
0802b9ca  00f1af80  bmi.w	#350 ; -> 0x0802bb2c ; branch_target=0x0802bb2c
0802b9ce  1646      mov	r6, r2
0802b9d0  0022      movs	r2, #0
0802b9d2  a261      str	r2, [r4, #24]
0802b9d4  002e      cmp	r6, #0
0802b9d6  c4d0      beq	#-120 ; -> 0x0802b962 ; branch_target=0x0802b962
0802b9d8  b9f80a80  ldrh.w	r8, [r9, #10]
0802b9dc  4fea4828  lsl.w	r8, r8, #9
0802b9e0  002b      cmp	r3, #0
0802b9e2  70d1      bne	#224 ; -> 0x0802bac6 ; branch_target=0x0802bac6
0802b9e4  a768      ldr	r7, [r4, #8]
0802b9e6  002f      cmp	r7, #0
0802b9e8  00f02681  beq.w	#588 ; -> 0x0802bc38 ; branch_target=0x0802bc38
0802b9ec  0023      movs	r3, #0
0802b9ee  e761      str	r7, [r4, #28]
0802b9f0  b045      cmp	r8, r6
0802b9f2  80f07781  bhs.w	#750 ; -> 0x0802bce4 ; branch_target=0x0802bce4
0802b9f6  3946      mov	r1, r7
0802b9f8  14e0      b	#40 ; -> 0x0802ba24 ; branch_target=0x0802ba24
0802b9fa  fef707fd  bl	#-5618 ; -> 0x0802a40c ; branch_target=0x0802a40c
0802b9fe  0146      mov	r1, r0
0802ba00  0028      cmp	r0, #0
0802ba02  00f06a81  beq.w	#724 ; -> 0x0802bcda ; branch_target=0x0802bcda
0802ba06  4b1c      adds	r3, r1, #1
0802ba08  00f0dc80  beq.w	#440 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802ba0c  0129      cmp	r1, #1
0802ba0e  1dd9      bls	#58 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802ba10  d9f81430  ldr.w	r3, [r9, #20]
0802ba14  8b42      cmp	r3, r1
0802ba16  19d9      bls	#50 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802ba18  a269      ldr	r2, [r4, #24]
0802ba1a  b045      cmp	r8, r6
0802ba1c  e161      str	r1, [r4, #28]
0802ba1e  1346      mov	r3, r2
0802ba20  80f04281  bhs.w	#644 ; -> 0x0802bca8 ; branch_target=0x0802bca8
0802ba24  4344      add	r3, r8
0802ba26  2046      mov	r0, r4
0802ba28  a6eb0806  sub.w	r6, r6, r8
0802ba2c  a361      str	r3, [r4, #24]
0802ba2e  237d      ldrb	r3, [r4, #20]
0802ba30  9a07      lsls	r2, r3, #30
0802ba32  e2d4      bmi	#-60 ; -> 0x0802b9fa ; branch_target=0x0802b9fa
0802ba34  2068      ldr	r0, [r4]
0802ba36  fef7e9fb  bl	#-6190 ; -> 0x0802a20c ; branch_target=0x0802a20c
0802ba3a  0146      mov	r1, r0
0802ba3c  e3e7      b	#-58 ; -> 0x0802ba06 ; branch_target=0x0802ba06
0802ba3e  8046      mov	r8, r0
0802ba40  a268      ldr	r2, [r4, #8]
0802ba42  58f8041b  ldr	r1, [r8], #4
0802ba46  2ab1      cbz	r2, #10 ; -> 0x0802ba54 ; branch_target=0x0802ba54
0802ba48  012a      cmp	r2, #1
0802ba4a  73d1      bne	#230 ; -> 0x0802bb34 ; branch_target=0x0802bb34
0802ba4c  0223      movs	r3, #2
0802ba4e  1d46      mov	r5, r3
0802ba50  6375      strb	r3, [r4, #21]
0802ba52  86e7      b	#-244 ; -> 0x0802b962 ; branch_target=0x0802b962
0802ba54  4ff0020a  mov.w	r10, #2
0802ba58  8a45      cmp	r10, r1
0802ba5a  c0f800a0  str.w	r10, [r0]
0802ba5e  67d8      bhi	#206 ; -> 0x0802bb30 ; branch_target=0x0802bb30
0802ba60  0023      movs	r3, #0
0802ba62  c8f80030  str.w	r3, [r8]
0802ba66  7ce7      b	#-264 ; -> 0x0802b962 ; branch_target=0x0802b962
0802ba68  6ff00101  mvn	r1, #1
0802ba6c  e361      str	r3, [r4, #28]
0802ba6e  d9f81430  ldr.w	r3, [r9, #20]
0802ba72  023b      subs	r3, #2
0802ba74  8b42      cmp	r3, r1
0802ba76  e9d9      bls	#-46 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802ba78  b9f80a00  ldrh.w	r0, [r9, #10]
0802ba7c  d9f82820  ldr.w	r2, [r9, #40]
0802ba80  01fb0022  mla	r2, r1, r0, r2
0802ba84  002a      cmp	r2, #0
0802ba86  e1d0      beq	#-62 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802ba88  c6f30806  ubfx	r6, r6, #0, #9
0802ba8c  002e      cmp	r6, #0
0802ba8e  3ff468af  beq.w	#-304 ; -> 0x0802b962 ; branch_target=0x0802b962
0802ba92  0138      subs	r0, #1
0802ba94  3840      ands	r0, r7
0802ba96  8618      adds	r6, r0, r2
0802ba98  226a      ldr	r2, [r4, #32]
0802ba9a  b242      cmp	r2, r6
0802ba9c  3ff461af  beq.w	#-318 ; -> 0x0802b962 ; branch_target=0x0802b962
0802baa0  94f91430  ldrsb.w	r3, [r4, #20]
0802baa4  04f13007  add.w	r7, r4, #48
0802baa8  99f80100  ldrb.w	r0, [r9, #1]
0802baac  002b      cmp	r3, #0
0802baae  c0f2ee80  blt.w	#476 ; -> 0x0802bc8e ; branch_target=0x0802bc8e
0802bab2  3946      mov	r1, r7
0802bab4  0123      movs	r3, #1
0802bab6  3246      mov	r2, r6
0802bab8  fdf71eff  bl	#-8644 ; -> 0x080298f8 ; branch_target=0x080298f8
0802babc  0028      cmp	r0, #0
0802babe  40f08180  bne.w	#258 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802bac2  2662      str	r6, [r4, #32]
0802bac4  4de7      b	#-358 ; -> 0x0802b962 ; branch_target=0x0802b962
0802bac6  013b      subs	r3, #1
0802bac8  721e      subs	r2, r6, #1
0802baca  b3fbf8f1  udiv	r1, r3, r8
0802bace  b2fbf8f2  udiv	r2, r2, r8
0802bad2  8a42      cmp	r2, r1
0802bad4  86d3      blo	#-244 ; -> 0x0802b9e4 ; branch_target=0x0802b9e4
0802bad6  c8f10002  rsb.w	r2, r8, #0
0802bada  e769      ldr	r7, [r4, #28]
0802badc  1340      ands	r3, r2
0802bade  f61a      subs	r6, r6, r3
0802bae0  a361      str	r3, [r4, #24]
0802bae2  002f      cmp	r7, #0
0802bae4  84d1      bne	#-248 ; -> 0x0802b9f0 ; branch_target=0x0802b9f0
0802bae6  e268      ldr	r2, [r4, #12]
0802bae8  9a42      cmp	r2, r3
0802baea  04d2      bhs	#8 ; -> 0x0802baf6 ; branch_target=0x0802baf6
0802baec  227d      ldrb	r2, [r4, #20]
0802baee  e360      str	r3, [r4, #12]
0802baf0  42f04002  orr	r2, r2, #64
0802baf4  2275      strb	r2, [r4, #20]
0802baf6  c3f30803  ubfx	r3, r3, #0, #9
0802bafa  002b      cmp	r3, #0
0802bafc  3ff431af  beq.w	#-414 ; -> 0x0802b962 ; branch_target=0x0802b962
0802bb00  226a      ldr	r2, [r4, #32]
0802bb02  ba42      cmp	r2, r7
0802bb04  3ff42daf  beq.w	#-422 ; -> 0x0802b962 ; branch_target=0x0802b962
0802bb08  94f91430  ldrsb.w	r3, [r4, #20]
0802bb0c  04f13006  add.w	r6, r4, #48
0802bb10  99f80100  ldrb.w	r0, [r9, #1]
0802bb14  002b      cmp	r3, #0
0802bb16  c0f29d80  blt.w	#314 ; -> 0x0802bc54 ; branch_target=0x0802bc54
0802bb1a  3146      mov	r1, r6
0802bb1c  0123      movs	r3, #1
0802bb1e  3a46      mov	r2, r7
0802bb20  fdf7eafe  bl	#-8748 ; -> 0x080298f8 ; branch_target=0x080298f8
0802bb24  0028      cmp	r0, #0
0802bb26  4dd1      bne	#154 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802bb28  2762      str	r7, [r4, #32]
0802bb2a  1ae7      b	#-460 ; -> 0x0802b962 ; branch_target=0x0802b962
0802bb2c  a561      str	r5, [r4, #24]
0802bb2e  53e7      b	#-346 ; -> 0x0802b9d8 ; branch_target=0x0802b9d8
0802bb30  1125      movs	r5, #17
0802bb32  16e7      b	#-468 ; -> 0x0802b962 ; branch_target=0x0802b962
0802bb34  4f46      mov	r7, r9
0802bb36  4ff0020a  mov.w	r10, #2
0802bb3a  c346      mov	r11, r8
0802bb3c  1646      mov	r6, r2
0802bb3e  0291      str	r1, [sp, #8]
0802bb40  0395      str	r5, [sp, #12]
0802bb42  3546      mov	r5, r6
0802bb44  4ff00108  mov.w	r8, #1
0802bb48  0096      str	r6, [sp]
0802bb4a  34e0      b	#104 ; -> 0x0802bbb6 ; branch_target=0x0802bbb6
0802bb4c  3b78      ldrb	r3, [r7]
0802bb4e  022b      cmp	r3, #2
0802bb50  4fd0      beq	#158 ; -> 0x0802bbf2 ; branch_target=0x0802bbf2
0802bb52  032b      cmp	r3, #3
0802bb54  3ad0      beq	#116 ; -> 0x0802bbcc ; branch_target=0x0802bbcc
0802bb56  012b      cmp	r3, #1
0802bb58  7ff478af  bne.w	#-272 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802bb5c  396a      ldr	r1, [r7, #32]
0802bb5e  05eb5506  add.w	r6, r5, r5, lsr #1
0802bb62  3846      mov	r0, r7
0802bb64  01eb5621  add.w	r1, r1, r6, lsr #9
0802bb68  fef728f9  bl	#-7600 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802bb6c  50bb      cbnz	r0, #84 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802bb6e  c6f30802  ubfx	r2, r6, #0, #9
0802bb72  396a      ldr	r1, [r7, #32]
0802bb74  0136      adds	r6, #1
0802bb76  3846      mov	r0, r7
0802bb78  3a44      add	r2, r7
0802bb7a  01eb5621  add.w	r1, r1, r6, lsr #9
0802bb7e  92f83030  ldrb.w	r3, [r2, #48]
0802bb82  0193      str	r3, [sp, #4]
0802bb84  fef71af9  bl	#-7628 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802bb88  e0b9      cbnz	r0, #56 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802bb8a  c6f30803  ubfx	r3, r6, #0, #9
0802bb8e  019a      ldr	r2, [sp, #4]
0802bb90  ee07      lsls	r6, r5, #31
0802bb92  3b44      add	r3, r7
0802bb94  93f83030  ldrb.w	r3, [r3, #48]
0802bb98  42ea0322  orr.w	r2, r2, r3, lsl #8
0802bb9c  70d5      bpl	#224 ; -> 0x0802bc80 ; branch_target=0x0802bc80
0802bb9e  1f2a      cmp	r2, #31
0802bba0  4fea1213  lsr.w	r3, r2, #4
0802bba4  7ff652af  bls.w	#-348 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802bba8  0135      adds	r5, #1
0802bbaa  9d42      cmp	r5, r3
0802bbac  31d1      bne	#98 ; -> 0x0802bc12 ; branch_target=0x0802bc12
0802bbae  08f10108  add.w	r8, r8, #1
0802bbb2  1d46      mov	r5, r3
0802bbb4  2768      ldr	r7, [r4]
0802bbb6  7b69      ldr	r3, [r7, #20]
0802bbb8  ab42      cmp	r3, r5
0802bbba  c7d8      bhi	#-114 ; -> 0x0802bb4c ; branch_target=0x0802bb4c
0802bbbc  0223      movs	r3, #2
0802bbbe  1d46      mov	r5, r3
0802bbc0  6375      strb	r3, [r4, #21]
0802bbc2  cee6      b	#-612 ; -> 0x0802b962 ; branch_target=0x0802b962
0802bbc4  0123      movs	r3, #1
0802bbc6  1d46      mov	r5, r3
0802bbc8  6375      strb	r3, [r4, #21]
0802bbca  cae6      b	#-620 ; -> 0x0802b962 ; branch_target=0x0802b962
0802bbcc  396a      ldr	r1, [r7, #32]
0802bbce  3846      mov	r0, r7
0802bbd0  01ebd511  add.w	r1, r1, r5, lsr #7
0802bbd4  fef7f2f8  bl	#-7708 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802bbd8  0028      cmp	r0, #0
0802bbda  f3d1      bne	#-26 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802bbdc  ab00      lsls	r3, r5, #2
0802bbde  4549      ldr	r1, [pc, #276] ; [0x0802bcf4] = 0x0ffffffe
0802bbe0  03f4fe73  and	r3, r3, #508
0802bbe4  3b44      add	r3, r7
0802bbe6  1a6b      ldr	r2, [r3, #48]
0802bbe8  0a42      tst	r2, r1
0802bbea  22f07043  bic	r3, r2, #4026531840
0802bbee  dbd1      bne	#-74 ; -> 0x0802bba8 ; branch_target=0x0802bba8
0802bbf0  2ce7      b	#-424 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802bbf2  396a      ldr	r1, [r7, #32]
0802bbf4  3846      mov	r0, r7
0802bbf6  01eb1521  add.w	r1, r1, r5, lsr #8
0802bbfa  fef7dff8  bl	#-7746 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802bbfe  0028      cmp	r0, #0
0802bc00  e0d1      bne	#-64 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802bc02  6b00      lsls	r3, r5, #1
0802bc04  03f4ff73  and	r3, r3, #510
0802bc08  3b44      add	r3, r7
0802bc0a  1b8e      ldrh	r3, [r3, #48]
0802bc0c  012b      cmp	r3, #1
0802bc0e  cbd8      bhi	#-106 ; -> 0x0802bba8 ; branch_target=0x0802bba8
0802bc10  1ce7      b	#-456 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802bc12  0af1020a  add.w	r10, r10, #2
0802bc16  029a      ldr	r2, [sp, #8]
0802bc18  009e      ldr	r6, [sp]
0802bc1a  5245      cmp	r2, r10
0802bc1c  27d3      blo	#78 ; -> 0x0802bc6e ; branch_target=0x0802bc6e
0802bc1e  5a46      mov	r2, r11
0802bc20  42f8088b  str	r8, [r2], #8
0802bc24  cbf80460  str.w	r6, [r11, #4]
0802bc28  d9f81410  ldr.w	r1, [r9, #20]
0802bc2c  9942      cmp	r1, r3
0802bc2e  5dd9      bls	#186 ; -> 0x0802bcec ; branch_target=0x0802bcec
0802bc30  9346      mov	r11, r2
0802bc32  1e46      mov	r6, r3
0802bc34  2768      ldr	r7, [r4]
0802bc36  84e7      b	#-248 ; -> 0x0802bb42 ; branch_target=0x0802bb42
0802bc38  3946      mov	r1, r7
0802bc3a  2046      mov	r0, r4
0802bc3c  fef7e6fb  bl	#-6196 ; -> 0x0802a40c ; branch_target=0x0802a40c
0802bc40  0128      cmp	r0, #1
0802bc42  0746      mov	r7, r0
0802bc44  3ff402af  beq.w	#-508 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802bc48  411c      adds	r1, r0, #1
0802bc4a  bbd0      beq	#-138 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802bc4c  a369      ldr	r3, [r4, #24]
0802bc4e  a060      str	r0, [r4, #8]
0802bc50  e061      str	r0, [r4, #28]
0802bc52  46e7      b	#-372 ; -> 0x0802bae2 ; branch_target=0x0802bae2
0802bc54  0123      movs	r3, #1
0802bc56  3146      mov	r1, r6
0802bc58  fdf75cfe  bl	#-9032 ; -> 0x08029914 ; branch_target=0x08029914
0802bc5c  0028      cmp	r0, #0
0802bc5e  b1d1      bne	#-158 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802bc60  237d      ldrb	r3, [r4, #20]
0802bc62  03f07f03  and	r3, r3, #127
0802bc66  2375      strb	r3, [r4, #20]
0802bc68  99f80100  ldrb.w	r0, [r9, #1]
0802bc6c  55e7      b	#-342 ; -> 0x0802bb1a ; branch_target=0x0802bb1a
0802bc6e  d9f81420  ldr.w	r2, [r9, #20]
0802bc72  9a42      cmp	r2, r3
0802bc74  ddd8      bhi	#-70 ; -> 0x0802bc32 ; branch_target=0x0802bc32
0802bc76  d846      mov	r8, r11
0802bc78  dde90215  ldrd	r1, r5, [sp, #8]
0802bc7c  e06a      ldr	r0, [r4, #44]
0802bc7e  ebe6      b	#-554 ; -> 0x0802ba58 ; branch_target=0x0802ba58
0802bc80  40f6fe71  movw	r1, #4094
0802bc84  c2f30b03  ubfx	r3, r2, #0, #12
0802bc88  0a42      tst	r2, r1
0802bc8a  8dd1      bne	#-230 ; -> 0x0802bba8 ; branch_target=0x0802bba8
0802bc8c  dee6      b	#-580 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802bc8e  0123      movs	r3, #1
0802bc90  3946      mov	r1, r7
0802bc92  fdf73ffe  bl	#-9090 ; -> 0x08029914 ; branch_target=0x08029914
0802bc96  0028      cmp	r0, #0
0802bc98  94d1      bne	#-216 ; -> 0x0802bbc4 ; branch_target=0x0802bbc4
0802bc9a  237d      ldrb	r3, [r4, #20]
0802bc9c  03f07f03  and	r3, r3, #127
0802bca0  2375      strb	r3, [r4, #20]
0802bca2  99f80100  ldrb.w	r0, [r9, #1]
0802bca6  04e7      b	#-504 ; -> 0x0802bab2 ; branch_target=0x0802bab2
0802bca8  0f46      mov	r7, r1
0802bcaa  c6f30801  ubfx	r1, r6, #0, #9
0802bcae  b318      adds	r3, r6, r2
0802bcb0  a361      str	r3, [r4, #24]
0802bcb2  a9b1      cbz	r1, #42 ; -> 0x0802bce0 ; branch_target=0x0802bce0
0802bcb4  d9f81420  ldr.w	r2, [r9, #20]
0802bcb8  b91e      subs	r1, r7, #2
0802bcba  023a      subs	r2, #2
0802bcbc  9142      cmp	r1, r2
0802bcbe  bff4c5ae  bhs.w	#-630 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802bcc2  b9f80a20  ldrh.w	r2, [r9, #10]
0802bcc6  d9f82870  ldr.w	r7, [r9, #40]
0802bcca  01fb0277  mla	r7, r1, r2, r7
0802bcce  002f      cmp	r7, #0
0802bcd0  3ff4bcae  beq.w	#-648 ; -> 0x0802ba4c ; branch_target=0x0802ba4c
0802bcd4  07eb5627  add.w	r7, r7, r6, lsr #9
0802bcd8  05e7      b	#-502 ; -> 0x0802bae6 ; branch_target=0x0802bae6
0802bcda  0746      mov	r7, r0
0802bcdc  a369      ldr	r3, [r4, #24]
0802bcde  02e7      b	#-508 ; -> 0x0802bae6 ; branch_target=0x0802bae6
0802bce0  0f46      mov	r7, r1
0802bce2  00e7      b	#-512 ; -> 0x0802bae6 ; branch_target=0x0802bae6
0802bce4  c6f30801  ubfx	r1, r6, #0, #9
0802bce8  a269      ldr	r2, [r4, #24]
0802bcea  e0e7      b	#-64 ; -> 0x0802bcae ; branch_target=0x0802bcae
0802bcec  0299      ldr	r1, [sp, #8]
0802bcee  9046      mov	r8, r2
0802bcf0  039d      ldr	r5, [sp, #12]
0802bcf2  c3e7      b	#-122 ; -> 0x0802bc7c ; branch_target=0x0802bc7c
