; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08027d48  90f83c30  ldrb.w	r3, [r0, #60]
08027d4c  012b      cmp	r3, #1
08027d4e  00f08580  beq.w	#266 ; -> 0x08027e5c ; branch_target=0x08027e5c
08027d52  0246      mov	r2, r0
08027d54  0223      movs	r3, #2
08027d56  70b4      push	{r4, r5, r6}
08027d58  0124      movs	r4, #1
08027d5a  0068      ldr	r0, [r0]
08027d5c  82f83d30  strb.w	r3, [r2, #61]
08027d60  82f83c40  strb.w	r4, [r2, #60]
08027d64  594b      ldr	r3, [pc, #356] ; [0x08027ecc] = 0xffce0088
08027d66  8468      ldr	r4, [r0, #8]
08027d68  2340      ands	r3, r4
08027d6a  8360      str	r3, [r0, #8]
08027d6c  0b68      ldr	r3, [r1]
08027d6e  602b      cmp	r3, #96
08027d70  76d0      beq	#236 ; -> 0x08027e60 ; branch_target=0x08027e60
08027d72  1ed9      bls	#60 ; -> 0x08027db2 ; branch_target=0x08027db2
08027d74  b3f5005f  cmp.w	r3, #8192
08027d78  5ed0      beq	#188 ; -> 0x08027e38 ; branch_target=0x08027e38
08027d7a  45d9      bls	#138 ; -> 0x08027e08 ; branch_target=0x08027e08
08027d7c  5449      ldr	r1, [pc, #336] ; [0x08027ed0] = 0x00100020
08027d7e  8b42      cmp	r3, r1
08027d80  06d0      beq	#12 ; -> 0x08027d90 ; branch_target=0x08027d90
08027d82  3bd9      bls	#118 ; -> 0x08027dfc ; branch_target=0x08027dfc
08027d84  5349      ldr	r1, [pc, #332] ; [0x08027ed4] = 0x00100030
08027d86  8b42      cmp	r3, r1
08027d88  02d0      beq	#4 ; -> 0x08027d90 ; branch_target=0x08027d90
08027d8a  1031      adds	r1, #16
08027d8c  8b42      cmp	r3, r1
08027d8e  07d1      bne	#14 ; -> 0x08027da0 ; branch_target=0x08027da0
08027d90  1068      ldr	r0, [r2]
08027d92  5149      ldr	r1, [pc, #324] ; [0x08027ed8] = 0xffcfff8f
08027d94  8468      ldr	r4, [r0, #8]
08027d96  2140      ands	r1, r4
08027d98  1943      orrs	r1, r3
08027d9a  41f00701  orr	r1, r1, #7
08027d9e  8160      str	r1, [r0, #8]
08027da0  0023      movs	r3, #0
08027da2  0121      movs	r1, #1
08027da4  1846      mov	r0, r3
08027da6  82f83d10  strb.w	r1, [r2, #61]
08027daa  82f83c30  strb.w	r3, [r2, #60]
08027dae  70bc      pop	{r4, r5, r6}
08027db0  7047      bx	lr
08027db2  402b      cmp	r3, #64
08027db4  6fd0      beq	#222 ; -> 0x08027e96 ; branch_target=0x08027e96
08027db6  1bd9      bls	#54 ; -> 0x08027df0 ; branch_target=0x08027df0
08027db8  502b      cmp	r3, #80
08027dba  f1d1      bne	#-30 ; -> 0x08027da0 ; branch_target=0x08027da0
08027dbc  1368      ldr	r3, [r2]
08027dbe  4c68      ldr	r4, [r1, #4]
08027dc0  186a      ldr	r0, [r3, #32]
08027dc2  cd68      ldr	r5, [r1, #12]
08027dc4  20f00a00  bic	r0, r0, #10
08027dc8  4349      ldr	r1, [pc, #268] ; [0x08027ed8] = 0xffcfff8f
08027dca  0443      orrs	r4, r0
08027dcc  186a      ldr	r0, [r3, #32]
08027dce  20f00100  bic	r0, r0, #1
08027dd2  1862      str	r0, [r3, #32]
08027dd4  9869      ldr	r0, [r3, #24]
08027dd6  20f0f000  bic	r0, r0, #240
08027dda  40ea0510  orr.w	r0, r0, r5, lsl #4
08027dde  9861      str	r0, [r3, #24]
08027de0  1c62      str	r4, [r3, #32]
08027de2  1368      ldr	r3, [r2]
08027de4  9868      ldr	r0, [r3, #8]
08027de6  0140      ands	r1, r0
08027de8  41f05701  orr	r1, r1, #87
08027dec  9960      str	r1, [r3, #8]
08027dee  d7e7      b	#-82 ; -> 0x08027da0 ; branch_target=0x08027da0
08027df0  202b      cmp	r3, #32
08027df2  cdd0      beq	#-102 ; -> 0x08027d90 ; branch_target=0x08027d90
08027df4  1cd9      bls	#56 ; -> 0x08027e30 ; branch_target=0x08027e30
08027df6  302b      cmp	r3, #48
08027df8  d2d1      bne	#-92 ; -> 0x08027da0 ; branch_target=0x08027da0
08027dfa  c9e7      b	#-110 ; -> 0x08027d90 ; branch_target=0x08027d90
08027dfc  23f01001  bic	r1, r3, #16
08027e00  b1f5801f  cmp.w	r1, #1048576
08027e04  ccd1      bne	#-104 ; -> 0x08027da0 ; branch_target=0x08027da0
08027e06  c3e7      b	#-122 ; -> 0x08027d90 ; branch_target=0x08027d90
08027e08  702b      cmp	r3, #112
08027e0a  c9d1      bne	#-110 ; -> 0x08027da0 ; branch_target=0x08027da0
08027e0c  1068      ldr	r0, [r2]
08027e0e  cc68      ldr	r4, [r1, #12]
08027e10  d1e90153  ldrd	r5, r3, [r1, #4]
08027e14  8168      ldr	r1, [r0, #8]
08027e16  2b43      orrs	r3, r5
08027e18  21f47f41  bic	r1, r1, #65280
08027e1c  43ea0423  orr.w	r3, r3, r4, lsl #8
08027e20  0b43      orrs	r3, r1
08027e22  8360      str	r3, [r0, #8]
08027e24  1168      ldr	r1, [r2]
08027e26  8b68      ldr	r3, [r1, #8]
08027e28  43f07703  orr	r3, r3, #119
08027e2c  8b60      str	r3, [r1, #8]
08027e2e  b7e7      b	#-146 ; -> 0x08027da0 ; branch_target=0x08027da0
08027e30  33f01001  bics	r1, r3, #16
08027e34  b4d1      bne	#-152 ; -> 0x08027da0 ; branch_target=0x08027da0
08027e36  abe7      b	#-170 ; -> 0x08027d90 ; branch_target=0x08027d90
08027e38  d1e90153  ldrd	r5, r3, [r1, #4]
08027e3c  1068      ldr	r0, [r2]
08027e3e  cc68      ldr	r4, [r1, #12]
08027e40  2b43      orrs	r3, r5
08027e42  8168      ldr	r1, [r0, #8]
08027e44  43ea0423  orr.w	r3, r3, r4, lsl #8
08027e48  21f47f41  bic	r1, r1, #65280
08027e4c  0b43      orrs	r3, r1
08027e4e  8360      str	r3, [r0, #8]
08027e50  1168      ldr	r1, [r2]
08027e52  8b68      ldr	r3, [r1, #8]
08027e54  43f48043  orr	r3, r3, #16384
08027e58  8b60      str	r3, [r1, #8]
08027e5a  a1e7      b	#-190 ; -> 0x08027da0 ; branch_target=0x08027da0
08027e5c  0220      movs	r0, #2
08027e5e  7047      bx	lr
08027e60  1368      ldr	r3, [r2]
08027e62  4d68      ldr	r5, [r1, #4]
08027e64  186a      ldr	r0, [r3, #32]
08027e66  ce68      ldr	r6, [r1, #12]
08027e68  20f01000  bic	r0, r0, #16
08027e6c  1a49      ldr	r1, [pc, #104] ; [0x08027ed8] = 0xffcfff8f
08027e6e  1862      str	r0, [r3, #32]
08027e70  9c69      ldr	r4, [r3, #24]
08027e72  186a      ldr	r0, [r3, #32]
08027e74  24f47044  bic	r4, r4, #61440
08027e78  20f0a000  bic	r0, r0, #160
08027e7c  44ea0634  orr.w	r4, r4, r6, lsl #12
08027e80  40ea0510  orr.w	r0, r0, r5, lsl #4
08027e84  9c61      str	r4, [r3, #24]
08027e86  1862      str	r0, [r3, #32]
08027e88  1368      ldr	r3, [r2]
08027e8a  9868      ldr	r0, [r3, #8]
08027e8c  0140      ands	r1, r0
08027e8e  41f06701  orr	r1, r1, #103
08027e92  9960      str	r1, [r3, #8]
08027e94  84e7      b	#-248 ; -> 0x08027da0 ; branch_target=0x08027da0
08027e96  1368      ldr	r3, [r2]
08027e98  4c68      ldr	r4, [r1, #4]
08027e9a  186a      ldr	r0, [r3, #32]
08027e9c  cd68      ldr	r5, [r1, #12]
08027e9e  20f00a00  bic	r0, r0, #10
08027ea2  0d49      ldr	r1, [pc, #52] ; [0x08027ed8] = 0xffcfff8f
08027ea4  0443      orrs	r4, r0
08027ea6  186a      ldr	r0, [r3, #32]
08027ea8  20f00100  bic	r0, r0, #1
08027eac  1862      str	r0, [r3, #32]
08027eae  9869      ldr	r0, [r3, #24]
08027eb0  20f0f000  bic	r0, r0, #240
08027eb4  40ea0510  orr.w	r0, r0, r5, lsl #4
08027eb8  9861      str	r0, [r3, #24]
08027eba  1c62      str	r4, [r3, #32]
08027ebc  1368      ldr	r3, [r2]
08027ebe  9868      ldr	r0, [r3, #8]
08027ec0  0140      ands	r1, r0
08027ec2  41f04701  orr	r1, r1, #71
08027ec6  9960      str	r1, [r3, #8]
08027ec8  6ae7      b	#-300 ; -> 0x08027da0 ; branch_target=0x08027da0
