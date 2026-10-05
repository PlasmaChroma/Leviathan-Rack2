0000ae18 <_getStrikeCvToShMem>:
    ae18: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    ae1c: e1a06000     	mov	r6, r0
    ae20: e3a00024     	mov	r0, #36
    ae24: e59f70b8     	ldr	r7, [pc, #0xb8]         @ 0xaee4 <_getStrikeCvToShMem+0xcc>
    ae28: ebffe256     	bl	0x3788 <.plt+0x8c>      @ imm = #-0x76a8
    ae2c: e2865a01     	add	r5, r6, #4096
    ae30: e08f7007     	add	r7, pc, r7
    ae34: e5d7308c     	ldrb	r3, [r7, #0x8c]
    ae38: e16f4f10     	clz	r4, r0
    ae3c: e1a042a4     	lsr	r4, r4, #5
    ae40: e1530004     	cmp	r3, r4
    ae44: 03a00000     	moveq	r0, #0
    ae48: 12040001     	andne	r0, r4, #1
    ae4c: e3500000     	cmp	r0, #0
    ae50: 1a000008     	bne	0xae78 <_getStrikeCvToShMem+0x60> @ imm = #0x20
    ae54: e285eedd     	add	lr, r5, #3536
    ae58: e59f7088     	ldr	r7, [pc, #0x88]         @ 0xaee8 <_getStrikeCvToShMem+0xd0>
    ae5c: e59600cc     	ldr	r0, [r6, #0xcc]
    ae60: edde0a00     	vldr	s1, [lr]
    ae64: e08f3007     	add	r3, pc, r7
    ae68: e5c3408c     	strb	r4, [r3, #0x8c]
    ae6c: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
    ae70: eeb70ae0     	vcvt.f64.f32	d0, s1
    ae74: eaffe2ac     	b	0x392c <.plt+0x230>     @ imm = #-0x7550
    ae78: e5d58df5     	ldrb	r8, [r5, #0xdf5]
    ae7c: e3580000     	cmp	r8, #0
    ae80: 1a000014     	bne	0xaed8 <_getStrikeCvToShMem+0xc0> @ imm = #0x50
    ae84: e5972090     	ldr	r2, [r7, #0x90]
    ae88: e1a00006     	mov	r0, r6
    ae8c: e3a0101c     	mov	r1, #28
    ae90: e282c001     	add	r12, r2, #1
    ae94: e587c090     	str	r12, [r7, #0x90]
    ae98: ebffe312     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x73b8
    ae9c: ed960a3b     	vldr	s0, [r6, #236]
    aea0: e5972090     	ldr	r2, [r7, #0x90]
    aea4: e3021710     	movw	r1, #0x2710
    aea8: e59600e4     	ldr	r0, [r6, #0xe4]
    aeac: e1520001     	cmp	r2, r1
    aeb0: c5878090     	strgt	r8, [r7, #0x90]
    aeb4: eeb70ac0     	vcvt.f64.f32	d0, s0
    aeb8: ebffe29b     	bl	0x392c <.plt+0x230>     @ imm = #-0x7594
    aebc: e1a00006     	mov	r0, r6
    aec0: e3a01004     	mov	r1, #4
    aec4: ebffe331     	bl	0x3b90 <.plt+0x494>     @ imm = #-0x733c
    aec8: e59600e0     	ldr	r0, [r6, #0xe0]
    aecc: eeb20b04     	vmov.f64	d0, #1.000000e+01
    aed0: ebffe295     	bl	0x392c <.plt+0x230>     @ imm = #-0x75ac
    aed4: eaffffde     	b	0xae54 <_getStrikeCvToShMem+0x3c> @ imm = #-0x88
    aed8: e1a00006     	mov	r0, r6
    aedc: ebffe21a     	bl	0x374c <.plt+0x50>      @ imm = #-0x7798
    aee0: eaffffdb     	b	0xae54 <_getStrikeCvToShMem+0x3c> @ imm = #-0x94
    aee4: 80 c5 01 00  	.word	0x0001c580
    aee8: 4c c5 01 00  	.word	0x0001c54c

