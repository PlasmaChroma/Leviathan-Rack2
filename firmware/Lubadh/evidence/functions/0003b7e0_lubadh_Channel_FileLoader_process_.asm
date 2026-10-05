; lubadh::Channel::FileLoader::process()
; VA 0x3b7e0 size 972

   3b7e0: e5903008     	ldr	r3, [r0, #0x8]
   3b7e4: e92d40f0     	push	{r4, r5, r6, r7, lr}
   3b7e8: e1a05000     	mov	r5, r0
   3b7ec: e5933000     	ldr	r3, [r3]
   3b7f0: ed2d8b04     	vpush	{d8, d9}
   3b7f4: e24dd01c     	sub	sp, sp, #28
   3b7f8: e2432002     	sub	r2, r3, #2
   3b7fc: e3520004     	cmp	r2, #4
   3b800: 979ff102     	ldrls	pc, [pc, r2, lsl #2]
   3b804: ea00006c     	b	0x3b9bc
   3b808: 38 bb 03 00  	.word	0x0003bb38
   3b80c: 38 ba 03 00  	.word	0x0003ba38
   3b810: bc b9 03 00  	.word	0x0003b9bc
   3b814: cc b9 03 00  	.word	0x0003b9cc
   3b818: 1c b8 03 00  	.word	0x0003b81c
   3b81c: e5901000     	ldr	r1, [r0]
   3b820: e302279c     	movw	r2, #0x279c
   3b824: e3402007     	movt	r2, #0x7
   3b828: e1a0000d     	mov	r0, sp
   3b82c: e2811004     	add	r1, r1, #4
   3b830: ebffcce4     	bl	0x2ebc8
   3b834: e3090fec     	movw	r0, #0x9fec
   3b838: e3400009     	movt	r0, #0x9
   3b83c: e1a0100d     	mov	r1, sp
   3b840: e3a02000     	mov	r2, #0
   3b844: eb00d1b5     	bl	0x6ff20
   3b848: e59d0000     	ldr	r0, [sp]
   3b84c: e28d3008     	add	r3, sp, #8
   3b850: e1500003     	cmp	r0, r3
   3b854: 0a000000     	beq	0x3b85c
   3b858: ebff6978     	bl	0x15e40    @ imm = #-0x25a20 ; _ZdlPv
   3b85c: f2c00010     	vmov.i32	d16, #0x0
   3b860: e3a00004     	mov	r0, #4
   3b864: e3a03000     	mov	r3, #0
   3b868: e5957000     	ldr	r7, [r5]
   3b86c: e58d3008     	str	r3, [sp, #0x8]
   3b870: edcd0b00     	vstr	d16, [sp]
   3b874: ebff6824     	bl	0x1590c     @ imm = #-0x25f70 ; _Znwj
   3b878: e5952000     	ldr	r2, [r5]
   3b87c: e2806004     	add	r6, r0, #4
   3b880: e5807000     	str	r7, [r0]
   3b884: e1a04000     	mov	r4, r0
   3b888: e58d0000     	str	r0, [sp]
   3b88c: e5923020     	ldr	r3, [r2, #0x20]
   3b890: e58d6008     	str	r6, [sp, #0x8]
   3b894: e2833915     	add	r3, r3, #344064
   3b898: e58d6004     	str	r6, [sp, #0x4]
   3b89c: e5d33abc     	ldrb	r3, [r3, #0xabc]
   3b8a0: e3530000     	cmp	r3, #0
   3b8a4: 0a000006     	beq	0x3b8c4
   3b8a8: e282201c     	add	r2, r2, #28
   3b8ac: e1a01006     	mov	r1, r6
   3b8b0: e1a0000d     	mov	r0, sp
   3b8b4: eb0012b4     	bl	0x4038c
   3b8b8: e89d0050     	ldm	sp, {r4, r6}
   3b8bc: e1560004     	cmp	r6, r4
   3b8c0: 0a000035     	beq	0x3b99c
   3b8c4: f2809010     	vmov.i32	d9, #0x0
   3b8c8: ed9f8bb2     	vldr	d8, [pc, #712]          @ 0x3bb98 ; float 5.26354424712e-315
   3b8cc: e3037004     	movw	r7, #0x3004
   3b8d0: e59530dc     	ldr	r3, [r5, #0xdc]
   3b8d4: e3a01003     	mov	r1, #3
   3b8d8: e4940004     	ldr	r0, [r4], #4
   3b8dc: e5933000     	ldr	r3, [r3]
   3b8e0: e590e058     	ldr	lr, [r0, #0x58]
   3b8e4: e2432a03     	sub	r2, r3, #12288
   3b8e8: e243cd99     	sub	r12, r3, #9792
   3b8ec: e24cc02a     	sub	r12, r12, #42
   3b8f0: e58e3000     	str	r3, [lr]
   3b8f4: e2423005     	sub	r3, r2, #5
   3b8f8: e1530007     	cmp	r3, r7
   3b8fc: e2422004     	sub	r2, r2, #4
   3b900: a1a03007     	movge	r3, r7
   3b904: e580c050     	str	r12, [r0, #0x50]
   3b908: e580204c     	str	r2, [r0, #0x4c]
   3b90c: e5803084     	str	r3, [r0, #0x84]
   3b910: ebfff9c0     	bl	0x3a018
   3b914: e5140004     	ldr	r0, [r4, #-0x4]
   3b918: e3a02001     	mov	r2, #1
   3b91c: e3a01000     	mov	r1, #0
   3b920: ebffef61     	bl	0x376ac
   3b924: e5143004     	ldr	r3, [r4, #-0x4]
   3b928: e3a0c001     	mov	r12, #1
   3b92c: e3a015fe     	mov	r1, #1065353216
   3b930: e3a00000     	mov	r0, #0
   3b934: e283efb6     	add	lr, r3, #728
   3b938: e59320e8     	ldr	r2, [r3, #0xe8]
   3b93c: e5c3c287     	strb	r12, [r3, #0x287]
   3b940: e583102c     	str	r1, [r3, #0x2c]
   3b944: e592c0b0     	ldr	r12, [r2, #0xb0]
   3b948: f40e878f     	vst1.32	{d8}, [lr]
   3b94c: e592208c     	ldr	r2, [r2, #0x8c]
   3b950: e5831024     	str	r1, [r3, #0x24]
   3b954: e59c1040     	ldr	r1, [r12, #0x40]
   3b958: e58302e0     	str	r0, [r3, #0x2e0]
   3b95c: e3510003     	cmp	r1, #3
   3b960: 058300ec     	streq	r0, [r3, #0xec]
   3b964: e3520001     	cmp	r2, #1
   3b968: 0a000003     	beq	0x3b97c
   3b96c: ed839b8e     	vstr	d9, [r3, #568]
   3b970: e5140004     	ldr	r0, [r4, #-0x4]
   3b974: ebfff1c9     	bl	0x380a0
   3b978: e5143004     	ldr	r3, [r4, #-0x4]
   3b97c: e2831a29     	add	r1, r3, #167936
   3b980: e5932058     	ldr	r2, [r3, #0x58]
   3b984: e1560004     	cmp	r6, r4
   3b988: e5913f5c     	ldr	r3, [r1, #0xf5c]
   3b98c: e5922000     	ldr	r2, [r2]
   3b990: e5832004     	str	r2, [r3, #0x4]
   3b994: 1affffcd     	bne	0x3b8d0
   3b998: e59d4000     	ldr	r4, [sp]
   3b99c: e3540000     	cmp	r4, #0
   3b9a0: 0a000001     	beq	0x3b9ac
   3b9a4: e1a00004     	mov	r0, r4
   3b9a8: ebff6924     	bl	0x15e40    @ imm = #-0x25b70 ; _ZdlPv
   3b9ac: e1a00005     	mov	r0, r5
   3b9b0: ebfff6eb     	bl	0x39564
   3b9b4: e5953008     	ldr	r3, [r5, #0x8]
   3b9b8: e5933000     	ldr	r3, [r3]
   3b9bc: e5853028     	str	r3, [r5, #0x28]
   3b9c0: e28dd01c     	add	sp, sp, #28
   3b9c4: ecbd8b04     	vpop	{d8, d9}
   3b9c8: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   3b9cc: e5901000     	ldr	r1, [r0]
   3b9d0: e3022780     	movw	r2, #0x2780
   3b9d4: e3402007     	movt	r2, #0x7
   3b9d8: e1a0000d     	mov	r0, sp
   3b9dc: e2811004     	add	r1, r1, #4
   3b9e0: ebffcc78     	bl	0x2ebc8
   3b9e4: e3090fec     	movw	r0, #0x9fec
   3b9e8: e3400009     	movt	r0, #0x9
   3b9ec: e1a0100d     	mov	r1, sp
   3b9f0: e3a02002     	mov	r2, #2
   3b9f4: eb00d149     	bl	0x6ff20
   3b9f8: e59d0000     	ldr	r0, [sp]
   3b9fc: e28d3008     	add	r3, sp, #8
   3ba00: e1500003     	cmp	r0, r3
   3ba04: 0a000000     	beq	0x3ba0c
   3ba08: ebff690c     	bl	0x15e40    @ imm = #-0x25bd0 ; _ZdlPv
   3ba0c: e5953000     	ldr	r3, [r5]
   3ba10: e1a00005     	mov	r0, r5
   3ba14: eddf0b61     	vldr	d16, [pc, #388]         @ 0x3bba0 ; float 6.36598738204e-313
   3ba18: edc30ba4     	vstr	d16, [r3, #656]
   3ba1c: ebfff6d0     	bl	0x39564
   3ba20: e5953008     	ldr	r3, [r5, #0x8]
   3ba24: e5933000     	ldr	r3, [r3]
   3ba28: e5853028     	str	r3, [r5, #0x28]
   3ba2c: e28dd01c     	add	sp, sp, #28
   3ba30: ecbd8b04     	vpop	{d8, d9}
   3ba34: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   3ba38: e590104c     	ldr	r1, [r0, #0x4c]
   3ba3c: f2c06050     	vmov.i32	q11, #0x0
   3ba40: e5902028     	ldr	r2, [r0, #0x28]
   3ba44: e281e004     	add	lr, r1, #4
   3ba48: e2810014     	add	r0, r1, #20
   3ba4c: e281c024     	add	r12, r1, #36
   3ba50: e2422001     	sub	r2, r2, #1
   3ba54: f3c78e5f     	vmov.i8	q12, #0xff
   3ba58: e16f2f12     	clz	r2, r2
   3ba5c: f46e0a8f     	vld1.32	{d16, d17}, [lr]
   3ba60: e1a022a2     	lsr	r2, r2, #5
   3ba64: f4604a8f     	vld1.32	{d20, d21}, [r0]
   3ba68: e5c52104     	strb	r2, [r5, #0x104]
   3ba6c: f46c2a8f     	vld1.32	{d18, d19}, [r12]
   3ba70: f2c0a051     	vmov.i32	q13, #0x1
   3ba74: e5952000     	ldr	r2, [r5]
   3ba78: f26003e6     	vcgt.s32	q8, q8, q11
   3ba7c: e5910000     	ldr	r0, [r1]
   3ba80: f26443e6     	vcgt.s32	q10, q10, q11
   3ba84: e282ea2a     	add	lr, r2, #172032
   3ba88: f26223e6     	vcgt.s32	q9, q9, q11
   3ba8c: e592c01c     	ldr	r12, [r2, #0x1c]
   3ba90: eddf6a44     	vldr	s13, [pc, #272]         @ 0x3bba8 ; float 0.000244140625
   3ba94: e3500000     	cmp	r0, #0
   3ba98: f35a01f6     	vbsl	q8, q13, q11
   3ba9c: e28c2ba9     	add	r2, r12, #173056
   3baa0: f35841f6     	vbsl	q10, q12, q11
   3baa4: e28220c8     	add	r2, r2, #200
   3baa8: f35821f6     	vbsl	q9, q12, q11
   3baac: c3a00001     	movgt	r0, #1
   3bab0: edd27a00     	vldr	s15, [r2]
   3bab4: d3a00000     	movle	r0, #0
   3bab8: d3a04001     	movle	r4, #1
   3babc: c3a04000     	movgt	r4, #0
   3bac0: f36008e4     	vsub.i32	q8, q8, q10
   3bac4: eef87ae7     	vcvt.f32.s32	s15, s15
   3bac8: f36008e2     	vsub.i32	q8, q8, q9
   3bacc: ee277aa6     	vmul.f32	s14, s15, s13
   3bad0: f26008a1     	vadd.i32	d16, d16, d17
   3bad4: f2600bb0     	vpadd.i32	d16, d16, d16
   3bad8: ee102b90     	vmov.32	r2, d16[0]
   3badc: e0802002     	add	r2, r0, r2
   3bae0: ee072a90     	vmov	s15, r2
   3bae4: e28e0d13     	add	r0, lr, #1216
   3bae8: eef87ae7     	vcvt.f32.s32	s15, s15
   3baec: e2800004     	add	r0, r0, #4
   3baf0: ee677a27     	vmul.f32	s15, s14, s15
   3baf4: eefd7ae7     	vcvt.s32.f32	s15, s15
   3baf8: ee172a90     	vmov	r2, s15
   3bafc: e0822004     	add	r2, r2, r4
   3bb00: e585206c     	str	r2, [r5, #0x6c]
   3bb04: ed907a00     	vldr	s14, [r0]
   3bb08: e0812102     	add	r2, r1, r2, lsl #2
   3bb0c: eeb87ac7     	vcvt.f32.s32	s14, s14
   3bb10: edd27a00     	vldr	s15, [r2]
   3bb14: e5853028     	str	r3, [r5, #0x28]
   3bb18: eef87ae7     	vcvt.f32.s32	s15, s15
   3bb1c: ee277a26     	vmul.f32	s14, s14, s13
   3bb20: ee677a87     	vmul.f32	s15, s15, s14
   3bb24: eefd7ae7     	vcvt.s32.f32	s15, s15
   3bb28: edc57a1c     	vstr	s15, [r5, #112]
   3bb2c: e28dd01c     	add	sp, sp, #28
   3bb30: ecbd8b04     	vpop	{d8, d9}
   3bb34: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   3bb38: e1a0000d     	mov	r0, sp
   3bb3c: e3021764     	movw	r1, #0x2764
   3bb40: e3401007     	movt	r1, #0x7
   3bb44: ebffecbf     	bl	0x36e48
   3bb48: e3090fec     	movw	r0, #0x9fec
   3bb4c: e3400009     	movt	r0, #0x9
   3bb50: e1a0100d     	mov	r1, sp
   3bb54: e3a02000     	mov	r2, #0
   3bb58: eb00d0f0     	bl	0x6ff20
   3bb5c: eaffffa5     	b	0x3b9f8
   3bb60: e59d0000     	ldr	r0, [sp]
   3bb64: e3500000     	cmp	r0, #0
   3bb68: 0a000000     	beq	0x3bb70
   3bb6c: ebff68b3     	bl	0x15e40    @ imm = #-0x25d34 ; _ZdlPv
   3bb70: ebff68fa     	bl	0x15f60    @ imm = #-0x25c18 ; __cxa_end_cleanup
   3bb74: e59d0000     	ldr	r0, [sp]
   3bb78: e28d3008     	add	r3, sp, #8
   3bb7c: e1500003     	cmp	r0, r3
   3bb80: 1afffff9     	bne	0x3bb6c
   3bb84: eafffff9     	b	0x3bb70
   3bb88: eafffff4     	b	0x3bb60
   3bb8c: eafffff8     	b	0x3bb74
   3bb90: eafffff7     	b	0x3bb74
   3bb94: e320f000     	nop
   3bb98: 00 00 80 3f  	.word	0x3f800000
   3bb9c: 00 00 00 00  	.word	0x00000000
   3bba0: b9 00 00 00  	.word	0x000000b9
   3bba4: 1e 00 00 00  	.word	0x0000001e
   3bba8: 00 00 80 39  	.word	0x39800000
