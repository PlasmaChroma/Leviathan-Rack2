0000eb1c <animateFileActivityBusy>:
    eb1c: e92d4030     	push	{r4, r5, lr}
    eb20: e24dd024     	sub	sp, sp, #36
    eb24: e59f40c8     	ldr	r4, [pc, #0xc8]         @ 0xebf4 <animateFileActivityBusy+0xd8>
    eb28: e3a01000     	mov	r1, #0
    eb2c: e344130e     	movt	r1, #0x430e
    eb30: e3a05001     	mov	r5, #1
    eb34: e08f4004     	add	r4, pc, r4
    eb38: e3a0c5fe     	mov	r12, #1065353216
    eb3c: e1c427dc     	ldrd	r2, r3, [r4, #124]
    eb40: e58d100c     	str	r1, [sp, #0xc]
    eb44: e0821003     	add	r1, r2, r3
    eb48: e58d5000     	str	r5, [sp]
    eb4c: ee072a90     	vmov	s15, r2
    eb50: e3510048     	cmp	r1, #72
    eb54: e3a02000     	mov	r2, #0
    eb58: e584107c     	str	r1, [r4, #0x7c]
    eb5c: eeb80ae7     	vcvt.f32.s32	s0, s15
    eb60: e344230f     	movt	r2, #0x430f
    eb64: e58d5008     	str	r5, [sp, #0x8]
    eb68: e58d5010     	str	r5, [sp, #0x10]
    eb6c: e58dc004     	str	r12, [sp, #0x4]
    eb70: e58d5018     	str	r5, [sp, #0x18]
    eb74: e58d201c     	str	r2, [sp, #0x1c]
    eb78: ed8d0a05     	vstr	s0, [sp, #20]
    eb7c: 9a000010     	bls	0xebc4 <animateFileActivityBusy+0xa8> @ imm = #0x40
    eb80: e351003e     	cmp	r1, #62
    eb84: da000006     	ble	0xeba4 <animateFileActivityBusy+0x88> @ imm = #0x18
    eb88: e59f3068     	ldr	r3, [pc, #0x68]         @ 0xebf8 <animateFileActivityBusy+0xdc>
    eb8c: e3a04013     	mov	r4, #19
    eb90: e3a05001     	mov	r5, #1
    eb94: e08f0003     	add	r0, pc, r3
    eb98: e1c047fc     	strd	r4, r5, [r0, #124]
    eb9c: e28dd024     	add	sp, sp, #36
    eba0: e8bd8030     	pop	{r4, r5, pc}
    eba4: e3510000     	cmp	r1, #0
    eba8: aafffffb     	bge	0xeb9c <animateFileActivityBusy+0x80> @ imm = #-0x14
    ebac: e59f4048     	ldr	r4, [pc, #0x48]         @ 0xebfc <animateFileActivityBusy+0xe0>
    ebb0: e3a02001     	mov	r2, #1
    ebb4: e08fc004     	add	r12, pc, r4
    ebb8: e58c2080     	str	r2, [r12, #0x80]
    ebbc: e28dd024     	add	sp, sp, #36
    ebc0: e8bd8030     	pop	{r4, r5, pc}
    ebc4: e2803a01     	add	r3, r0, #4096
    ebc8: e59f0030     	ldr	r0, [pc, #0x30]         @ 0xec00 <animateFileActivityBusy+0xe4>
    ebcc: e08f0000     	add	r0, pc, r0
    ebd0: e5935dac     	ldr	r5, [r3, #0xdac]
    ebd4: ebffd2d3     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xb4b4
    ebd8: e1a0300d     	mov	r3, sp
    ebdc: e3a02004     	mov	r2, #4
    ebe0: e1a01000     	mov	r1, r0
    ebe4: e1a00005     	mov	r0, r5
    ebe8: ebffd421     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xaf7c
    ebec: e594107c     	ldr	r1, [r4, #0x7c]
    ebf0: eaffffe2     	b	0xeb80 <animateFileActivityBusy+0x64> @ imm = #-0x78
    ebf4: b0 87 01 00  	.word	0x000187b0
    ebf8: 50 87 01 00  	.word	0x00018750
    ebfc: 30 87 01 00  	.word	0x00018730
    ec00: e8 5e 00 00  	.word	0x00005ee8

