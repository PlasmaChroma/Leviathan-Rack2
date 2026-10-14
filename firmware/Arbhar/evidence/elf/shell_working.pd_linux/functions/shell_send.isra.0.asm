000015c8 <shell_send.isra.0>:
    15c8: e5903000     	ldr	r3, [r0]
    15cc: e3730001     	cmn	r3, #1
    15d0: 012fff1e     	bxeq	lr
    15d4: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
    15d8: e1a07001     	mov	r7, r1
    15dc: e24dde3f     	sub	sp, sp, #1008
    15e0: e3570000     	cmp	r7, #0
    15e4: e28d1008     	add	r1, sp, #8
    15e8: e1a04002     	mov	r4, r2
    15ec: e1a06000     	mov	r6, r0
    15f0: e58d1004     	str	r1, [sp, #0x4]
    15f4: da000081     	ble	0x1800 <shell_send.isra.0+0x238> @ imm = #0x204
    15f8: e3a02ffa     	mov	r2, #1000
    15fc: e1a00004     	mov	r0, r4
    1600: ebfffdcd     	bl	0xd3c <.plt+0x230>      @ imm = #-0x8cc
    1604: e59d0004     	ldr	r0, [sp, #0x4]
    1608: ebfffd95     	bl	0xc64 <.plt+0x158>      @ imm = #-0x9ac
    160c: e28d2e3f     	add	r2, sp, #1008
    1610: e3a08001     	mov	r8, #1
    1614: e1580007     	cmp	r8, r7
    1618: e2479001     	sub	r9, r7, #1
    161c: e3a05020     	mov	r5, #32
    1620: e209a003     	and	r10, r9, #3
    1624: e2844008     	add	r4, r4, #8
    1628: e082c000     	add	r12, r2, r0
    162c: e0801008     	add	r1, r0, r8
    1630: e54c53e8     	strb	r5, [r12, #-0x3e8]
    1634: 0a000061     	beq	0x17c0 <shell_send.isra.0+0x1f8> @ imm = #0x184
    1638: e35a0000     	cmp	r10, #0
    163c: 0a00002c     	beq	0x16f4 <shell_send.isra.0+0x12c> @ imm = #0xb0
    1640: e35a0001     	cmp	r10, #1
    1644: 0a00001b     	beq	0x16b8 <shell_send.isra.0+0xf0> @ imm = #0x6c
    1648: e35a0002     	cmp	r10, #2
    164c: 0a00000c     	beq	0x1684 <shell_send.isra.0+0xbc> @ imm = #0x30
    1650: e59d0004     	ldr	r0, [sp, #0x4]
    1654: e2612ffa     	rsb	r2, r1, #1000
    1658: e28d8e3f     	add	r8, sp, #1008
    165c: e0801001     	add	r1, r0, r1
    1660: e1a00004     	mov	r0, r4
    1664: ebfffdb4     	bl	0xd3c <.plt+0x230>      @ imm = #-0x930
    1668: e59d0004     	ldr	r0, [sp, #0x4]
    166c: ebfffd7c     	bl	0xc64 <.plt+0x158>      @ imm = #-0xa10
    1670: e2844008     	add	r4, r4, #8
    1674: e0883000     	add	r3, r8, r0
    1678: e2801001     	add	r1, r0, #1
    167c: e3a08002     	mov	r8, #2
    1680: e54353e8     	strb	r5, [r3, #-0x3e8]
    1684: e59de004     	ldr	lr, [sp, #0x4]
    1688: e2612ffa     	rsb	r2, r1, #1000
    168c: e1a00004     	mov	r0, r4
    1690: e2888001     	add	r8, r8, #1
    1694: e08e1001     	add	r1, lr, r1
    1698: e2844008     	add	r4, r4, #8
    169c: ebfffda6     	bl	0xd3c <.plt+0x230>      @ imm = #-0x968
    16a0: e59d0004     	ldr	r0, [sp, #0x4]
    16a4: ebfffd6e     	bl	0xc64 <.plt+0x158>      @ imm = #-0xa48
    16a8: e28d1e3f     	add	r1, sp, #1008
    16ac: e0819000     	add	r9, r1, r0
    16b0: e2801001     	add	r1, r0, #1
    16b4: e54953e8     	strb	r5, [r9, #-0x3e8]
    16b8: e59da004     	ldr	r10, [sp, #0x4]
    16bc: e2612ffa     	rsb	r2, r1, #1000
    16c0: e1a00004     	mov	r0, r4
    16c4: e2888001     	add	r8, r8, #1
    16c8: e08a1001     	add	r1, r10, r1
    16cc: e2844008     	add	r4, r4, #8
    16d0: ebfffd99     	bl	0xd3c <.plt+0x230>      @ imm = #-0x99c
    16d4: e59d0004     	ldr	r0, [sp, #0x4]
    16d8: ebfffd61     	bl	0xc64 <.plt+0x158>      @ imm = #-0xa7c
    16dc: e28d2e3f     	add	r2, sp, #1008
    16e0: e1580007     	cmp	r8, r7
    16e4: e082c000     	add	r12, r2, r0
    16e8: e2801001     	add	r1, r0, #1
    16ec: e54c53e8     	strb	r5, [r12, #-0x3e8]
    16f0: 0a000032     	beq	0x17c0 <shell_send.isra.0+0x1f8> @ imm = #0xc8
    16f4: e59d0004     	ldr	r0, [sp, #0x4]
    16f8: e2612ffa     	rsb	r2, r1, #1000
    16fc: e2849008     	add	r9, r4, #8
    1700: e2888004     	add	r8, r8, #4
    1704: e0801001     	add	r1, r0, r1
    1708: e1a00004     	mov	r0, r4
    170c: ebfffd8a     	bl	0xd3c <.plt+0x230>      @ imm = #-0x9d8
    1710: e59d0004     	ldr	r0, [sp, #0x4]
    1714: ebfffd52     	bl	0xc64 <.plt+0x158>      @ imm = #-0xab8
    1718: e28d3e3f     	add	r3, sp, #1008
    171c: e59d1004     	ldr	r1, [sp, #0x4]
    1720: e083c000     	add	r12, r3, r0
    1724: e280a001     	add	r10, r0, #1
    1728: e26a2ffa     	rsb	r2, r10, #1000
    172c: e081100a     	add	r1, r1, r10
    1730: e1a00009     	mov	r0, r9
    1734: e54c53e8     	strb	r5, [r12, #-0x3e8]
    1738: ebfffd7f     	bl	0xd3c <.plt+0x230>      @ imm = #-0xa04
    173c: e59d0004     	ldr	r0, [sp, #0x4]
    1740: ebfffd47     	bl	0xc64 <.plt+0x158>      @ imm = #-0xae4
    1744: e289a008     	add	r10, r9, #8
    1748: e59dc004     	ldr	r12, [sp, #0x4]
    174c: e28d2e3f     	add	r2, sp, #1008
    1750: e2849018     	add	r9, r4, #24
    1754: e2844020     	add	r4, r4, #32
    1758: e0823000     	add	r3, r2, r0
    175c: e2801001     	add	r1, r0, #1
    1760: e2612ffa     	rsb	r2, r1, #1000
    1764: e1a0000a     	mov	r0, r10
    1768: e08c1001     	add	r1, r12, r1
    176c: e54353e8     	strb	r5, [r3, #-0x3e8]
    1770: ebfffd71     	bl	0xd3c <.plt+0x230>      @ imm = #-0xa3c
    1774: e59d0004     	ldr	r0, [sp, #0x4]
    1778: ebfffd39     	bl	0xc64 <.plt+0x158>      @ imm = #-0xb1c
    177c: e28d1e3f     	add	r1, sp, #1008
    1780: e0813000     	add	r3, r1, r0
    1784: e280a001     	add	r10, r0, #1
    1788: e59d0004     	ldr	r0, [sp, #0x4]
    178c: e26a2ffa     	rsb	r2, r10, #1000
    1790: e54353e8     	strb	r5, [r3, #-0x3e8]
    1794: e080100a     	add	r1, r0, r10
    1798: e1a00009     	mov	r0, r9
    179c: ebfffd66     	bl	0xd3c <.plt+0x230>      @ imm = #-0xa68
    17a0: e59d0004     	ldr	r0, [sp, #0x4]
    17a4: ebfffd2e     	bl	0xc64 <.plt+0x158>      @ imm = #-0xb48
    17a8: e28d9e3f     	add	r9, sp, #1008
    17ac: e1580007     	cmp	r8, r7
    17b0: e0892000     	add	r2, r9, r0
    17b4: e2801001     	add	r1, r0, #1
    17b8: e54253e8     	strb	r5, [r2, #-0x3e8]
    17bc: 1affffcc     	bne	0x16f4 <shell_send.isra.0+0x12c> @ imm = #-0xd0
    17c0: e59d1004     	ldr	r1, [sp, #0x4]
    17c4: e59fe03c     	ldr	lr, [pc, #0x3c]         @ 0x1808 <shell_send.isra.0+0x240>
    17c8: e28d7e3f     	add	r7, sp, #1008
    17cc: e0875000     	add	r5, r7, r0
    17d0: e3a08000     	mov	r8, #0
    17d4: e08f000e     	add	r0, pc, lr
    17d8: e54583e8     	strb	r8, [r5, #-0x3e8]
    17dc: ebfffd26     	bl	0xc7c <.plt+0x170>      @ imm = #-0xb68
    17e0: e59d0004     	ldr	r0, [sp, #0x4]
    17e4: ebfffd1e     	bl	0xc64 <.plt+0x158>      @ imm = #-0xb88
    17e8: e59d1004     	ldr	r1, [sp, #0x4]
    17ec: e1a02000     	mov	r2, r0
    17f0: e5960000     	ldr	r0, [r6]
    17f4: ebfffd23     	bl	0xc88 <.plt+0x17c>      @ imm = #-0xb74
    17f8: e28dde3f     	add	sp, sp, #1008
    17fc: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    1800: e3e00000     	mvn	r0, #0
    1804: eaffffee     	b	0x17c4 <shell_send.isra.0+0x1fc> @ imm = #-0x48
    1808: c0 05 00 00  	.word	0x000005c0

