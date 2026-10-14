00008404 <_setVpo3VoltValue>:
    8404: edd07ab1     	vldr	s15, [r0, #708]
    8408: e3003126     	movw	r3, #0x126
    840c: e92d4070     	push	{r4, r5, r6, lr}
    8410: e1a04000     	mov	r4, r0
    8414: ed947a48     	vldr	s14, [r4, #288]
    8418: e19010b3     	ldrh	r1, [r0, r3]
    841c: e59f0100     	ldr	r0, [pc, #0x100]        @ 0x8524 <_setVpo3VoltValue+0x120>  // u32=0xcac8; f32?=7.27442059e-41
    8420: eefd0ae7     	vcvt.s32.f32	s1, s15
    8424: ed2d8b02     	vpush	{d8}
    8428: e08f0000     	add	r0, pc, r0
    842c: ee105a90     	vmov	r5, s1
    8430: eeb08a40     	vmov.f32	s16, s0
    8434: ee103a90     	vmov	r3, s1
    8438: eebd0ac7     	vcvt.s32.f32	s0, s14
    843c: ee102a10     	vmov	r2, s0
    8440: ebffedcc     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x48d0  // CALL post
    8444: e2451fa2     	sub	r1, r5, #648
    8448: e2412003     	sub	r2, r1, #3
    844c: e3520e12     	cmp	r2, #288
    8450: 9a00001e     	bls	0x84d0 <_setVpo3VoltValue+0xcc> @ imm = #0x78
    8454: ed9f1a30     	vldr	s2, [pc, #192]          @ 0x851c <_setVpo3VoltValue+0x118>  // f32=650
    8458: eddf1a30     	vldr	s3, [pc, #192]          @ 0x8520 <_setVpo3VoltValue+0x11c>  // f32=850
    845c: eeb48ac1     	vcmpe.f32	s16, s2
    8460: eef1fa10     	vmrs	APSR_nzcv, fpscr
    8464: eeb48ae1     	vcmpe.f32	s16, s3
    8468: c3a02001     	movgt	r2, #1
    846c: d3a02000     	movle	r2, #0
    8470: eef1fa10     	vmrs	APSR_nzcv, fpscr
    8474: 42022001     	andmi	r2, r2, #1
    8478: 53a02000     	movpl	r2, #0
    847c: e3520000     	cmp	r2, #0
    8480: 0a00000e     	beq	0x84c0 <_setVpo3VoltValue+0xbc> @ imm = #0x38
    8484: eeb70a00     	vmov.f32	s0, #1.000000e+00
    8488: ebffed39     	bl	0x3974 <.plt+0x278>     @ imm = #-0x4b1c  // CALL _setPlusLed
    848c: e1a00004     	mov	r0, r4
    8490: e3a01068     	mov	r1, #104
    8494: eebd2ac8     	vcvt.s32.f32	s4, s16
    8498: eefc2ac8     	vcvt.u32.f32	s5, s16
    849c: ed842a08     	vstr	s4, [r4, #32]
    84a0: ee122a90     	vmov	r2, s5
    84a4: ebffed8f     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x49c4  // CALL writeToSharedMem
    84a8: ecbd8b02     	vpop	{d8}
    84ac: e59fc074     	ldr	r12, [pc, #0x74]        @ 0x8528 <_setVpo3VoltValue+0x124>  // u32=0xca9c; f32?=7.26825487e-41
    84b0: e5941020     	ldr	r1, [r4, #0x20]
    84b4: e08f000c     	add	r0, pc, r12
    84b8: e8bd4070     	pop	{r4, r5, r6, lr}
    84bc: eaffedad     	b	0x3b78 <.plt+0x47c>     @ imm = #-0x494c  // CALL post
    84c0: ecbd8b02     	vpop	{d8}
    84c4: eeb00a00     	vmov.f32	s0, #2.000000e+00
    84c8: e8bd4070     	pop	{r4, r5, r6, lr}
    84cc: eaffed28     	b	0x3974 <.plt+0x278>     @ imm = #-0x4b60  // CALL _setPlusLed
    84d0: eeb70a00     	vmov.f32	s0, #1.000000e+00
    84d4: ebffed26     	bl	0x3974 <.plt+0x278>     @ imm = #-0x4b68  // CALL _setPlusLed
    84d8: e59f004c     	ldr	r0, [pc, #0x4c]         @ 0x852c <_setVpo3VoltValue+0x128>  // u32=0xca28; f32?=7.25199981e-41
    84dc: e1a03005     	mov	r3, r5
    84e0: e3a02feb     	mov	r2, #940
    84e4: e300128a     	movw	r1, #0x28a
    84e8: e08f0000     	add	r0, pc, r0
    84ec: ebffeda1     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x497c  // CALL post
    84f0: e5845020     	str	r5, [r4, #0x20]
    84f4: e1a00004     	mov	r0, r4
    84f8: e1a02005     	mov	r2, r5
    84fc: e3a01068     	mov	r1, #104
    8500: ebffed78     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x4a20  // CALL writeToSharedMem
    8504: ecbd8b02     	vpop	{d8}
    8508: e5941020     	ldr	r1, [r4, #0x20]
    850c: e59f401c     	ldr	r4, [pc, #0x1c]         @ 0x8530 <_setVpo3VoltValue+0x12c>  // u32=0xca24; f32?=7.25143929e-41
    8510: e08f0004     	add	r0, pc, r4
    8514: e8bd4070     	pop	{r4, r5, r6, lr}
    8518: eaffed96     	b	0x3b78 <.plt+0x47c>     @ imm = #-0x49a8  // CALL post
    851c: 00 80 22 44  	.word	0x44228000
    8520: 00 80 54 44  	.word	0x44548000
    8524: c8 ca 00 00  	.word	0x0000cac8
    8528: 9c ca 00 00  	.word	0x0000ca9c
    852c: 28 ca 00 00  	.word	0x0000ca28
    8530: 24 ca 00 00  	.word	0x0000ca24

