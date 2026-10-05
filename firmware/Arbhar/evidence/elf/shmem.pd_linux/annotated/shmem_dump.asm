000014b8 <shmem_dump>:
    14b8: e5901024     	ldr	r1, [r0, #0x24]
    14bc: e3510000     	cmp	r1, #0
    14c0: 0a000050     	beq	0x1608 <shmem_dump+0x150> @ imm = #0x140
    14c4: e3520000     	cmp	r2, #0
    14c8: da00004b     	ble	0x15fc <shmem_dump+0x144> @ imm = #0x12c
    14cc: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    14d0: e1a08002     	mov	r8, r2
    14d4: e5932000     	ldr	r2, [r3]
    14d8: e24dd014     	sub	sp, sp, #20
    14dc: e1a05000     	mov	r5, r0
    14e0: e1a06003     	mov	r6, r3
    14e4: e3520002     	cmp	r2, #2
    14e8: 0a000008     	beq	0x1510 <shmem_dump+0x58> @ imm = #0x20
    14ec: e2423001     	sub	r3, r2, #1
    14f0: e3580001     	cmp	r8, #1
    14f4: e16f0f13     	clz	r0, r3
    14f8: e1a042a0     	lsr	r4, r0, #5
    14fc: 03a04000     	moveq	r4, #0
    1500: e3540000     	cmp	r4, #0
    1504: 1a000015     	bne	0x1560 <shmem_dump+0xa8> @ imm = #0x54
    1508: e28dd014     	add	sp, sp, #20
    150c: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    1510: e3a09000     	mov	r9, #0
    1514: e1a04009     	mov	r4, r9
    1518: e1a02006     	mov	r2, r6
    151c: e1a01008     	mov	r1, r8
    1520: e1a00004     	mov	r0, r4
    1524: e2844001     	add	r4, r4, #1
    1528: ebfffcce     	bl	0x868 <.plt+0xf8>       @ imm = #-0xcc8  // CALL atom_getsymbolarg
    152c: e5951028     	ldr	r1, [r5, #0x28]
    1530: e1a02009     	mov	r2, r9
    1534: e3a03000     	mov	r3, #0
    1538: e58d1000     	str	r1, [sp]
    153c: e1a01000     	mov	r1, r0
    1540: e1a00005     	mov	r0, r5
    1544: ebfffca6     	bl	0x7e4 <.plt+0x74>       @ imm = #-0xd68  // CALL shmem_dump_tab
    1548: e7962184     	ldr	r2, [r6, r4, lsl #3]
    154c: e3520002     	cmp	r2, #2
    1550: e0899000     	add	r9, r9, r0
    1554: 0affffef     	beq	0x1518 <shmem_dump+0x60> @ imm = #-0x44
    1558: e28dd014     	add	sp, sp, #20
    155c: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    1560: e1a02006     	mov	r2, r6
    1564: e1a01008     	mov	r1, r8
    1568: e3a00000     	mov	r0, #0
    156c: ebfffcc3     	bl	0x880 <.plt+0x110>      @ imm = #-0xcf4  // CALL atom_getfloatarg
    1570: e1a02006     	mov	r2, r6
    1574: e1a01008     	mov	r1, r8
    1578: e3a00001     	mov	r0, #1
    157c: eebd0ac0     	vcvt.s32.f32	s0, s0
    1580: ee107a10     	vmov	r7, s0
    1584: ebfffcb7     	bl	0x868 <.plt+0xf8>       @ imm = #-0xd24  // CALL atom_getsymbolarg
    1588: e3580002     	cmp	r8, #2
    158c: 03a03000     	moveq	r3, #0
    1590: e1c74fc7     	bic	r4, r7, r7, asr #31
    1594: e5957028     	ldr	r7, [r5, #0x28]
    1598: e1a09000     	mov	r9, r0
    159c: 0a000010     	beq	0x15e4 <shmem_dump+0x12c> @ imm = #0x40
    15a0: e5963010     	ldr	r3, [r6, #0x10]
    15a4: e3530001     	cmp	r3, #1
    15a8: 13a03000     	movne	r3, #0
    15ac: 0a000018     	beq	0x1614 <shmem_dump+0x15c> @ imm = #0x60
    15b0: e3580003     	cmp	r8, #3
    15b4: 0a00000a     	beq	0x15e4 <shmem_dump+0x12c> @ imm = #0x28
    15b8: e596c018     	ldr	r12, [r6, #0x18]
    15bc: e35c0001     	cmp	r12, #1
    15c0: 1a000007     	bne	0x15e4 <shmem_dump+0x12c> @ imm = #0x1c
    15c4: e1a02006     	mov	r2, r6
    15c8: e1a01008     	mov	r1, r8
    15cc: e3a00003     	mov	r0, #3
    15d0: e58d300c     	str	r3, [sp, #0xc]
    15d4: ebfffca9     	bl	0x880 <.plt+0x110>      @ imm = #-0xd5c  // CALL atom_getfloatarg
    15d8: e59d300c     	ldr	r3, [sp, #0xc]
    15dc: eefd0ac0     	vcvt.s32.f32	s1, s0
    15e0: ee107a90     	vmov	r7, s1
    15e4: e58d7000     	str	r7, [sp]
    15e8: e1a02004     	mov	r2, r4
    15ec: e1a01009     	mov	r1, r9
    15f0: e1a00005     	mov	r0, r5
    15f4: ebfffc7a     	bl	0x7e4 <.plt+0x74>       @ imm = #-0xe18  // CALL shmem_dump_tab
    15f8: eaffffc2     	b	0x1508 <shmem_dump+0x50> @ imm = #-0xf8
    15fc: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x1630 <shmem_dump+0x178>  // u32=0x3ac; f32?=1.31722056e-42
    1600: e08f1003     	add	r1, pc, r3
    1604: eafffca0     	b	0x88c <.plt+0x11c>      @ imm = #-0xd80  // CALL pd_error
    1608: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x1634 <shmem_dump+0x17c>  // u32=0x36c; f32?=1.22753745e-42
    160c: e08f0000     	add	r0, pc, r0
    1610: eafffc70     	b	0x7d8 <.plt+0x68>       @ imm = #-0xe40  // CALL error
    1614: e1a02006     	mov	r2, r6
    1618: e1a01008     	mov	r1, r8
    161c: e3a00002     	mov	r0, #2
    1620: ebfffc96     	bl	0x880 <.plt+0x110>      @ imm = #-0xda8  // CALL atom_getfloatarg
    1624: eefd7ac0     	vcvt.s32.f32	s15, s0
    1628: ee173a90     	vmov	r3, s15
    162c: eaffffdf     	b	0x15b0 <shmem_dump+0xf8> @ imm = #-0x84
    1630: ac 03 00 00  	.word	0x000003ac
    1634: 6c 03 00 00  	.word	0x0000036c

