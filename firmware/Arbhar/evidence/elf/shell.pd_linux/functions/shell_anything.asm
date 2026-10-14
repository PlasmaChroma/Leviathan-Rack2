000014b4 <shell_anything>:
    14b4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    14b8: e24ddb01     	sub	sp, sp, #1024
    14bc: e59fb4c4     	ldr	r11, [pc, #0x4c4]       @ 0x1988 <shell_anything+0x4d4>
    14c0: e24dd004     	sub	sp, sp, #4
    14c4: e5917000     	ldr	r7, [r1]
    14c8: e1a0a001     	mov	r10, r1
    14cc: e08fb00b     	add	r11, pc, r11
    14d0: e1a04000     	mov	r4, r0
    14d4: e1a06002     	mov	r6, r2
    14d8: e1a05003     	mov	r5, r3
    14dc: e1a0100b     	mov	r1, r11
    14e0: e1a00007     	mov	r0, r7
    14e4: ebfffd28     	bl	0x98c <.plt+0x20>       @ imm = #-0xb60
    14e8: e59f949c     	ldr	r9, [pc, #0x49c]        @ 0x198c <shell_anything+0x4d8>
    14ec: e08f9009     	add	r9, pc, r9
    14f0: e2508000     	subs	r8, r0, #0
    14f4: 0a000097     	beq	0x1758 <shell_anything+0x2a4> @ imm = #0x25c
    14f8: e5943030     	ldr	r3, [r4, #0x30]
    14fc: e58d7004     	str	r7, [sp, #0x4]
    1500: e3730001     	cmn	r3, #1
    1504: 1a000072     	bne	0x16d4 <shell_anything+0x220> @ imm = #0x1c8
    1508: e2840030     	add	r0, r4, #48
    150c: ebfffd75     	bl	0xae8 <.plt+0x17c>      @ imm = #-0xa2c
    1510: e3500000     	cmp	r0, #0
    1514: ba00007a     	blt	0x1704 <shell_anything+0x250> @ imm = #0x1e8
    1518: e2840038     	add	r0, r4, #56
    151c: ebfffd71     	bl	0xae8 <.plt+0x17c>      @ imm = #-0xa3c
    1520: e3500000     	cmp	r0, #0
    1524: ba0000f7     	blt	0x1908 <shell_anything+0x454> @ imm = #0x3dc
    1528: e59f1460     	ldr	r1, [pc, #0x460]        @ 0x1990 <shell_anything+0x4dc>
    152c: e1a02004     	mov	r2, r4
    1530: e5940030     	ldr	r0, [r4, #0x30]
    1534: e7991001     	ldr	r1, [r9, r1]
    1538: ebfffd5e     	bl	0xab8 <.plt+0x14c>      @ imm = #-0xa88
    153c: ebfffd6c     	bl	0xaf4 <.plt+0x188>      @ imm = #-0xa50
    1540: e3500000     	cmp	r0, #0
    1544: e1a07000     	mov	r7, r0
    1548: e5840040     	str	r0, [r4, #0x40]
    154c: 1a000073     	bne	0x1720 <shell_anything+0x26c> @ imm = #0x1cc
    1550: e3a01001     	mov	r1, #1
    1554: e5940034     	ldr	r0, [r4, #0x34]
    1558: ebfffd2f     	bl	0xa1c <.plt+0xb0>       @ imm = #-0xb44
    155c: e1a01007     	mov	r1, r7
    1560: e594003c     	ldr	r0, [r4, #0x3c]
    1564: ebfffd2c     	bl	0xa1c <.plt+0xb0>       @ imm = #-0xb50
    1568: e1a0200d     	mov	r2, sp
    156c: e1a01007     	mov	r1, r7
    1570: e1a00007     	mov	r0, r7
    1574: e58d7000     	str	r7, [sp]
    1578: ebfffd7b     	bl	0xb6c <.plt+0x200>      @ imm = #-0xa14
    157c: ebfffd0e     	bl	0x9bc <.plt+0x50>       @ imm = #-0xbc8
    1580: ebfffd28     	bl	0xa28 <.plt+0xbc>       @ imm = #-0xb60
    1584: e3560000     	cmp	r6, #0
    1588: da0000fc     	ble	0x1980 <shell_anything+0x4cc> @ imm = #0x3f0
    158c: e0857186     	add	r7, r5, r6, lsl #3
    1590: e28d4008     	add	r4, sp, #8
    1594: e047c005     	sub	r12, r7, r5
    1598: e24ce008     	sub	lr, r12, #8
    159c: e1a0b1ae     	lsr	r11, lr, #3
    15a0: e28b9001     	add	r9, r11, #1
    15a4: e2193003     	ands	r3, r9, #3
    15a8: 0a000017     	beq	0x160c <shell_anything+0x158> @ imm = #0x5c
    15ac: e3530001     	cmp	r3, #1
    15b0: 0a00000a     	beq	0x15e0 <shell_anything+0x12c> @ imm = #0x28
    15b4: e3530002     	cmp	r3, #2
    15b8: 1a0000d9     	bne	0x1924 <shell_anything+0x470> @ imm = #0x364
    15bc: e3a000ff     	mov	r0, #255
    15c0: ebfffd06     	bl	0x9e0 <.plt+0x74>       @ imm = #-0xbe8
    15c4: e3a020ff     	mov	r2, #255
    15c8: e1a08000     	mov	r8, r0
    15cc: e1a01000     	mov	r1, r0
    15d0: e4848004     	str	r8, [r4], #4
    15d4: e1a00005     	mov	r0, r5
    15d8: ebfffd66     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xa68
    15dc: e2855008     	add	r5, r5, #8
    15e0: e3a000ff     	mov	r0, #255
    15e4: ebfffcfd     	bl	0x9e0 <.plt+0x74>       @ imm = #-0xc0c
    15e8: e3a020ff     	mov	r2, #255
    15ec: e1a0c000     	mov	r12, r0
    15f0: e1a01000     	mov	r1, r0
    15f4: e484c004     	str	r12, [r4], #4
    15f8: e1a00005     	mov	r0, r5
    15fc: e2855008     	add	r5, r5, #8
    1600: ebfffd5c     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xa90
    1604: e1570005     	cmp	r7, r5
    1608: 0a000027     	beq	0x16ac <shell_anything+0x1f8> @ imm = #0x9c
    160c: e3a000ff     	mov	r0, #255
    1610: e1a08004     	mov	r8, r4
    1614: ebfffcf1     	bl	0x9e0 <.plt+0x74>       @ imm = #-0xc3c
    1618: e3a020ff     	mov	r2, #255
    161c: e2859008     	add	r9, r5, #8
    1620: e2844010     	add	r4, r4, #16
    1624: e1a01000     	mov	r1, r0
    1628: e1a0b000     	mov	r11, r0
    162c: e1a00005     	mov	r0, r5
    1630: e488b004     	str	r11, [r8], #4
    1634: ebfffd4f     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xac4
    1638: e3a000ff     	mov	r0, #255
    163c: ebfffce7     	bl	0x9e0 <.plt+0x74>       @ imm = #-0xc64
    1640: e3a020ff     	mov	r2, #255
    1644: e285b010     	add	r11, r5, #16
    1648: e1a03000     	mov	r3, r0
    164c: e1a01000     	mov	r1, r0
    1650: e504300c     	str	r3, [r4, #-0xc]
    1654: e1a00009     	mov	r0, r9
    1658: ebfffd46     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xae8
    165c: e3a000ff     	mov	r0, #255
    1660: ebfffcde     	bl	0x9e0 <.plt+0x74>       @ imm = #-0xc88
    1664: e3a020ff     	mov	r2, #255
    1668: e1a0c000     	mov	r12, r0
    166c: e1a01000     	mov	r1, r0
    1670: e588c004     	str	r12, [r8, #0x4]
    1674: e1a0000b     	mov	r0, r11
    1678: ebfffd3e     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xb08
    167c: e3a000ff     	mov	r0, #255
    1680: ebfffcd6     	bl	0x9e0 <.plt+0x74>       @ imm = #-0xca8
    1684: e2858018     	add	r8, r5, #24
    1688: e3a020ff     	mov	r2, #255
    168c: e2855020     	add	r5, r5, #32
    1690: e1a09000     	mov	r9, r0
    1694: e1a01000     	mov	r1, r0
    1698: e5049004     	str	r9, [r4, #-0x4]
    169c: e1a00008     	mov	r0, r8
    16a0: ebfffd34     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xb30
    16a4: e1570005     	cmp	r7, r5
    16a8: 1affffd7     	bne	0x160c <shell_anything+0x158> @ imm = #-0xa4
    16ac: e2865001     	add	r5, r6, #1
    16b0: e28d6b01     	add	r6, sp, #1024
    16b4: e59a0000     	ldr	r0, [r10]
    16b8: e0862105     	add	r2, r6, r5, lsl #2
    16bc: e3a0a000     	mov	r10, #0
    16c0: e28d1004     	add	r1, sp, #4
    16c4: e502a3fc     	str	r10, [r2, #-0x3fc]
    16c8: ebfffcc7     	bl	0x9ec <.plt+0x80>       @ imm = #-0xce4
    16cc: e1a0000a     	mov	r0, r10
    16d0: ebfffcec     	bl	0xa88 <.plt+0x11c>      @ imm = #-0xc50
    16d4: e59f02b8     	ldr	r0, [pc, #0x2b8]        @ 0x1994 <shell_anything+0x4e0>
    16d8: e08f0000     	add	r0, pc, r0
    16dc: ebfffcf8     	bl	0xac4 <.plt+0x158>      @ imm = #-0xc20
    16e0: e3a01009     	mov	r1, #9
    16e4: e5940040     	ldr	r0, [r4, #0x40]
    16e8: ebfffce3     	bl	0xa7c <.plt+0x110>      @ imm = #-0xc74
    16ec: e1a00004     	mov	r0, r4
    16f0: ebfffcc0     	bl	0x9f8 <.plt+0x8c>       @ imm = #-0xd00
    16f4: e2840030     	add	r0, r4, #48
    16f8: ebfffcfa     	bl	0xae8 <.plt+0x17c>      @ imm = #-0xc18
    16fc: e3500000     	cmp	r0, #0
    1700: aaffff84     	bge	0x1518 <shell_anything+0x64> @ imm = #-0x1f0
    1704: e59f728c     	ldr	r7, [pc, #0x28c]        @ 0x1998 <shell_anything+0x4e4>
    1708: e1a00004     	mov	r0, r4
    170c: e08f1007     	add	r1, pc, r7
    1710: ebfffd0f     	bl	0xb54 <.plt+0x1e8>      @ imm = #-0xbc4
    1714: e28ddb01     	add	sp, sp, #1024
    1718: e28dd004     	add	sp, sp, #4
    171c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1720: e3a02004     	mov	r2, #4
    1724: eeb10b00     	vmov.f64	d0, #4.000000e+00
    1728: e5842044     	str	r2, [r4, #0x44]
    172c: e594004c     	ldr	r0, [r4, #0x4c]
    1730: ebfffcbf     	bl	0xa34 <.plt+0xc8>       @ imm = #-0xd04
    1734: e594801c     	ldr	r8, [r4, #0x1c]
    1738: e3580000     	cmp	r8, #0
    173c: 0afffff4     	beq	0x1714 <shell_anything+0x260> @ imm = #-0x30
    1740: e1a03005     	mov	r3, r5
    1744: e1a02006     	mov	r2, r6
    1748: e1a0100a     	mov	r1, r10
    174c: e594000c     	ldr	r0, [r4, #0xc]
    1750: ebfffcf6     	bl	0xb30 <.plt+0x1c4>      @ imm = #-0xc28
    1754: eaffffee     	b	0x1714 <shell_anything+0x260> @ imm = #-0x48
    1758: e1a0000b     	mov	r0, r11
    175c: ebfffcd8     	bl	0xac4 <.plt+0x158>      @ imm = #-0xca0
    1760: e5940038     	ldr	r0, [r4, #0x38]
    1764: e3700001     	cmn	r0, #1
    1768: 0affffe9     	beq	0x1714 <shell_anything+0x260> @ imm = #-0x5c
    176c: e3560000     	cmp	r6, #0
    1770: d28d7004     	addle	r7, sp, #4
    1774: d3e00000     	mvnle	r0, #0
    1778: da000051     	ble	0x18c4 <shell_anything+0x410> @ imm = #0x144
    177c: e216e003     	ands	lr, r6, #3
    1780: e1a0a008     	mov	r10, r8
    1784: e28d7004     	add	r7, sp, #4
    1788: e3a09020     	mov	r9, #32
    178c: 0a00001d     	beq	0x1808 <shell_anything+0x354> @ imm = #0x74
    1790: e35e0001     	cmp	lr, #1
    1794: 0a00000d     	beq	0x17d0 <shell_anything+0x31c> @ imm = #0x34
    1798: e35e0002     	cmp	lr, #2
    179c: 1a00006a     	bne	0x194c <shell_anything+0x498> @ imm = #0x1a8
    17a0: e2682ffa     	rsb	r2, r8, #1000
    17a4: e0871008     	add	r1, r7, r8
    17a8: e1a00005     	mov	r0, r5
    17ac: e28aa001     	add	r10, r10, #1
    17b0: ebfffcf0     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xc40
    17b4: e1a00007     	mov	r0, r7
    17b8: ebfffcbb     	bl	0xaac <.plt+0x140>      @ imm = #-0xd14
    17bc: e28dcb01     	add	r12, sp, #1024
    17c0: e2855008     	add	r5, r5, #8
    17c4: e08c2000     	add	r2, r12, r0
    17c8: e2808001     	add	r8, r0, #1
    17cc: e54293fc     	strb	r9, [r2, #-0x3fc]
    17d0: e0871008     	add	r1, r7, r8
    17d4: e2682ffa     	rsb	r2, r8, #1000
    17d8: e1a00005     	mov	r0, r5
    17dc: e28d8b01     	add	r8, sp, #1024
    17e0: ebfffce4     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xc70
    17e4: e1a00007     	mov	r0, r7
    17e8: ebfffcaf     	bl	0xaac <.plt+0x140>      @ imm = #-0xd44
    17ec: e28aa001     	add	r10, r10, #1
    17f0: e156000a     	cmp	r6, r10
    17f4: e2855008     	add	r5, r5, #8
    17f8: e0881000     	add	r1, r8, r0
    17fc: e2808001     	add	r8, r0, #1
    1800: e54193fc     	strb	r9, [r1, #-0x3fc]
    1804: 0a00002e     	beq	0x18c4 <shell_anything+0x410> @ imm = #0xb8
    1808: e2682ffa     	rsb	r2, r8, #1000
    180c: e0871008     	add	r1, r7, r8
    1810: e1a00005     	mov	r0, r5
    1814: e285b008     	add	r11, r5, #8
    1818: ebfffcd6     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xca8
    181c: e1a00007     	mov	r0, r7
    1820: ebfffca1     	bl	0xaac <.plt+0x140>      @ imm = #-0xd7c
    1824: e28d3b01     	add	r3, sp, #1024
    1828: e28aa004     	add	r10, r10, #4
    182c: e0838000     	add	r8, r3, r0
    1830: e280c001     	add	r12, r0, #1
    1834: e26c2ffa     	rsb	r2, r12, #1000
    1838: e087100c     	add	r1, r7, r12
    183c: e1a0000b     	mov	r0, r11
    1840: e54893fc     	strb	r9, [r8, #-0x3fc]
    1844: ebfffccb     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xcd4
    1848: e1a00007     	mov	r0, r7
    184c: ebfffc96     	bl	0xaac <.plt+0x140>      @ imm = #-0xda8
    1850: e28bb008     	add	r11, r11, #8
    1854: e28d2b01     	add	r2, sp, #1024
    1858: e2858018     	add	r8, r5, #24
    185c: e2855020     	add	r5, r5, #32
    1860: e082c000     	add	r12, r2, r0
    1864: e2801001     	add	r1, r0, #1
    1868: e2612ffa     	rsb	r2, r1, #1000
    186c: e1a0000b     	mov	r0, r11
    1870: e0871001     	add	r1, r7, r1
    1874: e54c93fc     	strb	r9, [r12, #-0x3fc]
    1878: ebfffcbe     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xd08
    187c: e1a00007     	mov	r0, r7
    1880: ebfffc89     	bl	0xaac <.plt+0x140>      @ imm = #-0xddc
    1884: e28d3b01     	add	r3, sp, #1024
    1888: e083c000     	add	r12, r3, r0
    188c: e280b001     	add	r11, r0, #1
    1890: e087100b     	add	r1, r7, r11
    1894: e1a00008     	mov	r0, r8
    1898: e26b2ffa     	rsb	r2, r11, #1000
    189c: e54c93fc     	strb	r9, [r12, #-0x3fc]
    18a0: ebfffcb4     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xd30
    18a4: e1a00007     	mov	r0, r7
    18a8: ebfffc7f     	bl	0xaac <.plt+0x140>      @ imm = #-0xe04
    18ac: e28d8b01     	add	r8, sp, #1024
    18b0: e156000a     	cmp	r6, r10
    18b4: e0881000     	add	r1, r8, r0
    18b8: e2808001     	add	r8, r0, #1
    18bc: e54193fc     	strb	r9, [r1, #-0x3fc]
    18c0: 1affffd0     	bne	0x1808 <shell_anything+0x354> @ imm = #-0xc0
    18c4: e28d5b01     	add	r5, sp, #1024
    18c8: e1a01007     	mov	r1, r7
    18cc: e0856000     	add	r6, r5, r0
    18d0: e59f00c4     	ldr	r0, [pc, #0xc4]         @ 0x199c <shell_anything+0x4e8>
    18d4: e3a0e000     	mov	lr, #0
    18d8: e08f0000     	add	r0, pc, r0
    18dc: e546e3fc     	strb	lr, [r6, #-0x3fc]
    18e0: ebfffc77     	bl	0xac4 <.plt+0x158>      @ imm = #-0xe24
    18e4: e1a00007     	mov	r0, r7
    18e8: ebfffc6f     	bl	0xaac <.plt+0x140>      @ imm = #-0xe44
    18ec: e1a01007     	mov	r1, r7
    18f0: e1a02000     	mov	r2, r0
    18f4: e5940038     	ldr	r0, [r4, #0x38]
    18f8: ebfffc74     	bl	0xad0 <.plt+0x164>      @ imm = #-0xe30
    18fc: e28ddb01     	add	sp, sp, #1024
    1900: e28dd004     	add	sp, sp, #4
    1904: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1908: e59f1090     	ldr	r1, [pc, #0x90]         @ 0x19a0 <shell_anything+0x4ec>
    190c: e1a00004     	mov	r0, r4
    1910: e08f1001     	add	r1, pc, r1
    1914: ebfffc8e     	bl	0xb54 <.plt+0x1e8>      @ imm = #-0xdc8
    1918: e28ddb01     	add	sp, sp, #1024
    191c: e28dd004     	add	sp, sp, #4
    1920: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1924: e3a000ff     	mov	r0, #255
    1928: ebfffc2c     	bl	0x9e0 <.plt+0x74>       @ imm = #-0xf50
    192c: e3a020ff     	mov	r2, #255
    1930: e5840000     	str	r0, [r4]
    1934: e1a01000     	mov	r1, r0
    1938: e1a00005     	mov	r0, r5
    193c: e28d400c     	add	r4, sp, #12
    1940: ebfffc8c     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xdd0
    1944: e2855008     	add	r5, r5, #8
    1948: eaffff1b     	b	0x15bc <shell_anything+0x108> @ imm = #-0x394
    194c: e1a00005     	mov	r0, r5
    1950: e3a02ffa     	mov	r2, #1000
    1954: e1a01007     	mov	r1, r7
    1958: e28dbb01     	add	r11, sp, #1024
    195c: ebfffc85     	bl	0xb78 <.plt+0x20c>      @ imm = #-0xdec
    1960: e1a00007     	mov	r0, r7
    1964: ebfffc50     	bl	0xaac <.plt+0x140>      @ imm = #-0xec0
    1968: e2855008     	add	r5, r5, #8
    196c: e3a0a001     	mov	r10, #1
    1970: e08b3000     	add	r3, r11, r0
    1974: e2808001     	add	r8, r0, #1
    1978: e54393fc     	strb	r9, [r3, #-0x3fc]
    197c: eaffff87     	b	0x17a0 <shell_anything+0x2ec> @ imm = #-0x1e4
    1980: e3a05001     	mov	r5, #1
    1984: eaffff49     	b	0x16b0 <shell_anything+0x1fc> @ imm = #-0x2dc
    1988: c8 05 00 00  	.word	0x000005c8
    198c: 0c 0b 01 00  	.word	0x00010b0c
    1990: d0 00 00 00  	.word	0x000000d0
    1994: d0 03 00 00  	.word	0x000003d0
    1998: c0 03 00 00  	.word	0x000003c0
    199c: c4 01 00 00  	.word	0x000001c4
    19a0: d4 01 00 00  	.word	0x000001d4

