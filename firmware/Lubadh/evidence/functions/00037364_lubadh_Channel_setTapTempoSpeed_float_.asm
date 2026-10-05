; lubadh::Channel::setTapTempoSpeed(float)
; VA 0x37364 size 164

   37364: eef17a00     	vmov.f32	s15, #4.000000e+00
   37368: eeb40ae7     	vcmpe.f32	s0, s15
   3736c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37370: 5eb00a67     	vmovpl.f32	s0, s15
   37374: 5a000003     	bpl	0x37388
   37378: eddf7a20     	vldr	s15, [pc, #128]         @ 0x37400 ; float 0.00999999977648
   3737c: eeb40ae7     	vcmpe.f32	s0, s15
   37380: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37384: deb00a67     	vmovle.f32	s0, s15
   37388: e59030e8     	ldr	r3, [r0, #0xe8]
   3738c: e59320b0     	ldr	r2, [r3, #0xb0]
   37390: e5922040     	ldr	r2, [r2, #0x40]
   37394: e3520003     	cmp	r2, #3
   37398: 0a000013     	beq	0x373ec
   3739c: e2802a2a     	add	r2, r0, #172032
   373a0: e5922498     	ldr	r2, [r2, #0x498]
   373a4: e3520b02     	cmp	r2, #2048
   373a8: ba000012     	blt	0x373f8
   373ac: ed800a0b     	vstr	s0, [r0, #44]
   373b0: eef76a00     	vmov.f32	s13, #1.000000e+00
   373b4: ed937a2b     	vldr	s14, [r3, #172]
   373b8: edd07ab6     	vldr	s15, [r0, #728]
   373bc: ed9f6a10     	vldr	s12, [pc, #64]          @ 0x37404 ; float 2.70000004768
   373c0: eebd7ac7     	vcvt.s32.f32	s14, s14
   373c4: ee300a67     	vsub.f32	s0, s0, s15
   373c8: eeb87ac7     	vcvt.f32.s32	s14, s14
   373cc: eec77a06     	vdiv.f32	s15, s14, s12
   373d0: ee777aa6     	vadd.f32	s15, s15, s13
   373d4: eefd7ae7     	vcvt.s32.f32	s15, s15
   373d8: eeb87ae7     	vcvt.f32.s32	s14, s15
   373dc: edc07ab8     	vstr	s15, [r0, #736]
   373e0: eec07a07     	vdiv.f32	s15, s0, s14
   373e4: edc07ab7     	vstr	s15, [r0, #732]
   373e8: e12fff1e     	bx	lr
   373ec: e59020ec     	ldr	r2, [r0, #0xec]
   373f0: e3520000     	cmp	r2, #0
   373f4: 0affffec     	beq	0x373ac
   373f8: eeb10a40     	vneg.f32	s0, s0
   373fc: eaffffea     	b	0x373ac
   37400: 0a d7 23 3c  	.word	0x3c23d70a
   37404: cd cc 2c 40  	.word	0x402ccccd
