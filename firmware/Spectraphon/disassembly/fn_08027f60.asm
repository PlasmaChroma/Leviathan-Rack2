; CANDIDATE FUNCTION - inferred boundary, not recovered original symbol
; Entry evidence: direct_call_candidate,prologue_heuristic
08027f60  f8b5      push	{r3, r4, r5, r6, r7, lr}
08027f62  90f83c30  ldrb.w	r3, [r0, #60]
08027f66  012b      cmp	r3, #1
08027f68  00f02a81  beq.w	#596 ; -> 0x080281c0 ; branch_target=0x080281c0
08027f6c  0123      movs	r3, #1
08027f6e  0446      mov	r4, r0
08027f70  80f83c30  strb.w	r3, [r0, #60]
08027f74  142a      cmp	r2, #20
08027f76  54d8      bhi	#168 ; -> 0x08028022 ; branch_target=0x08028022
08027f78  dfe812f0  tbh	[pc, r2, lsl #1]
08027fa6  0368      ldr	r3, [r0]
08027fa8  1a6a      ldr	r2, [r3, #32]
08027faa  22f48052  bic	r2, r2, #4096
08027fae  1a62      str	r2, [r3, #32]
08027fb0  1a6a      ldr	r2, [r3, #32]
08027fb2  5d68      ldr	r5, [r3, #4]
08027fb4  d869      ldr	r0, [r3, #28]
08027fb6  22f40052  bic	r2, r2, #8192
08027fba  0e68      ldr	r6, [r1]
08027fbc  20f4e640  bic	r0, r0, #29440
08027fc0  40ea0620  orr.w	r0, r0, r6, lsl #8
08027fc4  8e68      ldr	r6, [r1, #8]
08027fc6  42ea0632  orr.w	r2, r2, r6, lsl #12
08027fca  7e4e      ldr	r6, [pc, #504] ; [0x080281c4] = 0x40010000 / f32_bits_interpretation=2.015625
08027fcc  b342      cmp	r3, r6
08027fce  0ed0      beq	#28 ; -> 0x08027fee ; branch_target=0x08027fee
08027fd0  06f58066  add.w	r6, r6, #1024
08027fd4  b342      cmp	r3, r6
08027fd6  0ad0      beq	#20 ; -> 0x08027fee ; branch_target=0x08027fee
08027fd8  7b4f      ldr	r7, [pc, #492] ; [0x080281c8] = 0x40014000 / f32_bits_interpretation=2.01953125
08027fda  06f58046  add.w	r6, r6, #16384
08027fde  b342      cmp	r3, r6
08027fe0  18bf      it	ne
08027fe2  bb42      cmpne	r3, r7
08027fe4  03d0      beq	#6 ; -> 0x08027fee ; branch_target=0x08027fee
08027fe6  06f58066  add.w	r6, r6, #1024
08027fea  b342      cmp	r3, r6
08027fec  04d1      bne	#8 ; -> 0x08027ff8 ; branch_target=0x08027ff8
08027fee  25f48045  bic	r5, r5, #16384
08027ff2  4e69      ldr	r6, [r1, #20]
08027ff4  45ea8615  orr.w	r5, r5, r6, lsl #6
08027ff8  5d60      str	r5, [r3, #4]
08027ffa  d861      str	r0, [r3, #28]
08027ffc  4868      ldr	r0, [r1, #4]
08027ffe  1864      str	r0, [r3, #64]
08028000  1a62      str	r2, [r3, #32]
08028002  2268      ldr	r2, [r4]
08028004  d369      ldr	r3, [r2, #28]
08028006  43f40063  orr	r3, r3, #2048
0802800a  d361      str	r3, [r2, #28]
0802800c  2268      ldr	r2, [r4]
0802800e  d369      ldr	r3, [r2, #28]
08028010  23f48063  bic	r3, r3, #1024
08028014  d361      str	r3, [r2, #28]
08028016  2268      ldr	r2, [r4]
08028018  0969      ldr	r1, [r1, #16]
0802801a  d369      ldr	r3, [r2, #28]
0802801c  43ea0123  orr.w	r3, r3, r1, lsl #8
08028020  d361      str	r3, [r2, #28]
08028022  0023      movs	r3, #0
08028024  1846      mov	r0, r3
08028026  84f83c30  strb.w	r3, [r4, #60]
0802802a  f8bd      pop	{r3, r4, r5, r6, r7, pc}
0802802c  0368      ldr	r3, [r0]
0802802e  1a6a      ldr	r2, [r3, #32]
08028030  22f48012  bic	r2, r2, #1048576
08028034  1a62      str	r2, [r3, #32]
08028036  1a6a      ldr	r2, [r3, #32]
08028038  5d68      ldr	r5, [r3, #4]
0802803a  586d      ldr	r0, [r3, #84]
0802803c  22f40012  bic	r2, r2, #2097152
08028040  0e68      ldr	r6, [r1]
08028042  20f4e040  bic	r0, r0, #28672
08028046  40ea0620  orr.w	r0, r0, r6, lsl #8
0802804a  8e68      ldr	r6, [r1, #8]
0802804c  42ea0652  orr.w	r2, r2, r6, lsl #20
08028050  5c4e      ldr	r6, [pc, #368] ; [0x080281c4] = 0x40010000 / f32_bits_interpretation=2.015625
08028052  b342      cmp	r3, r6
08028054  0ed0      beq	#28 ; -> 0x08028074 ; branch_target=0x08028074
08028056  06f58066  add.w	r6, r6, #1024
0802805a  b342      cmp	r3, r6
0802805c  0ad0      beq	#20 ; -> 0x08028074 ; branch_target=0x08028074
0802805e  5a4f      ldr	r7, [pc, #360] ; [0x080281c8] = 0x40014000 / f32_bits_interpretation=2.01953125
08028060  06f58046  add.w	r6, r6, #16384
08028064  b342      cmp	r3, r6
08028066  18bf      it	ne
08028068  bb42      cmpne	r3, r7
0802806a  03d0      beq	#6 ; -> 0x08028074 ; branch_target=0x08028074
0802806c  06f58066  add.w	r6, r6, #1024
08028070  b342      cmp	r3, r6
08028072  04d1      bne	#8 ; -> 0x0802807e ; branch_target=0x0802807e
08028074  25f40035  bic	r5, r5, #131072
08028078  4e69      ldr	r6, [r1, #20]
0802807a  45ea8625  orr.w	r5, r5, r6, lsl #10
0802807e  5d60      str	r5, [r3, #4]
08028080  5865      str	r0, [r3, #84]
08028082  4868      ldr	r0, [r1, #4]
08028084  d865      str	r0, [r3, #92]
08028086  1a62      str	r2, [r3, #32]
08028088  2268      ldr	r2, [r4]
0802808a  536d      ldr	r3, [r2, #84]
0802808c  43f40063  orr	r3, r3, #2048
08028090  5365      str	r3, [r2, #84]
08028092  2268      ldr	r2, [r4]
08028094  536d      ldr	r3, [r2, #84]
08028096  23f48063  bic	r3, r3, #1024
0802809a  5365      str	r3, [r2, #84]
0802809c  2268      ldr	r2, [r4]
0802809e  0969      ldr	r1, [r1, #16]
080280a0  536d      ldr	r3, [r2, #84]
080280a2  43ea0123  orr.w	r3, r3, r1, lsl #8
080280a6  5365      str	r3, [r2, #84]
080280a8  0023      movs	r3, #0
080280aa  1846      mov	r0, r3
080280ac  84f83c30  strb.w	r3, [r4, #60]
080280b0  bbe7      b	#-138 ; -> 0x0802802a ; branch_target=0x0802802a
080280b2  0068      ldr	r0, [r0]
080280b4  fff78cfb  bl	#-2280 ; -> 0x080277d0 ; branch_target=0x080277d0
080280b8  2268      ldr	r2, [r4]
080280ba  9369      ldr	r3, [r2, #24]
080280bc  43f00803  orr	r3, r3, #8
080280c0  9361      str	r3, [r2, #24]
080280c2  2268      ldr	r2, [r4]
080280c4  9369      ldr	r3, [r2, #24]
080280c6  23f00403  bic	r3, r3, #4
080280ca  9361      str	r3, [r2, #24]
080280cc  2268      ldr	r2, [r4]
080280ce  0969      ldr	r1, [r1, #16]
080280d0  9369      ldr	r3, [r2, #24]
080280d2  0b43      orrs	r3, r1
080280d4  9361      str	r3, [r2, #24]
080280d6  0023      movs	r3, #0
080280d8  1846      mov	r0, r3
080280da  84f83c30  strb.w	r3, [r4, #60]
080280de  a4e7      b	#-184 ; -> 0x0802802a ; branch_target=0x0802802a
080280e0  0068      ldr	r0, [r0]
080280e2  fff7fbfe  bl	#-522 ; -> 0x08027edc ; branch_target=0x08027edc
080280e6  2268      ldr	r2, [r4]
080280e8  9369      ldr	r3, [r2, #24]
080280ea  43f40063  orr	r3, r3, #2048
080280ee  9361      str	r3, [r2, #24]
080280f0  2268      ldr	r2, [r4]
080280f2  9369      ldr	r3, [r2, #24]
080280f4  23f48063  bic	r3, r3, #1024
080280f8  9361      str	r3, [r2, #24]
080280fa  2268      ldr	r2, [r4]
080280fc  0969      ldr	r1, [r1, #16]
080280fe  9369      ldr	r3, [r2, #24]
08028100  43ea0123  orr.w	r3, r3, r1, lsl #8
08028104  9361      str	r3, [r2, #24]
08028106  0023      movs	r3, #0
08028108  1846      mov	r0, r3
0802810a  84f83c30  strb.w	r3, [r4, #60]
0802810e  8ce7      b	#-232 ; -> 0x0802802a ; branch_target=0x0802802a
08028110  0068      ldr	r0, [r0]
08028112  fff799fb  bl	#-2254 ; -> 0x08027848 ; branch_target=0x08027848
08028116  2268      ldr	r2, [r4]
08028118  d369      ldr	r3, [r2, #28]
0802811a  43f00803  orr	r3, r3, #8
0802811e  d361      str	r3, [r2, #28]
08028120  2268      ldr	r2, [r4]
08028122  d369      ldr	r3, [r2, #28]
08028124  23f00403  bic	r3, r3, #4
08028128  d361      str	r3, [r2, #28]
0802812a  2268      ldr	r2, [r4]
0802812c  0969      ldr	r1, [r1, #16]
0802812e  d369      ldr	r3, [r2, #28]
08028130  0b43      orrs	r3, r1
08028132  d361      str	r3, [r2, #28]
08028134  0023      movs	r3, #0
08028136  1846      mov	r0, r3
08028138  84f83c30  strb.w	r3, [r4, #60]
0802813c  75e7      b	#-278 ; -> 0x0802802a ; branch_target=0x0802802a
0802813e  0368      ldr	r3, [r0]
08028140  1a6a      ldr	r2, [r3, #32]
08028142  22f48032  bic	r2, r2, #65536
08028146  1a62      str	r2, [r3, #32]
08028148  1a6a      ldr	r2, [r3, #32]
0802814a  5d68      ldr	r5, [r3, #4]
0802814c  586d      ldr	r0, [r3, #84]
0802814e  22f40032  bic	r2, r2, #131072
08028152  0e68      ldr	r6, [r1]
08028154  20f07000  bic	r0, r0, #112
08028158  3043      orrs	r0, r6
0802815a  8e68      ldr	r6, [r1, #8]
0802815c  42ea0642  orr.w	r2, r2, r6, lsl #16
08028160  184e      ldr	r6, [pc, #96] ; [0x080281c4] = 0x40010000 / f32_bits_interpretation=2.015625
08028162  b342      cmp	r3, r6
08028164  0ed0      beq	#28 ; -> 0x08028184 ; branch_target=0x08028184
08028166  06f58066  add.w	r6, r6, #1024
0802816a  b342      cmp	r3, r6
0802816c  0ad0      beq	#20 ; -> 0x08028184 ; branch_target=0x08028184
0802816e  164f      ldr	r7, [pc, #88] ; [0x080281c8] = 0x40014000 / f32_bits_interpretation=2.01953125
08028170  06f58046  add.w	r6, r6, #16384
08028174  b342      cmp	r3, r6
08028176  18bf      it	ne
08028178  bb42      cmpne	r3, r7
0802817a  03d0      beq	#6 ; -> 0x08028184 ; branch_target=0x08028184
0802817c  06f58066  add.w	r6, r6, #1024
08028180  b342      cmp	r3, r6
08028182  04d1      bne	#8 ; -> 0x0802818e ; branch_target=0x0802818e
08028184  25f48035  bic	r5, r5, #65536
08028188  4e69      ldr	r6, [r1, #20]
0802818a  45ea0625  orr.w	r5, r5, r6, lsl #8
0802818e  5d60      str	r5, [r3, #4]
08028190  5865      str	r0, [r3, #84]
08028192  4868      ldr	r0, [r1, #4]
08028194  9865      str	r0, [r3, #88]
08028196  1a62      str	r2, [r3, #32]
08028198  2268      ldr	r2, [r4]
0802819a  536d      ldr	r3, [r2, #84]
0802819c  43f00803  orr	r3, r3, #8
080281a0  5365      str	r3, [r2, #84]
080281a2  2268      ldr	r2, [r4]
080281a4  536d      ldr	r3, [r2, #84]
080281a6  23f00403  bic	r3, r3, #4
080281aa  5365      str	r3, [r2, #84]
080281ac  2268      ldr	r2, [r4]
080281ae  0969      ldr	r1, [r1, #16]
080281b0  536d      ldr	r3, [r2, #84]
080281b2  0b43      orrs	r3, r1
080281b4  5365      str	r3, [r2, #84]
080281b6  0023      movs	r3, #0
080281b8  1846      mov	r0, r3
080281ba  84f83c30  strb.w	r3, [r4, #60]
080281be  34e7      b	#-408 ; -> 0x0802802a ; branch_target=0x0802802a
080281c0  0220      movs	r0, #2
080281c2  f8bd      pop	{r3, r4, r5, r6, r7, pc}
