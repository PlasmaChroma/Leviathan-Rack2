; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08027bc8  1029      cmp	r1, #16
08027bca  0ad8      bhi	#20 ; -> 0x08027be2 ; branch_target=0x08027be2
08027bcc  dfe801f0  tbb	[pc, r1]
08027be2  90f84330  ldrb.w	r3, [r0, #67]
08027be6  012b      cmp	r3, #1
08027be8  40f09780  bne.w	#302 ; -> 0x08027d1a ; branch_target=0x08027d1a
08027bec  0b1f      subs	r3, r1, #4
08027bee  0c2b      cmp	r3, #12
08027bf0  00f28f80  bhi.w	#286 ; -> 0x08027d12 ; branch_target=0x08027d12
08027bf4  dfe803f0  tbb	[pc, r3]
08027c06  90f83e30  ldrb.w	r3, [r0, #62]
08027c0a  012b      cmp	r3, #1
08027c0c  40f08580  bne.w	#266 ; -> 0x08027d1a ; branch_target=0x08027d1a
08027c10  0223      movs	r3, #2
08027c12  80f83e30  strb.w	r3, [r0, #62]
08027c16  01f01f01  and	r1, r1, #31
08027c1a  0123      movs	r3, #1
08027c1c  70b4      push	{r4, r5, r6}
08027c1e  0468      ldr	r4, [r0]
08027c20  8b40      lsls	r3, r1
08027c22  404d      ldr	r5, [pc, #256] ; [0x08027d24] = 0x40010000 / f32_bits_interpretation=2.015625
08027c24  226a      ldr	r2, [r4, #32]
08027c26  404e      ldr	r6, [pc, #256] ; [0x08027d28] = 0x40014000 / f32_bits_interpretation=2.01953125
08027c28  22ea0302  bic.w	r2, r2, r3
08027c2c  2262      str	r2, [r4, #32]
08027c2e  226a      ldr	r2, [r4, #32]
08027c30  1a43      orrs	r2, r3
08027c32  3e4b      ldr	r3, [pc, #248] ; [0x08027d2c] = 0x40010400 / f32_bits_interpretation=2.015869141
08027c34  2262      str	r2, [r4, #32]
08027c36  0268      ldr	r2, [r0]
08027c38  3d4c      ldr	r4, [pc, #244] ; [0x08027d30] = 0x40014400 / f32_bits_interpretation=2.019775391
08027c3a  9a42      cmp	r2, r3
08027c3c  18bf      it	ne
08027c3e  aa42      cmpne	r2, r5
08027c40  0cbf      ite	eq
08027c42  0121      moveq	r1, #1
08027c44  0021      movne	r1, #0
08027c46  b242      cmp	r2, r6
08027c48  08bf      it	eq
08027c4a  41f00101  orreq	r1, r1, #1
08027c4e  a242      cmp	r2, r4
08027c50  08bf      it	eq
08027c52  41f00101  orreq	r1, r1, #1
08027c56  04f58064  add.w	r4, r4, #1024
08027c5a  a242      cmp	r2, r4
08027c5c  08bf      it	eq
08027c5e  41f00101  orreq	r1, r1, #1
08027c62  0029      cmp	r1, #0
08027c64  5bd0      beq	#182 ; -> 0x08027d1e ; branch_target=0x08027d1e
08027c66  516c      ldr	r1, [r2, #68]
08027c68  41f40041  orr	r1, r1, #32768
08027c6c  5164      str	r1, [r2, #68]
08027c6e  0268      ldr	r2, [r0]
08027c70  511b      subs	r1, r2, r5
08027c72  d31a      subs	r3, r2, r3
08027c74  b1fa81f1  clz	r1, r1
08027c78  b3fa83f3  clz	r3, r3
08027c7c  4909      lsrs	r1, r1, #5
08027c7e  5b09      lsrs	r3, r3, #5
08027c80  0b43      orrs	r3, r1
08027c82  2c4c      ldr	r4, [pc, #176] ; [0x08027d34] = 0x40000400 / f32_bits_interpretation=2.000244141
08027c84  2c48      ldr	r0, [pc, #176] ; [0x08027d38] = 0x40000800 / f32_bits_interpretation=2.000488281
08027c86  b2f1804f  cmp.w	r2, #1073741824
08027c8a  08bf      it	eq
08027c8c  43f00103  orreq	r3, r3, #1
08027c90  2a49      ldr	r1, [pc, #168] ; [0x08027d3c] = 0x40000c00 / f32_bits_interpretation=2.000732422
08027c92  a242      cmp	r2, r4
08027c94  08bf      it	eq
08027c96  43f00103  orreq	r3, r3, #1
08027c9a  8242      cmp	r2, r0
08027c9c  08bf      it	eq
08027c9e  43f00103  orreq	r3, r3, #1
08027ca2  8a42      cmp	r2, r1
08027ca4  08bf      it	eq
08027ca6  43f00103  orreq	r3, r3, #1
08027caa  dbb2      uxtb	r3, r3
08027cac  13b9      cbnz	r3, #4 ; -> 0x08027cb4 ; branch_target=0x08027cb4
08027cae  244b      ldr	r3, [pc, #144] ; [0x08027d40] = 0x40001800 / f32_bits_interpretation=2.001464844
08027cb0  9a42      cmp	r2, r3
08027cb2  07d1      bne	#14 ; -> 0x08027cc4 ; branch_target=0x08027cc4
08027cb4  9168      ldr	r1, [r2, #8]
08027cb6  234b      ldr	r3, [pc, #140] ; [0x08027d44] = 0x00010007
08027cb8  0b40      ands	r3, r1
08027cba  062b      cmp	r3, #6
08027cbc  06d0      beq	#12 ; -> 0x08027ccc ; branch_target=0x08027ccc
08027cbe  b3f5803f  cmp.w	r3, #65536
08027cc2  03d0      beq	#6 ; -> 0x08027ccc ; branch_target=0x08027ccc
08027cc4  1368      ldr	r3, [r2]
08027cc6  43f00103  orr	r3, r3, #1
08027cca  1360      str	r3, [r2]
08027ccc  0020      movs	r0, #0
08027cce  70bc      pop	{r4, r5, r6}
08027cd0  7047      bx	lr
08027cd2  90f83f30  ldrb.w	r3, [r0, #63]
08027cd6  012b      cmp	r3, #1
08027cd8  1fd1      bne	#62 ; -> 0x08027d1a ; branch_target=0x08027d1a
08027cda  0223      movs	r3, #2
08027cdc  80f83f30  strb.w	r3, [r0, #63]
08027ce0  99e7      b	#-206 ; -> 0x08027c16 ; branch_target=0x08027c16
08027ce2  90f84030  ldrb.w	r3, [r0, #64]
08027ce6  012b      cmp	r3, #1
08027ce8  17d1      bne	#46 ; -> 0x08027d1a ; branch_target=0x08027d1a
08027cea  0223      movs	r3, #2
08027cec  80f84030  strb.w	r3, [r0, #64]
08027cf0  91e7      b	#-222 ; -> 0x08027c16 ; branch_target=0x08027c16
08027cf2  90f84130  ldrb.w	r3, [r0, #65]
08027cf6  012b      cmp	r3, #1
08027cf8  0fd1      bne	#30 ; -> 0x08027d1a ; branch_target=0x08027d1a
08027cfa  0223      movs	r3, #2
08027cfc  80f84130  strb.w	r3, [r0, #65]
08027d00  89e7      b	#-238 ; -> 0x08027c16 ; branch_target=0x08027c16
08027d02  90f84230  ldrb.w	r3, [r0, #66]
08027d06  012b      cmp	r3, #1
08027d08  07d1      bne	#14 ; -> 0x08027d1a ; branch_target=0x08027d1a
08027d0a  0223      movs	r3, #2
08027d0c  80f84230  strb.w	r3, [r0, #66]
08027d10  81e7      b	#-254 ; -> 0x08027c16 ; branch_target=0x08027c16
08027d12  0223      movs	r3, #2
08027d14  80f84330  strb.w	r3, [r0, #67]
08027d18  7de7      b	#-262 ; -> 0x08027c16 ; branch_target=0x08027c16
08027d1a  0120      movs	r0, #1
08027d1c  7047      bx	lr
08027d1e  0b46      mov	r3, r1
08027d20  aee7      b	#-164 ; -> 0x08027c80 ; branch_target=0x08027c80
