; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate
08021cb0  0028      cmp	r0, #0
08021cb2  00f00982  beq.w	#1042 ; -> 0x080220c8 ; branch_target=0x080220c8
08021cb6  f0b4      push	{r4, r5, r6, r7}
08021cb8  90f83440  ldrb.w	r4, [r0, #52]
08021cbc  012c      cmp	r4, #1
08021cbe  00f00082  beq.w	#1024 ; -> 0x080220c2 ; branch_target=0x080220c2
08021cc2  0124      movs	r4, #1
08021cc4  80f83440  strb.w	r4, [r0, #52]
08021cc8  90f83540  ldrb.w	r4, [r0, #53]
08021ccc  012c      cmp	r4, #1
08021cce  08d0      beq	#16 ; -> 0x08021ce2 ; branch_target=0x08021ce2
08021cd0  0022      movs	r2, #0
08021cd2  4ff40063  mov.w	r3, #2048
08021cd6  80f83420  strb.w	r2, [r0, #52]
08021cda  4365      str	r3, [r0, #84]
08021cdc  0120      movs	r0, #1
08021cde  f0bc      pop	{r4, r5, r6, r7}
08021ce0  7047      bx	lr
08021ce2  0224      movs	r4, #2
08021ce4  0568      ldr	r5, [r0]
08021ce6  9c4f      ldr	r7, [pc, #624] ; [0x08021f58] = 0x40020028 / f32_bits_interpretation=2.031259537
08021ce8  80f83540  strb.w	r4, [r0, #53]
08021cec  0024      movs	r4, #0
08021cee  4465      str	r4, [r0, #84]
08021cf0  2c68      ldr	r4, [r5]
08021cf2  24f00104  bic	r4, r4, #1
08021cf6  2c60      str	r4, [r5]
08021cf8  984d      ldr	r5, [pc, #608] ; [0x08021f5c] = 0x40020010 / f32_bits_interpretation=2.031253815
08021cfa  0468      ldr	r4, [r0]
08021cfc  866d      ldr	r6, [r0, #88]
08021cfe  bc42      cmp	r4, r7
08021d00  18bf      it	ne
08021d02  ac42      cmpne	r4, r5
08021d04  07f11807  add.w	r7, r7, #24
08021d08  0cbf      ite	eq
08021d0a  0125      moveq	r5, #1
08021d0c  0025      movne	r5, #0
08021d0e  bc42      cmp	r4, r7
08021d10  08bf      it	eq
08021d12  45f00105  orreq	r5, r5, #1
08021d16  1837      adds	r7, #24
08021d18  bc42      cmp	r4, r7
08021d1a  08bf      it	eq
08021d1c  45f00105  orreq	r5, r5, #1
08021d20  1837      adds	r7, #24
08021d22  bc42      cmp	r4, r7
08021d24  08bf      it	eq
08021d26  45f00105  orreq	r5, r5, #1
08021d2a  1837      adds	r7, #24
08021d2c  bc42      cmp	r4, r7
08021d2e  08bf      it	eq
08021d30  45f00105  orreq	r5, r5, #1
08021d34  1837      adds	r7, #24
08021d36  bc42      cmp	r4, r7
08021d38  08bf      it	eq
08021d3a  45f00105  orreq	r5, r5, #1
08021d3e  1837      adds	r7, #24
08021d40  bc42      cmp	r4, r7
08021d42  08bf      it	eq
08021d44  45f00105  orreq	r5, r5, #1
08021d48  07f55677  add.w	r7, r7, #856
08021d4c  bc42      cmp	r4, r7
08021d4e  08bf      it	eq
08021d50  45f00105  orreq	r5, r5, #1
08021d54  1837      adds	r7, #24
08021d56  bc42      cmp	r4, r7
08021d58  08bf      it	eq
08021d5a  45f00105  orreq	r5, r5, #1
08021d5e  1837      adds	r7, #24
08021d60  bc42      cmp	r4, r7
08021d62  08bf      it	eq
08021d64  45f00105  orreq	r5, r5, #1
08021d68  1837      adds	r7, #24
08021d6a  bc42      cmp	r4, r7
08021d6c  08bf      it	eq
08021d6e  45f00105  orreq	r5, r5, #1
08021d72  1837      adds	r7, #24
08021d74  bc42      cmp	r4, r7
08021d76  08bf      it	eq
08021d78  45f00105  orreq	r5, r5, #1
08021d7c  1837      adds	r7, #24
08021d7e  bc42      cmp	r4, r7
08021d80  08bf      it	eq
08021d82  45f00105  orreq	r5, r5, #1
08021d86  1837      adds	r7, #24
08021d88  bc42      cmp	r4, r7
08021d8a  08bf      it	eq
08021d8c  45f00105  orreq	r5, r5, #1
08021d90  1837      adds	r7, #24
08021d92  bc42      cmp	r4, r7
08021d94  08bf      it	eq
08021d96  45f00105  orreq	r5, r5, #1
08021d9a  714f      ldr	r7, [pc, #452] ; [0x08021f60] = 0x58025408
08021d9c  bc42      cmp	r4, r7
08021d9e  08bf      it	eq
08021da0  45f00105  orreq	r5, r5, #1
08021da4  1437      adds	r7, #20
08021da6  bc42      cmp	r4, r7
08021da8  08bf      it	eq
08021daa  45f00105  orreq	r5, r5, #1
08021dae  1437      adds	r7, #20
08021db0  bc42      cmp	r4, r7
08021db2  08bf      it	eq
08021db4  45f00105  orreq	r5, r5, #1
08021db8  1437      adds	r7, #20
08021dba  bc42      cmp	r4, r7
08021dbc  08bf      it	eq
08021dbe  45f00105  orreq	r5, r5, #1
08021dc2  1437      adds	r7, #20
08021dc4  bc42      cmp	r4, r7
08021dc6  08bf      it	eq
08021dc8  45f00105  orreq	r5, r5, #1
08021dcc  1437      adds	r7, #20
08021dce  bc42      cmp	r4, r7
08021dd0  08bf      it	eq
08021dd2  45f00105  orreq	r5, r5, #1
08021dd6  1437      adds	r7, #20
08021dd8  bc42      cmp	r4, r7
08021dda  08bf      it	eq
08021ddc  45f00105  orreq	r5, r5, #1
08021de0  15b9      cbnz	r5, #4 ; -> 0x08021de8 ; branch_target=0x08021de8
08021de2  604d      ldr	r5, [pc, #384] ; [0x08021f64] = 0x58025494
08021de4  ac42      cmp	r4, r5
08021de6  08d1      bne	#16 ; -> 0x08021dfa ; branch_target=0x08021dfa
08021de8  d0e91945  ldrd	r4, r5, [r0, #100]
08021dec  6560      str	r5, [r4, #4]
08021dee  c46e      ldr	r4, [r0, #108]
08021df0  14b1      cbz	r4, #4 ; -> 0x08021df8 ; branch_target=0x08021df8
08021df2  d0e91c45  ldrd	r4, r5, [r0, #112]
08021df6  6560      str	r5, [r4, #4]
08021df8  0468      ldr	r4, [r0]
08021dfa  584d      ldr	r5, [pc, #352] ; [0x08021f5c] = 0x40020010 / f32_bits_interpretation=2.031253815
08021dfc  564f      ldr	r7, [pc, #344] ; [0x08021f58] = 0x40020028 / f32_bits_interpretation=2.031259537
08021dfe  bc42      cmp	r4, r7
08021e00  18bf      it	ne
08021e02  ac42      cmpne	r4, r5
08021e04  07f11807  add.w	r7, r7, #24
08021e08  0cbf      ite	eq
08021e0a  0125      moveq	r5, #1
08021e0c  0025      movne	r5, #0
08021e0e  bc42      cmp	r4, r7
08021e10  08bf      it	eq
08021e12  45f00105  orreq	r5, r5, #1
08021e16  1837      adds	r7, #24
08021e18  bc42      cmp	r4, r7
08021e1a  08bf      it	eq
08021e1c  45f00105  orreq	r5, r5, #1
08021e20  1837      adds	r7, #24
08021e22  bc42      cmp	r4, r7
08021e24  08bf      it	eq
08021e26  45f00105  orreq	r5, r5, #1
08021e2a  1837      adds	r7, #24
08021e2c  bc42      cmp	r4, r7
08021e2e  08bf      it	eq
08021e30  45f00105  orreq	r5, r5, #1
08021e34  1837      adds	r7, #24
08021e36  bc42      cmp	r4, r7
08021e38  08bf      it	eq
08021e3a  45f00105  orreq	r5, r5, #1
08021e3e  1837      adds	r7, #24
08021e40  bc42      cmp	r4, r7
08021e42  08bf      it	eq
08021e44  45f00105  orreq	r5, r5, #1
08021e48  07f55677  add.w	r7, r7, #856
08021e4c  bc42      cmp	r4, r7
08021e4e  08bf      it	eq
08021e50  45f00105  orreq	r5, r5, #1
08021e54  1837      adds	r7, #24
08021e56  bc42      cmp	r4, r7
08021e58  08bf      it	eq
08021e5a  45f00105  orreq	r5, r5, #1
08021e5e  1837      adds	r7, #24
08021e60  bc42      cmp	r4, r7
08021e62  08bf      it	eq
08021e64  45f00105  orreq	r5, r5, #1
08021e68  1837      adds	r7, #24
08021e6a  bc42      cmp	r4, r7
08021e6c  08bf      it	eq
08021e6e  45f00105  orreq	r5, r5, #1
08021e72  1837      adds	r7, #24
08021e74  bc42      cmp	r4, r7
08021e76  08bf      it	eq
08021e78  45f00105  orreq	r5, r5, #1
08021e7c  1837      adds	r7, #24
08021e7e  bc42      cmp	r4, r7
08021e80  08bf      it	eq
08021e82  45f00105  orreq	r5, r5, #1
08021e86  1837      adds	r7, #24
08021e88  bc42      cmp	r4, r7
08021e8a  08bf      it	eq
08021e8c  45f00105  orreq	r5, r5, #1
08021e90  1db9      cbnz	r5, #6 ; -> 0x08021e9a ; branch_target=0x08021e9a
08021e92  354d      ldr	r5, [pc, #212] ; [0x08021f68] = 0x400204b8 / f32_bits_interpretation=2.03153801
08021e94  ac42      cmp	r4, r5
08021e96  40f01981  bne.w	#562 ; -> 0x080220cc ; branch_target=0x080220cc
08021e9a  c46d      ldr	r4, [r0, #92]
08021e9c  04f01f05  and	r5, r4, #31
08021ea0  3f24      movs	r4, #63
08021ea2  ac40      lsls	r4, r5
08021ea4  b460      str	r4, [r6, #8]
08021ea6  0568      ldr	r5, [r0]
08021ea8  2c68      ldr	r4, [r5]
08021eaa  24f48024  bic	r4, r4, #262144
08021eae  2c60      str	r4, [r5]
08021eb0  0468      ldr	r4, [r0]
08021eb2  6360      str	r3, [r4, #4]
08021eb4  8368      ldr	r3, [r0, #8]
08021eb6  402b      cmp	r3, #64
08021eb8  0368      ldr	r3, [r0]
08021eba  00f0fe80  beq.w	#508 ; -> 0x080220ba ; branch_target=0x080220ba
08021ebe  9960      str	r1, [r3, #8]
08021ec0  0368      ldr	r3, [r0]
08021ec2  da60      str	r2, [r3, #12]
08021ec4  0468      ldr	r4, [r0]
08021ec6  254b      ldr	r3, [pc, #148] ; [0x08021f5c] = 0x40020010 / f32_bits_interpretation=2.031253815
08021ec8  2349      ldr	r1, [pc, #140] ; [0x08021f58] = 0x40020028 / f32_bits_interpretation=2.031259537
08021eca  284a      ldr	r2, [pc, #160] ; [0x08021f6c] = 0x40020040 / f32_bits_interpretation=2.031265259
08021ecc  8c42      cmp	r4, r1
08021ece  18bf      it	ne
08021ed0  9c42      cmpne	r4, r3
08021ed2  01f13001  add.w	r1, r1, #48
08021ed6  0cbf      ite	eq
08021ed8  0123      moveq	r3, #1
08021eda  0023      movne	r3, #0
08021edc  9442      cmp	r4, r2
08021ede  08bf      it	eq
08021ee0  43f00103  orreq	r3, r3, #1
08021ee4  3032      adds	r2, #48
08021ee6  8c42      cmp	r4, r1
08021ee8  08bf      it	eq
08021eea  43f00103  orreq	r3, r3, #1
08021eee  3031      adds	r1, #48
08021ef0  9442      cmp	r4, r2
08021ef2  08bf      it	eq
08021ef4  43f00103  orreq	r3, r3, #1
08021ef8  3032      adds	r2, #48
08021efa  8c42      cmp	r4, r1
08021efc  08bf      it	eq
08021efe  43f00103  orreq	r3, r3, #1
08021f02  3031      adds	r1, #48
08021f04  9442      cmp	r4, r2
08021f06  08bf      it	eq
08021f08  43f00103  orreq	r3, r3, #1
08021f0c  02f55c72  add.w	r2, r2, #880
08021f10  8c42      cmp	r4, r1
08021f12  08bf      it	eq
08021f14  43f00103  orreq	r3, r3, #1
08021f18  01f55c71  add.w	r1, r1, #880
08021f1c  9442      cmp	r4, r2
08021f1e  08bf      it	eq
08021f20  43f00103  orreq	r3, r3, #1
08021f24  3032      adds	r2, #48
08021f26  8c42      cmp	r4, r1
08021f28  08bf      it	eq
08021f2a  43f00103  orreq	r3, r3, #1
08021f2e  3031      adds	r1, #48
08021f30  9442      cmp	r4, r2
08021f32  08bf      it	eq
08021f34  43f00103  orreq	r3, r3, #1
08021f38  3032      adds	r2, #48
08021f3a  8c42      cmp	r4, r1
08021f3c  08bf      it	eq
08021f3e  43f00103  orreq	r3, r3, #1
08021f42  3031      adds	r1, #48
08021f44  9442      cmp	r4, r2
08021f46  08bf      it	eq
08021f48  43f00103  orreq	r3, r3, #1
08021f4c  3032      adds	r2, #48
08021f4e  8c42      cmp	r4, r1
08021f50  08bf      it	eq
08021f52  43f00103  orreq	r3, r3, #1
08021f56  0be0      b	#22 ; -> 0x08021f70 ; branch_target=0x08021f70
08021f70  9442      cmp	r4, r2
08021f72  08bf      it	eq
08021f74  43f00103  orreq	r3, r3, #1
08021f78  1bb9      cbnz	r3, #6 ; -> 0x08021f82 ; branch_target=0x08021f82
08021f7a  734b      ldr	r3, [pc, #460] ; [0x08022148] = 0x400204b8 / f32_bits_interpretation=2.03153801
08021f7c  9c42      cmp	r4, r3
08021f7e  40f0d280  bne.w	#420 ; -> 0x08022126 ; branch_target=0x08022126
08021f82  2368      ldr	r3, [r4]
08021f84  23f01e03  bic	r3, r3, #30
08021f88  43f01603  orr	r3, r3, #22
08021f8c  2360      str	r3, [r4]
08021f8e  036c      ldr	r3, [r0, #64]
08021f90  23b1      cbz	r3, #8 ; -> 0x08021f9c ; branch_target=0x08021f9c
08021f92  0268      ldr	r2, [r0]
08021f94  1368      ldr	r3, [r2]
08021f96  43f00803  orr	r3, r3, #8
08021f9a  1360      str	r3, [r2]
08021f9c  0468      ldr	r4, [r0]
08021f9e  6b4b      ldr	r3, [pc, #428] ; [0x0802214c] = 0x40020010 / f32_bits_interpretation=2.031253815
08021fa0  6b49      ldr	r1, [pc, #428] ; [0x08022150] = 0x40020028 / f32_bits_interpretation=2.031259537
08021fa2  6c4a      ldr	r2, [pc, #432] ; [0x08022154] = 0x40020040 / f32_bits_interpretation=2.031265259
08021fa4  8c42      cmp	r4, r1
08021fa6  18bf      it	ne
08021fa8  9c42      cmpne	r4, r3
08021faa  01f13001  add.w	r1, r1, #48
08021fae  0cbf      ite	eq
08021fb0  0123      moveq	r3, #1
08021fb2  0023      movne	r3, #0
08021fb4  9442      cmp	r4, r2
08021fb6  08bf      it	eq
08021fb8  43f00103  orreq	r3, r3, #1
08021fbc  3032      adds	r2, #48
08021fbe  8c42      cmp	r4, r1
08021fc0  08bf      it	eq
08021fc2  43f00103  orreq	r3, r3, #1
08021fc6  3031      adds	r1, #48
08021fc8  9442      cmp	r4, r2
08021fca  08bf      it	eq
08021fcc  43f00103  orreq	r3, r3, #1
08021fd0  3032      adds	r2, #48
08021fd2  8c42      cmp	r4, r1
08021fd4  08bf      it	eq
08021fd6  43f00103  orreq	r3, r3, #1
08021fda  3031      adds	r1, #48
08021fdc  9442      cmp	r4, r2
08021fde  08bf      it	eq
08021fe0  43f00103  orreq	r3, r3, #1
08021fe4  02f55c72  add.w	r2, r2, #880
08021fe8  8c42      cmp	r4, r1
08021fea  08bf      it	eq
08021fec  43f00103  orreq	r3, r3, #1
08021ff0  01f55c71  add.w	r1, r1, #880
08021ff4  9442      cmp	r4, r2
08021ff6  08bf      it	eq
08021ff8  43f00103  orreq	r3, r3, #1
08021ffc  3032      adds	r2, #48
08021ffe  8c42      cmp	r4, r1
08022000  08bf      it	eq
08022002  43f00103  orreq	r3, r3, #1
08022006  3031      adds	r1, #48
08022008  9442      cmp	r4, r2
0802200a  08bf      it	eq
0802200c  43f00103  orreq	r3, r3, #1
08022010  3032      adds	r2, #48
08022012  8c42      cmp	r4, r1
08022014  08bf      it	eq
08022016  43f00103  orreq	r3, r3, #1
0802201a  3031      adds	r1, #48
0802201c  9442      cmp	r4, r2
0802201e  08bf      it	eq
08022020  43f00103  orreq	r3, r3, #1
08022024  3032      adds	r2, #48
08022026  8c42      cmp	r4, r1
08022028  08bf      it	eq
0802202a  43f00103  orreq	r3, r3, #1
0802202e  3031      adds	r1, #48
08022030  9442      cmp	r4, r2
08022032  08bf      it	eq
08022034  43f00103  orreq	r3, r3, #1
08022038  474a      ldr	r2, [pc, #284] ; [0x08022158] = 0x58025408
0802203a  8c42      cmp	r4, r1
0802203c  08bf      it	eq
0802203e  43f00103  orreq	r3, r3, #1
08022042  4649      ldr	r1, [pc, #280] ; [0x0802215c] = 0x5802541c
08022044  9442      cmp	r4, r2
08022046  08bf      it	eq
08022048  43f00103  orreq	r3, r3, #1
0802204c  2832      adds	r2, #40
0802204e  8c42      cmp	r4, r1
08022050  08bf      it	eq
08022052  43f00103  orreq	r3, r3, #1
08022056  2831      adds	r1, #40
08022058  9442      cmp	r4, r2
0802205a  08bf      it	eq
0802205c  43f00103  orreq	r3, r3, #1
08022060  2832      adds	r2, #40
08022062  8c42      cmp	r4, r1
08022064  08bf      it	eq
08022066  43f00103  orreq	r3, r3, #1
0802206a  2831      adds	r1, #40
0802206c  9442      cmp	r4, r2
0802206e  08bf      it	eq
08022070  43f00103  orreq	r3, r3, #1
08022074  2832      adds	r2, #40
08022076  8c42      cmp	r4, r1
08022078  08bf      it	eq
0802207a  43f00103  orreq	r3, r3, #1
0802207e  9442      cmp	r4, r2
08022080  08bf      it	eq
08022082  43f00103  orreq	r3, r3, #1
08022086  13b9      cbnz	r3, #4 ; -> 0x0802208e ; branch_target=0x0802208e
08022088  354b      ldr	r3, [pc, #212] ; [0x08022160] = 0x58025494
0802208a  9c42      cmp	r4, r3
0802208c  0ed1      bne	#28 ; -> 0x080220ac ; branch_target=0x080220ac
0802208e  036e      ldr	r3, [r0, #96]
08022090  1a68      ldr	r2, [r3]
08022092  d203      lsls	r2, r2, #15
08022094  03d5      bpl	#6 ; -> 0x0802209e ; branch_target=0x0802209e
08022096  1a68      ldr	r2, [r3]
08022098  42f48072  orr	r2, r2, #256
0802209c  1a60      str	r2, [r3]
0802209e  c36e      ldr	r3, [r0, #108]
080220a0  1bb1      cbz	r3, #6 ; -> 0x080220aa ; branch_target=0x080220aa
080220a2  1a68      ldr	r2, [r3]
080220a4  42f48072  orr	r2, r2, #256
080220a8  1a60      str	r2, [r3]
080220aa  0468      ldr	r4, [r0]
080220ac  2368      ldr	r3, [r4]
080220ae  0020      movs	r0, #0
080220b0  43f00103  orr	r3, r3, #1
080220b4  2360      str	r3, [r4]
080220b6  f0bc      pop	{r4, r5, r6, r7}
080220b8  7047      bx	lr
080220ba  9a60      str	r2, [r3, #8]
080220bc  0368      ldr	r3, [r0]
080220be  d960      str	r1, [r3, #12]
080220c0  00e7      b	#-512 ; -> 0x08021ec4 ; branch_target=0x08021ec4
080220c2  0220      movs	r0, #2
080220c4  f0bc      pop	{r4, r5, r6, r7}
080220c6  7047      bx	lr
080220c8  0120      movs	r0, #1
080220ca  7047      bx	lr
080220cc  224d      ldr	r5, [pc, #136] ; [0x08022158] = 0x58025408
080220ce  234f      ldr	r7, [pc, #140] ; [0x0802215c] = 0x5802541c
080220d0  bc42      cmp	r4, r7
080220d2  18bf      it	ne
080220d4  ac42      cmpne	r4, r5
080220d6  07f11407  add.w	r7, r7, #20
080220da  0cbf      ite	eq
080220dc  0125      moveq	r5, #1
080220de  0025      movne	r5, #0
080220e0  bc42      cmp	r4, r7
080220e2  08bf      it	eq
080220e4  45f00105  orreq	r5, r5, #1
080220e8  1437      adds	r7, #20
080220ea  bc42      cmp	r4, r7
080220ec  08bf      it	eq
080220ee  45f00105  orreq	r5, r5, #1
080220f2  1437      adds	r7, #20
080220f4  bc42      cmp	r4, r7
080220f6  08bf      it	eq
080220f8  45f00105  orreq	r5, r5, #1
080220fc  1437      adds	r7, #20
080220fe  bc42      cmp	r4, r7
08022100  08bf      it	eq
08022102  45f00105  orreq	r5, r5, #1
08022106  1437      adds	r7, #20
08022108  bc42      cmp	r4, r7
0802210a  08bf      it	eq
0802210c  45f00105  orreq	r5, r5, #1
08022110  15b9      cbnz	r5, #4 ; -> 0x08022118 ; branch_target=0x08022118
08022112  134d      ldr	r5, [pc, #76] ; [0x08022160] = 0x58025494
08022114  ac42      cmp	r4, r5
08022116  06d1      bne	#12 ; -> 0x08022126 ; branch_target=0x08022126
08022118  c46d      ldr	r4, [r0, #92]
0802211a  04f01f05  and	r5, r4, #31
0802211e  0124      movs	r4, #1
08022120  ac40      lsls	r4, r5
08022122  7460      str	r4, [r6, #4]
08022124  c4e6      b	#-632 ; -> 0x08021eb0 ; branch_target=0x08021eb0
08022126  2368      ldr	r3, [r4]
08022128  23f00e03  bic	r3, r3, #14
0802212c  43f00a03  orr	r3, r3, #10
08022130  2360      str	r3, [r4]
08022132  036c      ldr	r3, [r0, #64]
08022134  002b      cmp	r3, #0
08022136  3ff431af  beq.w	#-414 ; -> 0x08021f9c ; branch_target=0x08021f9c
0802213a  0268      ldr	r2, [r0]
0802213c  1368      ldr	r3, [r2]
0802213e  43f00403  orr	r3, r3, #4
08022142  1360      str	r3, [r2]
08022144  2ae7      b	#-428 ; -> 0x08021f9c ; branch_target=0x08021f9c
