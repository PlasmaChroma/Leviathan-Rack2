0000826c <tick_strikeLed>:
    826c: eef77a00     	vmov.f32	s15, #1.000000e+00
    8270: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    8274: e59f507c     	ldr	r5, [pc, #0x7c]         @ 0x82f8 <tick_strikeLed+0x8c>
    8278: e5d0603c     	ldrb	r6, [r0, #0x3c]
    827c: e08f5005     	add	r5, pc, r5
    8280: e5d07104     	ldrb	r7, [r0, #0x104]
    8284: e3560000     	cmp	r6, #0
    8288: ed957a12     	vldr	s14, [r5, #72]
    828c: ee776ac7     	vsub.f32	s13, s15, s14
    8290: edc56a12     	vstr	s13, [r5, #72]
    8294: 18bd81f0     	popne	{r4, r5, r6, r7, r8, pc}
    8298: e3a01016     	mov	r1, #22
    829c: e1a04000     	mov	r4, r0
    82a0: eeb47ae7     	vcmpe.f32	s14, s15
    82a4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    82a8: 43a02001     	movmi	r2, #1
    82ac: 53a02000     	movpl	r2, #0
    82b0: ebffee0c     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x47d0
    82b4: ed950a12     	vldr	s0, [r5, #72]
    82b8: eeb50ac0     	vcmpe.f32	s0, #0
    82bc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    82c0: da000006     	ble	0x82e0 <tick_strikeLed+0x74> @ imm = #0x18
    82c4: e1a01007     	mov	r1, r7
    82c8: e1a00004     	mov	r0, r4
    82cc: ebffee2f     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x4744
    82d0: e59400dc     	ldr	r0, [r4, #0xdc]
    82d4: ed9f0b05     	vldr	d0, [pc, #20]           @ 0x82f0 <tick_strikeLed+0x84>
    82d8: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
    82dc: eaffed92     	b	0x392c <.plt+0x230>     @ imm = #-0x49b8
    82e0: e1a01006     	mov	r1, r6
    82e4: e1a00004     	mov	r0, r4
    82e8: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
    82ec: eaffee27     	b	0x3b90 <.plt+0x494>     @ imm = #-0x4764
    82f0: 00 00 00 00  	.word	0x00000000
    82f4: 00 00 49 40  	.word	0x40490000
    82f8: 34 f1 01 00  	.word	0x0001f134

