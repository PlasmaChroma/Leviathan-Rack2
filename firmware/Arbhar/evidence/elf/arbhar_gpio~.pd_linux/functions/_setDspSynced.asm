00004db4 <_setDspSynced>:
    4db4: eefc7ac0     	vcvt.u32.f32	s15, s0
    4db8: e92d4010     	push	{r4, lr}
    4dbc: e1a04000     	mov	r4, r0
    4dc0: ed2d8b02     	vpush	{d8}
    4dc4: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x4e28 <_setDspSynced+0x74>
    4dc8: e08f0000     	add	r0, pc, r0
    4dcc: e24dd008     	sub	sp, sp, #8
    4dd0: eeb08a40     	vmov.f32	s16, s0
    4dd4: edcd7a01     	vstr	s15, [sp, #4]
    4dd8: e5dd1004     	ldrb	r1, [sp, #0x4]
    4ddc: e5c410bb     	strb	r1, [r4, #0xbb]
    4de0: ebfffb64     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x1270
    4de4: eeb58a40     	vcmp.f32	s16, #0
    4de8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4dec: 1a000002     	bne	0x4dfc <_setDspSynced+0x48> @ imm = #0x8
    4df0: e28dd008     	add	sp, sp, #8
    4df4: ecbd8b02     	vpop	{d8}
    4df8: e8bd8010     	pop	{r4, pc}
    4dfc: e59400c8     	ldr	r0, [r4, #0xc8]
    4e00: ed9f0b06     	vldr	d0, [pc, #24]           @ 0x4e20 <_setDspSynced+0x6c>
    4e04: ebfffb79     	bl	0x3bf0 <.plt+0x4f4>     @ imm = #-0x121c
    4e08: e59400cc     	ldr	r0, [r4, #0xcc]
    4e0c: ed9f0b03     	vldr	d0, [pc, #12]           @ 0x4e20 <_setDspSynced+0x6c>
    4e10: e28dd008     	add	sp, sp, #8
    4e14: ecbd8b02     	vpop	{d8}
    4e18: e8bd4010     	pop	{r4, lr}
    4e1c: eafffb73     	b	0x3bf0 <.plt+0x4f4>     @ imm = #-0x1234
    4e20: 00 00 00 00  	.word	0x00000000
    4e24: 00 00 00 00  	.word	0x00000000
    4e28: 08 fe 00 00  	.word	0x0000fe08

