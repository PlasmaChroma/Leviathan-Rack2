000027b4 <comport_float>:
    27b4: e5903020     	ldr	r3, [r0, #0x20]
    27b8: e92d4010     	push	{r4, lr}
    27bc: e3730001     	cmn	r3, #1
    27c0: 0a000012     	beq	0x2810 <comport_float+0x5c> @ imm = #0x48
    27c4: e2800a01     	add	r0, r0, #4096
    27c8: e59030fc     	ldr	r3, [r0, #0xfc]
    27cc: e59020f8     	ldr	r2, [r0, #0xf8]
    27d0: e1530002     	cmp	r3, r2
    27d4: aa000006     	bge	0x27f4 <comport_float+0x40> @ imm = #0x18
    27d8: eefd7ac0     	vcvt.s32.f32	s15, s0
    27dc: e59010f0     	ldr	r1, [r0, #0xf0]
    27e0: e283c001     	add	r12, r3, #1
    27e4: e580c0fc     	str	r12, [r0, #0xfc]
    27e8: ee172a90     	vmov	r2, s15
    27ec: e7c12003     	strb	r2, [r1, r3]
    27f0: e8bd8010     	pop	{r4, pc}
    27f4: e59f1024     	ldr	r1, [pc, #0x24]         @ 0x2820 <comport_float+0x6c>
    27f8: e08f0001     	add	r0, pc, r1
    27fc: ebfff8ce     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1cc8
    2800: e59fe01c     	ldr	lr, [pc, #0x1c]         @ 0x2824 <comport_float+0x70>
    2804: e08f000e     	add	r0, pc, lr
    2808: e8bd4010     	pop	{r4, lr}
    280c: eafff8ca     	b	0xb3c <.plt+0x134>      @ imm = #-0x1cd8
    2810: e59f4010     	ldr	r4, [pc, #0x10]         @ 0x2828 <comport_float+0x74>
    2814: e08f0004     	add	r0, pc, r4
    2818: ebfff8c7     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1ce4
    281c: eafffff7     	b	0x2800 <comport_float+0x4c> @ imm = #-0x24
    2820: 24 1e 00 00  	.word	0x00001e24
    2824: e0 20 00 00  	.word	0x000020e0
    2828: e4 1d 00 00  	.word	0x00001de4

