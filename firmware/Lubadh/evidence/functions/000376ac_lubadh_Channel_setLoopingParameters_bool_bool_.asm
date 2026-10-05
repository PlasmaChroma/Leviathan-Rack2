; lubadh::Channel::setLoopingParameters(bool, bool)
; VA 0x376ac size 672

   376ac: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   376b0: e1a04000     	mov	r4, r0
   376b4: e1a07002     	mov	r7, r2
   376b8: ed2d8b02     	vpush	{d8}
   376bc: e24dd00c     	sub	sp, sp, #12
   376c0: e59420e8     	ldr	r2, [r4, #0xe8]
   376c4: e280aba9     	add	r10, r0, #173056
   376c8: e594604c     	ldr	r6, [r4, #0x4c]
   376cc: e28a0f46     	add	r0, r10, #280
   376d0: e5948088     	ldr	r8, [r4, #0x88]
   376d4: e592b080     	ldr	r11, [r2, #0x80]
   376d8: e2466001     	sub	r6, r6, #1
   376dc: e5949084     	ldr	r9, [r4, #0x84]
   376e0: e5945080     	ldr	r5, [r4, #0x80]
   376e4: e58d1000     	str	r1, [sp]
   376e8: eb00d293     	bl	0x6c13c
   376ec: e1800007     	orr	r0, r0, r7
   376f0: e28a2f42     	add	r2, r10, #264
   376f4: e71bfb16     	sdiv	r11, r6, r11
   376f8: e31000ff     	tst	r0, #255
   376fc: 0a00008b     	beq	0x37930
   37700: e59430e8     	ldr	r3, [r4, #0xe8]
   37704: e2841a2a     	add	r1, r4, #172032
   37708: ed9f8a8e     	vldr	s16, [pc, #568]         @ 0x37948 ; float 4095
   3770c: e5930028     	ldr	r0, [r3, #0x28]
   37710: e5d3c06c     	ldrb	r12, [r3, #0x6c]
   37714: e59134a4     	ldr	r3, [r1, #0x4a4]
   37718: e240e002     	sub	lr, r0, #2
   3771c: ee073a90     	vmov	s15, r3
   37720: eef86ae7     	vcvt.f32.s32	s13, s15
   37724: ee070a90     	vmov	s15, r0
   37728: eeb87ae7     	vcvt.f32.s32	s14, s15
   3772c: eec67a88     	vdiv.f32	s15, s13, s16
   37730: ee677a87     	vmul.f32	s15, s15, s14
   37734: eefd7ae7     	vcvt.s32.f32	s15, s15
   37738: ee173a90     	vmov	r3, s15
   3773c: e153000e     	cmp	r3, lr
   37740: a1a0300e     	movge	r3, lr
   37744: e3530001     	cmp	r3, #1
   37748: b3a03001     	movlt	r3, #1
   3774c: e35c0000     	cmp	r12, #0
   37750: e5843038     	str	r3, [r4, #0x38]
   37754: 0a000016     	beq	0x377b4
   37758: e59d0000     	ldr	r0, [sp]
   3775c: e3500000     	cmp	r0, #0
   37760: 0a000010     	beq	0x377a8
   37764: e1a00002     	mov	r0, r2
   37768: e58d1000     	str	r1, [sp]
   3776c: eb00d272     	bl	0x6c13c
   37770: e1877000     	orr	r7, r7, r0
   37774: e59d1000     	ldr	r1, [sp]
   37778: e31700ff     	tst	r7, #255
   3777c: 0a00002c     	beq	0x37834
   37780: e59134a0     	ldr	r3, [r1, #0x4a0]
   37784: ee073a90     	vmov	s15, r3
   37788: eeb87ae7     	vcvt.f32.s32	s14, s15
   3778c: eec77a08     	vdiv.f32	s15, s14, s16
   37790: ee076a10     	vmov	s14, r6
   37794: eeb87ac7     	vcvt.f32.s32	s14, s14
   37798: ee677a87     	vmul.f32	s15, s15, s14
   3779c: eefd7ae7     	vcvt.s32.f32	s15, s15
   377a0: ee173a90     	vmov	r3, s15
   377a4: ea000019     	b	0x37810
   377a8: e713fb13     	sdiv	r3, r3, r11
   377ac: e003039b     	mul	r3, r11, r3
   377b0: e5843038     	str	r3, [r4, #0x38]
   377b4: e1a00002     	mov	r0, r2
   377b8: e58d1004     	str	r1, [sp, #0x4]
   377bc: eb00d25e     	bl	0x6c13c
   377c0: e1877000     	orr	r7, r7, r0
   377c4: e59d1004     	ldr	r1, [sp, #0x4]
   377c8: e31700ff     	tst	r7, #255
   377cc: 0a000018     	beq	0x37834
   377d0: e59124a0     	ldr	r2, [r1, #0x4a0]
   377d4: ee072a90     	vmov	s15, r2
   377d8: eddf6a5a     	vldr	s13, [pc, #360]         @ 0x37948 ; float 4095
   377dc: e5d43090     	ldrb	r3, [r4, #0x90]
   377e0: eeb87ae7     	vcvt.f32.s32	s14, s15
   377e4: e59d2000     	ldr	r2, [sp]
   377e8: e2233001     	eor	r3, r3, #1
   377ec: e1923003     	orrs	r3, r2, r3
   377f0: eec77a26     	vdiv.f32	s15, s14, s13
   377f4: ee076a10     	vmov	s14, r6
   377f8: eeb87ac7     	vcvt.f32.s32	s14, s14
   377fc: ee677a87     	vmul.f32	s15, s15, s14
   37800: eefd7ae7     	vcvt.s32.f32	s15, s15
   37804: ee173a90     	vmov	r3, s15
   37808: 0713fb13     	sdiveq	r3, r3, r11
   3780c: 0023bb93     	mlaeq	r3, r3, r11, r11
   37810: e59420d4     	ldr	r2, [r4, #0xd4]
   37814: e5922030     	ldr	r2, [r2, #0x30]
   37818: e3520001     	cmp	r2, #1
   3781c: b3a02001     	movlt	r2, #1
   37820: e1560003     	cmp	r6, r3
   37824: a1a06003     	movge	r6, r3
   37828: e1520006     	cmp	r2, r6
   3782c: a1a06002     	movge	r6, r2
   37830: e5846080     	str	r6, [r4, #0x80]
   37834: e2813e4b     	add	r3, r1, #1200
   37838: ed9f6a42     	vldr	s12, [pc, #264]         @ 0x37948 ; float 4095
   3783c: e0852fa5     	add	r2, r5, r5, lsr #31
   37840: e2833004     	add	r3, r3, #4
   37844: ed937a00     	vldr	s14, [r3]
   37848: e1a020c2     	asr	r2, r2, #1
   3784c: e1520009     	cmp	r2, r9
   37850: e5943038     	ldr	r3, [r4, #0x38]
   37854: a1a02009     	movge	r2, r9
   37858: ee072a90     	vmov	s15, r2
   3785c: eeb87ac7     	vcvt.f32.s32	s14, s14
   37860: e0835005     	add	r5, r3, r5
   37864: eef87ae7     	vcvt.f32.s32	s15, s15
   37868: e0838008     	add	r8, r3, r8
   3786c: e5942050     	ldr	r2, [r4, #0x50]
   37870: e5845040     	str	r5, [r4, #0x40]
   37874: eec76a06     	vdiv.f32	s13, s14, s12
   37878: e584803c     	str	r8, [r4, #0x3c]
   3787c: ee677aa6     	vmul.f32	s15, s15, s13
   37880: eefd7ae7     	vcvt.s32.f32	s15, s15
   37884: ee173a90     	vmov	r3, s15
   37888: e3530080     	cmp	r3, #128
   3788c: b3a03080     	movlt	r3, #128
   37890: e1550002     	cmp	r5, r2
   37894: e5843088     	str	r3, [r4, #0x88]
   37898: a594204c     	ldrge	r2, [r4, #0x4c]
   3789c: a711f215     	sdivge	r1, r5, r2
   378a0: a0655192     	mlsge	r5, r2, r1, r5
   378a4: e5d4207c     	ldrb	r2, [r4, #0x7c]
   378a8: a5845040     	strge	r5, [r4, #0x40]
   378ac: e0833005     	add	r3, r3, r5
   378b0: e3520000     	cmp	r2, #0
   378b4: e5843044     	str	r3, [r4, #0x44]
   378b8: 0a00000d     	beq	0x378f4
   378bc: e59410e8     	ldr	r1, [r4, #0xe8]
   378c0: e5943078     	ldr	r3, [r4, #0x78]
   378c4: e5910014     	ldr	r0, [r1, #0x14]
   378c8: e591201c     	ldr	r2, [r1, #0x1c]
   378cc: e1530000     	cmp	r3, r0
   378d0: d3a0c000     	movle	r12, #0
   378d4: c3a0c001     	movgt	r12, #1
   378d8: e1530002     	cmp	r3, r2
   378dc: c3a03000     	movgt	r3, #0
   378e0: d3a03001     	movle	r3, #1
   378e4: e1500002     	cmp	r0, r2
   378e8: ba000004     	blt	0x37900
   378ec: e193300c     	orrs	r3, r3, r12
   378f0: 0a000004     	beq	0x37908
   378f4: e28dd00c     	add	sp, sp, #12
   378f8: ecbd8b02     	vpop	{d8}
   378fc: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   37900: e113000c     	tst	r3, r12
   37904: 1afffffa     	bne	0x378f4
   37908: edd17a00     	vldr	s15, [r1]
   3790c: e3a03001     	mov	r3, #1
   37910: eef57ac0     	vcmpe.f32	s15, #0
   37914: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37918: b5912018     	ldrlt	r2, [r1, #0x18]
   3791c: e5842078     	str	r2, [r4, #0x78]
   37920: e5c4307c     	strb	r3, [r4, #0x7c]
   37924: e28dd00c     	add	sp, sp, #12
   37928: ecbd8b02     	vpop	{d8}
   3792c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   37930: e1a00002     	mov	r0, r2
   37934: eb00d200     	bl	0x6c13c
   37938: e2841a2a     	add	r1, r4, #172032
   3793c: e3500000     	cmp	r0, #0
   37940: 1affffa2     	bne	0x377d0
   37944: eaffffba     	b	0x37834
   37948: 00 f0 7f 45  	.word	0x457ff000
