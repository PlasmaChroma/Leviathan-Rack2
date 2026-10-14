000027d4 <setActive>:
    27d4: eefd7ac0     	vcvt.s32.f32	s15, s0
    27d8: e2803923     	add	r3, r0, #573440
    27dc: e92d4010     	push	{r4, lr}
    27e0: e1a04000     	mov	r4, r0
    27e4: ed2d8b02     	vpush	{d8}
    27e8: e59f0040     	ldr	r0, [pc, #0x40]         @ 0x2830 <setActive+0x5c>
    27ec: e08f0000     	add	r0, pc, r0
    27f0: ee171a90     	vmov	r1, s15
    27f4: eeb08a40     	vmov.f32	s16, s0
    27f8: e5831a2c     	str	r1, [r3, #0xa2c]
    27fc: ebfffe3f     	bl	0x2100 <.plt+0x170>     @ imm = #-0x704
    2800: eeb58a40     	vcmp.f32	s16, #0
    2804: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2808: 1a000006     	bne	0x2828 <setActive+0x54> @ imm = #0x18
    280c: ecbd8b02     	vpop	{d8}
    2810: e2840028     	add	r0, r4, #40
    2814: e3a02cca     	mov	r2, #51712
    2818: e3a01000     	mov	r1, #0
    281c: e3402008     	movt	r2, #0x8
    2820: e8bd4010     	pop	{r4, lr}
    2824: eafffe2f     	b	0x20e8 <.plt+0x158>     @ imm = #-0x744
    2828: ecbd8b02     	vpop	{d8}
    282c: e8bd8010     	pop	{r4, pc}
    2830: ac 4d 00 00  	.word	0x00004dac

