0000e3ec <led_setSelectedLayerLed>:
    e3ec: e92d4010     	push	{r4, lr}
    e3f0: e24dd020     	sub	sp, sp, #32
    e3f4: e5d03037     	ldrb	r3, [r0, #0x37]
    e3f8: e5d02039     	ldrb	r2, [r0, #0x39]
    e3fc: e2631006     	rsb	r1, r3, #6
    e400: e3520000     	cmp	r2, #0
    e404: e6ef4071     	uxtb	r4, r1
    e408: 1a00001b     	bne	0xe47c <led_setSelectedLayerLed+0x90> @ imm = #0x6c
    e40c: e5d0c032     	ldrb	r12, [r0, #0x32]
    e410: e3a02001     	mov	r2, #1
    e414: e1a04412     	lsl	r4, r2, r4
    e418: e26c3006     	rsb	r3, r12, #6
    e41c: ee004a10     	vmov	s0, r4
    e420: e6ef1073     	uxtb	r1, r3
    e424: e1a0c112     	lsl	r12, r2, r1
    e428: eef87ac0     	vcvt.f32.s32	s15, s0
    e42c: ee07ca10     	vmov	s14, r12
    e430: eeb87ac7     	vcvt.f32.s32	s14, s14
    e434: e5d01030     	ldrb	r1, [r0, #0x30]
    e438: e3a03001     	mov	r3, #1
    e43c: e3a04000     	mov	r4, #0
    e440: e3a02000     	mov	r2, #0
    e444: e3510062     	cmp	r1, #98
    e448: e3444312     	movt	r4, #0x4312
    e44c: e3442313     	movt	r2, #0x4313
    e450: ed8d7a01     	vstr	s14, [sp, #4]
    e454: e58d400c     	str	r4, [sp, #0xc]
    e458: e58d3000     	str	r3, [sp]
    e45c: edcd7a05     	vstr	s15, [sp, #20]
    e460: e58d201c     	str	r2, [sp, #0x1c]
    e464: e58d3008     	str	r3, [sp, #0x8]
    e468: e58d3010     	str	r3, [sp, #0x10]
    e46c: e58d3018     	str	r3, [sp, #0x18]
    e470: 9a00000c     	bls	0xe4a8 <led_setSelectedLayerLed+0xbc> @ imm = #0x30
    e474: e28dd020     	add	sp, sp, #32
    e478: e8bd8010     	pop	{r4, pc}
    e47c: e3520001     	cmp	r2, #1
    e480: 1a000014     	bne	0xe4d8 <led_setSelectedLayerLed+0xec> @ imm = #0x50
    e484: e5d01031     	ldrb	r1, [r0, #0x31]
    e488: e3510000     	cmp	r1, #0
    e48c: 01a04412     	lsleq	r4, r2, r4
    e490: 1ef77a00     	vmovne.f32	s15, #1.000000e+00
    e494: 0e074a90     	vmoveq	s15, r4
    e498: 0eb77a00     	vmoveq.f32	s14, #1.000000e+00
    e49c: 0ef87ae7     	vcvteq.f32.s32	s15, s15
    e4a0: 1eb07a67     	vmovne.f32	s14, s15
    e4a4: eaffffe2     	b	0xe434 <led_setSelectedLayerLed+0x48> @ imm = #-0x78
    e4a8: e1a0e000     	mov	lr, r0
    e4ac: e59f003c     	ldr	r0, [pc, #0x3c]         @ 0xe4f0 <led_setSelectedLayerLed+0x104>
    e4b0: e59e4070     	ldr	r4, [lr, #0x70]
    e4b4: e08f0000     	add	r0, pc, r0
    e4b8: ebffd49a     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xad98
    e4bc: e1a0300d     	mov	r3, sp
    e4c0: e3a02004     	mov	r2, #4
    e4c4: e1a01000     	mov	r1, r0
    e4c8: e1a00004     	mov	r0, r4
    e4cc: ebffd5e8     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xa860
    e4d0: e28dd020     	add	sp, sp, #32
    e4d4: e8bd8010     	pop	{r4, pc}
    e4d8: e3a0c001     	mov	r12, #1
    e4dc: eeb77a00     	vmov.f32	s14, #1.000000e+00
    e4e0: e1a0341c     	lsl	r3, r12, r4
    e4e4: ee073a90     	vmov	s15, r3
    e4e8: eef87ae7     	vcvt.f32.s32	s15, s15
    e4ec: eaffffd0     	b	0xe434 <led_setSelectedLayerLed+0x48> @ imm = #-0xc0
    e4f0: 00 66 00 00  	.word	0x00006600

