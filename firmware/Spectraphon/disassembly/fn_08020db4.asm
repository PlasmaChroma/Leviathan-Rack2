; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08020db4  38b5      push	{r3, r4, r5, lr}
08020db6  554a      ldr	r2, [pc, #340] ; [0x08020f0c] = 0x40022000 / f32_bits_interpretation=2.033203125
08020db8  0446      mov	r4, r0
08020dba  0368      ldr	r3, [r0]
08020dbc  9342      cmp	r3, r2
08020dbe  25d0      beq	#74 ; -> 0x08020e0c ; branch_target=0x08020e0c
08020dc0  02f58072  add.w	r2, r2, #256
08020dc4  9342      cmp	r3, r2
08020dc6  21d0      beq	#66 ; -> 0x08020e0c ; branch_target=0x08020e0c
08020dc8  514b      ldr	r3, [pc, #324] ; [0x08020f10] = 0x58026300
08020dca  9b68      ldr	r3, [r3, #8]
08020dcc  13f4403f  tst.w	r3, #196608
08020dd0  21d0      beq	#66 ; -> 0x08020e16 ; branch_target=0x08020e16
08020dd2  03f025f9  bl	#12874 ; -> 0x08024020 ; branch_target=0x08024020
08020dd6  6368      ldr	r3, [r4, #4]
08020dd8  0546      mov	r5, r0
08020dda  b3f5003f  cmp.w	r3, #131072
08020dde  00f08580  beq.w	#266 ; -> 0x08020eec ; branch_target=0x08020eec
08020de2  b3f5403f  cmp.w	r3, #196608
08020de6  6ed0      beq	#220 ; -> 0x08020ec6 ; branch_target=0x08020ec6
08020de8  b3f5803f  cmp.w	r3, #65536
08020dec  7ed0      beq	#252 ; -> 0x08020eec ; branch_target=0x08020eec
08020dee  fff7edfa  bl	#-2598 ; -> 0x080203cc ; branch_target=0x080203cc
08020df2  41f20303  movw	r3, #4099
08020df6  9842      cmp	r0, r3
08020df8  4ad8      bhi	#148 ; -> 0x08020e90 ; branch_target=0x08020e90
08020dfa  464a      ldr	r2, [pc, #280] ; [0x08020f14] = 0x01312d00
08020dfc  2368      ldr	r3, [r4]
08020dfe  9542      cmp	r5, r2
08020e00  29d9      bls	#82 ; -> 0x08020e56 ; branch_target=0x08020e56
08020e02  9a68      ldr	r2, [r3, #8]
08020e04  42f48072  orr	r2, r2, #256
08020e08  9a60      str	r2, [r3, #8]
08020e0a  38bd      pop	{r3, r4, r5, pc}
08020e0c  424b      ldr	r3, [pc, #264] ; [0x08020f18] = 0x40022300 / f32_bits_interpretation=2.03338623
08020e0e  9b68      ldr	r3, [r3, #8]
08020e10  13f4403f  tst.w	r3, #196608
08020e14  ddd1      bne	#-70 ; -> 0x08020dd2 ; branch_target=0x08020dd2
08020e16  4ff40020  mov.w	r0, #524288
08020e1a  04f083fa  bl	#17670 ; -> 0x08025324 ; branch_target=0x08025324
08020e1e  6368      ldr	r3, [r4, #4]
08020e20  0546      mov	r5, r0
08020e22  b3f5101f  cmp.w	r3, #2359296
08020e26  6bd0      beq	#214 ; -> 0x08020f00 ; branch_target=0x08020f00
08020e28  08d8      bhi	#16 ; -> 0x08020e3c ; branch_target=0x08020e3c
08020e2a  b3f5e01f  cmp.w	r3, #1835008
08020e2e  54d0      beq	#168 ; -> 0x08020eda ; branch_target=0x08020eda
08020e30  16d9      bls	#44 ; -> 0x08020e60 ; branch_target=0x08020e60
08020e32  b3f5001f  cmp.w	r3, #2097152
08020e36  dad1      bne	#-76 ; -> 0x08020dee ; branch_target=0x08020dee
08020e38  4509      lsrs	r5, r0, #5
08020e3a  d8e7      b	#-80 ; -> 0x08020dee ; branch_target=0x08020dee
08020e3c  b3f5201f  cmp.w	r3, #2621440
08020e40  49d0      beq	#146 ; -> 0x08020ed6 ; branch_target=0x08020ed6
08020e42  b3f5301f  cmp.w	r3, #2883584
08020e46  d2d1      bne	#-92 ; -> 0x08020dee ; branch_target=0x08020dee
08020e48  fff7c0fa  bl	#-2688 ; -> 0x080203cc ; branch_target=0x080203cc
08020e4c  41f20303  movw	r3, #4099
08020e50  9842      cmp	r0, r3
08020e52  2ed8      bhi	#92 ; -> 0x08020eb2 ; branch_target=0x08020eb2
08020e54  2368      ldr	r3, [r4]
08020e56  9a68      ldr	r2, [r3, #8]
08020e58  22f48072  bic	r2, r2, #256
08020e5c  9a60      str	r2, [r3, #8]
08020e5e  38bd      pop	{r3, r4, r5, pc}
08020e60  b3f5801f  cmp.w	r3, #1048576
08020e64  06d0      beq	#12 ; -> 0x08020e74 ; branch_target=0x08020e74
08020e66  0ad9      bls	#20 ; -> 0x08020e7e ; branch_target=0x08020e7e
08020e68  b3f5a01f  cmp.w	r3, #1310720
08020e6c  02d0      beq	#4 ; -> 0x08020e74 ; branch_target=0x08020e74
08020e6e  b3f5c01f  cmp.w	r3, #1572864
08020e72  bcd1      bne	#-136 ; -> 0x08020dee ; branch_target=0x08020dee
08020e74  9b0c      lsrs	r3, r3, #18
08020e76  5b00      lsls	r3, r3, #1
08020e78  b5fbf3f5  udiv	r5, r5, r3
08020e7c  b7e7      b	#-146 ; -> 0x08020dee ; branch_target=0x08020dee
08020e7e  b3f5002f  cmp.w	r3, #524288
08020e82  f7d0      beq	#-18 ; -> 0x08020e74 ; branch_target=0x08020e74
08020e84  23f40022  bic	r2, r3, #524288
08020e88  b2f5802f  cmp.w	r2, #262144
08020e8c  f2d0      beq	#-28 ; -> 0x08020e74 ; branch_target=0x08020e74
08020e8e  aee7      b	#-164 ; -> 0x08020dee ; branch_target=0x08020dee
08020e90  224a      ldr	r2, [pc, #136] ; [0x08020f1c] = 0x00bebc21
08020e92  2368      ldr	r3, [r4]
08020e94  9542      cmp	r5, r2
08020e96  11d9      bls	#34 ; -> 0x08020ebc ; branch_target=0x08020ebc
08020e98  214a      ldr	r2, [pc, #132] ; [0x08020f20] = 0x017d7841
08020e9a  9542      cmp	r5, r2
08020e9c  1fd9      bls	#62 ; -> 0x08020ede ; branch_target=0x08020ede
08020e9e  214a      ldr	r2, [pc, #132] ; [0x08020f24] = 0x02faf081
08020ea0  9542      cmp	r5, r2
08020ea2  9a68      ldr	r2, [r3, #8]
08020ea4  2ed8      bhi	#92 ; -> 0x08020f04 ; branch_target=0x08020f04
08020ea6  22f44072  bic	r2, r2, #768
08020eaa  42f40072  orr	r2, r2, #512
08020eae  9a60      str	r2, [r3, #8]
08020eb0  38bd      pop	{r3, r4, r5, pc}
08020eb2  1a4b      ldr	r3, [pc, #104] ; [0x08020f1c] = 0x00bebc21
08020eb4  b3eb152f  cmp.w	r3, r5, lsr #8
08020eb8  2368      ldr	r3, [r4]
08020eba  10d3      blo	#32 ; -> 0x08020ede ; branch_target=0x08020ede
08020ebc  9a68      ldr	r2, [r3, #8]
08020ebe  22f44072  bic	r2, r2, #768
08020ec2  9a60      str	r2, [r3, #8]
08020ec4  38bd      pop	{r3, r4, r5, pc}
08020ec6  8508      lsrs	r5, r0, #2
08020ec8  fff780fa  bl	#-2816 ; -> 0x080203cc ; branch_target=0x080203cc
08020ecc  41f20303  movw	r3, #4099
08020ed0  9842      cmp	r0, r3
08020ed2  ddd8      bhi	#-70 ; -> 0x08020e90 ; branch_target=0x08020e90
08020ed4  91e7      b	#-222 ; -> 0x08020dfa ; branch_target=0x08020dfa
08020ed6  c509      lsrs	r5, r0, #7
08020ed8  89e7      b	#-238 ; -> 0x08020dee ; branch_target=0x08020dee
08020eda  0509      lsrs	r5, r0, #4
08020edc  87e7      b	#-242 ; -> 0x08020dee ; branch_target=0x08020dee
08020ede  9a68      ldr	r2, [r3, #8]
08020ee0  22f44072  bic	r2, r2, #768
08020ee4  42f48072  orr	r2, r2, #256
08020ee8  9a60      str	r2, [r3, #8]
08020eea  38bd      pop	{r3, r4, r5, pc}
08020eec  1b0c      lsrs	r3, r3, #16
08020eee  b5fbf3f5  udiv	r5, r5, r3
08020ef2  fff76bfa  bl	#-2858 ; -> 0x080203cc ; branch_target=0x080203cc
08020ef6  41f20303  movw	r3, #4099
08020efa  9842      cmp	r0, r3
08020efc  c8d8      bhi	#-112 ; -> 0x08020e90 ; branch_target=0x08020e90
08020efe  7ce7      b	#-264 ; -> 0x08020dfa ; branch_target=0x08020dfa
08020f00  8509      lsrs	r5, r0, #6
08020f02  74e7      b	#-280 ; -> 0x08020dee ; branch_target=0x08020dee
08020f04  42f44072  orr	r2, r2, #768
08020f08  9a60      str	r2, [r3, #8]
08020f0a  38bd      pop	{r3, r4, r5, pc}
