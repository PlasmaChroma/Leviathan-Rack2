00003bbc <tick_getPlayData>:
    3bbc: e92d4070     	push	{r4, r5, r6, lr}
    3bc0: eef30a0e     	vmov.f32	s1, #3.000000e+01
    3bc4: ed2d8b06     	vpush	{d8, d9, d10}
    3bc8: e1a04000     	mov	r4, r0
    3bcc: ed9f0a6d     	vldr	s0, [pc, #436]          @ 0x3d88 <tick_getPlayData+0x1cc>  // f32=0
    3bd0: ebfffa31     	bl	0x249c <.plt+0x1b8>     @ imm = #-0x173c  // CALL _memRead
    3bd4: e1a00004     	mov	r0, r4
    3bd8: e3a01000     	mov	r1, #0
    3bdc: ed948a07     	vldr	s16, [r4, #28]
    3be0: eeb77a00     	vmov.f32	s14, #1.000000e+00
    3be4: ed9f9b5f     	vldr	d9, [pc, #380]          @ 0x3d68 <tick_getPlayData+0x1ac>  // f64=1.0000000000000001e-05
    3be8: eef2aa04     	vmov.f32	s21, #1.000000e+01
    3bec: ee680a2a     	vmul.f32	s1, s16, s21
    3bf0: ee707a00     	vadd.f32	s15, s0, s0
    3bf4: ed9f0a64     	vldr	s0, [pc, #400]          @ 0x3d8c <tick_getPlayData+0x1d0>  // f32=100
    3bf8: ee371a87     	vadd.f32	s2, s15, s14
    3bfc: eefd1ac1     	vcvt.s32.f32	s3, s2
    3c00: eeb78ae0     	vcvt.f64.f32	d8, s1
    3c04: ee115a90     	vmov	r5, s3
    3c08: ebfff9f0     	bl	0x23d0 <.plt+0xec>      @ imm = #-0x1840  // CALL _memReadLayer
    3c0c: e1a00004     	mov	r0, r4
    3c10: e3a01000     	mov	r1, #0
    3c14: ed94aa07     	vldr	s20, [r4, #28]
    3c18: ee282b09     	vmul.f64	d2, d8, d9
    3c1c: eef70ac0     	vcvt.f64.f32	d16, s0
    3c20: ed9f0a5a     	vldr	s0, [pc, #360]          @ 0x3d90 <tick_getPlayData+0x1d4>  // f32=103
    3c24: ee228b20     	vmul.f64	d8, d2, d16
    3c28: ebfff9e8     	bl	0x23d0 <.plt+0xec>      @ imm = #-0x1860  // CALL _memReadLayer
    3c2c: e59f3168     	ldr	r3, [pc, #0x168]        @ 0x3d9c <tick_getPlayData+0x1e0>  // u32=0x15534; f32?=1.22400618e-40
    3c30: e08f3003     	add	r3, pc, r3
    3c34: e5932010     	ldr	r2, [r3, #0x10]
    3c38: e3520078     	cmp	r2, #120
    3c3c: eeb78bc8     	vcvt.f32.f64	s16, d8
    3c40: 8a000030     	bhi	0x3d08 <tick_getPlayData+0x14c> @ imm = #0xc0
    3c44: ed939a05     	vldr	s18, [r3, #20]
    3c48: e3a01000     	mov	r1, #0
    3c4c: e1a00004     	mov	r0, r4
    3c50: edd42a07     	vldr	s5, [r4, #28]
    3c54: eeb20a04     	vmov.f32	s0, #1.000000e+01
    3c58: ee223a80     	vmul.f32	s6, s5, s0
    3c5c: ee783a09     	vadd.f32	s7, s16, s18
    3c60: eef43ac3     	vcmpe.f32	s7, s6
    3c64: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3c68: ce783a49     	vsubgt.f32	s7, s16, s18
    3c6c: eef53ac0     	vcmpe.f32	s7, #0
    3c70: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3c74: aefd3ae3     	vcvtge.s32.f32	s7, s7
    3c78: b3a03000     	movlt	r3, #0
    3c7c: ae133a90     	vmovge	r3, s7
    3c80: eeb20a0c     	vmov.f32	s0, #1.400000e+01
    3c84: e584303c     	str	r3, [r4, #0x3c]
    3c88: ebfff9d0     	bl	0x23d0 <.plt+0xec>      @ imm = #-0x18c0  // CALL _memReadLayer
    3c8c: ed9f4a40     	vldr	s8, [pc, #256]          @ 0x3d94 <tick_getPlayData+0x1d8>  // f32=4095
    3c90: e59f0108     	ldr	r0, [pc, #0x108]        @ 0x3da0 <tick_getPlayData+0x1e4>  // u32=0x154cc; f32?=1.22254883e-40
    3c94: eef74a00     	vmov.f32	s9, #1.000000e+00
    3c98: e08f1000     	add	r1, pc, r0
    3c9c: eddf6a3d     	vldr	s13, [pc, #244]         @ 0x3d98 <tick_getPlayData+0x1dc>  // f32=0.000488519785
    3ca0: e5916010     	ldr	r6, [r1, #0x10]
    3ca4: eddf3b31     	vldr	d19, [pc, #196]         @ 0x3d70 <tick_getPlayData+0x1b4>  // f64=0.94999999999999996
    3ca8: e286c001     	add	r12, r6, #1
    3cac: e581c010     	str	r12, [r1, #0x10]
    3cb0: eddf2b30     	vldr	d18, [pc, #192]         @ 0x3d78 <tick_getPlayData+0x1bc>  // f64=0.90000000000000002
    3cb4: eddf1b31     	vldr	d17, [pc, #196]         @ 0x3d80 <tick_getPlayData+0x1c4>  // f64=0.0054999999999999997
    3cb8: ee345a40     	vsub.f32	s10, s8, s0
    3cbc: ee554a26     	vnmls.f32	s9, s10, s13
    3cc0: eeb76ae4     	vcvt.f64.f32	d6, s9
    3cc4: ee269b23     	vmul.f64	d9, d6, d19
    3cc8: eef75bc9     	vcvt.f32.f64	s11, d9
    3ccc: eefd8ae5     	vcvt.s32.f32	s17, s11
    3cd0: edc45a12     	vstr	s11, [r4, #72]
    3cd4: ee183a90     	vmov	r3, s17
    3cd8: e3530000     	cmp	r3, #0
    3cdc: b2633000     	rsblt	r3, r3, #0
    3ce0: e3550001     	cmp	r5, #1
    3ce4: ee093a90     	vmov	s19, r3
    3ce8: b3a05001     	movlt	r5, #1
    3cec: e5845050     	str	r5, [r4, #0x50]
    3cf0: eeb8abe9     	vcvt.f64.s32	d10, s19
    3cf4: ee4a1b22     	vmla.f64	d17, d10, d18
    3cf8: eef7abe1     	vcvt.f32.f64	s21, d17
    3cfc: edc4aa16     	vstr	s21, [r4, #88]
    3d00: ecbd8b06     	vpop	{d8, d9, d10}
    3d04: e8bd8070     	pop	{r4, r5, r6, pc}
    3d08: ee2aaa2a     	vmul.f32	s20, s20, s21
    3d0c: eeb7aaca     	vcvt.f64.f32	d10, s20
    3d10: ee2a9b09     	vmul.f64	d9, d10, d9
    3d14: eeb70ac0     	vcvt.f64.f32	d0, s0
    3d18: ee299b00     	vmul.f64	d9, d9, d0
    3d1c: ebfffa05     	bl	0x2538 <.plt+0x254>     @ imm = #-0x17ec  // CALL rand
    3d20: eeb79bc9     	vcvt.f32.f64	s18, d9
    3d24: ee391a09     	vadd.f32	s2, s18, s18
    3d28: ee070a90     	vmov	s15, r0
    3d2c: eeb51ac0     	vcmpe.f32	s2, #0
    3d30: eeb80be7     	vcvt.f64.s32	d0, s15
    3d34: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3d38: ceb71ac1     	vcvtgt.f64.f32	d1, s2
    3d3c: deb71b00     	vmovle.f64	d1, #1.000000e+00
    3d40: ebfff9c9     	bl	0x246c <.plt+0x188>     @ imm = #-0x18dc  // CALL __fmod_finite
    3d44: e59f3058     	ldr	r3, [pc, #0x58]         @ 0x3da4 <tick_getPlayData+0x1e8>  // u32=0x15418; f32?=1.22002649e-40
    3d48: e3a02000     	mov	r2, #0
    3d4c: e08f3003     	add	r3, pc, r3
    3d50: e5832010     	str	r2, [r3, #0x10]
    3d54: eeb70bc0     	vcvt.f32.f64	s0, d0
    3d58: ee309a49     	vsub.f32	s18, s0, s18
    3d5c: eeb09ac9     	vabs.f32	s18, s18
    3d60: ed839a05     	vstr	s18, [r3, #20]
    3d64: eaffffb7     	b	0x3c48 <tick_getPlayData+0x8c> @ imm = #-0x124
    3d68: f1 68 e3 88  	.word	0x88e368f1
    3d6c: b5 f8 e4 3e  	.word	0x3ee4f8b5
    3d70: 66 66 66 66  	.word	0x66666666
    3d74: 66 66 ee 3f  	.word	0x3fee6666
    3d78: cd cc cc cc  	.word	0xcccccccd
    3d7c: cc cc ec 3f  	.word	0x3feccccc
    3d80: ba 49 0c 02  	.word	0x020c49ba
    3d84: 2b 87 76 3f  	.word	0x3f76872b
    3d88: 00 00 00 00  	.word	0x00000000
    3d8c: 00 00 c8 42  	.word	0x42c80000
    3d90: 00 00 ce 42  	.word	0x42ce0000
    3d94: 00 f0 7f 45  	.word	0x457ff000
    3d98: 02 10 00 3a  	.word	0x3a001002
    3d9c: 34 55 01 00  	.word	0x00015534
    3da0: cc 54 01 00  	.word	0x000154cc
    3da4: 18 54 01 00  	.word	0x00015418

