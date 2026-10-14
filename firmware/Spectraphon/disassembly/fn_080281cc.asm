; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
080281cc  90f83c30  ldrb.w	r3, [r0, #60]
080281d0  012b      cmp	r3, #1
080281d2  4dd0      beq	#154 ; -> 0x08028270 ; branch_target=0x08028270
080281d4  0246      mov	r2, r0
080281d6  0223      movs	r3, #2
080281d8  30b4      push	{r4, r5}
080281da  264d      ldr	r5, [pc, #152] ; [0x08028274] = 0x40010000 / f32_bits_interpretation=2.015625
080281dc  0124      movs	r4, #1
080281de  0068      ldr	r0, [r0]
080281e0  82f83c40  strb.w	r4, [r2, #60]
080281e4  a842      cmp	r0, r5
080281e6  82f83d30  strb.w	r3, [r2, #61]
080281ea  4368      ldr	r3, [r0, #4]
080281ec  8468      ldr	r4, [r0, #8]
080281ee  3ad0      beq	#116 ; -> 0x08028266 ; branch_target=0x08028266
080281f0  05f58065  add.w	r5, r5, #1024
080281f4  a842      cmp	r0, r5
080281f6  36d0      beq	#108 ; -> 0x08028266 ; branch_target=0x08028266
080281f8  0d68      ldr	r5, [r1]
080281fa  23f07003  bic	r3, r3, #112
080281fe  2b43      orrs	r3, r5
08028200  1d4d      ldr	r5, [pc, #116] ; [0x08028278] = 0x40000400 / f32_bits_interpretation=2.000244141
08028202  4360      str	r3, [r0, #4]
08028204  1b4b      ldr	r3, [pc, #108] ; [0x08028274] = 0x40010000 / f32_bits_interpretation=2.015625
08028206  1068      ldr	r0, [r2]
08028208  b0f1804f  cmp.w	r0, #1073741824
0802820c  18bf      it	ne
0802820e  9842      cmpne	r0, r3
08028210  0cbf      ite	eq
08028212  0123      moveq	r3, #1
08028214  0023      movne	r3, #0
08028216  a842      cmp	r0, r5
08028218  08bf      it	eq
0802821a  43f00103  orreq	r3, r3, #1
0802821e  05f58065  add.w	r5, r5, #1024
08028222  a842      cmp	r0, r5
08028224  08bf      it	eq
08028226  43f00103  orreq	r3, r3, #1
0802822a  05f58065  add.w	r5, r5, #1024
0802822e  a842      cmp	r0, r5
08028230  08bf      it	eq
08028232  43f00103  orreq	r3, r3, #1
08028236  05f57845  add.w	r5, r5, #63488
0802823a  a842      cmp	r0, r5
0802823c  08bf      it	eq
0802823e  43f00103  orreq	r3, r3, #1
08028242  13b9      cbnz	r3, #4 ; -> 0x0802824a ; branch_target=0x0802824a
08028244  0d4b      ldr	r3, [pc, #52] ; [0x0802827c] = 0x40001800 / f32_bits_interpretation=2.001464844
08028246  9842      cmp	r0, r3
08028248  04d1      bne	#8 ; -> 0x08028254 ; branch_target=0x08028254
0802824a  8b68      ldr	r3, [r1, #8]
0802824c  24f08004  bic	r4, r4, #128
08028250  1c43      orrs	r4, r3
08028252  8460      str	r4, [r0, #8]
08028254  0023      movs	r3, #0
08028256  0121      movs	r1, #1
08028258  1846      mov	r0, r3
0802825a  82f83d10  strb.w	r1, [r2, #61]
0802825e  82f83c30  strb.w	r3, [r2, #60]
08028262  30bc      pop	{r4, r5}
08028264  7047      bx	lr
08028266  23f47003  bic	r3, r3, #15728640
0802826a  4d68      ldr	r5, [r1, #4]
0802826c  2b43      orrs	r3, r5
0802826e  c3e7      b	#-122 ; -> 0x080281f8 ; branch_target=0x080281f8
08028270  0220      movs	r0, #2
08028272  7047      bx	lr
