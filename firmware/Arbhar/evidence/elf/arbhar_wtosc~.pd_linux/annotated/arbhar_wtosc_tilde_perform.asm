0000378c <arbhar_wtosc_tilde_perform>:
    378c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    3790: e1a08000     	mov	r8, r0
    3794: e5909004     	ldr	r9, [r0, #0x4]
    3798: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
    379c: e5995080     	ldr	r5, [r9, #0x80]
    37a0: e599b02c     	ldr	r11, [r9, #0x2c]
    37a4: e24dd00c     	sub	sp, sp, #12
    37a8: e35b0000     	cmp	r11, #0
    37ac: 13550000     	cmpne	r5, #0
    37b0: 0a0000c3     	beq	0x3ac4 <arbhar_wtosc_tilde_perform+0x338> @ imm = #0x30c
    37b4: ed995a1e     	vldr	s10, [r9, #120]
    37b8: e590e010     	ldr	lr, [r0, #0x10]
    37bc: e5997028     	ldr	r7, [r9, #0x28]
    37c0: eddf2bd0     	vldr	d18, [pc, #832]         @ 0x3b08 <arbhar_wtosc_tilde_perform+0x37c>  // f64=1572864
    37c4: e35e0000     	cmp	lr, #0
    37c8: e599c050     	ldr	r12, [r9, #0x50]
    37cc: e24ee001     	sub	lr, lr, #1
    37d0: eef75ac5     	vcvt.f64.f32	d21, s10
    37d4: edd90b22     	vldr	d16, [r9, #136]
    37d8: ed998a12     	vldr	s16, [r9, #72]
    37dc: eef0bb62     	vmov.f64	d27, d18
    37e0: edd9ca16     	vldr	s25, [r9, #88]
    37e4: ee45bba0     	vmla.f64	d27, d21, d16
    37e8: ed99ba17     	vldr	s22, [r9, #92]
    37ec: edd9ba18     	vldr	s23, [r9, #96]
    37f0: edd99a1b     	vldr	s19, [r9, #108]
    37f4: edd9fa1a     	vldr	s31, [r9, #104]
    37f8: edcdbb00     	vstr	d27, [sp]
    37fc: 0a000033     	beq	0x38d0 <arbhar_wtosc_tilde_perform+0x144> @ imm = #0xcc
    3800: e5900008     	ldr	r0, [r0, #0x8]
    3804: e598600c     	ldr	r6, [r8, #0xc]
    3808: eef77a00     	vmov.f32	s15, #1.000000e+00
    380c: ed997a24     	vldr	s14, [r9, #144]
    3810: e046a000     	sub	r10, r6, r0
    3814: eddf4bbd     	vldr	d20, [pc, #756]         @ 0x3b10 <arbhar_wtosc_tilde_perform+0x384>  // f64=0.16666670143604279
    3818: ee350a67     	vsub.f32	s0, s10, s15
    381c: eebd1ac0     	vcvt.s32.f32	s2, s0
    3820: ee114a10     	vmov	r4, s2
    3824: ee650a07     	vmul.f32	s1, s10, s14
    3828: eef05a08     	vmov.f32	s11, #3.000000e+00
    382c: eef73b00     	vmov.f64	d19, #1.000000e+00
    3830: e59d1004     	ldr	r1, [sp, #0x4]
    3834: e24ee001     	sub	lr, lr, #1
    3838: e1cd20d0     	ldrd	r2, r3, [sp]
    383c: e3a03000     	mov	r3, #0
    3840: e0011004     	and	r1, r1, r4
    3844: e3443138     	movt	r3, #0x4138
    3848: edddab00     	vldr	d26, [sp]
    384c: e37e0001     	cmn	lr, #1
    3850: e0856101     	add	r6, r5, r1, lsl #2
    3854: e080100a     	add	r1, r0, r10
    3858: ed962a01     	vldr	s4, [r6, #4]
    385c: edd66a00     	vldr	s13, [r6]
    3860: edd61a03     	vldr	s3, [r6, #12]
    3864: edd64a02     	vldr	s9, [r6, #8]
    3868: ee763ae1     	vsub.f32	s7, s13, s3
    386c: ecb06a01     	vldmia	r0!, {s12}
    3870: ee742ac2     	vsub.f32	s5, s9, s4
    3874: ec432b14     	vmov	d4, r2, r3
    3878: ee521a25     	vnmls.f32	s3, s4, s11
    387c: ee34ab62     	vsub.f64	d10, d4, d18
    3880: ee423aa5     	vmla.f32	s7, s5, s11
    3884: ee363aa6     	vadd.f32	s6, s13, s13
    3888: eef78bca     	vcvt.f32.f64	s17, d10
    388c: ee319ac3     	vsub.f32	s18, s3, s6
    3890: eef77ae8     	vcvt.f64.f32	d23, s17
    3894: ee039aa8     	vmla.f32	s18, s7, s17
    3898: ee736be7     	vsub.f64	d22, d19, d23
    389c: ee26dba4     	vmul.f64	d13, d22, d20
    38a0: eeb7eac9     	vcvt.f64.f32	d14, s18
    38a4: eef71ae2     	vcvt.f64.f32	d17, s5
    38a8: ee4e1b0d     	vmla.f64	d17, d14, d13
    38ac: eef78ac2     	vcvt.f64.f32	d24, s4
    38b0: ee60aa86     	vmul.f32	s21, s1, s12
    38b4: ee418ba7     	vmla.f64	d24, d17, d23
    38b8: eef79aea     	vcvt.f64.f32	d25, s21
    38bc: ee79bbaa     	vadd.f64	d27, d25, d26
    38c0: eeb7cbe8     	vcvt.f32.f64	s24, d24
    38c4: edcdbb00     	vstr	d27, [sp]
    38c8: ed81ca00     	vstr	s24, [r1]
    38cc: 1affffd7     	bne	0x3830 <arbhar_wtosc_tilde_perform+0xa4> @ imm = #-0xa4
    38d0: e3570000     	cmp	r7, #0
    38d4: eef7cb00     	vmov.f64	d28, #1.000000e+00
    38d8: eddfdb8a     	vldr	d29, [pc, #552]         @ 0x3b08 <arbhar_wtosc_tilde_perform+0x37c>  // f64=1572864
    38dc: edd9da1f     	vldr	s27, [r9, #124]
    38e0: ee75ebec     	vsub.f64	d30, d21, d28
    38e4: ee4ebbad     	vmla.f64	d27, d30, d29
    38e8: ee257bad     	vmul.f64	d7, d21, d29
    38ec: edcdbb00     	vstr	d27, [sp]
    38f0: eef7faed     	vcvt.f64.f32	d31, s27
    38f4: edcd7a01     	vstr	s15, [sp, #4]
    38f8: ed9d5b00     	vldr	d5, [sp]
    38fc: ee750b47     	vsub.f64	d16, d5, d7
    3900: ee605baf     	vmul.f64	d21, d16, d31
    3904: edc95b22     	vstr	d21, [r9, #136]
    3908: da000069     	ble	0x3ab4 <arbhar_wtosc_tilde_perform+0x328> @ imm = #0x1a4
    390c: e5992054     	ldr	r2, [r9, #0x54]
    3910: e3520000     	cmp	r2, #0
    3914: da000066     	ble	0x3ab4 <arbhar_wtosc_tilde_perform+0x328> @ imm = #0x198
    3918: eef7ea00     	vmov.f32	s29, #1.000000e+00
    391c: e59f720c     	ldr	r7, [pc, #0x20c]        @ 0x3b30 <arbhar_wtosc_tilde_perform+0x3a4>  // u32=0x32fc; f32?=1.82897476e-41
    3920: e59f520c     	ldr	r5, [pc, #0x20c]        @ 0x3b34 <arbhar_wtosc_tilde_perform+0x3a8>  // u32=0x32f4; f32?=1.82785372e-41
    3924: ee00ca90     	vmov	s1, r12
    3928: eddf2b7a     	vldr	d18, [pc, #488]         @ 0x3b18 <arbhar_wtosc_tilde_perform+0x38c>  // f64=0.90000000000000002
    392c: e08f0007     	add	r0, pc, r7
    3930: e59f4200     	ldr	r4, [pc, #0x200]        @ 0x3b38 <arbhar_wtosc_tilde_perform+0x3ac>  // u32=0x52dc; f32?=2.9724343e-41
    3934: e08f5005     	add	r5, pc, r5
    3938: e285ab02     	add	r10, r5, #2048
    393c: e280eb02     	add	lr, r0, #2048
    3940: ed9ffa78     	vldr	s30, [pc, #480]         @ 0x3b28 <arbhar_wtosc_tilde_perform+0x39c>  // f32=200
    3944: e08f7004     	add	r7, pc, r4
    3948: e28e600c     	add	r6, lr, #12
    394c: e28a400c     	add	r4, r10, #12
    3950: e3a0a000     	mov	r10, #0
    3954: edd92a09     	vldr	s5, [r9, #36]
    3958: eeb17a48     	vneg.f32	s14, s16
    395c: ee38ea2e     	vadd.f32	s28, s16, s29
    3960: ee3e0ac8     	vsub.f32	s0, s29, s16
    3964: eef74ac7     	vcvt.f64.f32	d20, s14
    3968: eef08a6e     	vmov.f32	s17, s29
    396c: ee24aba2     	vmul.f64	d10, d20, d18
    3970: ee20da0e     	vmul.f32	s26, s0, s28
    3974: ee68da0f     	vmul.f32	s27, s16, s30
    3978: ee3ecae9     	vsub.f32	s24, s29, s19
    397c: eeb89ae0     	vcvt.f32.s32	s18, s1
    3980: ea000011     	b	0x39cc <arbhar_wtosc_tilde_perform+0x240> @ imm = #0x44
    3984: eef01ae0     	vabs.f32	s3, s1
    3988: ee212aac     	vmul.f32	s4, s3, s25
    398c: eeb41ac2     	vcmpe.f32	s2, s4
    3990: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3994: 8ef04a42     	vmovhi.f32	s9, s4
    3998: 9ef04a41     	vmovls.f32	s9, s2
    399c: ee7f6a64     	vsub.f32	s13, s30, s9
    39a0: edd93a09     	vldr	s7, [r9, #36]
    39a4: e28aa001     	add	r10, r10, #1
    39a8: e35a00c3     	cmp	r10, #195
    39ac: edd9ea13     	vldr	s29, [r9, #76]
    39b0: ee266aa9     	vmul.f32	s12, s13, s19
    39b4: ee0c6a2f     	vmla.f32	s12, s24, s31
    39b8: ee732aae     	vadd.f32	s5, s7, s29
    39bc: ecab6a01     	vstmia	r11!, {s12}
    39c0: eef0fa46     	vmov.f32	s31, s12
    39c4: edc92a09     	vstr	s5, [r9, #36]
    39c8: 0a000035     	beq	0x3aa4 <arbhar_wtosc_tilde_perform+0x318> @ imm = #0xd4
    39cc: e300c203     	movw	r12, #0x203
    39d0: eefd7ae2     	vcvt.s32.f32	s15, s5
    39d4: ee171a90     	vmov	r1, s15
    39d8: eeb58ac0     	vcmpe.f32	s16, #0
    39dc: e151000c     	cmp	r1, r12
    39e0: a1a0100c     	movge	r1, r12
    39e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    39e8: 9a000039     	bls	0x3ad4 <arbhar_wtosc_tilde_perform+0x348> @ imm = #0xe4
    39ec: ee051a90     	vmov	s11, r1
    39f0: e0853101     	add	r3, r5, r1, lsl #2
    39f4: eeb81ae5     	vcvt.f32.s32	s2, s11
    39f8: edd36a00     	vldr	s13, [r3]
    39fc: ee711a2d     	vadd.f32	s3, s2, s27
    3a00: eebd2ae1     	vcvt.s32.f32	s4, s3
    3a04: ee122a10     	vmov	r2, s4
    3a08: ee684a26     	vmul.f32	s9, s16, s13
    3a0c: e152000c     	cmp	r2, r12
    3a10: a1a0200c     	movge	r2, r12
    3a14: e0840102     	add	r0, r4, r2, lsl #2
    3a18: edd03a00     	vldr	s7, [r0]
    3a1c: ee4d4a23     	vmla.f32	s9, s26, s7
    3a20: eef44ae8     	vcmpe.f32	s9, s17
    3a24: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3a28: 8ef0ea68     	vmovhi.f32	s29, s17
    3a2c: 9ef0ea64     	vmovls.f32	s29, s9
    3a30: e5992038     	ldr	r2, [r9, #0x38]
    3a34: e1a00009     	mov	r0, r9
    3a38: ed9bfa00     	vldr	s30, [r11]
    3a3c: e08ae002     	add	lr, r10, r2
    3a40: ee05ea10     	vmov	s10, lr
    3a44: eeb00a49     	vmov.f32	s0, s18
    3a48: eef80ac5     	vcvt.f32.s32	s1, s10
    3a4c: ebfffa92     	bl	0x249c <.plt+0x1b8>     @ imm = #-0x15b8  // CALL _memRead
    3a50: ee207a2e     	vmul.f32	s14, s0, s29
    3a54: ee676a2b     	vmul.f32	s13, s14, s23
    3a58: ee160a90     	vmov	r0, s13
    3a5c: e020c0a0     	eor	r12, r0, r0, lsr #1
    3a60: e31c0202     	tst	r12, #536870912
    3a64: 0ddf6a30     	vldreq	s13, [pc, #192]         @ 0x3b2c <arbhar_wtosc_tilde_perform+0x3a0>
    3a68: 1e7f0a66     	vsubne.f32	s1, s30, s13
    3a6c: ee767acf     	vsub.f32	s15, s13, s30
    3a70: eef46acf     	vcmpe.f32	s13, s30
    3a74: eef05ae7     	vabs.f32	s11, s15
    3a78: 0ef00a4f     	vmoveq.f32	s1, s30
    3a7c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3a80: ee251a8b     	vmul.f32	s2, s11, s22
    3a84: 4affffbe     	bmi	0x3984 <arbhar_wtosc_tilde_perform+0x1f8> @ imm = #-0x108
    3a88: daffffc4     	ble	0x39a0 <arbhar_wtosc_tilde_perform+0x214> @ imm = #-0xf0
    3a8c: ee250aac     	vmul.f32	s0, s11, s25
    3a90: eeb41ac0     	vcmpe.f32	s2, s0
    3a94: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3a98: 9eb00a41     	vmovls.f32	s0, s2
    3a9c: ee706a0f     	vadd.f32	s13, s0, s30
    3aa0: eaffffbe     	b	0x39a0 <arbhar_wtosc_tilde_perform+0x214> @ imm = #-0x108
    3aa4: e599503c     	ldr	r5, [r9, #0x3c]
    3aa8: e3a0b000     	mov	r11, #0
    3aac: e589b024     	str	r11, [r9, #0x24]
    3ab0: e5895038     	str	r5, [r9, #0x38]
    3ab4: e5990074     	ldr	r0, [r9, #0x74]
    3ab8: edc9fa1a     	vstr	s31, [r9, #104]
    3abc: ed9f0b17     	vldr	d0, [pc, #92]           @ 0x3b20 <arbhar_wtosc_tilde_perform+0x394>  // f64=0
    3ac0: ebfffa4e     	bl	0x2400 <.plt+0x11c>     @ imm = #-0x16c8  // CALL clock_delay
    3ac4: e2880014     	add	r0, r8, #20
    3ac8: e28dd00c     	add	sp, sp, #12
    3acc: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
    3ad0: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    3ad4: e1a0e101     	lsl	lr, r1, #2
    3ad8: e247cefe     	sub	r12, r7, #4064
    3adc: e086100e     	add	r1, r6, lr
    3ae0: e08c300e     	add	r3, r12, lr
    3ae4: ed916a00     	vldr	s12, [r1]
    3ae8: edd32a00     	vldr	s5, [r3]
    3aec: ee2e4a06     	vmul.f32	s8, s28, s12
    3af0: eef73ae2     	vcvt.f64.f32	d19, s5
    3af4: eeb73ac4     	vcvt.f64.f32	d3, s8
    3af8: ee033b8a     	vmla.f64	d3, d19, d10
    3afc: eef7ebc3     	vcvt.f32.f64	s29, d3
    3b00: eaffffca     	b	0x3a30 <arbhar_wtosc_tilde_perform+0x2a4> @ imm = #-0xd8
    3b04: e320f000     	nop
    3b08: 00 00 00 00  	.word	0x00000000
    3b0c: 00 00 38 41  	.word	0x41380000
    3b10: 00 00 00 a0  	.word	0xa0000000
    3b14: 55 55 c5 3f  	.word	0x3fc55555
    3b18: cd cc cc cc  	.word	0xcccccccd
    3b1c: cc cc ec 3f  	.word	0x3feccccc
    3b20: 00 00 00 00  	.word	0x00000000
    3b24: 00 00 00 00  	.word	0x00000000
    3b28: 00 00 48 43  	.word	0x43480000
    3b2c: 00 00 00 00  	.word	0x00000000
    3b30: fc 32 00 00  	.word	0x000032fc
    3b34: f4 32 00 00  	.word	0x000032f4
    3b38: dc 52 00 00  	.word	0x000052dc

