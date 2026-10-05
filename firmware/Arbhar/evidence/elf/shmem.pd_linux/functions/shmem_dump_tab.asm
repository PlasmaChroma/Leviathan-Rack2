000012b8 <shmem_dump_tab>:
    12b8: e59fc1e8     	ldr	r12, [pc, #0x1e8]       @ 0x14a8 <shmem_dump_tab+0x1f0>
    12bc: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    12c0: e1a09001     	mov	r9, r1
    12c4: e59f11e0     	ldr	r1, [pc, #0x1e0]        @ 0x14ac <shmem_dump_tab+0x1f4>
    12c8: e08f400c     	add	r4, pc, r12
    12cc: e1a07003     	mov	r7, r3
    12d0: e24dd00c     	sub	sp, sp, #12
    12d4: e1a03004     	mov	r3, r4
    12d8: e1a08000     	mov	r8, r0
    12dc: e7943001     	ldr	r3, [r4, r1]
    12e0: e1a00009     	mov	r0, r9
    12e4: e1a05002     	mov	r5, r2
    12e8: e5931000     	ldr	r1, [r3]
    12ec: ebfffd45     	bl	0x808 <.plt+0x98>       @ imm = #-0xaec
    12f0: e2506000     	subs	r6, r0, #0
    12f4: 0a000062     	beq	0x1484 <shmem_dump_tab+0x1cc> @ imm = #0x188
    12f8: e1a0200d     	mov	r2, sp
    12fc: e28d1004     	add	r1, sp, #4
    1300: ebfffd3a     	bl	0x7f0 <.plt+0x80>       @ imm = #-0xb18
    1304: e2504000     	subs	r4, r0, #0
    1308: 0a000053     	beq	0x145c <shmem_dump_tab+0x1a4> @ imm = #0x14c
    130c: e5980028     	ldr	r0, [r8, #0x28]
    1310: e59d2004     	ldr	r2, [sp, #0x4]
    1314: e040e005     	sub	lr, r0, r5
    1318: eddd7a0a     	vldr	s15, [sp, #40]
    131c: e042c007     	sub	r12, r2, r7
    1320: ee07ea10     	vmov	s14, lr
    1324: ee00ca10     	vmov	s0, r12
    1328: eef80ac7     	vcvt.f32.s32	s1, s14
    132c: eeb81ac0     	vcvt.f32.s32	s2, s0
    1330: eef40ac1     	vcmpe.f32	s1, s2
    1334: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1338: 9eb01a60     	vmovls.f32	s2, s1
    133c: eefd1ac1     	vcvt.s32.f32	s3, s2
    1340: eef86ae7     	vcvt.f32.s32	s13, s15
    1344: eeb82ae1     	vcvt.f32.s32	s4, s3
    1348: eeb42ae6     	vcmpe.f32	s4, s13
    134c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    1350: 8eb02a66     	vmovhi.f32	s4, s13
    1354: eefd2ac2     	vcvt.s32.f32	s5, s4
    1358: ee124a90     	vmov	r4, s5
    135c: e3540000     	cmp	r4, #0
    1360: da00004e     	ble	0x14a0 <shmem_dump_tab+0x1e8> @ imm = #0x138
    1364: e5989024     	ldr	r9, [r8, #0x24]
    1368: e0841005     	add	r1, r4, r5
    136c: e59d8000     	ldr	r8, [sp]
    1370: e0892105     	add	r2, r9, r5, lsl #2
    1374: e0895101     	add	r5, r9, r1, lsl #2
    1378: e0450002     	sub	r0, r5, r2
    137c: e0883107     	add	r3, r8, r7, lsl #2
    1380: e2407004     	sub	r7, r0, #4
    1384: e1a0e127     	lsr	lr, r7, #2
    1388: e28ec001     	add	r12, lr, #1
    138c: e21c9007     	ands	r9, r12, #7
    1390: 0a00001a     	beq	0x1400 <shmem_dump_tab+0x148> @ imm = #0x68
    1394: e3590001     	cmp	r9, #1
    1398: 0a000014     	beq	0x13f0 <shmem_dump_tab+0x138> @ imm = #0x50
    139c: e3590002     	cmp	r9, #2
    13a0: 0a000010     	beq	0x13e8 <shmem_dump_tab+0x130> @ imm = #0x40
    13a4: e3590003     	cmp	r9, #3
    13a8: 0a00000c     	beq	0x13e0 <shmem_dump_tab+0x128> @ imm = #0x30
    13ac: e3590004     	cmp	r9, #4
    13b0: 0a000008     	beq	0x13d8 <shmem_dump_tab+0x120> @ imm = #0x20
    13b4: e3590005     	cmp	r9, #5
    13b8: 0a000004     	beq	0x13d0 <shmem_dump_tab+0x118> @ imm = #0x10
    13bc: e3590006     	cmp	r9, #6
    13c0: 14921004     	ldrne	r1, [r2], #4
    13c4: 14831004     	strne	r1, [r3], #4
    13c8: e4928004     	ldr	r8, [r2], #4
    13cc: e4838004     	str	r8, [r3], #4
    13d0: e4920004     	ldr	r0, [r2], #4
    13d4: e4830004     	str	r0, [r3], #4
    13d8: e4927004     	ldr	r7, [r2], #4
    13dc: e4837004     	str	r7, [r3], #4
    13e0: e492e004     	ldr	lr, [r2], #4
    13e4: e483e004     	str	lr, [r3], #4
    13e8: e492c004     	ldr	r12, [r2], #4
    13ec: e483c004     	str	r12, [r3], #4
    13f0: e4929004     	ldr	r9, [r2], #4
    13f4: e1550002     	cmp	r5, r2
    13f8: e4839004     	str	r9, [r3], #4
    13fc: 0a00001b     	beq	0x1470 <shmem_dump_tab+0x1b8> @ imm = #0x6c
    1400: e1a08002     	mov	r8, r2
    1404: e1a01003     	mov	r1, r3
    1408: e4980004     	ldr	r0, [r8], #4
    140c: e2822020     	add	r2, r2, #32
    1410: e2833020     	add	r3, r3, #32
    1414: e4810004     	str	r0, [r1], #4
    1418: e512701c     	ldr	r7, [r2, #-0x1c]
    141c: e503701c     	str	r7, [r3, #-0x1c]
    1420: e598e004     	ldr	lr, [r8, #0x4]
    1424: e581e004     	str	lr, [r1, #0x4]
    1428: e512c014     	ldr	r12, [r2, #-0x14]
    142c: e503c014     	str	r12, [r3, #-0x14]
    1430: e5129010     	ldr	r9, [r2, #-0x10]
    1434: e5039010     	str	r9, [r3, #-0x10]
    1438: e512800c     	ldr	r8, [r2, #-0xc]
    143c: e503800c     	str	r8, [r3, #-0xc]
    1440: e5121008     	ldr	r1, [r2, #-0x8]
    1444: e5031008     	str	r1, [r3, #-0x8]
    1448: e5120004     	ldr	r0, [r2, #-0x4]
    144c: e1550002     	cmp	r5, r2
    1450: e5030004     	str	r0, [r3, #-0x4]
    1454: 1affffe9     	bne	0x1400 <shmem_dump_tab+0x148> @ imm = #-0x5c
    1458: ea000004     	b	0x1470 <shmem_dump_tab+0x1b8> @ imm = #0x10
    145c: e59f504c     	ldr	r5, [pc, #0x4c]         @ 0x14b0 <shmem_dump_tab+0x1f8>
    1460: e1a00008     	mov	r0, r8
    1464: e5992000     	ldr	r2, [r9]
    1468: e08f1005     	add	r1, pc, r5
    146c: ebfffd06     	bl	0x88c <.plt+0x11c>      @ imm = #-0xbe8
    1470: e1a00006     	mov	r0, r6
    1474: ebfffcce     	bl	0x7b4 <.plt+0x44>       @ imm = #-0xcc8
    1478: e1a00004     	mov	r0, r4
    147c: e28dd00c     	add	sp, sp, #12
    1480: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    1484: e59f4028     	ldr	r4, [pc, #0x28]         @ 0x14b4 <shmem_dump_tab+0x1fc>
    1488: e1a00008     	mov	r0, r8
    148c: e5992000     	ldr	r2, [r9]
    1490: e08f1004     	add	r1, pc, r4
    1494: e1a04006     	mov	r4, r6
    1498: ebfffcfb     	bl	0x88c <.plt+0x11c>      @ imm = #-0xc14
    149c: eafffff3     	b	0x1470 <shmem_dump_tab+0x1b8> @ imm = #-0x34
    14a0: e3a04000     	mov	r4, #0
    14a4: eafffff1     	b	0x1470 <shmem_dump_tab+0x1b8> @ imm = #-0x3c
    14a8: 30 0d 01 00  	.word	0x00010d30
    14ac: 88 00 00 00  	.word	0x00000088
    14b0: f0 04 00 00  	.word	0x000004f0
    14b4: 08 04 00 00  	.word	0x00000408

