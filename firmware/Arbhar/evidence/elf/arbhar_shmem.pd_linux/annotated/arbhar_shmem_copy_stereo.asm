00001054 <arbhar_shmem_copy_stereo>:
    1054: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    1058: e3520001     	cmp	r2, #1
    105c: ed2d8b02     	vpush	{d8}
    1060: e24dd01c     	sub	sp, sp, #28
    1064: da0000bb     	ble	0x1358 <arbhar_shmem_copy_stereo+0x304> @ imm = #0x2ec
    1068: e1a07000     	mov	r7, r0
    106c: e59f03ac     	ldr	r0, [pc, #0x3ac]        @ 0x1420 <arbhar_shmem_copy_stereo+0x3cc>  // u32=0x1b64; f32?=9.82590483e-42
    1070: e1a05002     	mov	r5, r2
    1074: e1a09003     	mov	r9, r3
    1078: e08f0000     	add	r0, pc, r0
    107c: e3a03002     	mov	r3, #2
    1080: e58d3008     	str	r3, [sp, #0x8]
    1084: ebfffe5f     	bl	0xa08 <.plt+0x14>       @ imm = #-0x684  // CALL gensym
    1088: e1a02009     	mov	r2, r9
    108c: e1a01005     	mov	r1, r5
    1090: e58d000c     	str	r0, [sp, #0xc]
    1094: e3a00000     	mov	r0, #0
    1098: ebfffebd     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x50c  // CALL atom_getfloatarg
    109c: e3a00001     	mov	r0, #1
    10a0: e1a02009     	mov	r2, r9
    10a4: e1a01005     	mov	r1, r5
    10a8: eebd0ac0     	vcvt.s32.f32	s0, s0
    10ac: ee104a10     	vmov	r4, s0
    10b0: ebfffeb7     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x524  // CALL atom_getfloatarg
    10b4: e2446001     	sub	r6, r4, #1
    10b8: e1a02086     	lsl	r2, r6, #1
    10bc: e282a002     	add	r10, r2, #2
    10c0: eebd8ac0     	vcvt.s32.f32	s16, s0
    10c4: ee181a10     	vmov	r1, s16
    10c8: e2418001     	sub	r8, r1, #1
    10cc: e1a0c088     	lsl	r12, r8, #1
    10d0: e28cb001     	add	r11, r12, #1
    10d4: e28c4002     	add	r4, r12, #2
    10d8: e35b0000     	cmp	r11, #0
    10dc: c3a00000     	movgt	r0, #0
    10e0: d3a00001     	movle	r0, #1
    10e4: e1903fa2     	orrs	r3, r0, r2, lsr #31
    10e8: 1a000085     	bne	0x1304 <arbhar_shmem_copy_stereo+0x2b0> @ imm = #0x214
    10ec: e3540000     	cmp	r4, #0
    10f0: c35a0000     	cmpgt	r10, #0
    10f4: e3a03002     	mov	r3, #2
    10f8: d3a02001     	movle	r2, #1
    10fc: c3a02000     	movgt	r2, #0
    1100: da00007f     	ble	0x1304 <arbhar_shmem_copy_stereo+0x2b0> @ imm = #0x1fc
    1104: e3550002     	cmp	r5, #2
    1108: e0876186     	add	r6, r7, r6, lsl #3
    110c: 01a03002     	moveq	r3, r2
    1110: e5968090     	ldr	r8, [r6, #0x90]
    1114: 01a02003     	moveq	r2, r3
    1118: 1a000096     	bne	0x1378 <arbhar_shmem_copy_stereo+0x324> @ imm = #0x258
    111c: e1530008     	cmp	r3, r8
    1120: aa0000ae     	bge	0x13e0 <arbhar_shmem_copy_stereo+0x38c> @ imm = #0x2b8
    1124: e596e05c     	ldr	lr, [r6, #0x5c]
    1128: e1a09103     	lsl	r9, r3, #2
    112c: e28b5016     	add	r5, r11, #22
    1130: e284c016     	add	r12, r4, #22
    1134: e08e3009     	add	r3, lr, r9
    1138: e08eb108     	add	r11, lr, r8, lsl #2
    113c: e04b1003     	sub	r1, r11, r3
    1140: e28aa016     	add	r10, r10, #22
    1144: e2414004     	sub	r4, r1, #4
    1148: e7970105     	ldr	r0, [r7, r5, lsl #2]
    114c: e797610a     	ldr	r6, [r7, r10, lsl #2]
    1150: e1a02102     	lsl	r2, r2, #2
    1154: e1a05124     	lsr	r5, r4, #2
    1158: e797e10c     	ldr	lr, [r7, r12, lsl #2]
    115c: e285a001     	add	r10, r5, #1
    1160: e086c009     	add	r12, r6, r9
    1164: e21a9007     	ands	r9, r10, #7
    1168: e0806002     	add	r6, r0, r2
    116c: e08e2002     	add	r2, lr, r2
    1170: 0a000025     	beq	0x120c <arbhar_shmem_copy_stereo+0x1b8> @ imm = #0x94
    1174: e3590001     	cmp	r9, #1
    1178: 0a00001d     	beq	0x11f4 <arbhar_shmem_copy_stereo+0x1a0> @ imm = #0x74
    117c: e3590002     	cmp	r9, #2
    1180: 0a000017     	beq	0x11e4 <arbhar_shmem_copy_stereo+0x190> @ imm = #0x5c
    1184: e3590003     	cmp	r9, #3
    1188: 0a000011     	beq	0x11d4 <arbhar_shmem_copy_stereo+0x180> @ imm = #0x44
    118c: e3590004     	cmp	r9, #4
    1190: 0a00000b     	beq	0x11c4 <arbhar_shmem_copy_stereo+0x170> @ imm = #0x2c
    1194: e3590005     	cmp	r9, #5
    1198: 0a000005     	beq	0x11b4 <arbhar_shmem_copy_stereo+0x160> @ imm = #0x14
    119c: e3590006     	cmp	r9, #6
    11a0: 1a000095     	bne	0x13fc <arbhar_shmem_copy_stereo+0x3a8> @ imm = #0x254
    11a4: e4930004     	ldr	r0, [r3], #4
    11a8: e4860004     	str	r0, [r6], #4
    11ac: e49c5004     	ldr	r5, [r12], #4
    11b0: e4825004     	str	r5, [r2], #4
    11b4: e493e004     	ldr	lr, [r3], #4
    11b8: e486e004     	str	lr, [r6], #4
    11bc: e49ca004     	ldr	r10, [r12], #4
    11c0: e482a004     	str	r10, [r2], #4
    11c4: e4939004     	ldr	r9, [r3], #4
    11c8: e4869004     	str	r9, [r6], #4
    11cc: e49c1004     	ldr	r1, [r12], #4
    11d0: e4821004     	str	r1, [r2], #4
    11d4: e4934004     	ldr	r4, [r3], #4
    11d8: e4864004     	str	r4, [r6], #4
    11dc: e49c0004     	ldr	r0, [r12], #4
    11e0: e4820004     	str	r0, [r2], #4
    11e4: e4935004     	ldr	r5, [r3], #4
    11e8: e4865004     	str	r5, [r6], #4
    11ec: e49ce004     	ldr	lr, [r12], #4
    11f0: e482e004     	str	lr, [r2], #4
    11f4: e493a004     	ldr	r10, [r3], #4
    11f8: e153000b     	cmp	r3, r11
    11fc: e486a004     	str	r10, [r6], #4
    1200: e49c9004     	ldr	r9, [r12], #4
    1204: e4829004     	str	r9, [r2], #4
    1208: 0a000029     	beq	0x12b4 <arbhar_shmem_copy_stereo+0x260> @ imm = #0xa4
    120c: e1a00003     	mov	r0, r3
    1210: e1a04006     	mov	r4, r6
    1214: e4905004     	ldr	r5, [r0], #4
    1218: e1a0e00c     	mov	lr, r12
    121c: e1a01002     	mov	r1, r2
    1220: e2833020     	add	r3, r3, #32
    1224: e2866020     	add	r6, r6, #32
    1228: e28cc020     	add	r12, r12, #32
    122c: e4845004     	str	r5, [r4], #4
    1230: e2822020     	add	r2, r2, #32
    1234: e49ea004     	ldr	r10, [lr], #4
    1238: e481a004     	str	r10, [r1], #4
    123c: e513901c     	ldr	r9, [r3, #-0x1c]
    1240: e506901c     	str	r9, [r6, #-0x1c]
    1244: e51c501c     	ldr	r5, [r12, #-0x1c]
    1248: e502501c     	str	r5, [r2, #-0x1c]
    124c: e5900004     	ldr	r0, [r0, #0x4]
    1250: e5840004     	str	r0, [r4, #0x4]
    1254: e59e4004     	ldr	r4, [lr, #0x4]
    1258: e5814004     	str	r4, [r1, #0x4]
    125c: e513e014     	ldr	lr, [r3, #-0x14]
    1260: e506e014     	str	lr, [r6, #-0x14]
    1264: e51c1014     	ldr	r1, [r12, #-0x14]
    1268: e5021014     	str	r1, [r2, #-0x14]
    126c: e513a010     	ldr	r10, [r3, #-0x10]
    1270: e506a010     	str	r10, [r6, #-0x10]
    1274: e51c9010     	ldr	r9, [r12, #-0x10]
    1278: e5029010     	str	r9, [r2, #-0x10]
    127c: e513500c     	ldr	r5, [r3, #-0xc]
    1280: e506500c     	str	r5, [r6, #-0xc]
    1284: e51c000c     	ldr	r0, [r12, #-0xc]
    1288: e502000c     	str	r0, [r2, #-0xc]
    128c: e5134008     	ldr	r4, [r3, #-0x8]
    1290: e5064008     	str	r4, [r6, #-0x8]
    1294: e51ce008     	ldr	lr, [r12, #-0x8]
    1298: e502e008     	str	lr, [r2, #-0x8]
    129c: e5131004     	ldr	r1, [r3, #-0x4]
    12a0: e153000b     	cmp	r3, r11
    12a4: e5061004     	str	r1, [r6, #-0x4]
    12a8: e51ca004     	ldr	r10, [r12, #-0x4]
    12ac: e502a004     	str	r10, [r2, #-0x4]
    12b0: 1affffd5     	bne	0x120c <arbhar_shmem_copy_stereo+0x1b8> @ imm = #-0xac
    12b4: eef81ac8     	vcvt.f32.s32	s3, s16
    12b8: e59fb164     	ldr	r11, [pc, #0x164]       @ 0x1424 <arbhar_shmem_copy_stereo+0x3d0>  // u32=0x1928; f32?=9.02436211e-42
    12bc: e5975054     	ldr	r5, [r7, #0x54]
    12c0: e3a09001     	mov	r9, #1
    12c4: e08f000b     	add	r0, pc, r11
    12c8: e58d9010     	str	r9, [sp, #0x10]
    12cc: edcd1a05     	vstr	s3, [sp, #20]
    12d0: ebfffdcc     	bl	0xa08 <.plt+0x14>       @ imm = #-0x8d0  // CALL gensym
    12d4: e28d3008     	add	r3, sp, #8
    12d8: e3a02002     	mov	r2, #2
    12dc: e1a01000     	mov	r1, r0
    12e0: e1a00005     	mov	r0, r5
    12e4: ebfffe21     	bl	0xb70 <.plt+0x17c>      @ imm = #-0x77c  // CALL outlet_list
    12e8: e5970050     	ldr	r0, [r7, #0x50]
    12ec: ed9f0b49     	vldr	d0, [pc, #292]          @ 0x1418 <arbhar_shmem_copy_stereo+0x3c4>  // f64=100
    12f0: ebfffe12     	bl	0xb40 <.plt+0x14c>      @ imm = #-0x7b8  // CALL clock_set
    12f4: e1a00008     	mov	r0, r8
    12f8: e28dd01c     	add	sp, sp, #28
    12fc: ecbd8b02     	vpop	{d8}
    1300: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1304: e59f811c     	ldr	r8, [pc, #0x11c]        @ 0x1428 <arbhar_shmem_copy_stereo+0x3d4>  // u32=0x1824; f32?=8.66002451e-42
    1308: e08f0008     	add	r0, pc, r8
    130c: e3a08000     	mov	r8, #0
    1310: ebfffdfe     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x808  // CALL post
    1314: e59f0110     	ldr	r0, [pc, #0x110]        @ 0x142c <arbhar_shmem_copy_stereo+0x3d8>  // u32=0x18cc; f32?=8.89544265e-42
    1318: e3a01000     	mov	r1, #0
    131c: e3a03001     	mov	r3, #1
    1320: e08f0000     	add	r0, pc, r0
    1324: e5974054     	ldr	r4, [r7, #0x54]
    1328: e58d3010     	str	r3, [sp, #0x10]
    132c: e58d1014     	str	r1, [sp, #0x14]
    1330: ebfffdb4     	bl	0xa08 <.plt+0x14>       @ imm = #-0x930  // CALL gensym
    1334: e28d3008     	add	r3, sp, #8
    1338: e3a02002     	mov	r2, #2
    133c: e1a01000     	mov	r1, r0
    1340: e1a00004     	mov	r0, r4
    1344: ebfffe09     	bl	0xb70 <.plt+0x17c>      @ imm = #-0x7dc  // CALL outlet_list
    1348: e1a00008     	mov	r0, r8
    134c: e28dd01c     	add	sp, sp, #28
    1350: ecbd8b02     	vpop	{d8}
    1354: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1358: e59fe0d0     	ldr	lr, [pc, #0xd0]         @ 0x1430 <arbhar_shmem_copy_stereo+0x3dc>  // u32=0x1778; f32?=8.41900117e-42
    135c: e3a08000     	mov	r8, #0
    1360: e08f000e     	add	r0, pc, lr
    1364: ebfffde9     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x85c  // CALL post
    1368: e1a00008     	mov	r0, r8
    136c: e28dd01c     	add	sp, sp, #28
    1370: ecbd8b02     	vpop	{d8}
    1374: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    1378: e1a00003     	mov	r0, r3
    137c: e1a02009     	mov	r2, r9
    1380: e1a01005     	mov	r1, r5
    1384: ebfffe02     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x7f8  // CALL atom_getfloatarg
    1388: e3550003     	cmp	r5, #3
    138c: eefd7ac0     	vcvt.s32.f32	s15, s0
    1390: ee173a90     	vmov	r3, s15
    1394: 0a00001d     	beq	0x1410 <arbhar_shmem_copy_stereo+0x3bc> @ imm = #0x74
    1398: e1a02009     	mov	r2, r9
    139c: e1a01005     	mov	r1, r5
    13a0: e3a00003     	mov	r0, #3
    13a4: edcd7a01     	vstr	s15, [sp, #4]
    13a8: ebfffdf9     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x81c  // CALL atom_getfloatarg
    13ac: e3550004     	cmp	r5, #4
    13b0: e59d3004     	ldr	r3, [sp, #0x4]
    13b4: eefd0ac0     	vcvt.s32.f32	s1, s0
    13b8: ee108a90     	vmov	r8, s1
    13bc: da000013     	ble	0x1410 <arbhar_shmem_copy_stereo+0x3bc> @ imm = #0x4c
    13c0: e1a02009     	mov	r2, r9
    13c4: e1a01005     	mov	r1, r5
    13c8: e3a00004     	mov	r0, #4
    13cc: ebfffdf0     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x840  // CALL atom_getfloatarg
    13d0: e59d3004     	ldr	r3, [sp, #0x4]
    13d4: eebd1ac0     	vcvt.s32.f32	s2, s0
    13d8: ee112a10     	vmov	r2, s2
    13dc: eaffff4e     	b	0x111c <arbhar_shmem_copy_stereo+0xc8> @ imm = #-0x2c8
    13e0: e59f704c     	ldr	r7, [pc, #0x4c]         @ 0x1434 <arbhar_shmem_copy_stereo+0x3e0>  // u32=0x17ac; f32?=8.49186869e-42
    13e4: e1a02008     	mov	r2, r8
    13e8: e1a01003     	mov	r1, r3
    13ec: e3a08000     	mov	r8, #0
    13f0: e08f0007     	add	r0, pc, r7
    13f4: ebfffdc5     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x8ec  // CALL post
    13f8: eaffffd2     	b	0x1348 <arbhar_shmem_copy_stereo+0x2f4> @ imm = #-0xb8
    13fc: e4931004     	ldr	r1, [r3], #4
    1400: e4861004     	str	r1, [r6], #4
    1404: e49c4004     	ldr	r4, [r12], #4
    1408: e4824004     	str	r4, [r2], #4
    140c: eaffff64     	b	0x11a4 <arbhar_shmem_copy_stereo+0x150> @ imm = #-0x270
    1410: e1a02003     	mov	r2, r3
    1414: eaffff40     	b	0x111c <arbhar_shmem_copy_stereo+0xc8> @ imm = #-0x300
    1418: 00 00 00 00  	.word	0x00000000
    141c: 00 00 59 40  	.word	0x40590000
    1420: 64 1b 00 00  	.word	0x00001b64
    1424: 28 19 00 00  	.word	0x00001928
    1428: 24 18 00 00  	.word	0x00001824
    142c: cc 18 00 00  	.word	0x000018cc
    1430: 78 17 00 00  	.word	0x00001778
    1434: ac 17 00 00  	.word	0x000017ac

