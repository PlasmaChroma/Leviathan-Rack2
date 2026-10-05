0000e4f4 <_setHoldValue>:
    e4f4: e5d0203c     	ldrb	r2, [r0, #0x3c]
    e4f8: e3520000     	cmp	r2, #0
    e4fc: 112fff1e     	bxne	lr
    e500: e92d4070     	push	{r4, r5, r6, lr}
    e504: e1a05000     	mov	r5, r0
    e508: ed2d8b02     	vpush	{d8}
    e50c: e59f60b4     	ldr	r6, [pc, #0xb4]         @ 0xe5c8 <_setHoldValue+0xd4>
    e510: eeb08a40     	vmov.f32	s16, s0
    e514: e08f6006     	add	r6, pc, r6
    e518: ed9f0a24     	vldr	s0, [pc, #144]          @ 0xe5b0 <_setHoldValue+0xbc>
    e51c: ebffd4b1     	bl	0x37e8 <.plt+0xec>      @ imm = #-0xad3c
    e520: e59631a0     	ldr	r3, [r6, #0x1a0]
    e524: eddf7a22     	vldr	s15, [pc, #136]         @ 0xe5b4 <_setHoldValue+0xc0>
    e528: ed9f7a22     	vldr	s14, [pc, #136]         @ 0xe5b8 <_setHoldValue+0xc4>
    e52c: ee780a67     	vsub.f32	s1, s16, s15
    e530: ed9f1a21     	vldr	s2, [pc, #132]          @ 0xe5bc <_setHoldValue+0xc8>
    e534: ed9f6a21     	vldr	s12, [pc, #132]         @ 0xe5c0 <_setHoldValue+0xcc>
    e538: eddf6a21     	vldr	s13, [pc, #132]         @ 0xe5c4 <_setHoldValue+0xd0>
    e53c: ee300a80     	vadd.f32	s0, s1, s0
    e540: eeb40ac7     	vcmpe.f32	s0, s14
    e544: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e548: beb00a47     	vmovlt.f32	s0, s14
    e54c: eeb40ac1     	vcmpe.f32	s0, s2
    e550: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e554: 8eb00a41     	vmovhi.f32	s0, s2
    e558: ee711a40     	vsub.f32	s3, s2, s0
    e55c: ee212a86     	vmul.f32	s4, s3, s12
    e560: ee622a02     	vmul.f32	s5, s4, s4
    e564: ee223aa6     	vmul.f32	s6, s5, s13
    e568: eeb43ac7     	vcmpe.f32	s6, s14
    e56c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e570: beb03a47     	vmovlt.f32	s6, s14
    e574: eefd3ac3     	vcvt.s32.f32	s7, s6
    e578: ee134a90     	vmov	r4, s7
    e57c: e1540003     	cmp	r4, r3
    e580: 1a000001     	bne	0xe58c <_setHoldValue+0x98> @ imm = #0x4
    e584: ecbd8b02     	vpop	{d8}
    e588: e8bd8070     	pop	{r4, r5, r6, pc}
    e58c: eeb84ae3     	vcvt.f32.s32	s8, s7
    e590: e1a00005     	mov	r0, r5
    e594: e3a01026     	mov	r1, #38
    e598: eefc4ac4     	vcvt.u32.f32	s9, s8
    e59c: ee142a90     	vmov	r2, s9
    e5a0: ebffd550     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xaac0
    e5a4: ecbd8b02     	vpop	{d8}
    e5a8: e58641a0     	str	r4, [r6, #0x1a0]
    e5ac: e8bd8070     	pop	{r4, r5, r6, pc}
    e5b0: 00 00 4c 42  	.word	0x424c0000
    e5b4: 00 00 00 45  	.word	0x45000000
    e5b8: 00 00 00 00  	.word	0x00000000
    e5bc: 00 f0 7f 45  	.word	0x457ff000
    e5c0: 01 08 80 39  	.word	0x39800801
    e5c4: 00 40 1c 46  	.word	0x461c4000
    e5c8: 9c 8e 01 00  	.word	0x00018e9c

