000082fc <tick_clockedGrains>:
    82fc: e2802a01     	add	r2, r0, #4096
    8300: e30032ce     	movw	r3, #0x2ce
    8304: e2821edf     	add	r1, r2, #3568
    8308: ed9f7a38     	vldr	s14, [pc, #224]         @ 0x83f0 <tick_clockedGrains+0xf4>
    830c: e192c0b3     	ldrh	r12, [r2, r3]
    8310: edd17a02     	vldr	s15, [r1, #8]
    8314: e35c0efa     	cmp	r12, #4000
    8318: ee870a27     	vdiv.f32	s0, s14, s15
    831c: 8a00002d     	bhi	0x83d8 <tick_clockedGrains+0xdc> @ imm = #0xb4
    8320: e26c3b02     	rsb	r3, r12, #2048
    8324: ed9f1a32     	vldr	s2, [pc, #200]          @ 0x83f4 <tick_clockedGrains+0xf8>
    8328: e282cc0e     	add	r12, r2, #3584
    832c: e28c2004     	add	r2, r12, #4
    8330: ee003a90     	vmov	s1, r3
    8334: eef81ae0     	vcvt.f32.s32	s3, s1
    8338: ee212a81     	vmul.f32	s4, s3, s2
    833c: eef61b00     	vmov.f64	d17, #5.000000e-01
    8340: eef70ac2     	vcvt.f64.f32	d16, s4
    8344: ee303ba1     	vadd.f64	d3, d16, d17
    8348: eefd3bc3     	vcvt.s32.f64	s7, d3
    834c: ee131a90     	vmov	r1, s7
    8350: eef72a00     	vmov.f32	s5, #1.000000e+00
    8354: edc22a00     	vstr	s5, [r2]
    8358: e3510000     	cmp	r1, #0
    835c: ba000012     	blt	0x83ac <tick_clockedGrains+0xb0> @ imm = #0x48
    8360: e281cd1e     	add	r12, r1, #1920
    8364: e28c1002     	add	r1, r12, #2
    8368: e0803101     	add	r3, r0, r1, lsl #2
    836c: ed934a00     	vldr	s8, [r3]
    8370: ed824a00     	vstr	s8, [r2]
    8374: eeb54a40     	vcmp.f32	s8, #0
    8378: eef1fa10     	vmrs	APSR_nzcv, fpscr
    837c: 0a000018     	beq	0x83e4 <tick_clockedGrains+0xe8> @ imm = #0x60
    8380: ee800a04     	vdiv.f32	s0, s0, s8
    8384: e92d4010     	push	{r4, lr}
    8388: e1a04000     	mov	r4, r0
    838c: e59000fc     	ldr	r0, [r0, #0xfc]
    8390: eeb70ac0     	vcvt.f64.f32	d0, s0
    8394: ebffed64     	bl	0x392c <.plt+0x230>     @ imm = #-0x4a70
    8398: e1a00004     	mov	r0, r4
    839c: e3a02001     	mov	r2, #1
    83a0: e3a01018     	mov	r1, #24
    83a4: e8bd4010     	pop	{r4, lr}
    83a8: eaffedce     	b	0x3ae8 <.plt+0x3ec>     @ imm = #-0x48c8
    83ac: e0813fa1     	add	r3, r1, r1, lsr #31
    83b0: e300c7b4     	movw	r12, #0x7b4
    83b4: e04c10c3     	sub	r1, r12, r3, asr #1
    83b8: e0803101     	add	r3, r0, r1, lsl #2
    83bc: edd36a01     	vldr	s13, [r3, #4]
    83c0: eef56ac0     	vcmpe.f32	s13, #0
    83c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    83c8: daffffed     	ble	0x8384 <tick_clockedGrains+0x88> @ imm = #-0x4c
    83cc: ee824aa6     	vdiv.f32	s8, s5, s13
    83d0: ed824a00     	vstr	s8, [r2]
    83d4: eaffffe6     	b	0x8374 <tick_clockedGrains+0x78> @ imm = #-0x68
    83d8: e2822c0e     	add	r2, r2, #3584
    83dc: e3a01000     	mov	r1, #0
    83e0: e5821004     	str	r1, [r2, #0x4]
    83e4: e59000fc     	ldr	r0, [r0, #0xfc]
    83e8: eeb70ac0     	vcvt.f64.f32	d0, s0
    83ec: eaffed4e     	b	0x392c <.plt+0x230>     @ imm = #-0x4ac8
    83f0: 00 60 6a 47  	.word	0x476a6000
    83f4: 00 00 a0 3b  	.word	0x3ba00000

