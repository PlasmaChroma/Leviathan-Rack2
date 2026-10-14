0000135c <fftwObject_tilde_length>:
    135c: ed9f6af7     	vldr	s12, [pc, #988]         @ 0x1740 <fftwObject_tilde_length+0x3e4>  // f32=0
    1360: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1364: e2806866     	add	r6, r0, #6684672
    1368: eef76a00     	vmov.f32	s13, #1.000000e+00
    136c: e2863ece     	add	r3, r6, #3296
    1370: e5961d14     	ldr	r1, [r6, #0xd14]
    1374: e1a09000     	mov	r9, r0
    1378: ed937a03     	vldr	s14, [r3, #12]
    137c: e2860ecf     	add	r0, r6, #3312
    1380: e2414001     	sub	r4, r1, #1
    1384: e2865a01     	add	r5, r6, #4096
    1388: ee071a90     	vmov	s15, r1
    138c: e595c03c     	ldr	r12, [r5, #0x3c]
    1390: ed2d8b08     	vpush	{d8, d9, d10, d11}
    1394: e5957040     	ldr	r7, [r5, #0x40]
    1398: ed959a12     	vldr	s18, [r5, #72]
    139c: e24dd00c     	sub	sp, sp, #12
    13a0: eeb40ac6     	vcmpe.f32	s0, s12
    13a4: ed958a13     	vldr	s16, [r5, #76]
    13a8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    13ac: beb00a46     	vmovlt.f32	s0, s12
    13b0: eef80ae7     	vcvt.f32.s32	s1, s15
    13b4: eeb40ae6     	vcmpe.f32	s0, s13
    13b8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    13bc: 8eb00a66     	vmovhi.f32	s0, s13
    13c0: ee201a87     	vmul.f32	s2, s1, s14
    13c4: ed800a00     	vstr	s0, [r0]
    13c8: ee200a80     	vmul.f32	s0, s1, s0
    13cc: eefd1ac1     	vcvt.s32.f32	s3, s2
    13d0: eebd2ac0     	vcvt.s32.f32	s4, s0
    13d4: ee112a90     	vmov	r2, s3
    13d8: ee128a10     	vmov	r8, s4
    13dc: e1520004     	cmp	r2, r4
    13e0: a2412002     	subge	r2, r1, #2
    13e4: e5862d00     	str	r2, [r6, #0xd00]
    13e8: e3580002     	cmp	r8, #2
    13ec: b3a08002     	movlt	r8, #2
    13f0: e082b008     	add	r11, r2, r8
    13f4: e15b0001     	cmp	r11, r1
    13f8: a1a0b001     	movge	r11, r1
    13fc: e04b4002     	sub	r4, r11, r2
    1400: e1540008     	cmp	r4, r8
    1404: e586bd04     	str	r11, [r6, #0xd04]
    1408: a1a04008     	movge	r4, r8
    140c: e154000c     	cmp	r4, r12
    1410: e5864d10     	str	r4, [r6, #0xd10]
    1414: e084e584     	add	lr, r4, r4, lsl #11
    1418: b1a0a004     	movlt	r10, r4
    141c: a1a0a00c     	movge	r10, r12
    1420: e1540007     	cmp	r4, r7
    1424: e586ed18     	str	lr, [r6, #0xd18]
    1428: e585a03c     	str	r10, [r5, #0x3c]
    142c: b1a08004     	movlt	r8, r4
    1430: a1a08007     	movge	r8, r7
    1434: e35a0000     	cmp	r10, #0
    1438: e0443008     	sub	r3, r4, r8
    143c: e5858040     	str	r8, [r5, #0x40]
    1440: e58d3004     	str	r3, [sp, #0x4]
    1444: da00010a     	ble	0x1874 <fftwObject_tilde_length+0x518> @ imm = #0x428
    1448: eef72a00     	vmov.f32	s5, #1.000000e+00
    144c: e21a1003     	ands	r1, r10, #3
    1450: ee03ca10     	vmov	s6, r12
    1454: e3000d1c     	movw	r0, #0xd1c
    1458: e3400066     	movt	r0, #0x66
    145c: e3a05000     	mov	r5, #0
    1460: e089b000     	add	r11, r9, r0
    1464: eef83ac3     	vcvt.f32.s32	s7, s6
    1468: eeb7bac9     	vcvt.f64.f32	d11, s18
    146c: eec28aa3     	vdiv.f32	s17, s5, s7
    1470: 0a00001d     	beq	0x14ec <fftwObject_tilde_length+0x190> @ imm = #0x74
    1474: e3510001     	cmp	r1, #1
    1478: 0a000010     	beq	0x14c0 <fftwObject_tilde_length+0x164> @ imm = #0x40
    147c: e3510002     	cmp	r1, #2
    1480: 0a000005     	beq	0x149c <fftwObject_tilde_length+0x140> @ imm = #0x14
    1484: ed9f0bab     	vldr	d0, [pc, #684]          @ 0x1738 <fftwObject_tilde_length+0x3dc>  // f64=0
    1488: e3a05001     	mov	r5, #1
    148c: eeb01b4b     	vmov.f64	d1, d11
    1490: ebfffc9b     	bl	0x704 <.plt+0xec>       @ imm = #-0xd94  // CALL __pow_finite
    1494: eeb74bc0     	vcvt.f32.f64	s8, d0
    1498: ecab4a01     	vstmia	r11!, {s8}
    149c: ee045a90     	vmov	s9, r5
    14a0: e2855001     	add	r5, r5, #1
    14a4: eeb01b4b     	vmov.f64	d1, d11
    14a8: eeb85ae4     	vcvt.f32.s32	s10, s9
    14ac: ee655a28     	vmul.f32	s11, s10, s17
    14b0: eeb70ae5     	vcvt.f64.f32	d0, s11
    14b4: ebfffc92     	bl	0x704 <.plt+0xec>       @ imm = #-0xdb8  // CALL __pow_finite
    14b8: eef79bc0     	vcvt.f32.f64	s19, d0
    14bc: eceb9a01     	vstmia	r11!, {s19}
    14c0: ee0a5a10     	vmov	s20, r5
    14c4: e2855001     	add	r5, r5, #1
    14c8: eeb01b4b     	vmov.f64	d1, d11
    14cc: eef8aaca     	vcvt.f32.s32	s21, s20
    14d0: ee6a6aa8     	vmul.f32	s13, s21, s17
    14d4: eeb70ae6     	vcvt.f64.f32	d0, s13
    14d8: ebfffc89     	bl	0x704 <.plt+0xec>       @ imm = #-0xddc  // CALL __pow_finite
    14dc: e15a0005     	cmp	r10, r5
    14e0: eeb76bc0     	vcvt.f32.f64	s12, d0
    14e4: ecab6a01     	vstmia	r11!, {s12}
    14e8: 0a000028     	beq	0x1590 <fftwObject_tilde_length+0x234> @ imm = #0xa0
    14ec: e2852001     	add	r2, r5, #1
    14f0: e1a0800b     	mov	r8, r11
    14f4: e28bb010     	add	r11, r11, #16
    14f8: ee075a10     	vmov	s14, r5
    14fc: eeb01b4b     	vmov.f64	d1, d11
    1500: ee092a10     	vmov	s18, r2
    1504: eef87ac7     	vcvt.f32.s32	s15, s14
    1508: ee670aa8     	vmul.f32	s1, s15, s17
    150c: eeb70ae0     	vcvt.f64.f32	d0, s1
    1510: ebfffc7b     	bl	0x704 <.plt+0xec>       @ imm = #-0xe14  // CALL __pow_finite
    1514: eeb82ac9     	vcvt.f32.s32	s4, s18
    1518: ee622a28     	vmul.f32	s5, s4, s17
    151c: eeb01b4b     	vmov.f64	d1, d11
    1520: eeb73bc0     	vcvt.f32.f64	s6, d0
    1524: eeb70ae2     	vcvt.f64.f32	d0, s5
    1528: eca83a01     	vstmia	r8!, {s6}
    152c: ebfffc74     	bl	0x704 <.plt+0xec>       @ imm = #-0xe30  // CALL __pow_finite
    1530: ee19ca10     	vmov	r12, s18
    1534: eeb01b4b     	vmov.f64	d1, d11
    1538: e28c3001     	add	r3, r12, #1
    153c: ee033a90     	vmov	s7, r3
    1540: eeb84ae3     	vcvt.f32.s32	s8, s7
    1544: ee644a28     	vmul.f32	s9, s8, s17
    1548: eeb75bc0     	vcvt.f32.f64	s10, d0
    154c: eeb70ae4     	vcvt.f64.f32	d0, s9
    1550: ed0b5a03     	vstr	s10, [r11, #-12]
    1554: ebfffc6a     	bl	0x704 <.plt+0xec>       @ imm = #-0xe58  // CALL __pow_finite
    1558: e2851003     	add	r1, r5, #3
    155c: e2855004     	add	r5, r5, #4
    1560: ee051a90     	vmov	s11, r1
    1564: eef89ae5     	vcvt.f32.s32	s19, s11
    1568: ee29aaa8     	vmul.f32	s20, s19, s17
    156c: eeb01b4b     	vmov.f64	d1, d11
    1570: eef7abc0     	vcvt.f32.f64	s21, d0
    1574: eeb70aca     	vcvt.f64.f32	d0, s20
    1578: edc8aa01     	vstr	s21, [r8, #4]
    157c: ebfffc60     	bl	0x704 <.plt+0xec>       @ imm = #-0xe80  // CALL __pow_finite
    1580: e15a0005     	cmp	r10, r5
    1584: eeb71bc0     	vcvt.f32.f64	s2, d0
    1588: ed0b1a01     	vstr	s2, [r11, #-4]
    158c: 1affffd6     	bne	0x14ec <fftwObject_tilde_length+0x190> @ imm = #-0xa8
    1590: e59de004     	ldr	lr, [sp, #0x4]
    1594: e15e000a     	cmp	lr, r10
    1598: da00002e     	ble	0x1658 <fftwObject_tilde_length+0x2fc> @ imm = #0xb8
    159c: e3085347     	movw	r5, #0x8347
    15a0: e3405019     	movt	r5, #0x19
    15a4: e08aa005     	add	r10, r10, r5
    15a8: e3002d1c     	movw	r2, #0xd1c
    15ac: e3402066     	movt	r2, #0x66
    15b0: e3a085fe     	mov	r8, #1065353216
    15b4: e089c002     	add	r12, r9, r2
    15b8: e089310a     	add	r3, r9, r10, lsl #2
    15bc: e08c110e     	add	r1, r12, lr, lsl #2
    15c0: e041b003     	sub	r11, r1, r3
    15c4: e24be004     	sub	lr, r11, #4
    15c8: e1a0012e     	lsr	r0, lr, #2
    15cc: e2805001     	add	r5, r0, #1
    15d0: e215a007     	ands	r10, r5, #7
    15d4: 0a000013     	beq	0x1628 <fftwObject_tilde_length+0x2cc> @ imm = #0x4c
    15d8: e35a0001     	cmp	r10, #1
    15dc: 0a00000e     	beq	0x161c <fftwObject_tilde_length+0x2c0> @ imm = #0x38
    15e0: e35a0002     	cmp	r10, #2
    15e4: 0a00000b     	beq	0x1618 <fftwObject_tilde_length+0x2bc> @ imm = #0x2c
    15e8: e35a0003     	cmp	r10, #3
    15ec: 0a000008     	beq	0x1614 <fftwObject_tilde_length+0x2b8> @ imm = #0x20
    15f0: e35a0004     	cmp	r10, #4
    15f4: 0a000005     	beq	0x1610 <fftwObject_tilde_length+0x2b4> @ imm = #0x14
    15f8: e35a0005     	cmp	r10, #5
    15fc: 0a000002     	beq	0x160c <fftwObject_tilde_length+0x2b0> @ imm = #0x8
    1600: e35a0006     	cmp	r10, #6
    1604: 1a000098     	bne	0x186c <fftwObject_tilde_length+0x510> @ imm = #0x260
    1608: e4838004     	str	r8, [r3], #4
    160c: e4838004     	str	r8, [r3], #4
    1610: e4838004     	str	r8, [r3], #4
    1614: e4838004     	str	r8, [r3], #4
    1618: e4838004     	str	r8, [r3], #4
    161c: e4838004     	str	r8, [r3], #4
    1620: e1510003     	cmp	r1, r3
    1624: 0a00000b     	beq	0x1658 <fftwObject_tilde_length+0x2fc> @ imm = #0x2c
    1628: e1a02003     	mov	r2, r3
    162c: e2833020     	add	r3, r3, #32
    1630: e4828004     	str	r8, [r2], #4
    1634: e503801c     	str	r8, [r3, #-0x1c]
    1638: e5828004     	str	r8, [r2, #0x4]
    163c: e5038014     	str	r8, [r3, #-0x14]
    1640: e5038010     	str	r8, [r3, #-0x10]
    1644: e503800c     	str	r8, [r3, #-0xc]
    1648: e5038008     	str	r8, [r3, #-0x8]
    164c: e5038004     	str	r8, [r3, #-0x4]
    1650: e1510003     	cmp	r1, r3
    1654: 1afffff3     	bne	0x1628 <fftwObject_tilde_length+0x2cc> @ imm = #-0x34
    1658: e3570000     	cmp	r7, #0
    165c: da00006b     	ble	0x1810 <fftwObject_tilde_length+0x4b4> @ imm = #0x1ac
    1660: eef7ba00     	vmov.f32	s23, #1.000000e+00
    1664: e59d1004     	ldr	r1, [sp, #0x4]
    1668: ee017a90     	vmov	s3, r7
    166c: e3088347     	movw	r8, #0x8347
    1670: e3408019     	movt	r8, #0x19
    1674: e217c003     	ands	r12, r7, #3
    1678: e081b008     	add	r11, r1, r8
    167c: e3a08000     	mov	r8, #0
    1680: e089510b     	add	r5, r9, r11, lsl #2
    1684: eeb80ae1     	vcvt.f32.s32	s0, s3
    1688: eef80be1     	vcvt.f64.s32	d16, s3
    168c: ee8bba80     	vdiv.f32	s22, s23, s0
    1690: ee809ba0     	vdiv.f64	d9, d16, d16
    1694: eeb78ac8     	vcvt.f64.f32	d8, s16
    1698: eeb7ab00     	vmov.f64	d10, #1.000000e+00
    169c: 0a00002b     	beq	0x1750 <fftwObject_tilde_length+0x3f4> @ imm = #0xac
    16a0: e35c0001     	cmp	r12, #1
    16a4: 0a000014     	beq	0x16fc <fftwObject_tilde_length+0x3a0> @ imm = #0x50
    16a8: e35c0002     	cmp	r12, #2
    16ac: 0a000007     	beq	0x16d0 <fftwObject_tilde_length+0x374> @ imm = #0x1c
    16b0: ed9f0b20     	vldr	d0, [pc, #128]          @ 0x1738 <fftwObject_tilde_length+0x3dc>  // f64=0
    16b4: e3a08001     	mov	r8, #1
    16b8: eeb01b48     	vmov.f64	d1, d8
    16bc: ebfffc10     	bl	0x704 <.plt+0xec>       @ imm = #-0xfc0  // CALL __pow_finite
    16c0: ee7a1b40     	vsub.f64	d17, d10, d0
    16c4: ee290b21     	vmul.f64	d0, d9, d17
    16c8: eef76bc0     	vcvt.f32.f64	s13, d0
    16cc: ece56a01     	vstmia	r5!, {s13}
    16d0: ee068a10     	vmov	s12, r8
    16d4: e2888001     	add	r8, r8, #1
    16d8: eeb01b48     	vmov.f64	d1, d8
    16dc: eeb87ac6     	vcvt.f32.s32	s14, s12
    16e0: ee677a0b     	vmul.f32	s15, s14, s22
    16e4: eeb70ae7     	vcvt.f64.f32	d0, s15
    16e8: ebfffc05     	bl	0x704 <.plt+0xec>       @ imm = #-0xfec  // CALL __pow_finite
    16ec: ee7a2b40     	vsub.f64	d18, d10, d0
    16f0: ee290b22     	vmul.f64	d0, d9, d18
    16f4: eef70bc0     	vcvt.f32.f64	s1, d0
    16f8: ece50a01     	vstmia	r5!, {s1}
    16fc: ee028a10     	vmov	s4, r8
    1700: e2888001     	add	r8, r8, #1
    1704: eeb01b48     	vmov.f64	d1, d8
    1708: eef82ac2     	vcvt.f32.s32	s5, s4
    170c: ee223a8b     	vmul.f32	s6, s5, s22
    1710: eeb70ac3     	vcvt.f64.f32	d0, s6
    1714: ebfffbfa     	bl	0x704 <.plt+0xec>       @ imm = #-0x1018  // CALL __pow_finite
    1718: e1570008     	cmp	r7, r8
    171c: ee7a3b40     	vsub.f64	d19, d10, d0
    1720: ee290b23     	vmul.f64	d0, d9, d19
    1724: eef73bc0     	vcvt.f32.f64	s7, d0
    1728: ece53a01     	vstmia	r5!, {s7}
    172c: 0a000037     	beq	0x1810 <fftwObject_tilde_length+0x4b4> @ imm = #0xdc
    1730: ea000006     	b	0x1750 <fftwObject_tilde_length+0x3f4> @ imm = #0x18
    1734: e320f000     	nop
    1738: 00 00 00 00  	.word	0x00000000
    173c: 00 00 00 00  	.word	0x00000000
    1740: 00 00 00 00  	.word	0x00000000
    1744: 14 2a 00 00  	.word	0x00002a14
    1748: 04 2a 00 00  	.word	0x00002a04
    174c: f8 29 00 00  	.word	0x000029f8
    1750: e2889001     	add	r9, r8, #1
    1754: e1a0a005     	mov	r10, r5
    1758: e2855010     	add	r5, r5, #16
    175c: ee048a10     	vmov	s8, r8
    1760: eeb01b48     	vmov.f64	d1, d8
    1764: eef84ac4     	vcvt.f32.s32	s9, s8
    1768: ee245a8b     	vmul.f32	s10, s9, s22
    176c: eeb70ac5     	vcvt.f64.f32	d0, s10
    1770: ebfffbe3     	bl	0x704 <.plt+0xec>       @ imm = #-0x1074  // CALL __pow_finite
    1774: ee059a90     	vmov	s11, r9
    1778: eef8bae5     	vcvt.f32.s32	s23, s11
    177c: ee6b6a8b     	vmul.f32	s13, s23, s22
    1780: eeb01b48     	vmov.f64	d1, d8
    1784: ee7a4b40     	vsub.f64	d20, d10, d0
    1788: ee695b24     	vmul.f64	d21, d9, d20
    178c: eeb70ae6     	vcvt.f64.f32	d0, s13
    1790: eeb76be5     	vcvt.f32.f64	s12, d21
    1794: ecaa6a01     	vstmia	r10!, {s12}
    1798: ebfffbd9     	bl	0x704 <.plt+0xec>       @ imm = #-0x109c  // CALL __pow_finite
    179c: e2880002     	add	r0, r8, #2
    17a0: ee010a10     	vmov	s2, r0
    17a4: eeb87ac1     	vcvt.f32.s32	s14, s2
    17a8: ee677a0b     	vmul.f32	s15, s14, s22
    17ac: eeb01b48     	vmov.f64	d1, d8
    17b0: ee7a6b40     	vsub.f64	d22, d10, d0
    17b4: ee697b26     	vmul.f64	d23, d9, d22
    17b8: eeb70ae7     	vcvt.f64.f32	d0, s15
    17bc: eeb72be7     	vcvt.f32.f64	s4, d23
    17c0: ed052a03     	vstr	s4, [r5, #-12]
    17c4: ebfffbce     	bl	0x704 <.plt+0xec>       @ imm = #-0x10c8  // CALL __pow_finite
    17c8: e2882003     	add	r2, r8, #3
    17cc: e2888004     	add	r8, r8, #4
    17d0: ee012a90     	vmov	s3, r2
    17d4: eef82ae1     	vcvt.f32.s32	s5, s3
    17d8: ee223a8b     	vmul.f32	s6, s5, s22
    17dc: eeb01b48     	vmov.f64	d1, d8
    17e0: ee7a8b40     	vsub.f64	d24, d10, d0
    17e4: ee699b28     	vmul.f64	d25, d9, d24
    17e8: eeb70ac3     	vcvt.f64.f32	d0, s6
    17ec: eef73be9     	vcvt.f32.f64	s7, d25
    17f0: edca3a01     	vstr	s7, [r10, #4]
    17f4: ebfffbc2     	bl	0x704 <.plt+0xec>       @ imm = #-0x10f8  // CALL __pow_finite
    17f8: e1570008     	cmp	r7, r8
    17fc: ee7aab40     	vsub.f64	d26, d10, d0
    1800: ee290b2a     	vmul.f64	d0, d9, d26
    1804: eeb70bc0     	vcvt.f32.f64	s0, d0
    1808: ed050a01     	vstr	s0, [r5, #-4]
    180c: 1affffcf     	bne	0x1750 <fftwObject_tilde_length+0x3f4> @ imm = #-0xc4
    1810: e51f70d4     	ldr	r7, [pc, #-0xd4]        @ 0x1744 <fftwObject_tilde_length+0x3e8>  // u32=0x2a14; f32?=1.50947871e-41
    1814: ee094a90     	vmov	s19, r4
    1818: eeb80ae9     	vcvt.f32.s32	s0, s19
    181c: ebfffbc1     	bl	0x728 <.plt+0x110>      @ imm = #-0x10fc  // CALL postfloat
    1820: e08f0007     	add	r0, pc, r7
    1824: ebfffba7     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0x1164  // CALL post
    1828: e5964d00     	ldr	r4, [r6, #0xd00]
    182c: ee0a4a90     	vmov	s21, r4
    1830: eeb80aea     	vcvt.f32.s32	s0, s21
    1834: ebfffbbb     	bl	0x728 <.plt+0x110>      @ imm = #-0x1114  // CALL postfloat
    1838: e51f30f8     	ldr	r3, [pc, #-0xf8]        @ 0x1748 <fftwObject_tilde_length+0x3ec>  // u32=0x2a04; f32?=1.50723663e-41
    183c: e08f0003     	add	r0, pc, r3
    1840: ebfffba0     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0x1180  // CALL post
    1844: e5966d04     	ldr	r6, [r6, #0xd04]
    1848: ee0b6a10     	vmov	s22, r6
    184c: eeb80acb     	vcvt.f32.s32	s0, s22
    1850: ebfffbb4     	bl	0x728 <.plt+0x110>      @ imm = #-0x1130  // CALL postfloat
    1854: e51fc110     	ldr	r12, [pc, #-0x110]      @ 0x174c <fftwObject_tilde_length+0x3f0>  // u32=0x29f8; f32?=1.50555507e-41
    1858: e08f000c     	add	r0, pc, r12
    185c: e28dd00c     	add	sp, sp, #12
    1860: ecbd8b08     	vpop	{d8, d9, d10, d11}
    1864: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1868: eafffb96     	b	0x6c8 <.plt+0xb0>       @ imm = #-0x11a8  // CALL post
    186c: e4838004     	str	r8, [r3], #4
    1870: eaffff64     	b	0x1608 <fftwObject_tilde_length+0x2ac> @ imm = #-0x270
    1874: e3a0a000     	mov	r10, #0
    1878: eaffff44     	b	0x1590 <fftwObject_tilde_length+0x234> @ imm = #-0x2f0

