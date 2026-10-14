; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
0802a9e8  0129      cmp	r1, #1
0802a9ea  2de9f84f  push.w	{r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
0802a9ee  0c46      mov	r4, r1
0802a9f0  0668      ldr	r6, [r0]
0802a9f2  09d9      bls	#18 ; -> 0x0802aa08 ; branch_target=0x0802aa08
0802a9f4  7369      ldr	r3, [r6, #20]
0802a9f6  8b42      cmp	r3, r1
0802a9f8  06d9      bls	#12 ; -> 0x0802aa08 ; branch_target=0x0802aa08
0802a9fa  0546      mov	r5, r0
0802a9fc  1146      mov	r1, r2
0802a9fe  32b9      cbnz	r2, #12 ; -> 0x0802aa0e ; branch_target=0x0802aa0e
0802aa00  b146      mov	r9, r6
0802aa02  0127      movs	r7, #1
0802aa04  9c42      cmp	r4, r3
0802aa06  12d3      blo	#36 ; -> 0x0802aa2e ; branch_target=0x0802aa2e
0802aa08  0220      movs	r0, #2
0802aa0a  bde8f88f  pop.w	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802aa0e  9342      cmp	r3, r2
0802aa10  fad9      bls	#-12 ; -> 0x0802aa08 ; branch_target=0x0802aa08
0802aa12  012a      cmp	r2, #1
0802aa14  f8d0      beq	#-16 ; -> 0x0802aa08 ; branch_target=0x0802aa08
0802aa16  4ff0ff32  mov.w	r2, #4294967295
0802aa1a  3046      mov	r0, r6
0802aa1c  fff776fb  bl	#-2324 ; -> 0x0802a10c ; branch_target=0x0802a10c
0802aa20  0028      cmp	r0, #0
0802aa22  f2d1      bne	#-28 ; -> 0x0802aa0a ; branch_target=0x0802aa0a
0802aa24  d5f80090  ldr.w	r9, [r5]
0802aa28  d9f81430  ldr.w	r3, [r9, #20]
0802aa2c  e9e7      b	#-46 ; -> 0x0802aa02 ; branch_target=0x0802aa02
0802aa2e  99f80030  ldrb.w	r3, [r9]
0802aa32  022b      cmp	r3, #2
0802aa34  00f09880  beq.w	#304 ; -> 0x0802ab68 ; branch_target=0x0802ab68
0802aa38  032b      cmp	r3, #3
0802aa3a  10d0      beq	#32 ; -> 0x0802aa5e ; branch_target=0x0802aa5e
0802aa3c  012b      cmp	r3, #1
0802aa3e  e3d1      bne	#-58 ; -> 0x0802aa08 ; branch_target=0x0802aa08
0802aa40  d9f82010  ldr.w	r1, [r9, #32]
0802aa44  04eb5408  add.w	r8, r4, r4, lsr #1
0802aa48  4846      mov	r0, r9
0802aa4a  01eb5821  add.w	r1, r1, r8, lsr #9
0802aa4e  fff7b5f9  bl	#-3222 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802aa52  0028      cmp	r0, #0
0802aa54  00f0b380  beq.w	#358 ; -> 0x0802abbe ; branch_target=0x0802abbe
0802aa58  0120      movs	r0, #1
0802aa5a  bde8f88f  pop.w	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
0802aa5e  d9f82010  ldr.w	r1, [r9, #32]
0802aa62  4846      mov	r0, r9
0802aa64  01ebd411  add.w	r1, r1, r4, lsr #7
0802aa68  fff7a8f9  bl	#-3248 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802aa6c  0028      cmp	r0, #0
0802aa6e  f3d1      bne	#-26 ; -> 0x0802aa58 ; branch_target=0x0802aa58
0802aa70  a300      lsls	r3, r4, #2
0802aa72  03f4fe73  and	r3, r3, #508
0802aa76  4b44      add	r3, r9
0802aa78  d3f83080  ldr.w	r8, [r3, #48]
0802aa7c  38f07048  bics	r8, r8, #4026531840
0802aa80  00f0bb80  beq.w	#374 ; -> 0x0802abfa ; branch_target=0x0802abfa
0802aa84  b8f1010f  cmp.w	r8, #1
0802aa88  bed0      beq	#-132 ; -> 0x0802aa08 ; branch_target=0x0802aa08
0802aa8a  7369      ldr	r3, [r6, #20]
0802aa8c  9c42      cmp	r4, r3
0802aa8e  bbd2      bhs	#-138 ; -> 0x0802aa08 ; branch_target=0x0802aa08
0802aa90  96f80090  ldrb.w	r9, [r6]
0802aa94  b9f1020f  cmp.w	r9, #2
0802aa98  2ed0      beq	#92 ; -> 0x0802aaf8 ; branch_target=0x0802aaf8
0802aa9a  b9f1030f  cmp.w	r9, #3
0802aa9e  4cd0      beq	#152 ; -> 0x0802ab3a ; branch_target=0x0802ab3a
0802aaa0  b9f1010f  cmp.w	r9, #1
0802aaa4  b0d1      bne	#-160 ; -> 0x0802aa08 ; branch_target=0x0802aa08
0802aaa6  316a      ldr	r1, [r6, #32]
0802aaa8  04eb540a  add.w	r10, r4, r4, lsr #1
0802aaac  3046      mov	r0, r6
0802aaae  01eb5a21  add.w	r1, r1, r10, lsr #9
0802aab2  fff783f9  bl	#-3322 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802aab6  0028      cmp	r0, #0
0802aab8  a7d1      bne	#-178 ; -> 0x0802aa0a ; branch_target=0x0802aa0a
0802aaba  0af1010b  add.w	r11, r10, #1
0802aabe  e107      lsls	r1, r4, #31
0802aac0  caf30803  ubfx	r3, r10, #0, #9
0802aac4  06f1300a  add.w	r10, r6, #48
0802aac8  4fea5b22  lsr.w	r2, r11, #9
0802aacc  61d5      bpl	#194 ; -> 0x0802ab92 ; branch_target=0x0802ab92
0802aace  1af80310  ldrb.w	r1, [r10, r3]
0802aad2  3046      mov	r0, r6
0802aad4  01f00f01  and	r1, r1, #15
0802aad8  0af80310  strb.w	r1, [r10, r3]
0802aadc  316a      ldr	r1, [r6, #32]
0802aade  86f80390  strb.w	r9, [r6, #3]
0802aae2  1144      add	r1, r2
0802aae4  fff76af9  bl	#-3372 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802aae8  0028      cmp	r0, #0
0802aaea  8ed1      bne	#-228 ; -> 0x0802aa0a ; branch_target=0x0802aa0a
0802aaec  cbf3080b  ubfx	r11, r11, #0, #9
0802aaf0  0aeb0b03  add.w	r3, r10, r11
0802aaf4  1870      strb	r0, [r3]
0802aaf6  0ce0      b	#24 ; -> 0x0802ab12 ; branch_target=0x0802ab12
0802aaf8  316a      ldr	r1, [r6, #32]
0802aafa  3046      mov	r0, r6
0802aafc  01eb1421  add.w	r1, r1, r4, lsr #8
0802ab00  fff75cf9  bl	#-3400 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802ab04  0028      cmp	r0, #0
0802ab06  80d1      bne	#-256 ; -> 0x0802aa0a ; branch_target=0x0802aa0a
0802ab08  6400      lsls	r4, r4, #1
0802ab0a  04f4ff74  and	r4, r4, #510
0802ab0e  3444      add	r4, r6
0802ab10  2086      strh	r0, [r4, #48]
0802ab12  7269      ldr	r2, [r6, #20]
0802ab14  3369      ldr	r3, [r6, #16]
0802ab16  911e      subs	r1, r2, #2
0802ab18  f770      strb	r7, [r6, #3]
0802ab1a  8b42      cmp	r3, r1
0802ab1c  05d2      bhs	#10 ; -> 0x0802ab2a ; branch_target=0x0802ab2a
0802ab1e  0133      adds	r3, #1
0802ab20  3361      str	r3, [r6, #16]
0802ab22  3379      ldrb	r3, [r6, #4]
0802ab24  43f00103  orr	r3, r3, #1
0802ab28  3371      strb	r3, [r6, #4]
0802ab2a  4245      cmp	r2, r8
0802ab2c  65d9      bls	#202 ; -> 0x0802abfa ; branch_target=0x0802abfa
0802ab2e  d5f80090  ldr.w	r9, [r5]
0802ab32  4446      mov	r4, r8
0802ab34  d9f81430  ldr.w	r3, [r9, #20]
0802ab38  64e7      b	#-312 ; -> 0x0802aa04 ; branch_target=0x0802aa04
0802ab3a  316a      ldr	r1, [r6, #32]
0802ab3c  3046      mov	r0, r6
0802ab3e  01ebd411  add.w	r1, r1, r4, lsr #7
0802ab42  fff73bf9  bl	#-3466 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802ab46  0028      cmp	r0, #0
0802ab48  7ff45faf  bne.w	#-322 ; -> 0x0802aa0a ; branch_target=0x0802aa0a
0802ab4c  a400      lsls	r4, r4, #2
0802ab4e  06f13002  add.w	r2, r6, #48
0802ab52  04f4fe74  and	r4, r4, #508
0802ab56  1159      ldr	r1, [r2, r4]
0802ab58  1053      strh	r0, [r2, r4]
0802ab5a  2244      add	r2, r4
0802ab5c  0b0e      lsrs	r3, r1, #24
0802ab5e  9070      strb	r0, [r2, #2]
0802ab60  03f0f003  and	r3, r3, #240
0802ab64  d370      strb	r3, [r2, #3]
0802ab66  d4e7      b	#-88 ; -> 0x0802ab12 ; branch_target=0x0802ab12
0802ab68  d9f82010  ldr.w	r1, [r9, #32]
0802ab6c  4846      mov	r0, r9
0802ab6e  01eb1421  add.w	r1, r1, r4, lsr #8
0802ab72  fff723f9  bl	#-3514 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802ab76  0028      cmp	r0, #0
0802ab78  7ff46eaf  bne.w	#-292 ; -> 0x0802aa58 ; branch_target=0x0802aa58
0802ab7c  6300      lsls	r3, r4, #1
0802ab7e  03f4ff73  and	r3, r3, #510
0802ab82  4b44      add	r3, r9
0802ab84  b3f83080  ldrh.w	r8, [r3, #48]
0802ab88  b8f1000f  cmp.w	r8, #0
0802ab8c  7ff47aaf  bne.w	#-268 ; -> 0x0802aa84 ; branch_target=0x0802aa84
0802ab90  33e0      b	#102 ; -> 0x0802abfa ; branch_target=0x0802abfa
0802ab92  0af80300  strb.w	r0, [r10, r3]
0802ab96  3046      mov	r0, r6
0802ab98  316a      ldr	r1, [r6, #32]
0802ab9a  86f80390  strb.w	r9, [r6, #3]
0802ab9e  1144      add	r1, r2
0802aba0  fff70cf9  bl	#-3560 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802aba4  0028      cmp	r0, #0
0802aba6  7ff430af  bne.w	#-416 ; -> 0x0802aa0a ; branch_target=0x0802aa0a
0802abaa  cbf3080b  ubfx	r11, r11, #0, #9
0802abae  1af80b00  ldrb.w	r0, [r10, r11]
0802abb2  0aeb0b03  add.w	r3, r10, r11
0802abb6  00f0f000  and	r0, r0, #240
0802abba  1870      strb	r0, [r3]
0802abbc  a9e7      b	#-174 ; -> 0x0802ab12 ; branch_target=0x0802ab12
0802abbe  c8f30803  ubfx	r3, r8, #0, #9
0802abc2  d9f82010  ldr.w	r1, [r9, #32]
0802abc6  08f10108  add.w	r8, r8, #1
0802abca  4846      mov	r0, r9
0802abcc  4b44      add	r3, r9
0802abce  01eb5821  add.w	r1, r1, r8, lsr #9
0802abd2  93f830a0  ldrb.w	r10, [r3, #48]
0802abd6  fff7f1f8  bl	#-3614 ; -> 0x08029dbc ; branch_target=0x08029dbc
0802abda  0028      cmp	r0, #0
0802abdc  7ff43caf  bne.w	#-392 ; -> 0x0802aa58 ; branch_target=0x0802aa58
0802abe0  c8f30808  ubfx	r8, r8, #0, #9
0802abe4  e007      lsls	r0, r4, #31
0802abe6  c844      add	r8, r9
0802abe8  98f83030  ldrb.w	r3, [r8, #48]
0802abec  4aea0328  orr.w	r8, r10, r3, lsl #8
0802abf0  05d5      bpl	#10 ; -> 0x0802abfe ; branch_target=0x0802abfe
0802abf2  5fea1818  lsrs.w	r8, r8, #4
0802abf6  7ff445af  bne.w	#-374 ; -> 0x0802aa84 ; branch_target=0x0802aa84
0802abfa  0020      movs	r0, #0
0802abfc  05e7      b	#-502 ; -> 0x0802aa0a ; branch_target=0x0802aa0a
0802abfe  c8f30b08  ubfx	r8, r8, #0, #12
0802ac02  b8f1000f  cmp.w	r8, #0
0802ac06  7ff43daf  bne.w	#-390 ; -> 0x0802aa84 ; branch_target=0x0802aa84
0802ac0a  f6e7      b	#-20 ; -> 0x0802abfa ; branch_target=0x0802abfa
