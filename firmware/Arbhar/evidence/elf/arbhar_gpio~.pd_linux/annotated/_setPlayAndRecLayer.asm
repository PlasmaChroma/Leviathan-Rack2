000078c4 <_setPlayAndRecLayer>:
    78c4: e2803d52     	add	r3, r0, #5248
    78c8: ed9f0ac0     	vldr	s0, [pc, #768]          @ 0x7bd0 <_setPlayAndRecLayer+0x30c>  // f32=210
    78cc: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    78d0: e1a04000     	mov	r4, r0
    78d4: e1d311b2     	ldrh	r1, [r3, #18]
    78d8: ed2d8b02     	vpush	{d8}
    78dc: e59f6300     	ldr	r6, [pc, #0x300]        @ 0x7be4 <_setPlayAndRecLayer+0x320>  // u32=0x1facc; f32?=1.81804463e-40
    78e0: ee071a90     	vmov	s15, r1
    78e4: e08f8006     	add	r8, pc, r6
    78e8: eddf8ab9     	vldr	s17, [pc, #740]         @ 0x7bd4 <_setPlayAndRecLayer+0x310>  // f32=4095
    78ec: e24dd01c     	sub	sp, sp, #28
    78f0: eef80ae7     	vcvt.f32.s32	s1, s15
    78f4: ee388ae0     	vsub.f32	s16, s17, s1
    78f8: ebffefba     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x4118  // CALL readFromSharedMem
    78fc: ed9f1ab5     	vldr	s2, [pc, #724]          @ 0x7bd8 <_setPlayAndRecLayer+0x314>  // f32=32
    7900: e1a00004     	mov	r0, r4
    7904: ed9f7ab4     	vldr	s14, [pc, #720]         @ 0x7bdc <_setPlayAndRecLayer+0x318>  // f32=0
    7908: eddf2bac     	vldr	d18, [pc, #688]         @ 0x7bc0 <_setPlayAndRecLayer+0x2fc>  // f64=0.0014652000000000003
    790c: eef61b00     	vmov.f64	d17, #5.000000e-01
    7910: ee008a01     	vmla.f32	s16, s0, s2
    7914: eeb48ac7     	vcmpe.f32	s16, s14
    7918: eeb00a48     	vmov.f32	s0, s16
    791c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7920: beb00a47     	vmovlt.f32	s0, s14
    7924: eeb40ae8     	vcmpe.f32	s0, s17
    7928: eef1fa10     	vmrs	APSR_nzcv, fpscr
    792c: 8eb00a68     	vmovhi.f32	s0, s17
    7930: eef70ac0     	vcvt.f64.f32	d16, s0
    7934: ee401ba2     	vmla.f64	d17, d16, d18
    7938: eef78be1     	vcvt.f32.f64	s17, d17
    793c: eeb00a68     	vmov.f32	s0, s17
    7940: ebfff03e     	bl	0x3a40 <.plt+0x344>     @ imm = #-0x3f08  // CALL _controlLayerCoupling
    7944: e5d4203c     	ldrb	r2, [r4, #0x3c]
    7948: e5d40032     	ldrb	r0, [r4, #0x32]
    794c: e3520000     	cmp	r2, #0
    7950: e5880034     	str	r0, [r8, #0x34]
    7954: 1a000011     	bne	0x79a0 <_setPlayAndRecLayer+0xdc> @ imm = #0x44
    7958: eefc1ae8     	vcvt.u32.f32	s3, s17
    795c: e5d8903c     	ldrb	r9, [r8, #0x3c]
    7960: edc88a0e     	vstr	s17, [r8, #56]
    7964: edcd1a01     	vstr	s3, [sp, #4]
    7968: e5dd5004     	ldrb	r5, [sp, #0x4]
    796c: e3550005     	cmp	r5, #5
    7970: 93a05000     	movls	r5, #0
    7974: 83a05001     	movhi	r5, #1
    7978: e1590005     	cmp	r9, r5
    797c: 1a000063     	bne	0x7b10 <_setPlayAndRecLayer+0x24c> @ imm = #0x18c
    7980: eeb12a04     	vmov.f32	s4, #5.000000e+00
    7984: e59fc25c     	ldr	r12, [pc, #0x25c]       @ 0x7be8 <_setPlayAndRecLayer+0x324>  // u32=0x1fa28; f32?=1.8157465e-40
    7988: e08fe00c     	add	lr, pc, r12
    798c: e5ce503c     	strb	r5, [lr, #0x3c]
    7990: eef48ac2     	vcmpe.f32	s17, s4
    7994: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7998: 8ef08a42     	vmovhi.f32	s17, s4
    799c: edce8a0e     	vstr	s17, [lr, #56]
    79a0: e5d43039     	ldrb	r3, [r4, #0x39]
    79a4: e3530000     	cmp	r3, #0
    79a8: 1a000030     	bne	0x7a70 <_setPlayAndRecLayer+0x1ac> @ imm = #0xc0
    79ac: e30012ea     	movw	r1, #0x2ea
    79b0: eddf2a8a     	vldr	s5, [pc, #552]          @ 0x7be0 <_setPlayAndRecLayer+0x31c>  // f32=2048
    79b4: e19460b1     	ldrh	r6, [r4, r1]
    79b8: e30a8aab     	movw	r8, #0xaaab
    79bc: e59f2228     	ldr	r2, [pc, #0x228]        @ 0x7bec <_setPlayAndRecLayer+0x328>  // u32=0x1f9e4; f32?=1.81479362e-40
    79c0: e3428aaa     	movt	r8, #0x2aaa
    79c4: ed9f4b7f     	vldr	d4, [pc, #508]          @ 0x7bc8 <_setPlayAndRecLayer+0x304>  // f64=0.0058608000000000011
    79c8: e3a00006     	mov	r0, #6
    79cc: e08f9002     	add	r9, pc, r2
    79d0: ee036a10     	vmov	s6, r6
    79d4: edd96a0e     	vldr	s13, [r9, #56]
    79d8: eef83ac3     	vcvt.f32.s32	s7, s6
    79dc: ee325ae3     	vsub.f32	s10, s5, s7
    79e0: eef73ac5     	vcvt.f64.f32	d19, s10
    79e4: ee634b84     	vmul.f64	d20, d19, d4
    79e8: eefd4ae6     	vcvt.s32.f32	s9, s13
    79ec: eef75be4     	vcvt.f32.f64	s11, d20
    79f0: ee143a90     	vmov	r3, s9
    79f4: eebd6ae5     	vcvt.s32.f32	s12, s11
    79f8: ee165a10     	vmov	r5, s12
    79fc: e0cc2598     	smull	r2, r12, r8, r5
    7a00: e04cefc5     	sub	lr, r12, r5, asr #31
    7a04: e0615e90     	mls	r1, r0, lr, r5
    7a08: e0916003     	adds	r6, r1, r3
    7a0c: 40862000     	addmi	r2, r6, r0
    7a10: 50c21698     	smullpl	r1, r2, r8, r6
    7a14: e5d48038     	ldrb	r8, [r4, #0x38]
    7a18: 50422fc6     	subpl	r2, r2, r6, asr #31
    7a1c: 50626290     	mlspl	r2, r0, r2, r6
    7a20: e59f01c8     	ldr	r0, [pc, #0x1c8]        @ 0x7bf0 <_setPlayAndRecLayer+0x32c>  // u32=0x1f988; f32?=1.81350442e-40
    7a24: e3580000     	cmp	r8, #0
    7a28: e08f5000     	add	r5, pc, r0
    7a2c: e5852034     	str	r2, [r5, #0x34]
    7a30: 1a000011     	bne	0x7a7c <_setPlayAndRecLayer+0x1b8> @ imm = #0x44
    7a34: e59fe1b8     	ldr	lr, [pc, #0x1b8]        @ 0x7bf4 <_setPlayAndRecLayer+0x330>  // u32=0x1f970; f32?=1.81316811e-40
    7a38: e3a0101e     	mov	r1, #30
    7a3c: e1a00004     	mov	r0, r4
    7a40: e08f500e     	add	r5, pc, lr
    7a44: ebfff027     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x3f64  // CALL writeToSharedMem
    7a48: e5d43031     	ldrb	r3, [r4, #0x31]
    7a4c: e5952034     	ldr	r2, [r5, #0x34]
    7a50: e3530000     	cmp	r3, #0
    7a54: e5c42032     	strb	r2, [r4, #0x32]
    7a58: 0a000004     	beq	0x7a70 <_setPlayAndRecLayer+0x1ac> @ imm = #0x10
    7a5c: e3a0101f     	mov	r1, #31
    7a60: e1a00004     	mov	r0, r4
    7a64: ebfff01f     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x3f84  // CALL writeToSharedMem
    7a68: e5952034     	ldr	r2, [r5, #0x34]
    7a6c: e5c42037     	strb	r2, [r4, #0x37]
    7a70: e28dd01c     	add	sp, sp, #28
    7a74: ecbd8b02     	vpop	{d8}
    7a78: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    7a7c: e5d4603c     	ldrb	r6, [r4, #0x3c]
    7a80: e3560000     	cmp	r6, #0
    7a84: 1afffff9     	bne	0x7a70 <_setPlayAndRecLayer+0x1ac> @ imm = #-0x1c
    7a88: e5d49032     	ldrb	r9, [r4, #0x32]
    7a8c: e1590002     	cmp	r9, r2
    7a90: 0a000023     	beq	0x7b24 <_setPlayAndRecLayer+0x260> @ imm = #0x8c
    7a94: e59f215c     	ldr	r2, [pc, #0x15c]        @ 0x7bf8 <_setPlayAndRecLayer+0x334>  // u32=0x1f84c; f32?=1.80907632e-40
    7a98: e08fc002     	add	r12, pc, r2
    7a9c: e5dce003     	ldrb	lr, [r12, #0x3]
    7aa0: e35e0001     	cmp	lr, #1
    7aa4: 128d7008     	addne	r7, sp, #8
    7aa8: 0afffff0     	beq	0x7a70 <_setPlayAndRecLayer+0x1ac> @ imm = #-0x40
    7aac: e59f6148     	ldr	r6, [pc, #0x148]        @ 0x7bfc <_setPlayAndRecLayer+0x338>  // u32=0xd2f8; f32?=7.56813275e-41
    7ab0: e3a01001     	mov	r1, #1
    7ab4: e3a09001     	mov	r9, #1
    7ab8: e3a085fe     	mov	r8, #1065353216
    7abc: e08f0006     	add	r0, pc, r6
    7ac0: ebfff02c     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x3f50  // CALL post
    7ac4: e59f0134     	ldr	r0, [pc, #0x134]        @ 0x7c00 <_setPlayAndRecLayer+0x33c>  // u32=0xcfe4; f32?=7.45771043e-41
    7ac8: e5944070     	ldr	r4, [r4, #0x70]
    7acc: e3a0c000     	mov	r12, #0
    7ad0: e08f0000     	add	r0, pc, r0
    7ad4: e344c31d     	movt	r12, #0x431d
    7ad8: e58d9008     	str	r9, [sp, #0x8]
    7adc: e58dc014     	str	r12, [sp, #0x14]
    7ae0: e58d9010     	str	r9, [sp, #0x10]
    7ae4: e58d800c     	str	r8, [sp, #0xc]
    7ae8: ebffef0e     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x43c8  // CALL gensym
    7aec: e1a03007     	mov	r3, r7
    7af0: e59f710c     	ldr	r7, [pc, #0x10c]        @ 0x7c04 <_setPlayAndRecLayer+0x340>  // u32=0x1f7e0; f32?=1.80756292e-40
    7af4: e3a02002     	mov	r2, #2
    7af8: e1a01000     	mov	r1, r0
    7afc: e1a00004     	mov	r0, r4
    7b00: ebfff05b     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x3e94  // CALL outlet_list
    7b04: e08f1007     	add	r1, pc, r7
    7b08: e5c19003     	strb	r9, [r1, #0x3]
    7b0c: eaffffd7     	b	0x7a70 <_setPlayAndRecLayer+0x1ac> @ imm = #-0xa4
    7b10: e1a01005     	mov	r1, r5
    7b14: e1a00004     	mov	r0, r4
    7b18: ebfff00a     	bl	0x3b48 <.plt+0x44c>     @ imm = #-0x3fd8  // CALL _setOmega
    7b1c: edd88a0e     	vldr	s17, [r8, #56]
    7b20: eaffff96     	b	0x7980 <_setPlayAndRecLayer+0xbc> @ imm = #-0x1a8
    7b24: e59f70dc     	ldr	r7, [pc, #0xdc]         @ 0x7c08 <_setPlayAndRecLayer+0x344>  // u32=0x1f7b8; f32?=1.8070024e-40
    7b28: e5c46038     	strb	r6, [r4, #0x38]
    7b2c: e08f8007     	add	r8, pc, r7
    7b30: e5d81003     	ldrb	r1, [r8, #0x3]
    7b34: e3510000     	cmp	r1, #0
    7b38: 0affffbd     	beq	0x7a34 <_setPlayAndRecLayer+0x170> @ imm = #-0x10c
    7b3c: e59f30c8     	ldr	r3, [pc, #0xc8]         @ 0x7c0c <_setPlayAndRecLayer+0x348>  // u32=0xd26c; f32?=7.54851457e-41
    7b40: e1a01006     	mov	r1, r6
    7b44: e28d7008     	add	r7, sp, #8
    7b48: e08f0003     	add	r0, pc, r3
    7b4c: ebfff009     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x3fdc  // CALL post
    7b50: e59f00b8     	ldr	r0, [pc, #0xb8]         @ 0x7c10 <_setPlayAndRecLayer+0x34c>  // u32=0xcf58; f32?=7.43809225e-41
    7b54: e3a01001     	mov	r1, #1
    7b58: e3a0c000     	mov	r12, #0
    7b5c: e08f0000     	add	r0, pc, r0
    7b60: e5949070     	ldr	r9, [r4, #0x70]
    7b64: e58dc00c     	str	r12, [sp, #0xc]
    7b68: e3a02000     	mov	r2, #0
    7b6c: e58d1008     	str	r1, [sp, #0x8]
    7b70: e344231d     	movt	r2, #0x431d
    7b74: e58d1010     	str	r1, [sp, #0x10]
    7b78: e58d2014     	str	r2, [sp, #0x14]
    7b7c: ebffeee9     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x445c  // CALL gensym
    7b80: e1a03007     	mov	r3, r7
    7b84: e3a02002     	mov	r2, #2
    7b88: e1a01000     	mov	r1, r0
    7b8c: e1a00009     	mov	r0, r9
    7b90: ebfff037     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x3f24  // CALL outlet_list
    7b94: e5d43038     	ldrb	r3, [r4, #0x38]
    7b98: e5c86003     	strb	r6, [r8, #0x3]
    7b9c: e3530000     	cmp	r3, #0
    7ba0: 0a000003     	beq	0x7bb4 <_setPlayAndRecLayer+0x2f0> @ imm = #0xc
    7ba4: e5d4503c     	ldrb	r5, [r4, #0x3c]
    7ba8: e3550000     	cmp	r5, #0
    7bac: 0affffbe     	beq	0x7aac <_setPlayAndRecLayer+0x1e8> @ imm = #-0x108
    7bb0: eaffffae     	b	0x7a70 <_setPlayAndRecLayer+0x1ac> @ imm = #-0x148
    7bb4: e5952034     	ldr	r2, [r5, #0x34]
    7bb8: eaffff9d     	b	0x7a34 <_setPlayAndRecLayer+0x170> @ imm = #-0x18c
    7bbc: e320f000     	nop
    7bc0: 02 1d 41 85  	.word	0x85411d02
    7bc4: 7e 01 58 3f  	.word	0x3f58017e
    7bc8: 02 1d 41 85  	.word	0x85411d02
    7bcc: 7e 01 78 3f  	.word	0x3f78017e
    7bd0: 00 00 52 43  	.word	0x43520000
    7bd4: 00 f0 7f 45  	.word	0x457ff000
    7bd8: 00 00 00 42  	.word	0x42000000
    7bdc: 00 00 00 00  	.word	0x00000000
    7be0: 00 00 00 45  	.word	0x45000000
    7be4: cc fa 01 00  	.word	0x0001facc
    7be8: 28 fa 01 00  	.word	0x0001fa28
    7bec: e4 f9 01 00  	.word	0x0001f9e4
    7bf0: 88 f9 01 00  	.word	0x0001f988
    7bf4: 70 f9 01 00  	.word	0x0001f970
    7bf8: 4c f8 01 00  	.word	0x0001f84c
    7bfc: f8 d2 00 00  	.word	0x0000d2f8
    7c00: e4 cf 00 00  	.word	0x0000cfe4
    7c04: e0 f7 01 00  	.word	0x0001f7e0
    7c08: b8 f7 01 00  	.word	0x0001f7b8
    7c0c: 6c d2 00 00  	.word	0x0000d26c
    7c10: 58 cf 00 00  	.word	0x0000cf58

