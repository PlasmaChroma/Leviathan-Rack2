0000582c <_stabliseADC>:
    582c: e30031b2     	movw	r3, #0x1b2
    5830: e92d40f0     	push	{r4, r5, r6, r7, lr}
    5834: e19050b3     	ldrh	r5, [r0, r3]
    5838: e3550064     	cmp	r5, #100
    583c: 8a000005     	bhi	0x5858 <_stabliseADC+0x2c> @ imm = #0x14
    5840: e3550000     	cmp	r5, #0
    5844: 0a00009b     	beq	0x5ab8 <_stabliseADC+0x28c> @ imm = #0x26c
    5848: ee075a90     	vmov	s15, r5
    584c: e1a01005     	mov	r1, r5
    5850: eeb85a67     	vcvt.f32.u32	s10, s15
    5854: ea000002     	b	0x5864 <_stabliseADC+0x38> @ imm = #0x8
    5858: ed9f5aee     	vldr	s10, [pc, #952]         @ 0x5c18 <_stabliseADC+0x3ec>  // f32=100
    585c: e3a01064     	mov	r1, #100
    5860: e1a05001     	mov	r5, r1
    5864: e2802e1a     	add	r2, r0, #416
    5868: e2804e1b     	add	r4, r0, #432
    586c: ed900a01     	vldr	s0, [r0, #4]
    5870: e280cf6e     	add	r12, r0, #440
    5874: e1d220b0     	ldrh	r2, [r2]
    5878: e1d460b0     	ldrh	r6, [r4]
    587c: e1dce0b0     	ldrh	lr, [r12]
    5880: ee052a90     	vmov	s11, r2
    5884: edd06a03     	vldr	s13, [r0, #12]
    5888: eeb86a65     	vcvt.f32.u32	s12, s11
    588c: eeb40ac6     	vcmpe.f32	s0, s12
    5890: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5894: 4a0000c7     	bmi	0x5bb8 <_stabliseADC+0x38c> @ imm = #0x31c
    5898: e30031a2     	movw	r3, #0x1a2
    589c: e190c0b3     	ldrh	r12, [r0, r3]
    58a0: ee04ca10     	vmov	s8, r12
    58a4: eef84a44     	vcvt.f32.u32	s9, s8
    58a8: eeb40ae4     	vcmpe.f32	s0, s9
    58ac: eef1fa10     	vmrs	APSR_nzcv, fpscr
    58b0: ca000084     	bgt	0x5ac8 <_stabliseADC+0x29c> @ imm = #0x210
    58b4: ed907a69     	vldr	s14, [r0, #420]
    58b8: ed903a6b     	vldr	s6, [r0, #428]
    58bc: ee773a43     	vsub.f32	s7, s14, s6
    58c0: eeb40ae3     	vcmpe.f32	s0, s7
    58c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    58c8: aa0000bc     	bge	0x5bc0 <_stabliseADC+0x394> @ imm = #0x2f0
    58cc: ee700a46     	vsub.f32	s1, s0, s12
    58d0: ed9f1ad5     	vldr	s2, [pc, #852]          @ 0x5c2c <_stabliseADC+0x400>  // f32=2048
    58d4: ee601a81     	vmul.f32	s3, s1, s2
    58d8: ee332ac6     	vsub.f32	s4, s7, s12
    58dc: eec17a82     	vdiv.f32	s15, s3, s4
    58e0: eddf3acd     	vldr	s7, [pc, #820]          @ 0x5c1c <_stabliseADC+0x3f0>  // f32=4093
    58e4: edc07a01     	vstr	s15, [r0, #4]
    58e8: ee770ae6     	vsub.f32	s1, s15, s13
    58ec: eefd1ae0     	vcvt.s32.f32	s3, s1
    58f0: ee113a90     	vmov	r3, s3
    58f4: eeb01a00     	vmov.f32	s2, #2.000000e+00
    58f8: eef47ae3     	vcmpe.f32	s15, s7
    58fc: e3530000     	cmp	r3, #0
    5900: b2633000     	rsblt	r3, r3, #0
    5904: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5908: eef47ac1     	vcmpe.f32	s15, s2
    590c: c3a02001     	movgt	r2, #1
    5910: d3a02000     	movle	r2, #0
    5914: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5918: 43822001     	orrmi	r2, r2, #1
    591c: e3530faf     	cmp	r3, #700
    5920: d1a03002     	movle	r3, r2
    5924: c3823001     	orrgt	r3, r2, #1
    5928: e3530000     	cmp	r3, #0
    592c: 1a000067     	bne	0x5ad0 <_stabliseADC+0x2a4> @ imm = #0x19c
    5930: eeb84ac4     	vcvt.f32.s32	s8, s8
    5934: eeb44ae7     	vcmpe.f32	s8, s15
    5938: eef1fa10     	vmrs	APSR_nzcv, fpscr
    593c: 5a0000c5     	bpl	0x5c58 <_stabliseADC+0x42c> @ imm = #0x314
    5940: e0803106     	add	r3, r0, r6, lsl #2
    5944: e1a01101     	lsl	r1, r1, #2
    5948: e2802010     	add	r2, r0, #16
    594c: e3a0ca0f     	mov	r12, #61440
    5950: e344c57f     	movt	r12, #0x457f
    5954: e583c010     	str	r12, [r3, #0x10]
    5958: e286c001     	add	r12, r6, #1
    595c: e2413004     	sub	r3, r1, #4
    5960: eddf0aae     	vldr	s1, [pc, #696]          @ 0x5c20 <_stabliseADC+0x3f4>  // f32=0
    5964: e0821001     	add	r1, r2, r1
    5968: e6ffc07c     	uxth	r12, r12
    596c: e1a03123     	lsr	r3, r3, #2
    5970: e155000c     	cmp	r5, r12
    5974: e2833001     	add	r3, r3, #1
    5978: 93a0c000     	movls	r12, #0
    597c: e2133007     	ands	r3, r3, #7
    5980: e1c4c0b0     	strh	r12, [r4]
    5984: 0a000019     	beq	0x59f0 <_stabliseADC+0x1c4> @ imm = #0x64
    5988: e3530001     	cmp	r3, #1
    598c: 0a000013     	beq	0x59e0 <_stabliseADC+0x1b4> @ imm = #0x4c
    5990: e3530002     	cmp	r3, #2
    5994: 0a00000f     	beq	0x59d8 <_stabliseADC+0x1ac> @ imm = #0x3c
    5998: e3530003     	cmp	r3, #3
    599c: 0a00000b     	beq	0x59d0 <_stabliseADC+0x1a4> @ imm = #0x2c
    59a0: e3530004     	cmp	r3, #4
    59a4: 0a000007     	beq	0x59c8 <_stabliseADC+0x19c> @ imm = #0x1c
    59a8: e3530005     	cmp	r3, #5
    59ac: 0a000003     	beq	0x59c0 <_stabliseADC+0x194> @ imm = #0xc
    59b0: e3530006     	cmp	r3, #6
    59b4: 1a00008d     	bne	0x5bf0 <_stabliseADC+0x3c4> @ imm = #0x234
    59b8: ecb20a01     	vldmia	r2!, {s0}
    59bc: ee700a80     	vadd.f32	s1, s1, s0
    59c0: ecb23a01     	vldmia	r2!, {s6}
    59c4: ee700a83     	vadd.f32	s1, s1, s6
    59c8: ecb27a01     	vldmia	r2!, {s14}
    59cc: ee700a87     	vadd.f32	s1, s1, s14
    59d0: ecb26a01     	vldmia	r2!, {s12}
    59d4: ee700a86     	vadd.f32	s1, s1, s12
    59d8: ecf24a01     	vldmia	r2!, {s9}
    59dc: ee700aa4     	vadd.f32	s1, s1, s9
    59e0: ecf23a01     	vldmia	r2!, {s7}
    59e4: e1510002     	cmp	r1, r2
    59e8: ee700aa3     	vadd.f32	s1, s1, s7
    59ec: 0a000013     	beq	0x5a40 <_stabliseADC+0x214> @ imm = #0x4c
    59f0: e1a03002     	mov	r3, r2
    59f4: ed921a01     	vldr	s2, [r2, #4]
    59f8: e2822020     	add	r2, r2, #32
    59fc: ecf31a01     	vldmia	r3!, {s3}
    5a00: ed122a05     	vldr	s4, [r2, #-20]
    5a04: ee702aa1     	vadd.f32	s5, s1, s3
    5a08: edd37a01     	vldr	s15, [r3, #4]
    5a0c: ed124a04     	vldr	s8, [r2, #-16]
    5a10: ed120a03     	vldr	s0, [r2, #-12]
    5a14: ee323a81     	vadd.f32	s6, s5, s2
    5a18: ed525a02     	vldr	s11, [r2, #-8]
    5a1c: ed127a01     	vldr	s14, [r2, #-4]
    5a20: e1510002     	cmp	r1, r2
    5a24: ee336a27     	vadd.f32	s12, s6, s15
    5a28: ee764a02     	vadd.f32	s9, s12, s4
    5a2c: ee743a84     	vadd.f32	s7, s9, s8
    5a30: ee730a80     	vadd.f32	s1, s7, s0
    5a34: ee301aa5     	vadd.f32	s2, s1, s11
    5a38: ee710a07     	vadd.f32	s1, s2, s14
    5a3c: 1affffeb     	bne	0x59f0 <_stabliseADC+0x1c4> @ imm = #-0x54
    5a40: eec01a85     	vdiv.f32	s3, s1, s10
    5a44: ee365ae1     	vsub.f32	s10, s13, s3
    5a48: edc01a6a     	vstr	s3, [r0, #424]
    5a4c: eebd2ac5     	vcvt.s32.f32	s4, s10
    5a50: ee121a10     	vmov	r1, s4
    5a54: e281c031     	add	r12, r1, #49
    5a58: e35c0062     	cmp	r12, #98
    5a5c: 9ef70ae6     	vcvtls.f64.f32	d16, s13
    5a60: 9ddf2b6a     	vldrls	d18, [pc, #424]         @ 0x5c10 <_stabliseADC+0x3e4>
    5a64: edd06a6f     	vldr	s13, [r0, #444]
    5a68: 9ef71ac5     	vcvtls.f64.f32	d17, s10
    5a6c: 9e410ba2     	vmlals.f64	d16, d17, d18
    5a70: 9ef71be0     	vcvtls.f32.f64	s3, d16
    5a74: ee712ae6     	vsub.f32	s5, s3, s13
    5a78: edc01a03     	vstr	s3, [r0, #12]
    5a7c: eefd7ae2     	vcvt.s32.f32	s15, s5
    5a80: ee172a90     	vmov	r2, s15
    5a84: e3520000     	cmp	r2, #0
    5a88: b2622000     	rsblt	r2, r2, #0
    5a8c: e152000e     	cmp	r2, lr
    5a90: ba000038     	blt	0x5b78 <_stabliseADC+0x34c> @ imm = #0xe0
    5a94: e30011b6     	movw	r1, #0x1b6
    5a98: e3a03000     	mov	r3, #0
    5a9c: edc01a6f     	vstr	s3, [r0, #444]
    5aa0: e18030b1     	strh	r3, [r0, r1]
    5aa4: e5c031b4     	strb	r3, [r0, #0x1b4]
    5aa8: eebc4ae1     	vcvt.u32.f32	s8, s3
    5aac: ee142a10     	vmov	r2, s8
    5ab0: e1c020ba     	strh	r2, [r0, #10]
    5ab4: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    5ab8: e3a01001     	mov	r1, #1
    5abc: eeb75a00     	vmov.f32	s10, #1.000000e+00
    5ac0: e1a05001     	mov	r5, r1
    5ac4: eaffff66     	b	0x5864 <_stabliseADC+0x38> @ imm = #-0x268
    5ac8: eddf7a55     	vldr	s15, [pc, #340]         @ 0x5c24 <_stabliseADC+0x3f8>  // f32=4095
    5acc: edc07a01     	vstr	s15, [r0, #4]
    5ad0: e1a01101     	lsl	r1, r1, #2
    5ad4: e2802010     	add	r2, r0, #16
    5ad8: e241c004     	sub	r12, r1, #4
    5adc: e0827001     	add	r7, r2, r1
    5ae0: e1a03002     	mov	r3, r2
    5ae4: e1a0c12c     	lsr	r12, r12, #2
    5ae8: e28cc001     	add	r12, r12, #1
    5aec: e21cc007     	ands	r12, r12, #7
    5af0: 0a000013     	beq	0x5b44 <_stabliseADC+0x318> @ imm = #0x4c
    5af4: e35c0001     	cmp	r12, #1
    5af8: 0a00000e     	beq	0x5b38 <_stabliseADC+0x30c> @ imm = #0x38
    5afc: e35c0002     	cmp	r12, #2
    5b00: 0a00000b     	beq	0x5b34 <_stabliseADC+0x308> @ imm = #0x2c
    5b04: e35c0003     	cmp	r12, #3
    5b08: 0a000008     	beq	0x5b30 <_stabliseADC+0x304> @ imm = #0x20
    5b0c: e35c0004     	cmp	r12, #4
    5b10: 0a000005     	beq	0x5b2c <_stabliseADC+0x300> @ imm = #0x14
    5b14: e35c0005     	cmp	r12, #5
    5b18: 0a000002     	beq	0x5b28 <_stabliseADC+0x2fc> @ imm = #0x8
    5b1c: e35c0006     	cmp	r12, #6
    5b20: 1a000036     	bne	0x5c00 <_stabliseADC+0x3d4> @ imm = #0xd8
    5b24: ece37a01     	vstmia	r3!, {s15}
    5b28: ece37a01     	vstmia	r3!, {s15}
    5b2c: ece37a01     	vstmia	r3!, {s15}
    5b30: ece37a01     	vstmia	r3!, {s15}
    5b34: ece37a01     	vstmia	r3!, {s15}
    5b38: ece37a01     	vstmia	r3!, {s15}
    5b3c: e1570003     	cmp	r7, r3
    5b40: 0affff84     	beq	0x5958 <_stabliseADC+0x12c> @ imm = #-0x1f0
    5b44: e1a0c003     	mov	r12, r3
    5b48: e2833020     	add	r3, r3, #32
    5b4c: ecec7a01     	vstmia	r12!, {s15}
    5b50: ed437a07     	vstr	s15, [r3, #-28]
    5b54: edcc7a01     	vstr	s15, [r12, #4]
    5b58: ed437a05     	vstr	s15, [r3, #-20]
    5b5c: ed437a04     	vstr	s15, [r3, #-16]
    5b60: ed437a03     	vstr	s15, [r3, #-12]
    5b64: ed437a02     	vstr	s15, [r3, #-8]
    5b68: ed437a01     	vstr	s15, [r3, #-4]
    5b6c: e1570003     	cmp	r7, r3
    5b70: 1afffff3     	bne	0x5b44 <_stabliseADC+0x318> @ imm = #-0x34
    5b74: eaffff77     	b	0x5958 <_stabliseADC+0x12c> @ imm = #-0x224
    5b78: e5d0c1b4     	ldrb	r12, [r0, #0x1b4]
    5b7c: e35c0000     	cmp	r12, #0
    5b80: 18bd80f0     	popne	{r4, r5, r6, r7, pc}
    5b84: e30021b6     	movw	r2, #0x1b6
    5b88: e30011ba     	movw	r1, #0x1ba
    5b8c: e19030b2     	ldrh	r3, [r0, r2]
    5b90: e190c0b1     	ldrh	r12, [r0, r1]
    5b94: e2831001     	add	r1, r3, #1
    5b98: e6ff3071     	uxth	r3, r1
    5b9c: e18030b2     	strh	r3, [r0, r2]
    5ba0: e15c0003     	cmp	r12, r3
    5ba4: 2affffbf     	bhs	0x5aa8 <_stabliseADC+0x27c> @ imm = #-0x104
    5ba8: e3a0c001     	mov	r12, #1
    5bac: edc01a6f     	vstr	s3, [r0, #444]
    5bb0: e5c0c1b4     	strb	r12, [r0, #0x1b4]
    5bb4: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    5bb8: eddf7a18     	vldr	s15, [pc, #96]          @ 0x5c20 <_stabliseADC+0x3f4>  // f32=0
    5bbc: eaffffc2     	b	0x5acc <_stabliseADC+0x2a0> @ imm = #-0xf8
    5bc0: ee772a03     	vadd.f32	s5, s14, s6
    5bc4: eeb40ae2     	vcmpe.f32	s0, s5
    5bc8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5bcc: 9a000017     	bls	0x5c30 <_stabliseADC+0x404> @ imm = #0x5c
    5bd0: ee707a62     	vsub.f32	s15, s0, s5
    5bd4: ed9f0a13     	vldr	s0, [pc, #76]           @ 0x5c28 <_stabliseADC+0x3fc>  // f32=2047
    5bd8: ed9f3a13     	vldr	s6, [pc, #76]           @ 0x5c2c <_stabliseADC+0x400>  // f32=2048
    5bdc: ee277a80     	vmul.f32	s14, s15, s0
    5be0: ee346ae2     	vsub.f32	s12, s9, s5
    5be4: eec74a06     	vdiv.f32	s9, s14, s12
    5be8: ee747a83     	vadd.f32	s15, s9, s6
    5bec: eaffff3b     	b	0x58e0 <_stabliseADC+0xb4> @ imm = #-0x314
    5bf0: e592c000     	ldr	r12, [r2]
    5bf4: e2802014     	add	r2, r0, #20
    5bf8: ee00ca90     	vmov	s1, r12
    5bfc: eaffff6d     	b	0x59b8 <_stabliseADC+0x18c> @ imm = #-0x24c
    5c00: edc27a00     	vstr	s15, [r2]
    5c04: e2803014     	add	r3, r0, #20
    5c08: eaffffc5     	b	0x5b24 <_stabliseADC+0x2f8> @ imm = #-0xec
    5c0c: e320f000     	nop
    5c10: cd cc cc cc  	.word	0xcccccccd
    5c14: cc cc ec bf  	.word	0xbfeccccc
    5c18: 00 00 c8 42  	.word	0x42c80000
    5c1c: 00 d0 7f 45  	.word	0x457fd000
    5c20: 00 00 00 00  	.word	0x00000000
    5c24: 00 f0 7f 45  	.word	0x457ff000
    5c28: 00 e0 ff 44  	.word	0x44ffe000
    5c2c: 00 00 00 45  	.word	0x45000000
    5c30: ed5f7a03     	vldr	s15, [pc, #-12]         @ 0x5c2c <_stabliseADC+0x400>  // f32=2048
    5c34: ee372ae6     	vsub.f32	s4, s15, s13
    5c38: edc07a01     	vstr	s15, [r0, #4]
    5c3c: eefd2ac2     	vcvt.s32.f32	s5, s4
    5c40: ee123a90     	vmov	r3, s5
    5c44: e3530000     	cmp	r3, #0
    5c48: b2633000     	rsblt	r3, r3, #0
    5c4c: e3530faf     	cmp	r3, #700
    5c50: caffff9e     	bgt	0x5ad0 <_stabliseADC+0x2a4> @ imm = #-0x188
    5c54: eaffff35     	b	0x5930 <_stabliseADC+0x104> @ imm = #-0x32c
    5c58: eef85ae5     	vcvt.f32.s32	s11, s11
    5c5c: eef45ae7     	vcmpe.f32	s11, s15
    5c60: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5c64: c0803106     	addgt	r3, r0, r6, lsl #2
    5c68: d0803106     	addle	r3, r0, r6, lsl #2
    5c6c: c3a0c000     	movgt	r12, #0
    5c70: c1a01101     	lslgt	r1, r1, #2
    5c74: c2802010     	addgt	r2, r0, #16
    5c78: c583c010     	strgt	r12, [r3, #0x10]
    5c7c: d1a01101     	lslle	r1, r1, #2
    5c80: d2802010     	addle	r2, r0, #16
    5c84: ddc37a04     	vstrle	s15, [r3, #16]
    5c88: eaffff32     	b	0x5958 <_stabliseADC+0x12c> @ imm = #-0x338

