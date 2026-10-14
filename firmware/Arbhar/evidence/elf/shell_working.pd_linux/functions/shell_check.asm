0000187c <shell_check>:
    187c: e92d4010     	push	{r4, lr}
    1880: e24dd008     	sub	sp, sp, #8
    1884: e1a04000     	mov	r4, r0
    1888: e3a02001     	mov	r2, #1
    188c: e5900040     	ldr	r0, [r0, #0x40]
    1890: e28d1004     	add	r1, sp, #4
    1894: ebfffcce     	bl	0xbd4 <.plt+0xc8>       @ imm = #-0xcc8
    1898: e5943040     	ldr	r3, [r4, #0x40]
    189c: e1500003     	cmp	r0, r3
    18a0: 0a000009     	beq	0x18cc <shell_check+0x50> @ imm = #0x24
    18a4: e5941044     	ldr	r1, [r4, #0x44]
    18a8: e594004c     	ldr	r0, [r4, #0x4c]
    18ac: e3510063     	cmp	r1, #99
    18b0: d2811002     	addle	r1, r1, #2
    18b4: d5841044     	strle	r1, [r4, #0x44]
    18b8: ee071a90     	vmov	s15, r1
    18bc: eeb80be7     	vcvt.f64.s32	d0, s15
    18c0: ebfffcc0     	bl	0xbc8 <.plt+0xbc>       @ imm = #-0xd00
    18c4: e28dd008     	add	sp, sp, #8
    18c8: e8bd8010     	pop	{r4, pc}
    18cc: e1a00004     	mov	r0, r4
    18d0: ebfffcad     	bl	0xb8c <.plt+0x80>       @ imm = #-0xd4c
    18d4: e59d0004     	ldr	r0, [sp, #0x4]
    18d8: e310007f     	tst	r0, #127
    18dc: 07e70450     	ubfxeq	r0, r0, #0x8, #0x8
    18e0: 15940048     	ldrne	r0, [r4, #0x48]
    18e4: 0e000a10     	vmoveq	s0, r0
    18e8: 05940048     	ldreq	r0, [r4, #0x48]
    18ec: 1d9f0a03     	vldrne	s0, [pc, #12]           @ 0x1900 <shell_check+0x84>
    18f0: 0eb80ac0     	vcvteq.f32.s32	s0, s0
    18f4: ebfffcf2     	bl	0xcc4 <.plt+0x1b8>      @ imm = #-0xc38
    18f8: e28dd008     	add	sp, sp, #8
    18fc: e8bd8010     	pop	{r4, pc}
    1900: 00 00 00 00  	.word	0x00000000

