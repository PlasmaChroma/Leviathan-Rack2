00001568 <open_serial>:
    1568: e59f24e8     	ldr	r2, [pc, #0x4e8]        @ 0x1a58 <open_serial+0x4f0>
    156c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1570: e24dd03c     	sub	sp, sp, #60
    1574: e1500002     	cmp	r0, r2
    1578: 13500062     	cmpne	r0, #98
    157c: e2813024     	add	r3, r1, #36
    1580: e1a05000     	mov	r5, r0
    1584: 83a02001     	movhi	r2, #1
    1588: 93a02000     	movls	r2, #0
    158c: e58d3008     	str	r3, [sp, #0x8]
    1590: 8a000101     	bhi	0x199c <open_serial+0x434> @ imm = #0x404
    1594: e28160a0     	add	r6, r1, #160
    1598: e28d8014     	add	r8, sp, #20
    159c: e1a04001     	mov	r4, r1
    15a0: e2819060     	add	r9, r1, #96
    15a4: e1a00006     	mov	r0, r6
    15a8: e1a03008     	mov	r3, r8
    15ac: e1a01002     	mov	r1, r2
    15b0: ebfffd6a     	bl	0xb60 <.plt+0x158>      @ imm = #-0xa58
    15b4: e3500002     	cmp	r0, #2
    15b8: 0a0000ef     	beq	0x197c <open_serial+0x414> @ imm = #0x3bc
    15bc: e3500003     	cmp	r0, #3
    15c0: 0a000062     	beq	0x1750 <open_serial+0x1e8> @ imm = #0x188
    15c4: e3500001     	cmp	r0, #1
    15c8: 1a000004     	bne	0x15e0 <open_serial+0x78> @ imm = #0x10
    15cc: e59f1488     	ldr	r1, [pc, #0x488]        @ 0x1a5c <open_serial+0x4f4>
    15d0: e1a02006     	mov	r2, r6
    15d4: e08f1001     	add	r1, pc, r1
    15d8: e1a00004     	mov	r0, r4
    15dc: ebfffd80     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0xa00
    15e0: e59f2470     	ldr	r2, [pc, #0x470]        @ 0x1a58 <open_serial+0x4f0>
    15e4: e1550002     	cmp	r5, r2
    15e8: 0a000060     	beq	0x1770 <open_serial+0x208> @ imm = #0x180
    15ec: e59da014     	ldr	r10, [sp, #0x14]
    15f0: e155000a     	cmp	r5, r10
    15f4: 2a0000f9     	bhs	0x19e0 <open_serial+0x478> @ imm = #0x3e4
    15f8: e59db018     	ldr	r11, [sp, #0x18]
    15fc: e79b0105     	ldr	r0, [r11, r5, lsl #2]
    1600: ebfffd05     	bl	0xa1c <.plt+0x14>       @ imm = #-0xbec
    1604: e584009c     	str	r0, [r4, #0x9c]
    1608: e1a00008     	mov	r0, r8
    160c: ebfffd77     	bl	0xbf0 <.plt+0x1e8>      @ imm = #-0xa24
    1610: e594309c     	ldr	r3, [r4, #0x9c]
    1614: e59f1444     	ldr	r1, [pc, #0x444]        @ 0x1a60 <open_serial+0x4f8>
    1618: e5930000     	ldr	r0, [r3]
    161c: ebfffd31     	bl	0xae8 <.plt+0xe0>       @ imm = #-0xb3c
    1620: e3700001     	cmn	r0, #1
    1624: e1a06000     	mov	r6, r0
    1628: 0a0000f3     	beq	0x19fc <open_serial+0x494> @ imm = #0x3cc
    162c: e3a02b02     	mov	r2, #2048
    1630: e3a01004     	mov	r1, #4
    1634: ebfffd34     	bl	0xb0c <.plt+0x104>      @ imm = #-0xb30
    1638: e59d1008     	ldr	r1, [sp, #0x8]
    163c: e1a00006     	mov	r0, r6
    1640: ebfffd6d     	bl	0xbfc <.plt+0x1f4>      @ imm = #-0xa4c
    1644: e3700001     	cmn	r0, #1
    1648: 0a0000da     	beq	0x19b8 <open_serial+0x450> @ imm = #0x368
    164c: e1a01009     	mov	r1, r9
    1650: e1a00006     	mov	r0, r6
    1654: ebfffd68     	bl	0xbfc <.plt+0x1f4>      @ imm = #-0xa60
    1658: e3700001     	cmn	r0, #1
    165c: 0a0000d5     	beq	0x19b8 <open_serial+0x450> @ imm = #0x354
    1660: e2847a01     	add	r7, r4, #4096
    1664: e594e06c     	ldr	lr, [r4, #0x6c]
    1668: edd77a2a     	vldr	s15, [r7, #168]
    166c: e5941064     	ldr	r1, [r4, #0x64]
    1670: e5948068     	ldr	r8, [r4, #0x68]
    1674: e3ce201b     	bic	r2, lr, #27
    1678: eebd0ae7     	vcvt.s32.f32	s0, s15
    167c: e3c1b001     	bic	r11, r1, #1
    1680: e3c8a030     	bic	r10, r8, #48
    1684: e584206c     	str	r2, [r4, #0x6c]
    1688: e584b064     	str	r11, [r4, #0x64]
    168c: ee100a10     	vmov	r0, s0
    1690: e3500006     	cmp	r0, #6
    1694: 038aae89     	orreq	r10, r10, #2192
    1698: 0a000005     	beq	0x16b4 <open_serial+0x14c> @ imm = #0x14
    169c: e3500007     	cmp	r0, #7
    16a0: 038aae8a     	orreq	r10, r10, #2208
    16a4: 0a000002     	beq	0x16b4 <open_serial+0x14c> @ imm = #0x8
    16a8: e3500005     	cmp	r0, #5
    16ac: 038aad22     	orreq	r10, r10, #2176
    16b0: 138aae8b     	orrne	r10, r10, #2224
    16b4: edd70a2c     	vldr	s1, [r7, #176]
    16b8: e59730b8     	ldr	r3, [r7, #0xb8]
    16bc: e597e0b4     	ldr	lr, [r7, #0xb4]
    16c0: e5942060     	ldr	r2, [r4, #0x60]
    16c4: eebd1ae0     	vcvt.s32.f32	s2, s1
    16c8: e1a00004     	mov	r0, r4
    16cc: ed970a29     	vldr	s0, [r7, #164]
    16d0: ee11ca10     	vmov	r12, s2
    16d4: e35c0001     	cmp	r12, #1
    16d8: 038aa040     	orreq	r10, r10, #64
    16dc: 13caa040     	bicne	r10, r10, #64
    16e0: e3530001     	cmp	r3, #1
    16e4: 038aa102     	orreq	r10, r10, #-2147483648
    16e8: 13caa102     	bicne	r10, r10, #-2147483648
    16ec: e35e0001     	cmp	lr, #1
    16f0: 03822b07     	orreq	r2, r2, #7168
    16f4: 13c22b07     	bicne	r2, r2, #7168
    16f8: e5842060     	str	r2, [r4, #0x60]
    16fc: e584a068     	str	r10, [r4, #0x68]
    1700: ebfffefa     	bl	0x12f0 <set_baudrate>   @ imm = #-0x418
    1704: e3a01000     	mov	r1, #0
    1708: e1a02009     	mov	r2, r9
    170c: e58710d0     	str	r1, [r7, #0xd0]
    1710: e1a00006     	mov	r0, r6
    1714: e3a01002     	mov	r1, #2
    1718: ebfffce3     	bl	0xaac <.plt+0xa4>       @ imm = #-0xc74
    171c: e594809c     	ldr	r8, [r4, #0x9c]
    1720: e3700001     	cmn	r0, #1
    1724: e1a09000     	mov	r9, r0
    1728: 0a0000c1     	beq	0x1a34 <open_serial+0x4cc> @ imm = #0x304
    172c: e59f0330     	ldr	r0, [pc, #0x330]        @ 0x1a64 <open_serial+0x4fc>
    1730: e1a01005     	mov	r1, r5
    1734: e08f0000     	add	r0, pc, r0
    1738: e5982000     	ldr	r2, [r8]
    173c: ebfffcfe     	bl	0xb3c <.plt+0x134>      @ imm = #-0xc08
    1740: e1c75ab0     	strh	r5, [r7, #160]
    1744: e1a00006     	mov	r0, r6
    1748: e28dd03c     	add	sp, sp, #60
    174c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1750: e59f0310     	ldr	r0, [pc, #0x310]        @ 0x1a68 <open_serial+0x500>
    1754: e1a02006     	mov	r2, r6
    1758: e08f1000     	add	r1, pc, r0
    175c: e1a00004     	mov	r0, r4
    1760: ebfffd1f     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0xb84
    1764: e59f22ec     	ldr	r2, [pc, #0x2ec]        @ 0x1a58 <open_serial+0x4f0>
    1768: e1550002     	cmp	r5, r2
    176c: 1affff9e     	bne	0x15ec <open_serial+0x84> @ imm = #-0x188
    1770: e59dc014     	ldr	r12, [sp, #0x14]
    1774: e35c0000     	cmp	r12, #0
    1778: e58dc00c     	str	r12, [sp, #0xc]
    177c: 0affffa1     	beq	0x1608 <open_serial+0xa0> @ imm = #-0x17c
    1780: e594309c     	ldr	r3, [r4, #0x9c]
    1784: e59d6018     	ldr	r6, [sp, #0x18]
    1788: e24c2001     	sub	r2, r12, #1
    178c: e5937000     	ldr	r7, [r3]
    1790: e5961000     	ldr	r1, [r6]
    1794: e1a00007     	mov	r0, r7
    1798: e202b007     	and	r11, r2, #7
    179c: ebfffca1     	bl	0xa28 <.plt+0x20>       @ imm = #-0xd7c
    17a0: e3a0a000     	mov	r10, #0
    17a4: e150000a     	cmp	r0, r10
    17a8: 0a000079     	beq	0x1994 <open_serial+0x42c> @ imm = #0x1e4
    17ac: e59d100c     	ldr	r1, [sp, #0xc]
    17b0: e3a0a001     	mov	r10, #1
    17b4: e15a0001     	cmp	r10, r1
    17b8: 0affff92     	beq	0x1608 <open_serial+0xa0> @ imm = #-0x1b8
    17bc: e35b0000     	cmp	r11, #0
    17c0: 0a000038     	beq	0x18a8 <open_serial+0x340> @ imm = #0xe0
    17c4: e15b000a     	cmp	r11, r10
    17c8: 0a00002d     	beq	0x1884 <open_serial+0x31c> @ imm = #0xb4
    17cc: e35b0002     	cmp	r11, #2
    17d0: 0a000025     	beq	0x186c <open_serial+0x304> @ imm = #0x94
    17d4: e35b0003     	cmp	r11, #3
    17d8: 0a00001d     	beq	0x1854 <open_serial+0x2ec> @ imm = #0x74
    17dc: e35b0004     	cmp	r11, #4
    17e0: 0a000015     	beq	0x183c <open_serial+0x2d4> @ imm = #0x54
    17e4: e35b0005     	cmp	r11, #5
    17e8: 0a00000d     	beq	0x1824 <open_serial+0x2bc> @ imm = #0x34
    17ec: e35b0006     	cmp	r11, #6
    17f0: 0a000005     	beq	0x180c <open_serial+0x2a4> @ imm = #0x14
    17f4: e5b61004     	ldr	r1, [r6, #0x4]!
    17f8: e1a00007     	mov	r0, r7
    17fc: ebfffc89     	bl	0xa28 <.plt+0x20>       @ imm = #-0xddc
    1800: e3500000     	cmp	r0, #0
    1804: 0a000062     	beq	0x1994 <open_serial+0x42c> @ imm = #0x188
    1808: e08aa00a     	add	r10, r10, r10
    180c: e5b61004     	ldr	r1, [r6, #0x4]!
    1810: e1a00007     	mov	r0, r7
    1814: ebfffc83     	bl	0xa28 <.plt+0x20>       @ imm = #-0xdf4
    1818: e3500000     	cmp	r0, #0
    181c: 0a00005c     	beq	0x1994 <open_serial+0x42c> @ imm = #0x170
    1820: e28aa001     	add	r10, r10, #1
    1824: e5b61004     	ldr	r1, [r6, #0x4]!
    1828: e1a00007     	mov	r0, r7
    182c: ebfffc7d     	bl	0xa28 <.plt+0x20>       @ imm = #-0xe0c
    1830: e3500000     	cmp	r0, #0
    1834: 0a000056     	beq	0x1994 <open_serial+0x42c> @ imm = #0x158
    1838: e28aa001     	add	r10, r10, #1
    183c: e5b61004     	ldr	r1, [r6, #0x4]!
    1840: e1a00007     	mov	r0, r7
    1844: ebfffc77     	bl	0xa28 <.plt+0x20>       @ imm = #-0xe24
    1848: e3500000     	cmp	r0, #0
    184c: 0a000050     	beq	0x1994 <open_serial+0x42c> @ imm = #0x140
    1850: e28aa001     	add	r10, r10, #1
    1854: e5b61004     	ldr	r1, [r6, #0x4]!
    1858: e1a00007     	mov	r0, r7
    185c: ebfffc71     	bl	0xa28 <.plt+0x20>       @ imm = #-0xe3c
    1860: e3500000     	cmp	r0, #0
    1864: 0a00004a     	beq	0x1994 <open_serial+0x42c> @ imm = #0x128
    1868: e28aa001     	add	r10, r10, #1
    186c: e5b61004     	ldr	r1, [r6, #0x4]!
    1870: e1a00007     	mov	r0, r7
    1874: ebfffc6b     	bl	0xa28 <.plt+0x20>       @ imm = #-0xe54
    1878: e3500000     	cmp	r0, #0
    187c: 0a000044     	beq	0x1994 <open_serial+0x42c> @ imm = #0x110
    1880: e28aa001     	add	r10, r10, #1
    1884: e5b61004     	ldr	r1, [r6, #0x4]!
    1888: e1a00007     	mov	r0, r7
    188c: ebfffc65     	bl	0xa28 <.plt+0x20>       @ imm = #-0xe6c
    1890: e3500000     	cmp	r0, #0
    1894: 0a00003e     	beq	0x1994 <open_serial+0x42c> @ imm = #0xf8
    1898: e59d000c     	ldr	r0, [sp, #0xc]
    189c: e28aa001     	add	r10, r10, #1
    18a0: e15a0000     	cmp	r10, r0
    18a4: 0affff57     	beq	0x1608 <open_serial+0xa0> @ imm = #-0x2a4
    18a8: e5961004     	ldr	r1, [r6, #0x4]
    18ac: e1a00007     	mov	r0, r7
    18b0: ebfffc5c     	bl	0xa28 <.plt+0x20>       @ imm = #-0xe90
    18b4: e3500000     	cmp	r0, #0
    18b8: e1a00007     	mov	r0, r7
    18bc: 0a000034     	beq	0x1994 <open_serial+0x42c> @ imm = #0xd0
    18c0: e5961008     	ldr	r1, [r6, #0x8]
    18c4: ebfffc57     	bl	0xa28 <.plt+0x20>       @ imm = #-0xea4
    18c8: e28aa001     	add	r10, r10, #1
    18cc: e1a0b00a     	mov	r11, r10
    18d0: e3500000     	cmp	r0, #0
    18d4: e1a00007     	mov	r0, r7
    18d8: 0a00002d     	beq	0x1994 <open_serial+0x42c> @ imm = #0xb4
    18dc: e596100c     	ldr	r1, [r6, #0xc]
    18e0: ebfffc50     	bl	0xa28 <.plt+0x20>       @ imm = #-0xec0
    18e4: e28aa001     	add	r10, r10, #1
    18e8: e3500000     	cmp	r0, #0
    18ec: e1a00007     	mov	r0, r7
    18f0: 0a000027     	beq	0x1994 <open_serial+0x42c> @ imm = #0x9c
    18f4: e5961010     	ldr	r1, [r6, #0x10]
    18f8: ebfffc4a     	bl	0xa28 <.plt+0x20>       @ imm = #-0xed8
    18fc: e28ba002     	add	r10, r11, #2
    1900: e3500000     	cmp	r0, #0
    1904: e1a00007     	mov	r0, r7
    1908: 0a000021     	beq	0x1994 <open_serial+0x42c> @ imm = #0x84
    190c: e5961014     	ldr	r1, [r6, #0x14]
    1910: ebfffc44     	bl	0xa28 <.plt+0x20>       @ imm = #-0xef0
    1914: e28ba003     	add	r10, r11, #3
    1918: e3500000     	cmp	r0, #0
    191c: e1a00007     	mov	r0, r7
    1920: 0a00001b     	beq	0x1994 <open_serial+0x42c> @ imm = #0x6c
    1924: e5961018     	ldr	r1, [r6, #0x18]
    1928: ebfffc3e     	bl	0xa28 <.plt+0x20>       @ imm = #-0xf08
    192c: e28ba004     	add	r10, r11, #4
    1930: e3500000     	cmp	r0, #0
    1934: e1a00007     	mov	r0, r7
    1938: 0a000015     	beq	0x1994 <open_serial+0x42c> @ imm = #0x54
    193c: e596101c     	ldr	r1, [r6, #0x1c]
    1940: ebfffc38     	bl	0xa28 <.plt+0x20>       @ imm = #-0xf20
    1944: e28ba005     	add	r10, r11, #5
    1948: e3500000     	cmp	r0, #0
    194c: e1a00007     	mov	r0, r7
    1950: 0a00000f     	beq	0x1994 <open_serial+0x42c> @ imm = #0x3c
    1954: e5b61020     	ldr	r1, [r6, #0x20]!
    1958: ebfffc32     	bl	0xa28 <.plt+0x20>       @ imm = #-0xf38
    195c: e28ba006     	add	r10, r11, #6
    1960: e3500000     	cmp	r0, #0
    1964: 0a00000a     	beq	0x1994 <open_serial+0x42c> @ imm = #0x28
    1968: e59dc00c     	ldr	r12, [sp, #0xc]
    196c: e28ba007     	add	r10, r11, #7
    1970: e15a000c     	cmp	r10, r12
    1974: 1affffcb     	bne	0x18a8 <open_serial+0x340> @ imm = #-0xd4
    1978: eaffff22     	b	0x1608 <open_serial+0xa0> @ imm = #-0x378
    197c: e59f70e8     	ldr	r7, [pc, #0xe8]         @ 0x1a6c <open_serial+0x504>
    1980: e1a02006     	mov	r2, r6
    1984: e08f1007     	add	r1, pc, r7
    1988: e1a00004     	mov	r0, r4
    198c: ebfffc94     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0xdb0
    1990: eaffff12     	b	0x15e0 <open_serial+0x78> @ imm = #-0x3b8
    1994: e1a0500a     	mov	r5, r10
    1998: eaffff1a     	b	0x1608 <open_serial+0xa0> @ imm = #-0x398
    199c: e59f30cc     	ldr	r3, [pc, #0xcc]         @ 0x1a70 <open_serial+0x508>
    19a0: e1a01000     	mov	r1, r0
    19a4: e3a02062     	mov	r2, #98
    19a8: e08f0003     	add	r0, pc, r3
    19ac: ebfffc62     	bl	0xb3c <.plt+0x134>      @ imm = #-0xe78
    19b0: e3e06000     	mvn	r6, #0
    19b4: eaffff62     	b	0x1744 <open_serial+0x1dc> @ imm = #-0x278
    19b8: e594509c     	ldr	r5, [r4, #0x9c]
    19bc: e59f70b0     	ldr	r7, [pc, #0xb0]         @ 0x1a74 <open_serial+0x50c>
    19c0: e1a00004     	mov	r0, r4
    19c4: e08f1007     	add	r1, pc, r7
    19c8: e5952000     	ldr	r2, [r5]
    19cc: ebfffc84     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0xdf0
    19d0: e1a00006     	mov	r0, r6
    19d4: ebfffc7f     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0xe04
    19d8: e3e06000     	mvn	r6, #0
    19dc: eaffff58     	b	0x1744 <open_serial+0x1dc> @ imm = #-0x2a0
    19e0: e59f4090     	ldr	r4, [pc, #0x90]         @ 0x1a78 <open_serial+0x510>
    19e4: e24a2001     	sub	r2, r10, #1
    19e8: e1a01005     	mov	r1, r5
    19ec: e08f0004     	add	r0, pc, r4
    19f0: ebfffc51     	bl	0xb3c <.plt+0x134>      @ imm = #-0xebc
    19f4: e3e06000     	mvn	r6, #0
    19f8: eaffff51     	b	0x1744 <open_serial+0x1dc> @ imm = #-0x2bc
    19fc: ebfffc45     	bl	0xb18 <.plt+0x110>      @ imm = #-0xeec
    1a00: e594b09c     	ldr	r11, [r4, #0x9c]
    1a04: e59ba000     	ldr	r10, [r11]
    1a08: e5909000     	ldr	r9, [r0]
    1a0c: e1a00009     	mov	r0, r9
    1a10: ebfffc2b     	bl	0xac4 <.plt+0xbc>       @ imm = #-0xf54
    1a14: e59fc060     	ldr	r12, [pc, #0x60]        @ 0x1a7c <open_serial+0x514>
    1a18: e1a03009     	mov	r3, r9
    1a1c: e1a0200a     	mov	r2, r10
    1a20: e08f100c     	add	r1, pc, r12
    1a24: e58d0000     	str	r0, [sp]
    1a28: e1a00004     	mov	r0, r4
    1a2c: ebfffc6c     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0xe50
    1a30: eaffff43     	b	0x1744 <open_serial+0x1dc> @ imm = #-0x2f4
    1a34: e1a00004     	mov	r0, r4
    1a38: e59f4040     	ldr	r4, [pc, #0x40]         @ 0x1a80 <open_serial+0x518>
    1a3c: e5982000     	ldr	r2, [r8]
    1a40: e08f1004     	add	r1, pc, r4
    1a44: ebfffc66     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0xe68
    1a48: e1a00006     	mov	r0, r6
    1a4c: ebfffc61     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0xe7c
    1a50: e1a06009     	mov	r6, r9
    1a54: eaffff3a     	b	0x1744 <open_serial+0x1dc> @ imm = #-0x318
    1a58: 0f 27 00 00  	.word	0x0000270f
    1a5c: d4 27 00 00  	.word	0x000027d4
    1a60: 02 09 00 00  	.word	0x00000902
    1a64: ec 2d 00 00  	.word	0x00002dec
    1a68: 8c 26 00 00  	.word	0x0000268c
    1a6c: 48 24 00 00  	.word	0x00002448
    1a70: 6c 2a 00 00  	.word	0x00002a6c
    1a74: 18 2b 00 00  	.word	0x00002b18
    1a78: 6c 2a 00 00  	.word	0x00002a6c
    1a7c: 78 2a 00 00  	.word	0x00002a78
    1a80: 10 2b 00 00  	.word	0x00002b10

