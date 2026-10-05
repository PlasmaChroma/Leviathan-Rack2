00005f20 <tick_strikeCvLed>:
    5f20: e5d03030     	ldrb	r3, [r0, #0x30]
    5f24: e3530063     	cmp	r3, #99
    5f28: 012fff1e     	bxeq	lr
    5f2c: e92d4070     	push	{r4, r5, r6, lr}
    5f30: e2805a01     	add	r5, r0, #4096
    5f34: e5d52df5     	ldrb	r2, [r5, #0xdf5]
    5f38: e3520000     	cmp	r2, #0
    5f3c: 03a01004     	moveq	r1, #4
    5f40: 0a000005     	beq	0x5f5c <tick_strikeCvLed+0x3c> @ imm = #0x14
    5f44: e285cc0e     	add	r12, r5, #3584
    5f48: eddc7a01     	vldr	s15, [r12, #4]
    5f4c: eef57ac0     	vcmpe.f32	s15, #0
    5f50: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5f54: c3a01002     	movgt	r1, #2
    5f58: d3a01004     	movle	r1, #4
    5f5c: eeb70a00     	vmov.f32	s0, #1.000000e+00
    5f60: e59f30a8     	ldr	r3, [pc, #0xa8]         @ 0x6010 <tick_strikeCvLed+0xf0>
    5f64: e5d0c03c     	ldrb	r12, [r0, #0x3c]
    5f68: e08f3003     	add	r3, pc, r3
    5f6c: ed937a05     	vldr	s14, [r3, #20]
    5f70: ee706a47     	vsub.f32	s13, s0, s14
    5f74: eeb47ac0     	vcmpe.f32	s14, s0
    5f78: edc36a05     	vstr	s13, [r3, #20]
    5f7c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5f80: 5a000010     	bpl	0x5fc8 <tick_strikeCvLed+0xa8> @ imm = #0x40
    5f84: e35c0000     	cmp	r12, #0
    5f88: 18bd8070     	popne	{r4, r5, r6, pc}
    5f8c: e1a04000     	mov	r4, r0
    5f90: ebfff6fe     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x2408
    5f94: e5d50df5     	ldrb	r0, [r5, #0xdf5]
    5f98: e3500000     	cmp	r0, #0
    5f9c: 0a000013     	beq	0x5ff0 <tick_strikeCvLed+0xd0> @ imm = #0x4c
    5fa0: e2855c0e     	add	r5, r5, #3584
    5fa4: e59400e0     	ldr	r0, [r4, #0xe0]
    5fa8: e2851004     	add	r1, r5, #4
    5fac: edd10a00     	vldr	s1, [r1]
    5fb0: eef50a40     	vcmp.f32	s1, #0
    5fb4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5fb8: 0a00000e     	beq	0x5ff8 <tick_strikeCvLed+0xd8> @ imm = #0x38
    5fbc: eeb20b0e     	vmov.f64	d0, #1.500000e+01
    5fc0: e8bd4070     	pop	{r4, r5, r6, lr}
    5fc4: eafff658     	b	0x392c <.plt+0x230>     @ imm = #-0x26a0
    5fc8: e35c0000     	cmp	r12, #0
    5fcc: 18bd8070     	popne	{r4, r5, r6, pc}
    5fd0: e3520000     	cmp	r2, #0
    5fd4: 0a000002     	beq	0x5fe4 <tick_strikeCvLed+0xc4> @ imm = #0x8
    5fd8: e3a01003     	mov	r1, #3
    5fdc: e8bd4070     	pop	{r4, r5, r6, lr}
    5fe0: eafff6ea     	b	0x3b90 <.plt+0x494>     @ imm = #-0x2458
    5fe4: e1a01002     	mov	r1, r2
    5fe8: e8bd4070     	pop	{r4, r5, r6, lr}
    5fec: eafff6e7     	b	0x3b90 <.plt+0x494>     @ imm = #-0x2464
    5ff0: e59400e0     	ldr	r0, [r4, #0xe0]
    5ff4: eafffff0     	b	0x5fbc <tick_strikeCvLed+0x9c> @ imm = #-0x40
    5ff8: ed9f0b02     	vldr	d0, [pc, #8]            @ 0x6008 <tick_strikeCvLed+0xe8>
    5ffc: e8bd4070     	pop	{r4, r5, r6, lr}
    6000: eafff649     	b	0x392c <.plt+0x230>     @ imm = #-0x26dc
    6004: e320f000     	nop
    6008: 00 00 00 00  	.word	0x00000000
    600c: 00 00 49 40  	.word	0x40490000
    6010: 48 14 02 00  	.word	0x00021448

