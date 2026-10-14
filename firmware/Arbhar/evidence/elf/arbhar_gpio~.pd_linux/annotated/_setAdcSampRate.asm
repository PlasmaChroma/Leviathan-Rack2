00004360 <_setAdcSampRate>:
    4360: eef47a00     	vmov.f32	s15, #1.250000e-01
    4364: e92d4010     	push	{r4, lr}
    4368: e2804a01     	add	r4, r0, #4096
    436c: e2843d37     	add	r3, r4, #3520
    4370: ed2d8b02     	vpush	{d8}
    4374: e59f007c     	ldr	r0, [pc, #0x7c]         @ 0x43f8 <_setAdcSampRate+0x98>  // u32=0x106cc; f32?=9.42737555e-41
    4378: ed830a02     	vstr	s0, [r3, #8]
    437c: e08f0000     	add	r0, pc, r0
    4380: eeb40ae7     	vcmpe.f32	s0, s15
    4384: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4388: 42842d37     	addmi	r2, r4, #3520
    438c: 5ef07a40     	vmovpl.f32	s15, s0
    4390: 42833008     	addmi	r3, r3, #8
    4394: 4dd27a03     	vldrmi	s15, [r2, #12]
    4398: eeb08a40     	vmov.f32	s16, s0
    439c: 4dc37a00     	vstrmi	s15, [r3]
    43a0: eef70ae7     	vcvt.f64.f32	d16, s15
    43a4: ec532b30     	vmov	r2, r3, d16
    43a8: ebfffdf2     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x838  // CALL post
    43ac: e59f2048     	ldr	r2, [pc, #0x48]         @ 0x43fc <_setAdcSampRate+0x9c>  // u32=0x1067c; f32?=9.41616516e-41
    43b0: eeb70a00     	vmov.f32	s0, #1.000000e+00
    43b4: e08f0002     	add	r0, pc, r2
    43b8: eeb48ac0     	vcmpe.f32	s16, s0
    43bc: ecbd8b02     	vpop	{d8}
    43c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    43c4: 52844edd     	addpl	r4, r4, #3536
    43c8: 4d9f1b08     	vldrmi	d1, [pc, #32]           @ 0x43f0 <_setAdcSampRate+0x90>
    43cc: 42844edd     	addmi	r4, r4, #3536
    43d0: 43031958     	movwmi	r1, #0x3958
    43d4: 5d940a00     	vldrpl	s0, [r4]
    43d8: 5eb71ac0     	vcvtpl.f64.f32	d1, s0
    43dc: 43431c34     	movtmi	r1, #0x3c34
    43e0: ec532b11     	vmov	r2, r3, d1
    43e4: 45841000     	strmi	r1, [r4]
    43e8: e8bd4010     	pop	{r4, lr}
    43ec: eafffde1     	b	0x3b78 <.plt+0x47c>     @ imm = #-0x87c  // CALL post
    43f0: 00 00 00 00  	.word	0x00000000
    43f4: 2b 87 86 3f  	.word	0x3f86872b
    43f8: cc 06 01 00  	.word	0x000106cc
    43fc: 7c 06 01 00  	.word	0x0001067c

