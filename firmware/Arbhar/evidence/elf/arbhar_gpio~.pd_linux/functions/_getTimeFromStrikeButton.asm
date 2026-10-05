0000aeec <_getTimeFromStrikeButton>:
    aeec: e59f3098     	ldr	r3, [pc, #0x98]         @ 0xaf8c <_getTimeFromStrikeButton+0xa0>
    aef0: e2800a01     	add	r0, r0, #4096
    aef4: e92d4010     	push	{r4, lr}
    aef8: e08f1003     	add	r1, pc, r3
    aefc: e5904ddc     	ldr	r4, [r0, #0xddc]
    af00: e2802ede     	add	r2, r0, #3552
    af04: e591c094     	ldr	r12, [r1, #0x94]
    af08: e24dd008     	sub	sp, sp, #8
    af0c: ed9f7a1b     	vldr	s14, [pc, #108]         @ 0xaf80 <_getTimeFromStrikeButton+0x94>
    af10: e044e00c     	sub	lr, r4, r12
    af14: e59f3074     	ldr	r3, [pc, #0x74]         @ 0xaf90 <_getTimeFromStrikeButton+0xa4>
    af18: ee07ea90     	vmov	s15, lr
    af1c: eddf6a18     	vldr	s13, [pc, #96]          @ 0xaf84 <_getTimeFromStrikeButton+0x98>
    af20: eeb80ae7     	vcvt.f32.s32	s0, s15
    af24: ee600a07     	vmul.f32	s1, s0, s14
    af28: eef40ae6     	vcmpe.f32	s1, s13
    af2c: edc20a03     	vstr	s1, [r2, #12]
    af30: eef1fa10     	vmrs	APSR_nzcv, fpscr
    af34: 4d9f6a13     	vldrmi	s12, [pc, #76]          @ 0xaf88 <_getTimeFromStrikeButton+0x9c>
    af38: 52800edf     	addpl	r0, r0, #3568
    af3c: 4280eedf     	addmi	lr, r0, #3568
    af40: 5dd06a00     	vldrpl	s13, [r0]
    af44: 41a0000e     	movmi	r0, lr
    af48: 4ec66a00     	vdivmi.f32	s13, s12, s0
    af4c: eef71ae0     	vcvt.f64.f32	d17, s1
    af50: eef70ae6     	vcvt.f64.f32	d16, s13
    af54: 4dc06a00     	vstrmi	s13, [r0]
    af58: e08f0003     	add	r0, pc, r3
    af5c: 4dce6a03     	vstrmi	s13, [lr, #12]
    af60: ec532b31     	vmov	r2, r3, d17
    af64: edcd0b00     	vstr	d16, [sp]
    af68: ebffe302     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x73f8
    af6c: e59f1020     	ldr	r1, [pc, #0x20]         @ 0xaf94 <_getTimeFromStrikeButton+0xa8>
    af70: e08f0001     	add	r0, pc, r1
    af74: e5804094     	str	r4, [r0, #0x94]
    af78: e28dd008     	add	sp, sp, #8
    af7c: e8bd8010     	pop	{r4, pc}
    af80: ab aa aa 3f  	.word	0x3faaaaab
    af84: 00 40 1c 45  	.word	0x451c4000
    af88: 00 c8 2f 47  	.word	0x472fc800
    af8c: b8 c4 01 00  	.word	0x0001c4b8
    af90: 44 a6 00 00  	.word	0x0000a644
    af94: 40 c4 01 00  	.word	0x0001c440

