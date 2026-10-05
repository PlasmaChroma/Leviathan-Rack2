0000348c <fftwObject_tilde_releaseShape>:
    348c: eeb61a00     	vmov.f32	s2, #5.000000e-01
    3490: e92d4ff8     	push	{r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
    3494: e1a07000     	mov	r7, r0
    3498: ed2d8b08     	vpush	{d8, d9, d10, d11}
    349c: eeb40ac1     	vcmpe.f32	s0, s2
    34a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    34a4: 4a000100     	bmi	0x38ac <fftwObject_tilde_releaseShape+0x420> @ imm = #0x400
    34a8: eef71a00     	vmov.f32	s3, #1.000000e+00
    34ac: eeb12a04     	vmov.f32	s4, #5.000000e+00
    34b0: eef00a41     	vmov.f32	s1, s2
    34b4: eeb01a61     	vmov.f32	s2, s3
    34b8: ebfff476     	bl	0x698 <.plt+0x80>       @ imm = #-0x2e28
    34bc: e2872866     	add	r2, r7, #6684672
    34c0: e2823a01     	add	r3, r2, #4096
    34c4: e5925d10     	ldr	r5, [r2, #0xd10]
    34c8: e593003c     	ldr	r0, [r3, #0x3c]
    34cc: e5936040     	ldr	r6, [r3, #0x40]
    34d0: e1500005     	cmp	r0, r5
    34d4: ed939a12     	vldr	s18, [r3, #72]
    34d8: a1a0a005     	movge	r10, r5
    34dc: b1a0a000     	movlt	r10, r0
    34e0: e1550006     	cmp	r5, r6
    34e4: e583a03c     	str	r10, [r3, #0x3c]
    34e8: b1a01005     	movlt	r1, r5
    34ec: a1a01006     	movge	r1, r6
    34f0: e35a0000     	cmp	r10, #0
    34f4: eeb08a40     	vmov.f32	s16, s0
    34f8: e0455001     	sub	r5, r5, r1
    34fc: ed830a13     	vstr	s0, [r3, #76]
    3500: e5831040     	str	r1, [r3, #0x40]
    3504: da0000ee     	ble	0x38c4 <fftwObject_tilde_releaseShape+0x438> @ imm = #0x3b8
    3508: e21a9003     	ands	r9, r10, #3
    350c: e3004d1c     	movw	r4, #0xd1c
    3510: eeb77a00     	vmov.f32	s14, #1.000000e+00
    3514: e3404066     	movt	r4, #0x66
    3518: ee070a90     	vmov	s15, r0
    351c: e0874004     	add	r4, r7, r4
    3520: e3a08000     	mov	r8, #0
    3524: eeb80ae7     	vcvt.f32.s32	s0, s15
    3528: eeb7aac9     	vcvt.f64.f32	d10, s18
    352c: eec78a00     	vdiv.f32	s17, s14, s0
    3530: 0a00001d     	beq	0x35ac <fftwObject_tilde_releaseShape+0x120> @ imm = #0x74
    3534: e3590001     	cmp	r9, #1
    3538: 0a000010     	beq	0x3580 <fftwObject_tilde_releaseShape+0xf4> @ imm = #0x40
    353c: e3590002     	cmp	r9, #2
    3540: 0a000005     	beq	0x355c <fftwObject_tilde_releaseShape+0xd0> @ imm = #0x14
    3544: ed9f0be1     	vldr	d0, [pc, #900]          @ 0x38d0 <fftwObject_tilde_releaseShape+0x444>
    3548: e3a08001     	mov	r8, #1
    354c: eeb01b4a     	vmov.f64	d1, d10
    3550: ebfff46b     	bl	0x704 <.plt+0xec>       @ imm = #-0x2e54
    3554: eef70bc0     	vcvt.f32.f64	s1, d0
    3558: ece40a01     	vstmia	r4!, {s1}
    355c: ee028a10     	vmov	s4, r8
    3560: e2888001     	add	r8, r8, #1
    3564: eeb01b4a     	vmov.f64	d1, d10
    3568: eef82ac2     	vcvt.f32.s32	s5, s4
    356c: ee223aa8     	vmul.f32	s6, s5, s17
    3570: eeb70ac3     	vcvt.f64.f32	d0, s6
    3574: ebfff462     	bl	0x704 <.plt+0xec>       @ imm = #-0x2e78
    3578: eeb71bc0     	vcvt.f32.f64	s2, d0
    357c: eca41a01     	vstmia	r4!, {s2}
    3580: ee018a90     	vmov	s3, r8
    3584: e2888001     	add	r8, r8, #1
    3588: eef83ae1     	vcvt.f32.s32	s7, s3
    358c: ee234aa8     	vmul.f32	s8, s7, s17
    3590: eeb01b4a     	vmov.f64	d1, d10
    3594: eeb70ac4     	vcvt.f64.f32	d0, s8
    3598: ebfff459     	bl	0x704 <.plt+0xec>       @ imm = #-0x2e9c
    359c: e15a0008     	cmp	r10, r8
    35a0: eef74bc0     	vcvt.f32.f64	s9, d0
    35a4: ece44a01     	vstmia	r4!, {s9}
    35a8: 0a000027     	beq	0x364c <fftwObject_tilde_releaseShape+0x1c0> @ imm = #0x9c
    35ac: e2889001     	add	r9, r8, #1
    35b0: e1a0b004     	mov	r11, r4
    35b4: e2844010     	add	r4, r4, #16
    35b8: ee058a10     	vmov	s10, r8
    35bc: eeb01b4a     	vmov.f64	d1, d10
    35c0: eef85ac5     	vcvt.f32.s32	s11, s10
    35c4: ee256aa8     	vmul.f32	s12, s11, s17
    35c8: eeb70ac6     	vcvt.f64.f32	d0, s12
    35cc: ebfff44c     	bl	0x704 <.plt+0xec>       @ imm = #-0x2ed0
    35d0: ee069a90     	vmov	s13, r9
    35d4: eef89ae6     	vcvt.f32.s32	s19, s13
    35d8: ee29baa8     	vmul.f32	s22, s19, s17
    35dc: eeb01b4a     	vmov.f64	d1, d10
    35e0: eef7bbc0     	vcvt.f32.f64	s23, d0
    35e4: eeb70acb     	vcvt.f64.f32	d0, s22
    35e8: ecebba01     	vstmia	r11!, {s23}
    35ec: ebfff444     	bl	0x704 <.plt+0xec>       @ imm = #-0x2ef0
    35f0: e288c002     	add	r12, r8, #2
    35f4: ee09ca10     	vmov	s18, r12
    35f8: eeb87ac9     	vcvt.f32.s32	s14, s18
    35fc: ee677a28     	vmul.f32	s15, s14, s17
    3600: eeb01b4a     	vmov.f64	d1, d10
    3604: eeb72bc0     	vcvt.f32.f64	s4, d0
    3608: eeb70ae7     	vcvt.f64.f32	d0, s15
    360c: ed042a03     	vstr	s4, [r4, #-12]
    3610: ebfff43b     	bl	0x704 <.plt+0xec>       @ imm = #-0x2f14
    3614: e2882003     	add	r2, r8, #3
    3618: e2888004     	add	r8, r8, #4
    361c: ee022a90     	vmov	s5, r2
    3620: eeb83ae2     	vcvt.f32.s32	s6, s5
    3624: ee633a28     	vmul.f32	s7, s6, s17
    3628: eeb01b4a     	vmov.f64	d1, d10
    362c: eeb74bc0     	vcvt.f32.f64	s8, d0
    3630: eeb70ae3     	vcvt.f64.f32	d0, s7
    3634: ed8b4a01     	vstr	s8, [r11, #4]
    3638: ebfff431     	bl	0x704 <.plt+0xec>       @ imm = #-0x2f3c
    363c: e15a0008     	cmp	r10, r8
    3640: eeb70bc0     	vcvt.f32.f64	s0, d0
    3644: ed040a01     	vstr	s0, [r4, #-4]
    3648: 1affffd7     	bne	0x35ac <fftwObject_tilde_releaseShape+0x120> @ imm = #-0xa4
    364c: e155000a     	cmp	r5, r10
    3650: da00002e     	ble	0x3710 <fftwObject_tilde_releaseShape+0x284> @ imm = #0xb8
    3654: e308e347     	movw	lr, #0x8347
    3658: e340e019     	movt	lr, #0x19
    365c: e08aa00e     	add	r10, r10, lr
    3660: e3003d1c     	movw	r3, #0xd1c
    3664: e3403066     	movt	r3, #0x66
    3668: e3a015fe     	mov	r1, #1065353216
    366c: e0870003     	add	r0, r7, r3
    3670: e087a10a     	add	r10, r7, r10, lsl #2
    3674: e0808105     	add	r8, r0, r5, lsl #2
    3678: e048900a     	sub	r9, r8, r10
    367c: e249b004     	sub	r11, r9, #4
    3680: e1a0c12b     	lsr	r12, r11, #2
    3684: e28c2001     	add	r2, r12, #1
    3688: e2124007     	ands	r4, r2, #7
    368c: 0a000013     	beq	0x36e0 <fftwObject_tilde_releaseShape+0x254> @ imm = #0x4c
    3690: e3540001     	cmp	r4, #1
    3694: 0a00000e     	beq	0x36d4 <fftwObject_tilde_releaseShape+0x248> @ imm = #0x38
    3698: e3540002     	cmp	r4, #2
    369c: 0a00000b     	beq	0x36d0 <fftwObject_tilde_releaseShape+0x244> @ imm = #0x2c
    36a0: e3540003     	cmp	r4, #3
    36a4: 0a000008     	beq	0x36cc <fftwObject_tilde_releaseShape+0x240> @ imm = #0x20
    36a8: e3540004     	cmp	r4, #4
    36ac: 0a000005     	beq	0x36c8 <fftwObject_tilde_releaseShape+0x23c> @ imm = #0x14
    36b0: e3540005     	cmp	r4, #5
    36b4: 0a000002     	beq	0x36c4 <fftwObject_tilde_releaseShape+0x238> @ imm = #0x8
    36b8: e3540006     	cmp	r4, #6
    36bc: 1a00007e     	bne	0x38bc <fftwObject_tilde_releaseShape+0x430> @ imm = #0x1f8
    36c0: e48a1004     	str	r1, [r10], #4
    36c4: e48a1004     	str	r1, [r10], #4
    36c8: e48a1004     	str	r1, [r10], #4
    36cc: e48a1004     	str	r1, [r10], #4
    36d0: e48a1004     	str	r1, [r10], #4
    36d4: e48a1004     	str	r1, [r10], #4
    36d8: e158000a     	cmp	r8, r10
    36dc: 0a00000b     	beq	0x3710 <fftwObject_tilde_releaseShape+0x284> @ imm = #0x2c
    36e0: e1a0e00a     	mov	lr, r10
    36e4: e28aa020     	add	r10, r10, #32
    36e8: e48e1004     	str	r1, [lr], #4
    36ec: e50a101c     	str	r1, [r10, #-0x1c]
    36f0: e58e1004     	str	r1, [lr, #0x4]
    36f4: e50a1014     	str	r1, [r10, #-0x14]
    36f8: e50a1010     	str	r1, [r10, #-0x10]
    36fc: e50a100c     	str	r1, [r10, #-0xc]
    3700: e50a1008     	str	r1, [r10, #-0x8]
    3704: e50a1004     	str	r1, [r10, #-0x4]
    3708: e158000a     	cmp	r8, r10
    370c: 1afffff3     	bne	0x36e0 <fftwObject_tilde_releaseShape+0x254> @ imm = #-0x34
    3710: e3560000     	cmp	r6, #0
    3714: da000062     	ble	0x38a4 <fftwObject_tilde_releaseShape+0x418> @ imm = #0x188
    3718: eef7aa00     	vmov.f32	s21, #1.000000e+00
    371c: e3081347     	movw	r1, #0x8347
    3720: ee0a6a10     	vmov	s20, r6
    3724: e3401019     	movt	r1, #0x19
    3728: e0850001     	add	r0, r5, r1
    372c: e2163003     	ands	r3, r6, #3
    3730: e3a05000     	mov	r5, #0
    3734: e0874100     	add	r4, r7, r0, lsl #2
    3738: eef88aca     	vcvt.f32.s32	s17, s20
    373c: eef80bca     	vcvt.f64.s32	d16, s20
    3740: ee8abaa8     	vdiv.f32	s22, s21, s17
    3744: ee809ba0     	vdiv.f64	d9, d16, d16
    3748: eeb78ac8     	vcvt.f64.f32	d8, s16
    374c: eeb7ab00     	vmov.f64	d10, #1.000000e+00
    3750: 0a000023     	beq	0x37e4 <fftwObject_tilde_releaseShape+0x358> @ imm = #0x8c
    3754: e3530001     	cmp	r3, #1
    3758: 0a000014     	beq	0x37b0 <fftwObject_tilde_releaseShape+0x324> @ imm = #0x50
    375c: e3530002     	cmp	r3, #2
    3760: 0a000007     	beq	0x3784 <fftwObject_tilde_releaseShape+0x2f8> @ imm = #0x1c
    3764: ed9f0b59     	vldr	d0, [pc, #356]          @ 0x38d0 <fftwObject_tilde_releaseShape+0x444>
    3768: e3a05001     	mov	r5, #1
    376c: eeb01b48     	vmov.f64	d1, d8
    3770: ebfff3e3     	bl	0x704 <.plt+0xec>       @ imm = #-0x3074
    3774: ee7a1b40     	vsub.f64	d17, d10, d0
    3778: ee290b21     	vmul.f64	d0, d9, d17
    377c: eeb71bc0     	vcvt.f32.f64	s2, d0
    3780: eca41a01     	vstmia	r4!, {s2}
    3784: ee045a90     	vmov	s9, r5
    3788: e2855001     	add	r5, r5, #1
    378c: eeb01b48     	vmov.f64	d1, d8
    3790: eeb85ae4     	vcvt.f32.s32	s10, s9
    3794: ee655a0b     	vmul.f32	s11, s10, s22
    3798: eeb70ae5     	vcvt.f64.f32	d0, s11
    379c: ebfff3d8     	bl	0x704 <.plt+0xec>       @ imm = #-0x30a0
    37a0: ee7a2b40     	vsub.f64	d18, d10, d0
    37a4: ee290b22     	vmul.f64	d0, d9, d18
    37a8: eef71bc0     	vcvt.f32.f64	s3, d0
    37ac: ece41a01     	vstmia	r4!, {s3}
    37b0: ee065a10     	vmov	s12, r5
    37b4: e2855001     	add	r5, r5, #1
    37b8: eeb01b48     	vmov.f64	d1, d8
    37bc: eef86ac6     	vcvt.f32.s32	s13, s12
    37c0: ee66ba8b     	vmul.f32	s23, s13, s22
    37c4: eeb70aeb     	vcvt.f64.f32	d0, s23
    37c8: ebfff3cd     	bl	0x704 <.plt+0xec>       @ imm = #-0x30cc
    37cc: e1560005     	cmp	r6, r5
    37d0: ee7a3b40     	vsub.f64	d19, d10, d0
    37d4: ee290b23     	vmul.f64	d0, d9, d19
    37d8: eeb77bc0     	vcvt.f32.f64	s14, d0
    37dc: eca47a01     	vstmia	r4!, {s14}
    37e0: 0a00002f     	beq	0x38a4 <fftwObject_tilde_releaseShape+0x418> @ imm = #0xbc
    37e4: e2857001     	add	r7, r5, #1
    37e8: e1a08004     	mov	r8, r4
    37ec: e2859002     	add	r9, r5, #2
    37f0: e285b003     	add	r11, r5, #3
    37f4: ee075a90     	vmov	s15, r5
    37f8: e2855004     	add	r5, r5, #4
    37fc: eeb01b48     	vmov.f64	d1, d8
    3800: e2844010     	add	r4, r4, #16
    3804: eeb82ae7     	vcvt.f32.s32	s4, s15
    3808: ee622a0b     	vmul.f32	s5, s4, s22
    380c: eeb70ae2     	vcvt.f64.f32	d0, s5
    3810: ebfff3bb     	bl	0x704 <.plt+0xec>       @ imm = #-0x3114
    3814: ee037a10     	vmov	s6, r7
    3818: eef83ac3     	vcvt.f32.s32	s7, s6
    381c: ee234a8b     	vmul.f32	s8, s7, s22
    3820: eeb01b48     	vmov.f64	d1, d8
    3824: ee7a4b40     	vsub.f64	d20, d10, d0
    3828: ee695b24     	vmul.f64	d21, d9, d20
    382c: eeb70ac4     	vcvt.f64.f32	d0, s8
    3830: eef74be5     	vcvt.f32.f64	s9, d21
    3834: ece84a01     	vstmia	r8!, {s9}
    3838: ebfff3b1     	bl	0x704 <.plt+0xec>       @ imm = #-0x313c
    383c: ee019a10     	vmov	s2, r9
    3840: eeb85ac1     	vcvt.f32.s32	s10, s2
    3844: ee655a0b     	vmul.f32	s11, s10, s22
    3848: eeb01b48     	vmov.f64	d1, d8
    384c: ee7a6b40     	vsub.f64	d22, d10, d0
    3850: ee697b26     	vmul.f64	d23, d9, d22
    3854: eeb70ae5     	vcvt.f64.f32	d0, s11
    3858: eeb76be7     	vcvt.f32.f64	s12, d23
    385c: ed046a03     	vstr	s12, [r4, #-12]
    3860: ebfff3a7     	bl	0x704 <.plt+0xec>       @ imm = #-0x3164
    3864: ee01ba90     	vmov	s3, r11
    3868: eef86ae1     	vcvt.f32.s32	s13, s3
    386c: ee66ba8b     	vmul.f32	s23, s13, s22
    3870: eeb01b48     	vmov.f64	d1, d8
    3874: ee7a8b40     	vsub.f64	d24, d10, d0
    3878: ee699b28     	vmul.f64	d25, d9, d24
    387c: eeb70aeb     	vcvt.f64.f32	d0, s23
    3880: eeb77be9     	vcvt.f32.f64	s14, d25
    3884: ed887a01     	vstr	s14, [r8, #4]
    3888: ebfff39d     	bl	0x704 <.plt+0xec>       @ imm = #-0x318c
    388c: e1560005     	cmp	r6, r5
    3890: ee7aab40     	vsub.f64	d26, d10, d0
    3894: ee290b2a     	vmul.f64	d0, d9, d26
    3898: eeb70bc0     	vcvt.f32.f64	s0, d0
    389c: ed040a01     	vstr	s0, [r4, #-4]
    38a0: 1affffcf     	bne	0x37e4 <fftwObject_tilde_releaseShape+0x358> @ imm = #-0xc4
    38a4: ecbd8b08     	vpop	{d8, d9, d10, d11}
    38a8: e8bd8ff8     	pop	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
    38ac: eeb72a00     	vmov.f32	s4, #1.000000e+00
    38b0: eddf1a08     	vldr	s3, [pc, #32]           @ 0x38d8 <fftwObject_tilde_releaseShape+0x44c>
    38b4: eddf0a08     	vldr	s1, [pc, #32]           @ 0x38dc <fftwObject_tilde_releaseShape+0x450>
    38b8: eafffefe     	b	0x34b8 <fftwObject_tilde_releaseShape+0x2c> @ imm = #-0x408
    38bc: e48a1004     	str	r1, [r10], #4
    38c0: eaffff7e     	b	0x36c0 <fftwObject_tilde_releaseShape+0x234> @ imm = #-0x208
    38c4: e3a0a000     	mov	r10, #0
    38c8: eaffff5f     	b	0x364c <fftwObject_tilde_releaseShape+0x1c0> @ imm = #-0x284
    38cc: e320f000     	nop
    38d0: 00 00 00 00  	.word	0x00000000
    38d4: 00 00 00 00  	.word	0x00000000
    38d8: cd cc 4c 3e  	.word	0x3e4ccccd
    38dc: 00 00 00 00  	.word	0x00000000

