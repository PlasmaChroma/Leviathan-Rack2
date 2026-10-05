0000150c <arbhar_shmem_free>:
    150c: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
    1510: e1a05000     	mov	r5, r0
    1514: e2807088     	add	r7, r0, #136
    1518: e5b50058     	ldr	r0, [r5, #0x58]!
    151c: e59f82c4     	ldr	r8, [pc, #0x2c4]        @ 0x17e8 <arbhar_shmem_free+0x2dc>
    1520: e3a06000     	mov	r6, #0
    1524: e59f92c0     	ldr	r9, [pc, #0x2c0]        @ 0x17ec <arbhar_shmem_free+0x2e0>
    1528: e1500006     	cmp	r0, r6
    152c: e24dd058     	sub	sp, sp, #88
    1530: e08f8008     	add	r8, pc, r8
    1534: e08f9009     	add	r9, pc, r9
    1538: 0a000002     	beq	0x1548 <arbhar_shmem_free+0x3c> @ imm = #0x8
    153c: ebfffd76     	bl	0xb1c <.plt+0x128>      @ imm = #-0xa28
    1540: e3700001     	cmn	r0, #1
    1544: 0a0000a3     	beq	0x17d8 <arbhar_shmem_free+0x2cc> @ imm = #0x28c
    1548: e515403c     	ldr	r4, [r5, #-0x3c]
    154c: e5856000     	str	r6, [r5]
    1550: e3540000     	cmp	r4, #0
    1554: da000023     	ble	0x15e8 <arbhar_shmem_free+0xdc> @ imm = #0x8c
    1558: ea00007b     	b	0x174c <arbhar_shmem_free+0x240> @ imm = #0x1ec
    155c: e5940004     	ldr	r0, [r4, #0x4]
    1560: e2845004     	add	r5, r4, #4
    1564: e3500000     	cmp	r0, #0
    1568: 0a000002     	beq	0x1578 <arbhar_shmem_free+0x6c> @ imm = #0x8
    156c: ebfffd6a     	bl	0xb1c <.plt+0x128>      @ imm = #-0xa58
    1570: e3700001     	cmn	r0, #1
    1574: 0a00008f     	beq	0x17b8 <arbhar_shmem_free+0x2ac> @ imm = #0x23c
    1578: e515503c     	ldr	r5, [r5, #-0x3c]
    157c: e5846004     	str	r6, [r4, #0x4]
    1580: e3550000     	cmp	r5, #0
    1584: ca000048     	bgt	0x16ac <arbhar_shmem_free+0x1a0> @ imm = #0x120
    1588: e5940008     	ldr	r0, [r4, #0x8]
    158c: e2845008     	add	r5, r4, #8
    1590: e3500000     	cmp	r0, #0
    1594: 0a000002     	beq	0x15a4 <arbhar_shmem_free+0x98> @ imm = #0x8
    1598: ebfffd5f     	bl	0xb1c <.plt+0x128>      @ imm = #-0xa84
    159c: e3700001     	cmn	r0, #1
    15a0: 0a000080     	beq	0x17a8 <arbhar_shmem_free+0x29c> @ imm = #0x200
    15a4: e515503c     	ldr	r5, [r5, #-0x3c]
    15a8: e5846008     	str	r6, [r4, #0x8]
    15ac: e3550000     	cmp	r5, #0
    15b0: ca000050     	bgt	0x16f8 <arbhar_shmem_free+0x1ec> @ imm = #0x140
    15b4: e594000c     	ldr	r0, [r4, #0xc]
    15b8: e284500c     	add	r5, r4, #12
    15bc: e3500000     	cmp	r0, #0
    15c0: 0a000002     	beq	0x15d0 <arbhar_shmem_free+0xc4> @ imm = #0x8
    15c4: ebfffd54     	bl	0xb1c <.plt+0x128>      @ imm = #-0xab0
    15c8: e3700001     	cmn	r0, #1
    15cc: 0a000071     	beq	0x1798 <arbhar_shmem_free+0x28c> @ imm = #0x1c4
    15d0: e515a03c     	ldr	r10, [r5, #-0x3c]
    15d4: e584600c     	str	r6, [r4, #0xc]
    15d8: e35a0000     	cmp	r10, #0
    15dc: ca00001f     	bgt	0x1660 <arbhar_shmem_free+0x154> @ imm = #0x7c
    15e0: e1550007     	cmp	r5, r7
    15e4: 0a000056     	beq	0x1744 <arbhar_shmem_free+0x238> @ imm = #0x158
    15e8: e5950004     	ldr	r0, [r5, #0x4]
    15ec: e2854004     	add	r4, r5, #4
    15f0: e3500000     	cmp	r0, #0
    15f4: 0a000002     	beq	0x1604 <arbhar_shmem_free+0xf8> @ imm = #0x8
    15f8: ebfffd47     	bl	0xb1c <.plt+0x128>      @ imm = #-0xae4
    15fc: e3700001     	cmn	r0, #1
    1600: 0a000070     	beq	0x17c8 <arbhar_shmem_free+0x2bc> @ imm = #0x1c0
    1604: e514503c     	ldr	r5, [r4, #-0x3c]
    1608: e5846000     	str	r6, [r4]
    160c: e3550000     	cmp	r5, #0
    1610: daffffd1     	ble	0x155c <arbhar_shmem_free+0x50> @ imm = #-0xbc
    1614: e28da004     	add	r10, sp, #4
    1618: e3a01002     	mov	r1, #2
    161c: e1a00005     	mov	r0, r5
    1620: e1a0200a     	mov	r2, r10
    1624: ebfffd18     	bl	0xa8c <.plt+0x98>       @ imm = #-0xba0
    1628: e3700001     	cmn	r0, #1
    162c: 0affffca     	beq	0x155c <arbhar_shmem_free+0x50> @ imm = #-0xd8
    1630: e59d104c     	ldr	r1, [sp, #0x4c]
    1634: e3510000     	cmp	r1, #0
    1638: 1affffc7     	bne	0x155c <arbhar_shmem_free+0x50> @ imm = #-0xe4
    163c: e1a0200a     	mov	r2, r10
    1640: e1a00005     	mov	r0, r5
    1644: ebfffd10     	bl	0xa8c <.plt+0x98>       @ imm = #-0xbc0
    1648: e3700001     	cmn	r0, #1
    164c: 1affffc2     	bne	0x155c <arbhar_shmem_free+0x50> @ imm = #-0xf8
    1650: e1a01005     	mov	r1, r5
    1654: e1a00009     	mov	r0, r9
    1658: ebfffd0e     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xbc8
    165c: eaffffbe     	b	0x155c <arbhar_shmem_free+0x50> @ imm = #-0x108
    1660: e28d4004     	add	r4, sp, #4
    1664: e3a01002     	mov	r1, #2
    1668: e1a0000a     	mov	r0, r10
    166c: e1a02004     	mov	r2, r4
    1670: ebfffd05     	bl	0xa8c <.plt+0x98>       @ imm = #-0xbec
    1674: e3700001     	cmn	r0, #1
    1678: 0affffd8     	beq	0x15e0 <arbhar_shmem_free+0xd4> @ imm = #-0xa0
    167c: e59d104c     	ldr	r1, [sp, #0x4c]
    1680: e3510000     	cmp	r1, #0
    1684: 1affffd5     	bne	0x15e0 <arbhar_shmem_free+0xd4> @ imm = #-0xac
    1688: e1a02004     	mov	r2, r4
    168c: e1a0000a     	mov	r0, r10
    1690: ebfffcfd     	bl	0xa8c <.plt+0x98>       @ imm = #-0xc0c
    1694: e3700001     	cmn	r0, #1
    1698: 1affffd0     	bne	0x15e0 <arbhar_shmem_free+0xd4> @ imm = #-0xc0
    169c: e1a0100a     	mov	r1, r10
    16a0: e1a00009     	mov	r0, r9
    16a4: ebfffcfb     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xc14
    16a8: eaffffcc     	b	0x15e0 <arbhar_shmem_free+0xd4> @ imm = #-0xd0
    16ac: e28da004     	add	r10, sp, #4
    16b0: e3a01002     	mov	r1, #2
    16b4: e1a00005     	mov	r0, r5
    16b8: e1a0200a     	mov	r2, r10
    16bc: ebfffcf2     	bl	0xa8c <.plt+0x98>       @ imm = #-0xc38
    16c0: e3700001     	cmn	r0, #1
    16c4: 0affffaf     	beq	0x1588 <arbhar_shmem_free+0x7c> @ imm = #-0x144
    16c8: e59d104c     	ldr	r1, [sp, #0x4c]
    16cc: e3510000     	cmp	r1, #0
    16d0: 1affffac     	bne	0x1588 <arbhar_shmem_free+0x7c> @ imm = #-0x150
    16d4: e1a0200a     	mov	r2, r10
    16d8: e1a00005     	mov	r0, r5
    16dc: ebfffcea     	bl	0xa8c <.plt+0x98>       @ imm = #-0xc58
    16e0: e3700001     	cmn	r0, #1
    16e4: 1affffa7     	bne	0x1588 <arbhar_shmem_free+0x7c> @ imm = #-0x164
    16e8: e1a01005     	mov	r1, r5
    16ec: e1a00009     	mov	r0, r9
    16f0: ebfffce8     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xc60
    16f4: eaffffa3     	b	0x1588 <arbhar_shmem_free+0x7c> @ imm = #-0x174
    16f8: e28da004     	add	r10, sp, #4
    16fc: e3a01002     	mov	r1, #2
    1700: e1a00005     	mov	r0, r5
    1704: e1a0200a     	mov	r2, r10
    1708: ebfffcdf     	bl	0xa8c <.plt+0x98>       @ imm = #-0xc84
    170c: e3700001     	cmn	r0, #1
    1710: 0affffa7     	beq	0x15b4 <arbhar_shmem_free+0xa8> @ imm = #-0x164
    1714: e59d104c     	ldr	r1, [sp, #0x4c]
    1718: e3510000     	cmp	r1, #0
    171c: 1affffa4     	bne	0x15b4 <arbhar_shmem_free+0xa8> @ imm = #-0x170
    1720: e1a0200a     	mov	r2, r10
    1724: e1a00005     	mov	r0, r5
    1728: ebfffcd7     	bl	0xa8c <.plt+0x98>       @ imm = #-0xca4
    172c: e3700001     	cmn	r0, #1
    1730: 1affff9f     	bne	0x15b4 <arbhar_shmem_free+0xa8> @ imm = #-0x184
    1734: e1a01005     	mov	r1, r5
    1738: e1a00009     	mov	r0, r9
    173c: ebfffcd5     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xcac
    1740: eaffff9b     	b	0x15b4 <arbhar_shmem_free+0xa8> @ imm = #-0x194
    1744: e28dd058     	add	sp, sp, #88
    1748: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    174c: e28da004     	add	r10, sp, #4
    1750: e3a01002     	mov	r1, #2
    1754: e1a00004     	mov	r0, r4
    1758: e1a0200a     	mov	r2, r10
    175c: ebfffcca     	bl	0xa8c <.plt+0x98>       @ imm = #-0xcd8
    1760: e3700001     	cmn	r0, #1
    1764: 0affff9f     	beq	0x15e8 <arbhar_shmem_free+0xdc> @ imm = #-0x184
    1768: e59d104c     	ldr	r1, [sp, #0x4c]
    176c: e3510000     	cmp	r1, #0
    1770: 1affff9c     	bne	0x15e8 <arbhar_shmem_free+0xdc> @ imm = #-0x190
    1774: e1a0200a     	mov	r2, r10
    1778: e1a00004     	mov	r0, r4
    177c: ebfffcc2     	bl	0xa8c <.plt+0x98>       @ imm = #-0xcf8
    1780: e3700001     	cmn	r0, #1
    1784: 1affff97     	bne	0x15e8 <arbhar_shmem_free+0xdc> @ imm = #-0x1a4
    1788: e1a01004     	mov	r1, r4
    178c: e1a00009     	mov	r0, r9
    1790: ebfffcc0     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xd00
    1794: eaffff93     	b	0x15e8 <arbhar_shmem_free+0xdc> @ imm = #-0x1b4
    1798: e594100c     	ldr	r1, [r4, #0xc]
    179c: e1a00008     	mov	r0, r8
    17a0: ebfffcbc     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xd10
    17a4: eaffff89     	b	0x15d0 <arbhar_shmem_free+0xc4> @ imm = #-0x1dc
    17a8: e5941008     	ldr	r1, [r4, #0x8]
    17ac: e1a00008     	mov	r0, r8
    17b0: ebfffcb8     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xd20
    17b4: eaffff7a     	b	0x15a4 <arbhar_shmem_free+0x98> @ imm = #-0x218
    17b8: e5941004     	ldr	r1, [r4, #0x4]
    17bc: e1a00008     	mov	r0, r8
    17c0: ebfffcb4     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xd30
    17c4: eaffff6b     	b	0x1578 <arbhar_shmem_free+0x6c> @ imm = #-0x254
    17c8: e5941000     	ldr	r1, [r4]
    17cc: e1a00008     	mov	r0, r8
    17d0: ebfffcb0     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xd40
    17d4: eaffff8a     	b	0x1604 <arbhar_shmem_free+0xf8> @ imm = #-0x1d8
    17d8: e5951000     	ldr	r1, [r5]
    17dc: e1a00008     	mov	r0, r8
    17e0: ebfffcac     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xd50
    17e4: eaffff57     	b	0x1548 <arbhar_shmem_free+0x3c> @ imm = #-0x2a4
    17e8: d4 16 00 00  	.word	0x000016d4
    17ec: e4 16 00 00  	.word	0x000016e4

