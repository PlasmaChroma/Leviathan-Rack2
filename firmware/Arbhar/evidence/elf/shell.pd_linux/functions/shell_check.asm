0000142c <shell_check>:
    142c: e92d4010     	push	{r4, lr}
    1430: e24dd008     	sub	sp, sp, #8
    1434: e1a04000     	mov	r4, r0
    1438: e3a02001     	mov	r2, #1
    143c: e5900040     	ldr	r0, [r0, #0x40]
    1440: e28d1004     	add	r1, sp, #4
    1444: ebfffd7d     	bl	0xa40 <.plt+0xd4>       @ imm = #-0xa0c
    1448: e5943040     	ldr	r3, [r4, #0x40]
    144c: e1530000     	cmp	r3, r0
    1450: 0a000009     	beq	0x147c <shell_check+0x50> @ imm = #0x24
    1454: e5941044     	ldr	r1, [r4, #0x44]
    1458: e594004c     	ldr	r0, [r4, #0x4c]
    145c: e3510063     	cmp	r1, #99
    1460: d2811002     	addle	r1, r1, #2
    1464: d5841044     	strle	r1, [r4, #0x44]
    1468: ee071a90     	vmov	s15, r1
    146c: eeb80be7     	vcvt.f64.s32	d0, s15
    1470: ebfffd6f     	bl	0xa34 <.plt+0xc8>       @ imm = #-0xa44
    1474: e28dd008     	add	sp, sp, #8
    1478: e8bd8010     	pop	{r4, pc}
    147c: e1a00004     	mov	r0, r4
    1480: ebfffd5c     	bl	0x9f8 <.plt+0x8c>       @ imm = #-0xa90
    1484: e59d0004     	ldr	r0, [sp, #0x4]
    1488: e310007f     	tst	r0, #127
    148c: 07e70450     	ubfxeq	r0, r0, #0x8, #0x8
    1490: 15940048     	ldrne	r0, [r4, #0x48]
    1494: 0e000a10     	vmoveq	s0, r0
    1498: 05940048     	ldreq	r0, [r4, #0x48]
    149c: 1d9f0a03     	vldrne	s0, [pc, #12]           @ 0x14b0 <shell_check+0x84>
    14a0: 0eb80ac0     	vcvteq.f32.s32	s0, s0
    14a4: ebfffd98     	bl	0xb0c <.plt+0x1a0>      @ imm = #-0x9a0
    14a8: e28dd008     	add	sp, sp, #8
    14ac: e8bd8010     	pop	{r4, pc}
    14b0: 00 00 00 00  	.word	0x00000000

