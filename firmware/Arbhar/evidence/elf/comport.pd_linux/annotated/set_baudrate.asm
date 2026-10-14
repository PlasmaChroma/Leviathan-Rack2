000012f0 <set_baudrate>:
    12f0: e59f2254     	ldr	r2, [pc, #0x254]        @ 0x154c <set_baudrate+0x25c>  // u32=0x2a18; f32?=1.51003923e-41
    12f4: e92d4070     	push	{r4, r5, r6, lr}
    12f8: e08fc002     	add	r12, pc, r2
    12fc: ed2d8b04     	vpush	{d8, d9}
    1300: e59c3000     	ldr	r3, [r12]
    1304: eeb08a40     	vmov.f32	s16, s0
    1308: e1a05000     	mov	r5, r0
    130c: ee073a90     	vmov	s15, r3
    1310: e24dd008     	sub	sp, sp, #8
    1314: e2806060     	add	r6, r0, #96
    1318: eef84ae7     	vcvt.f32.s32	s9, s15
    131c: eef44ac0     	vcmpe.f32	s9, s0
    1320: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1324: d3a04000     	movle	r4, #0
    1328: da00007d     	ble	0x1524 <set_baudrate+0x234> @ imm = #0x1f4
    132c: e3a04001     	mov	r4, #1
    1330: e59c3004     	ldr	r3, [r12, #0x4]
    1334: e28c1004     	add	r1, r12, #4
    1338: ee003a10     	vmov	s0, r3
    133c: eef84ac0     	vcvt.f32.s32	s9, s0
    1340: eef44ac8     	vcmpe.f32	s9, s16
    1344: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1348: da000075     	ble	0x1524 <set_baudrate+0x234> @ imm = #0x1d4
    134c: e5913004     	ldr	r3, [r1, #0x4]
    1350: e2844001     	add	r4, r4, #1
    1354: e1a00004     	mov	r0, r4
    1358: ee003a90     	vmov	s1, r3
    135c: eef84ae0     	vcvt.f32.s32	s9, s1
    1360: eef44ac8     	vcmpe.f32	s9, s16
    1364: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1368: da00006d     	ble	0x1524 <set_baudrate+0x234> @ imm = #0x1b4
    136c: e59c300c     	ldr	r3, [r12, #0xc]
    1370: e2844001     	add	r4, r4, #1
    1374: ee013a10     	vmov	s2, r3
    1378: eef84ac1     	vcvt.f32.s32	s9, s2
    137c: eef44ac8     	vcmpe.f32	s9, s16
    1380: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1384: da000066     	ble	0x1524 <set_baudrate+0x234> @ imm = #0x198
    1388: e59c3010     	ldr	r3, [r12, #0x10]
    138c: e2804002     	add	r4, r0, #2
    1390: ee013a90     	vmov	s3, r3
    1394: eef84ae1     	vcvt.f32.s32	s9, s3
    1398: eef44ac8     	vcmpe.f32	s9, s16
    139c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    13a0: da00005f     	ble	0x1524 <set_baudrate+0x234> @ imm = #0x17c
    13a4: e59c3014     	ldr	r3, [r12, #0x14]
    13a8: e2804003     	add	r4, r0, #3
    13ac: ee023a10     	vmov	s4, r3
    13b0: eef84ac2     	vcvt.f32.s32	s9, s4
    13b4: eef44ac8     	vcmpe.f32	s9, s16
    13b8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    13bc: da000058     	ble	0x1524 <set_baudrate+0x234> @ imm = #0x160
    13c0: e59c3018     	ldr	r3, [r12, #0x18]
    13c4: e2804004     	add	r4, r0, #4
    13c8: ee023a90     	vmov	s5, r3
    13cc: eef84ae2     	vcvt.f32.s32	s9, s5
    13d0: eef44ac8     	vcmpe.f32	s9, s16
    13d4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    13d8: da000051     	ble	0x1524 <set_baudrate+0x234> @ imm = #0x144
    13dc: e59c301c     	ldr	r3, [r12, #0x1c]
    13e0: e2804005     	add	r4, r0, #5
    13e4: ee033a10     	vmov	s6, r3
    13e8: eef84ac3     	vcvt.f32.s32	s9, s6
    13ec: eef44ac8     	vcmpe.f32	s9, s16
    13f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    13f4: da00004a     	ble	0x1524 <set_baudrate+0x234> @ imm = #0x128
    13f8: e59c3020     	ldr	r3, [r12, #0x20]
    13fc: e2804006     	add	r4, r0, #6
    1400: ee033a90     	vmov	s7, r3
    1404: eef84ae3     	vcvt.f32.s32	s9, s7
    1408: eef44ac8     	vcmpe.f32	s9, s16
    140c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1410: da000043     	ble	0x1524 <set_baudrate+0x234> @ imm = #0x10c
    1414: e5bc3024     	ldr	r3, [r12, #0x24]!
    1418: e2804007     	add	r4, r0, #7
    141c: ee043a10     	vmov	s8, r3
    1420: eef84ac4     	vcvt.f32.s32	s9, s8
    1424: eef44ac8     	vcmpe.f32	s9, s16
    1428: eef1fa10     	vmrs	APSR_nzcv, fpscr
    142c: da00003c     	ble	0x1524 <set_baudrate+0x234> @ imm = #0xf0
    1430: e2804008     	add	r4, r0, #8
    1434: e3540013     	cmp	r4, #19
    1438: 1affffbc     	bne	0x1330 <set_baudrate+0x40> @ imm = #-0x110
    143c: eeb58a40     	vcmp.f32	s16, #0
    1440: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1444: 13a03000     	movne	r3, #0
    1448: 0a00002d     	beq	0x1504 <set_baudrate+0x214> @ imm = #0xb4
    144c: eeb79ac8     	vcvt.f64.f32	d9, s16
    1450: e59fe0f8     	ldr	lr, [pc, #0xf8]         @ 0x1550 <set_baudrate+0x260>  // u32=0x2ed0; f32?=1.67931608e-41
    1454: e58d3000     	str	r3, [sp]
    1458: e08f000e     	add	r0, pc, lr
    145c: ec532b19     	vmov	r2, r3, d9
    1460: ebfffdb5     	bl	0xb3c <.plt+0x134>      @ imm = #-0x92c  // CALL post
    1464: e3540013     	cmp	r4, #19
    1468: 0a000026     	beq	0x1508 <set_baudrate+0x218> @ imm = #0x98
    146c: e59f20e0     	ldr	r2, [pc, #0xe0]         @ 0x1554 <set_baudrate+0x264>  // u32=0x28a0; f32?=1.4573504e-41
    1470: e08fc002     	add	r12, pc, r2
    1474: e08c1104     	add	r1, r12, r4, lsl #2
    1478: e591004c     	ldr	r0, [r1, #0x4c]
    147c: e3500000     	cmp	r0, #0
    1480: ba00001f     	blt	0x1504 <set_baudrate+0x214> @ imm = #0x7c
    1484: e79c4104     	ldr	r4, [r12, r4, lsl #2]
    1488: ee054a10     	vmov	s10, r4
    148c: e1a04000     	mov	r4, r0
    1490: eef88ac5     	vcvt.f32.s32	s17, s10
    1494: e2853a01     	add	r3, r5, #4096
    1498: e59350e0     	ldr	r5, [r3, #0xe0]
    149c: e3550000     	cmp	r5, #0
    14a0: da000005     	ble	0x14bc <set_baudrate+0x1cc> @ imm = #0x14
    14a4: eeb77ae8     	vcvt.f64.f32	d7, s17
    14a8: e59fc0a8     	ldr	r12, [pc, #0xa8]        @ 0x1558 <set_baudrate+0x268>  // u32=0x2ef4; f32?=1.68436075e-41
    14ac: e58d4000     	str	r4, [sp]
    14b0: e08f000c     	add	r0, pc, r12
    14b4: ec532b17     	vmov	r2, r3, d7
    14b8: ebfffd9f     	bl	0xb3c <.plt+0x134>      @ imm = #-0x984  // CALL post
    14bc: e1a01004     	mov	r1, r4
    14c0: e1a00006     	mov	r0, r6
    14c4: ebfffd8d     	bl	0xb00 <.plt+0xf8>       @ imm = #-0x9cc  // CALL cfsetispeed
    14c8: e3500000     	cmp	r0, #0
    14cc: 1a000018     	bne	0x1534 <set_baudrate+0x244> @ imm = #0x60
    14d0: e1a00006     	mov	r0, r6
    14d4: e1a01004     	mov	r1, r4
    14d8: ebfffd67     	bl	0xa7c <.plt+0x74>       @ imm = #-0xa64  // CALL cfsetospeed
    14dc: e3500000     	cmp	r0, #0
    14e0: 0a000003     	beq	0x14f4 <set_baudrate+0x204> @ imm = #0xc
    14e4: e59f6070     	ldr	r6, [pc, #0x70]         @ 0x155c <set_baudrate+0x26c>  // u32=0x2efc; f32?=1.68548179e-41
    14e8: e1a01004     	mov	r1, r4
    14ec: e08f0006     	add	r0, pc, r6
    14f0: ebfffd91     	bl	0xb3c <.plt+0x134>      @ imm = #-0x9bc  // CALL post
    14f4: eeb00a68     	vmov.f32	s0, s17
    14f8: e28dd008     	add	sp, sp, #8
    14fc: ecbd8b04     	vpop	{d8, d9}
    1500: e8bd8070     	pop	{r4, r5, r6, pc}
    1504: eeb79ac8     	vcvt.f64.f32	d9, s16
    1508: e59fe050     	ldr	lr, [pc, #0x50]         @ 0x1560 <set_baudrate+0x270>  // u32=0x2e48; f32?=1.66025842e-41
    150c: ec532b19     	vmov	r2, r3, d9
    1510: e08f000e     	add	r0, pc, lr
    1514: ebfffd88     	bl	0xb3c <.plt+0x134>      @ imm = #-0x9e0  // CALL post
    1518: eddf8a0a     	vldr	s17, [pc, #40]          @ 0x1548 <set_baudrate+0x258>  // f32=1800
    151c: e3a0400a     	mov	r4, #10
    1520: eaffffdb     	b	0x1494 <set_baudrate+0x1a4> @ imm = #-0x94
    1524: eef44a48     	vcmp.f32	s9, s16
    1528: eef1fa10     	vmrs	APSR_nzcv, fpscr
    152c: 0affffce     	beq	0x146c <set_baudrate+0x17c> @ imm = #-0xc8
    1530: eaffffc5     	b	0x144c <set_baudrate+0x15c> @ imm = #-0xec
    1534: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x1564 <set_baudrate+0x274>  // u32=0x2eac; f32?=1.67427141e-41
    1538: e1a01004     	mov	r1, r4
    153c: e08f0002     	add	r0, pc, r2
    1540: ebfffd7d     	bl	0xb3c <.plt+0x134>      @ imm = #-0xa0c  // CALL post
    1544: eaffffe1     	b	0x14d0 <set_baudrate+0x1e0> @ imm = #-0x7c
    1548: 00 00 e1 44  	.word	0x44e10000
    154c: 18 2a 00 00  	.word	0x00002a18
    1550: d0 2e 00 00  	.word	0x00002ed0
    1554: a0 28 00 00  	.word	0x000028a0
    1558: f4 2e 00 00  	.word	0x00002ef4
    155c: fc 2e 00 00  	.word	0x00002efc
    1560: 48 2e 00 00  	.word	0x00002e48
    1564: ac 2e 00 00  	.word	0x00002eac

