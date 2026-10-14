000038e0 <fftwObject_tilde_attackShape>:
    38e0: eeb61a00     	vmov.f32	s2, #5.000000e-01
    38e4: e92d4ff8     	push	{r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    38e8: e1a07000     	mov	r7, r0
    38ec: ed2d8b08     	vpush	{d8, d9, d10, d11}
    38f0: eeb40ac1     	vcmpe.f32	s0, s2
    38f4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    38f8: 4a0000ff     	bmi	0x3cfc <fftwObject_tilde_attackShape+0x41c> @ imm = #0x3fc
    38fc: eef71a00     	vmov.f32	s3, #1.000000e+00
    3900: eef00a41     	vmov.f32	s1, s2
    3904: eeb12a04     	vmov.f32	s4, #5.000000e+00
    3908: eeb01a61     	vmov.f32	s2, s3
    390c: ebfff361     	bl	0x698 <.plt+0x80>       @ imm = #-0x327c  // CALL map
    3910: e2872866     	add	r2, r7, #6684672
    3914: e2823a01     	add	r3, r2, #4096
    3918: ed830a12     	vstr	s0, [r3, #72]
    391c: e5925d10     	ldr	r5, [r2, #0xd10]
    3920: e593003c     	ldr	r0, [r3, #0x3c]
    3924: e5936040     	ldr	r6, [r3, #0x40]
    3928: e1500005     	cmp	r0, r5
    392c: ed93aa13     	vldr	s20, [r3, #76]
    3930: a1a0a005     	movge	r10, r5
    3934: b1a0a000     	movlt	r10, r0
    3938: e1550006     	cmp	r5, r6
    393c: e583a03c     	str	r10, [r3, #0x3c]
    3940: b1a01005     	movlt	r1, r5
    3944: a1a01006     	movge	r1, r6
    3948: e35a0000     	cmp	r10, #0
    394c: e0455001     	sub	r5, r5, r1
    3950: e5831040     	str	r1, [r3, #0x40]
    3954: da0000f2     	ble	0x3d24 <fftwObject_tilde_attackShape+0x444> @ imm = #0x3c8
    3958: eeb77a00     	vmov.f32	s14, #1.000000e+00
    395c: e21a9003     	ands	r9, r10, #3
    3960: ee070a90     	vmov	s15, r0
    3964: e3004d1c     	movw	r4, #0xd1c
    3968: e3404066     	movt	r4, #0x66
    396c: e3a08000     	mov	r8, #0
    3970: e0874004     	add	r4, r7, r4
    3974: eef80ae7     	vcvt.f32.s32	s1, s15
    3978: eeb78ac0     	vcvt.f64.f32	d8, s0
    397c: ee879a20     	vdiv.f32	s18, s14, s1
    3980: 0a00001d     	beq	0x39fc <fftwObject_tilde_attackShape+0x11c> @ imm = #0x74
    3984: e3590001     	cmp	r9, #1
    3988: 0a000010     	beq	0x39d0 <fftwObject_tilde_attackShape+0xf0> @ imm = #0x40
    398c: e3590002     	cmp	r9, #2
    3990: 0a000005     	beq	0x39ac <fftwObject_tilde_attackShape+0xcc> @ imm = #0x14
    3994: ed9f0be5     	vldr	d0, [pc, #916]          @ 0x3d30 <fftwObject_tilde_attackShape+0x450>  // f64=0
    3998: e3a08001     	mov	r8, #1
    399c: eeb01b48     	vmov.f64	d1, d8
    39a0: ebfff357     	bl	0x704 <.plt+0xec>       @ imm = #-0x32a4  // CALL __pow_finite
    39a4: eeb70bc0     	vcvt.f32.f64	s0, d0
    39a8: eca40a01     	vstmia	r4!, {s0}
    39ac: ee028a10     	vmov	s4, r8
    39b0: e2888001     	add	r8, r8, #1
    39b4: eeb01b48     	vmov.f64	d1, d8
    39b8: eef82ac2     	vcvt.f32.s32	s5, s4
    39bc: ee223a89     	vmul.f32	s6, s5, s18
    39c0: eeb70ac3     	vcvt.f64.f32	d0, s6
    39c4: ebfff34e     	bl	0x704 <.plt+0xec>       @ imm = #-0x32c8  // CALL __pow_finite
    39c8: eeb71bc0     	vcvt.f32.f64	s2, d0
    39cc: eca41a01     	vstmia	r4!, {s2}
    39d0: ee018a90     	vmov	s3, r8
    39d4: e2888001     	add	r8, r8, #1
    39d8: eef83ae1     	vcvt.f32.s32	s7, s3
    39dc: ee234a89     	vmul.f32	s8, s7, s18
    39e0: eeb01b48     	vmov.f64	d1, d8
    39e4: eeb70ac4     	vcvt.f64.f32	d0, s8
    39e8: ebfff345     	bl	0x704 <.plt+0xec>       @ imm = #-0x32ec  // CALL __pow_finite
    39ec: e15a0008     	cmp	r10, r8
    39f0: eef74bc0     	vcvt.f32.f64	s9, d0
    39f4: ece44a01     	vstmia	r4!, {s9}
    39f8: 0a000027     	beq	0x3a9c <fftwObject_tilde_attackShape+0x1bc> @ imm = #0x9c
    39fc: e2889001     	add	r9, r8, #1
    3a00: e1a0b004     	mov	r11, r4
    3a04: e2844010     	add	r4, r4, #16
    3a08: ee058a10     	vmov	s10, r8
    3a0c: eeb01b48     	vmov.f64	d1, d8
    3a10: eef85ac5     	vcvt.f32.s32	s11, s10
    3a14: ee256a89     	vmul.f32	s12, s11, s18
    3a18: eeb70ac6     	vcvt.f64.f32	d0, s12
    3a1c: ebfff338     	bl	0x704 <.plt+0xec>       @ imm = #-0x3320  // CALL __pow_finite
    3a20: ee069a90     	vmov	s13, r9
    3a24: eef89ae6     	vcvt.f32.s32	s19, s13
    3a28: ee69aa89     	vmul.f32	s21, s19, s18
    3a2c: eeb01b48     	vmov.f64	d1, d8
    3a30: eeb7bbc0     	vcvt.f32.f64	s22, d0
    3a34: eeb70aea     	vcvt.f64.f32	d0, s21
    3a38: ecabba01     	vstmia	r11!, {s22}
    3a3c: ebfff330     	bl	0x704 <.plt+0xec>       @ imm = #-0x3340  // CALL __pow_finite
    3a40: e288c002     	add	r12, r8, #2
    3a44: ee0bca90     	vmov	s23, r12
    3a48: eeb87aeb     	vcvt.f32.s32	s14, s23
    3a4c: ee677a09     	vmul.f32	s15, s14, s18
    3a50: eeb01b48     	vmov.f64	d1, d8
    3a54: eeb72bc0     	vcvt.f32.f64	s4, d0
    3a58: eeb70ae7     	vcvt.f64.f32	d0, s15
    3a5c: ed042a03     	vstr	s4, [r4, #-12]
    3a60: ebfff327     	bl	0x704 <.plt+0xec>       @ imm = #-0x3364  // CALL __pow_finite
    3a64: e2882003     	add	r2, r8, #3
    3a68: e2888004     	add	r8, r8, #4
    3a6c: ee022a90     	vmov	s5, r2
    3a70: eeb83ae2     	vcvt.f32.s32	s6, s5
    3a74: ee633a09     	vmul.f32	s7, s6, s18
    3a78: eeb01b48     	vmov.f64	d1, d8
    3a7c: eeb74bc0     	vcvt.f32.f64	s8, d0
    3a80: eeb70ae3     	vcvt.f64.f32	d0, s7
    3a84: ed8b4a01     	vstr	s8, [r11, #4]
    3a88: ebfff31d     	bl	0x704 <.plt+0xec>       @ imm = #-0x338c  // CALL __pow_finite
    3a8c: e15a0008     	cmp	r10, r8
    3a90: eef70bc0     	vcvt.f32.f64	s1, d0
    3a94: ed440a01     	vstr	s1, [r4, #-4]
    3a98: 1affffd7     	bne	0x39fc <fftwObject_tilde_attackShape+0x11c> @ imm = #-0xa4
    3a9c: e155000a     	cmp	r5, r10
    3aa0: da00002e     	ble	0x3b60 <fftwObject_tilde_attackShape+0x280> @ imm = #0xb8
    3aa4: e308e347     	movw	lr, #0x8347
    3aa8: e340e019     	movt	lr, #0x19
    3aac: e08aa00e     	add	r10, r10, lr
    3ab0: e3003d1c     	movw	r3, #0xd1c
    3ab4: e3403066     	movt	r3, #0x66
    3ab8: e3a015fe     	mov	r1, #1065353216
    3abc: e0870003     	add	r0, r7, r3
    3ac0: e087a10a     	add	r10, r7, r10, lsl #2
    3ac4: e0808105     	add	r8, r0, r5, lsl #2
    3ac8: e048900a     	sub	r9, r8, r10
    3acc: e249b004     	sub	r11, r9, #4
    3ad0: e1a0c12b     	lsr	r12, r11, #2
    3ad4: e28c2001     	add	r2, r12, #1
    3ad8: e2124007     	ands	r4, r2, #7
    3adc: 0a000013     	beq	0x3b30 <fftwObject_tilde_attackShape+0x250> @ imm = #0x4c
    3ae0: e3540001     	cmp	r4, #1
    3ae4: 0a00000e     	beq	0x3b24 <fftwObject_tilde_attackShape+0x244> @ imm = #0x38
    3ae8: e3540002     	cmp	r4, #2
    3aec: 0a00000b     	beq	0x3b20 <fftwObject_tilde_attackShape+0x240> @ imm = #0x2c
    3af0: e3540003     	cmp	r4, #3
    3af4: 0a000008     	beq	0x3b1c <fftwObject_tilde_attackShape+0x23c> @ imm = #0x20
    3af8: e3540004     	cmp	r4, #4
    3afc: 0a000005     	beq	0x3b18 <fftwObject_tilde_attackShape+0x238> @ imm = #0x14
    3b00: e3540005     	cmp	r4, #5
    3b04: 0a000002     	beq	0x3b14 <fftwObject_tilde_attackShape+0x234> @ imm = #0x8
    3b08: e3540006     	cmp	r4, #6
    3b0c: 1a000082     	bne	0x3d1c <fftwObject_tilde_attackShape+0x43c> @ imm = #0x208
    3b10: e48a1004     	str	r1, [r10], #4
    3b14: e48a1004     	str	r1, [r10], #4
    3b18: e48a1004     	str	r1, [r10], #4
    3b1c: e48a1004     	str	r1, [r10], #4
    3b20: e48a1004     	str	r1, [r10], #4
    3b24: e48a1004     	str	r1, [r10], #4
    3b28: e158000a     	cmp	r8, r10
    3b2c: 0a00000b     	beq	0x3b60 <fftwObject_tilde_attackShape+0x280> @ imm = #0x2c
    3b30: e1a0e00a     	mov	lr, r10
    3b34: e28aa020     	add	r10, r10, #32
    3b38: e48e1004     	str	r1, [lr], #4
    3b3c: e50a101c     	str	r1, [r10, #-0x1c]
    3b40: e58e1004     	str	r1, [lr, #0x4]
    3b44: e50a1014     	str	r1, [r10, #-0x14]
    3b48: e50a1010     	str	r1, [r10, #-0x10]
    3b4c: e50a100c     	str	r1, [r10, #-0xc]
    3b50: e50a1008     	str	r1, [r10, #-0x8]
    3b54: e50a1004     	str	r1, [r10, #-0x4]
    3b58: e158000a     	cmp	r8, r10
    3b5c: 1afffff3     	bne	0x3b30 <fftwObject_tilde_attackShape+0x250> @ imm = #-0x34
    3b60: e3560000     	cmp	r6, #0
    3b64: da000062     	ble	0x3cf4 <fftwObject_tilde_attackShape+0x414> @ imm = #0x188
    3b68: eef78a00     	vmov.f32	s17, #1.000000e+00
    3b6c: e3081347     	movw	r1, #0x8347
    3b70: ee086a10     	vmov	s16, r6
    3b74: e3401019     	movt	r1, #0x19
    3b78: e0850001     	add	r0, r5, r1
    3b7c: e2163003     	ands	r3, r6, #3
    3b80: e3a05000     	mov	r5, #0
    3b84: e0874100     	add	r4, r7, r0, lsl #2
    3b88: eeb89ac8     	vcvt.f32.s32	s18, s16
    3b8c: eef80bc8     	vcvt.f64.s32	d16, s16
    3b90: ee88ba89     	vdiv.f32	s22, s17, s18
    3b94: eeb78aca     	vcvt.f64.f32	d8, s20
    3b98: ee809ba0     	vdiv.f64	d9, d16, d16
    3b9c: eeb7ab00     	vmov.f64	d10, #1.000000e+00
    3ba0: 0a000023     	beq	0x3c34 <fftwObject_tilde_attackShape+0x354> @ imm = #0x8c
    3ba4: e3530001     	cmp	r3, #1
    3ba8: 0a000014     	beq	0x3c00 <fftwObject_tilde_attackShape+0x320> @ imm = #0x50
    3bac: e3530002     	cmp	r3, #2
    3bb0: 0a000007     	beq	0x3bd4 <fftwObject_tilde_attackShape+0x2f4> @ imm = #0x1c
    3bb4: ed9f0b5d     	vldr	d0, [pc, #372]          @ 0x3d30 <fftwObject_tilde_attackShape+0x450>  // f64=0
    3bb8: e3a05001     	mov	r5, #1
    3bbc: eeb01b48     	vmov.f64	d1, d8
    3bc0: ebfff2cf     	bl	0x704 <.plt+0xec>       @ imm = #-0x34c4  // CALL __pow_finite
    3bc4: ee7a1b40     	vsub.f64	d17, d10, d0
    3bc8: ee290b21     	vmul.f64	d0, d9, d17
    3bcc: eeb71bc0     	vcvt.f32.f64	s2, d0
    3bd0: eca41a01     	vstmia	r4!, {s2}
    3bd4: ee045a90     	vmov	s9, r5
    3bd8: e2855001     	add	r5, r5, #1
    3bdc: eeb01b48     	vmov.f64	d1, d8
    3be0: eeb85ae4     	vcvt.f32.s32	s10, s9
    3be4: ee655a0b     	vmul.f32	s11, s10, s22
    3be8: eeb70ae5     	vcvt.f64.f32	d0, s11
    3bec: ebfff2c4     	bl	0x704 <.plt+0xec>       @ imm = #-0x34f0  // CALL __pow_finite
    3bf0: ee7a2b40     	vsub.f64	d18, d10, d0
    3bf4: ee290b22     	vmul.f64	d0, d9, d18
    3bf8: eef71bc0     	vcvt.f32.f64	s3, d0
    3bfc: ece41a01     	vstmia	r4!, {s3}
    3c00: ee065a10     	vmov	s12, r5
    3c04: e2855001     	add	r5, r5, #1
    3c08: eeb01b48     	vmov.f64	d1, d8
    3c0c: eef86ac6     	vcvt.f32.s32	s13, s12
    3c10: ee66ba8b     	vmul.f32	s23, s13, s22
    3c14: eeb70aeb     	vcvt.f64.f32	d0, s23
    3c18: ebfff2b9     	bl	0x704 <.plt+0xec>       @ imm = #-0x351c  // CALL __pow_finite
    3c1c: e1560005     	cmp	r6, r5
    3c20: ee7a3b40     	vsub.f64	d19, d10, d0
    3c24: ee290b23     	vmul.f64	d0, d9, d19
    3c28: eeb77bc0     	vcvt.f32.f64	s14, d0
    3c2c: eca47a01     	vstmia	r4!, {s14}
    3c30: 0a00002f     	beq	0x3cf4 <fftwObject_tilde_attackShape+0x414> @ imm = #0xbc
    3c34: e2857001     	add	r7, r5, #1
    3c38: e1a08004     	mov	r8, r4
    3c3c: e2859002     	add	r9, r5, #2
    3c40: e285b003     	add	r11, r5, #3
    3c44: ee075a90     	vmov	s15, r5
    3c48: e2855004     	add	r5, r5, #4
    3c4c: eeb01b48     	vmov.f64	d1, d8
    3c50: e2844010     	add	r4, r4, #16
    3c54: eeb82ae7     	vcvt.f32.s32	s4, s15
    3c58: ee622a0b     	vmul.f32	s5, s4, s22
    3c5c: eeb70ae2     	vcvt.f64.f32	d0, s5
    3c60: ebfff2a7     	bl	0x704 <.plt+0xec>       @ imm = #-0x3564  // CALL __pow_finite
    3c64: ee037a10     	vmov	s6, r7
    3c68: eef83ac3     	vcvt.f32.s32	s7, s6
    3c6c: ee234a8b     	vmul.f32	s8, s7, s22
    3c70: eeb01b48     	vmov.f64	d1, d8
    3c74: ee7a4b40     	vsub.f64	d20, d10, d0
    3c78: ee695b24     	vmul.f64	d21, d9, d20
    3c7c: eeb70ac4     	vcvt.f64.f32	d0, s8
    3c80: eef74be5     	vcvt.f32.f64	s9, d21
    3c84: ece84a01     	vstmia	r8!, {s9}
    3c88: ebfff29d     	bl	0x704 <.plt+0xec>       @ imm = #-0x358c  // CALL __pow_finite
    3c8c: ee019a10     	vmov	s2, r9
    3c90: eeb85ac1     	vcvt.f32.s32	s10, s2
    3c94: ee655a0b     	vmul.f32	s11, s10, s22
    3c98: eeb01b48     	vmov.f64	d1, d8
    3c9c: ee7a6b40     	vsub.f64	d22, d10, d0
    3ca0: ee697b26     	vmul.f64	d23, d9, d22
    3ca4: eeb70ae5     	vcvt.f64.f32	d0, s11
    3ca8: eeb76be7     	vcvt.f32.f64	s12, d23
    3cac: ed046a03     	vstr	s12, [r4, #-12]
    3cb0: ebfff293     	bl	0x704 <.plt+0xec>       @ imm = #-0x35b4  // CALL __pow_finite
    3cb4: ee01ba90     	vmov	s3, r11
    3cb8: eef86ae1     	vcvt.f32.s32	s13, s3
    3cbc: ee66ba8b     	vmul.f32	s23, s13, s22
    3cc0: eeb01b48     	vmov.f64	d1, d8
    3cc4: ee7a8b40     	vsub.f64	d24, d10, d0
    3cc8: ee699b28     	vmul.f64	d25, d9, d24
    3ccc: eeb70aeb     	vcvt.f64.f32	d0, s23
    3cd0: eeb77be9     	vcvt.f32.f64	s14, d25
    3cd4: ed887a01     	vstr	s14, [r8, #4]
    3cd8: ebfff289     	bl	0x704 <.plt+0xec>       @ imm = #-0x35dc  // CALL __pow_finite
    3cdc: e1560005     	cmp	r6, r5
    3ce0: ee7aab40     	vsub.f64	d26, d10, d0
    3ce4: ee290b2a     	vmul.f64	d0, d9, d26
    3ce8: eef70bc0     	vcvt.f32.f64	s1, d0
    3cec: ed440a01     	vstr	s1, [r4, #-4]
    3cf0: 1affffcf     	bne	0x3c34 <fftwObject_tilde_attackShape+0x354> @ imm = #-0xc4
    3cf4: ecbd8b08     	vpop	{d8, d9, d10, d11}
    3cf8: e8bd8ff8     	pop	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    3cfc: eeb72a00     	vmov.f32	s4, #1.000000e+00
    3d00: eddf1a0c     	vldr	s3, [pc, #48]           @ 0x3d38 <fftwObject_tilde_attackShape+0x458>  // f32=0.200000003
    3d04: eddf0a0c     	vldr	s1, [pc, #48]           @ 0x3d3c <fftwObject_tilde_attackShape+0x45c>  // f32=0
    3d08: ebfff262     	bl	0x698 <.plt+0x80>       @ imm = #-0x3678  // CALL map
    3d0c: e2872866     	add	r2, r7, #6684672
    3d10: e2823a01     	add	r3, r2, #4096
    3d14: ed830a12     	vstr	s0, [r3, #72]
    3d18: eafffeff     	b	0x391c <fftwObject_tilde_attackShape+0x3c> @ imm = #-0x404
    3d1c: e48a1004     	str	r1, [r10], #4
    3d20: eaffff7a     	b	0x3b10 <fftwObject_tilde_attackShape+0x230> @ imm = #-0x218
    3d24: e3a0a000     	mov	r10, #0
    3d28: eaffff5b     	b	0x3a9c <fftwObject_tilde_attackShape+0x1bc> @ imm = #-0x294
    3d2c: e320f000     	nop
    3d30: 00 00 00 00  	.word	0x00000000
    3d34: 00 00 00 00  	.word	0x00000000
    3d38: cd cc 4c 3e  	.word	0x3e4ccccd
    3d3c: 00 00 00 00  	.word	0x00000000

