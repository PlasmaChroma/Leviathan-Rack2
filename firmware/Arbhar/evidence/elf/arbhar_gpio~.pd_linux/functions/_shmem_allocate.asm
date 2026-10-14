0000712c <_shmem_allocate>:
    712c: eefd7ac0     	vcvt.s32.f32	s15, s0
    7130: e92d4030     	push	{r4, r5, lr}
    7134: e24dd05c     	sub	sp, sp, #92
    7138: ee175a90     	vmov	r5, s15
    713c: e3550000     	cmp	r5, #0
    7140: da00003a     	ble	0x7230 <_shmem_allocate+0x104> @ imm = #0xe8
    7144: eefd0ae0     	vcvt.s32.f32	s1, s1
    7148: e2804a01     	add	r4, r0, #4096
    714c: e5940dc0     	ldr	r0, [r4, #0xdc0]
    7150: e3500000     	cmp	r0, #0
    7154: ee103a90     	vmov	r3, s1
    7158: e5843dc4     	str	r3, [r4, #0xdc4]
    715c: 0a000002     	beq	0x716c <_shmem_allocate+0x40> @ imm = #0x8
    7160: ebfff293     	bl	0x3bb4 <.plt+0x4b8>     @ imm = #-0x35b4
    7164: e3a00000     	mov	r0, #0
    7168: e5840dc0     	str	r0, [r4, #0xdc0]
    716c: e5940dbc     	ldr	r0, [r4, #0xdbc]
    7170: e3700001     	cmn	r0, #1
    7174: 1a000015     	bne	0x71d0 <_shmem_allocate+0xa4> @ imm = #0x54
    7178: e594cdc4     	ldr	r12, [r4, #0xdc4]
    717c: e3002386     	movw	r2, #0x386
    7180: e1a00005     	mov	r0, r5
    7184: e1a0110c     	lsl	r1, r12, #2
    7188: ebfff1b4     	bl	0x3860 <.plt+0x164>     @ imm = #-0x3930
    718c: e3700001     	cmn	r0, #1
    7190: e5840dbc     	str	r0, [r4, #0xdbc]
    7194: 0a000013     	beq	0x71e8 <_shmem_allocate+0xbc> @ imm = #0x4c
    7198: e3a02000     	mov	r2, #0
    719c: e1a01002     	mov	r1, r2
    71a0: ebfff1cf     	bl	0x38e4 <.plt+0x1e8>     @ imm = #-0x38c4
    71a4: e28d2004     	add	r2, sp, #4
    71a8: e3a01002     	mov	r1, #2
    71ac: e5840dc0     	str	r0, [r4, #0xdc0]
    71b0: e5940dbc     	ldr	r0, [r4, #0xdbc]
    71b4: ebfff1f7     	bl	0x3998 <.plt+0x29c>     @ imm = #-0x3824
    71b8: e5942dc4     	ldr	r2, [r4, #0xdc4]
    71bc: e59d3028     	ldr	r3, [sp, #0x28]
    71c0: e1530102     	cmp	r3, r2, lsl #2
    71c4: ba000010     	blt	0x720c <_shmem_allocate+0xe0> @ imm = #0x40
    71c8: e28dd05c     	add	sp, sp, #92
    71cc: e8bd8030     	pop	{r4, r5, pc}
    71d0: e3a02000     	mov	r2, #0
    71d4: e1a01002     	mov	r1, r2
    71d8: ebfff1ee     	bl	0x3998 <.plt+0x29c>     @ imm = #-0x3848
    71dc: e3e01000     	mvn	r1, #0
    71e0: e5841dbc     	str	r1, [r4, #0xdbc]
    71e4: eaffffe3     	b	0x7178 <_shmem_allocate+0x4c> @ imm = #-0x74
    71e8: e59fe054     	ldr	lr, [pc, #0x54]         @ 0x7244 <_shmem_allocate+0x118>
    71ec: e1a01005     	mov	r1, r5
    71f0: e5942dc4     	ldr	r2, [r4, #0xdc4]
    71f4: e3a05000     	mov	r5, #0
    71f8: e08f000e     	add	r0, pc, lr
    71fc: ebfff1ee     	bl	0x39bc <.plt+0x2c0>     @ imm = #-0x3848
    7200: e5845dc4     	str	r5, [r4, #0xdc4]
    7204: e28dd05c     	add	sp, sp, #92
    7208: e8bd8030     	pop	{r4, r5, pc}
    720c: e59f0034     	ldr	r0, [pc, #0x34]         @ 0x7248 <_shmem_allocate+0x11c>
    7210: e1a01005     	mov	r1, r5
    7214: e08f0000     	add	r0, pc, r0
    7218: ebfff1e7     	bl	0x39bc <.plt+0x2c0>     @ imm = #-0x3864
    721c: e3a01000     	mov	r1, #0
    7220: e5841dc4     	str	r1, [r4, #0xdc4]
    7224: e5841dc0     	str	r1, [r4, #0xdc0]
    7228: e28dd05c     	add	sp, sp, #92
    722c: e8bd8030     	pop	{r4, r5, pc}
    7230: e59f4014     	ldr	r4, [pc, #0x14]         @ 0x724c <_shmem_allocate+0x120>
    7234: e08f0004     	add	r0, pc, r4
    7238: ebfff1df     	bl	0x39bc <.plt+0x2c0>     @ imm = #-0x3884
    723c: e28dd05c     	add	sp, sp, #92
    7240: e8bd8030     	pop	{r4, r5, pc}
    7244: ec db 00 00  	.word	0x0000dbec
    7248: d0 db 00 00  	.word	0x0000dbd0
    724c: 9c db 00 00  	.word	0x0000db9c

