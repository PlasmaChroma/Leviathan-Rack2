00002358 <comport_new>:
    2358: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
    235c: ed2d8b02     	vpush	{d8}
    2360: e59f82d4     	ldr	r8, [pc, #0x2d4]        @ 0x263c <comport_new+0x2e4>  // u32=0x12c90; f32?=1.07821509e-40
    2364: e2514000     	subs	r4, r1, #0
    2368: e08f8008     	add	r8, pc, r8
    236c: e24ddc11     	sub	sp, sp, #4352
    2370: da000097     	ble	0x25d4 <comport_new+0x27c> @ imm = #0x25c
    2374: e5923000     	ldr	r3, [r2]
    2378: e1a06002     	mov	r6, r2
    237c: e3530001     	cmp	r3, #1
    2380: 0a000099     	beq	0x25ec <comport_new+0x294> @ imm = #0x264
    2384: ed9f8aaa     	vldr	s16, [pc, #680]         @ 0x2634 <comport_new+0x2dc>  // f32=0
    2388: e3a05000     	mov	r5, #0
    238c: e3540001     	cmp	r4, #1
    2390: 0a000093     	beq	0x25e4 <comport_new+0x28c> @ imm = #0x24c
    2394: e1a02006     	mov	r2, r6
    2398: e1a01004     	mov	r1, r4
    239c: e3a00001     	mov	r0, #1
    23a0: ebfffa06     	bl	0xbc0 <.plt+0x1b8>      @ imm = #-0x17e8  // CALL atom_getfloatarg
    23a4: e59f0294     	ldr	r0, [pc, #0x294]        @ 0x2640 <comport_new+0x2e8>  // u32=0x24a4; f32?=1.31441796e-41
    23a8: e28dc0a0     	add	r12, sp, #160
    23ac: e08f4000     	add	r4, pc, r0
    23b0: e3a09441     	mov	r9, #1090519040
    23b4: e894000f     	ldm	r4, {r0, r1, r2, r3}
    23b8: e28d6c01     	add	r6, sp, #256
    23bc: e3a0e000     	mov	lr, #0
    23c0: e1a0a823     	lsr	r10, r3, #16
    23c4: e8ac0007     	stm	r12!, {r0, r1, r2}
    23c8: e1a00005     	mov	r0, r5
    23cc: e0cc30b2     	strh	r3, [r12], #2
    23d0: e5cca000     	strb	r10, [r12]
    23d4: e28dcd42     	add	r12, sp, #4224
    23d8: e28ca028     	add	r10, r12, #40
    23dc: e28d5d42     	add	r5, sp, #4224
    23e0: e2855024     	add	r5, r5, #36
    23e4: e28dcd42     	add	r12, sp, #4224
    23e8: e58a9000     	str	r9, [r10]
    23ec: e28d9d42     	add	r9, sp, #4224
    23f0: e28ca030     	add	r10, r12, #48
    23f4: ed850a00     	vstr	s0, [r5]
    23f8: e289502c     	add	r5, r9, #44
    23fc: e3a02001     	mov	r2, #1
    2400: e3a03000     	mov	r3, #0
    2404: e1a0100d     	mov	r1, sp
    2408: e5862fbc     	str	r2, [r6, #0xfbc]
    240c: e585e000     	str	lr, [r5]
    2410: e58ae000     	str	lr, [r10]
    2414: e5863fb8     	str	r3, [r6, #0xfb8]
    2418: e5863fb4     	str	r3, [r6, #0xfb4]
    241c: ebfffc51     	bl	0x1568 <open_serial>    @ imm = #-0xebc
    2420: e59f121c     	ldr	r1, [pc, #0x21c]        @ 0x2644 <comport_new+0x2ec>  // u32=0xbc; f32?=2.63444111e-43
    2424: e1a09000     	mov	r9, r0
    2428: e7980001     	ldr	r0, [r8, r1]
    242c: e5900000     	ldr	r0, [r0]
    2430: ebfff985     	bl	0xa4c <.plt+0x44>       @ imm = #-0x19ec  // CALL pd_new
    2434: e1a03004     	mov	r3, r4
    2438: e28d4d42     	add	r4, sp, #4224
    243c: e2842020     	add	r2, r4, #32
    2440: e3790001     	cmn	r9, #1
    2444: e1d2e0f0     	ldrsh	lr, [r2]
    2448: e1a05000     	mov	r5, r0
    244c: e8b30007     	ldm	r3!, {r0, r1, r2}
    2450: e2854a01     	add	r4, r5, #4096
    2454: e1c4eab0     	strh	lr, [r4, #160]
    2458: e1d3c0b0     	ldrh	r12, [r3]
    245c: e58510a4     	str	r1, [r5, #0xa4]
    2460: e5d3a002     	ldrb	r10, [r3, #0x2]
    2464: e28d1d42     	add	r1, sp, #4224
    2468: e58500a0     	str	r0, [r5, #0xa0]
    246c: e28d3d42     	add	r3, sp, #4224
    2470: e2810024     	add	r0, r1, #36
    2474: e58520a8     	str	r2, [r5, #0xa8]
    2478: e2832028     	add	r2, r3, #40
    247c: e1c5cabc     	strh	r12, [r5, #172]
    2480: e5c5a0ae     	strb	r10, [r5, #0xae]
    2484: e28dcd42     	add	r12, sp, #4224
    2488: e590a000     	ldr	r10, [r0]
    248c: e28d0d42     	add	r0, sp, #4224
    2490: e592e000     	ldr	lr, [r2]
    2494: e28c102c     	add	r1, r12, #44
    2498: e5162064     	ldr	r2, [r6, #-0x64]
    249c: e2803030     	add	r3, r0, #48
    24a0: e591c000     	ldr	r12, [r1]
    24a4: e5930000     	ldr	r0, [r3]
    24a8: e5961fb8     	ldr	r1, [r6, #0xfb8]
    24ac: e585209c     	str	r2, [r5, #0x9c]
    24b0: e5962fb4     	ldr	r2, [r6, #0xfb4]
    24b4: e5966fbc     	ldr	r6, [r6, #0xfbc]
    24b8: e584a0a4     	str	r10, [r4, #0xa4]
    24bc: e584e0a8     	str	lr, [r4, #0xa8]
    24c0: e584c0ac     	str	r12, [r4, #0xac]
    24c4: e58400b0     	str	r0, [r4, #0xb0]
    24c8: e58410b8     	str	r1, [r4, #0xb8]
    24cc: e58420b4     	str	r2, [r4, #0xb4]
    24d0: e58460bc     	str	r6, [r4, #0xbc]
    24d4: e5859020     	str	r9, [r5, #0x20]
    24d8: 0a000036     	beq	0x25b8 <comport_new+0x260> @ imm = #0xd8
    24dc: e3a0203c     	mov	r2, #60
    24e0: e2851024     	add	r1, r5, #36
    24e4: e28d0024     	add	r0, sp, #36
    24e8: ebfff96c     	bl	0xaa0 <.plt+0x98>       @ imm = #-0x1a50  // CALL bcopy
    24ec: e28d0060     	add	r0, sp, #96
    24f0: e3a0203c     	mov	r2, #60
    24f4: e2851060     	add	r1, r5, #96
    24f8: ebfff968     	bl	0xaa0 <.plt+0x98>       @ imm = #-0x1a60  // CALL bcopy
    24fc: e3a00901     	mov	r0, #16384
    2500: ebfff954     	bl	0xa58 <.plt+0x50>       @ imm = #-0x1ab0  // CALL getbytes
    2504: e3500000     	cmp	r0, #0
    2508: e1a09000     	mov	r9, r0
    250c: e58400ec     	str	r0, [r4, #0xec]
    2510: 0a00003b     	beq	0x2604 <comport_new+0x2ac> @ imm = #0xec
    2514: e3a0a901     	mov	r10, #16384
    2518: e584a0f4     	str	r10, [r4, #0xf4]
    251c: e1a0000a     	mov	r0, r10
    2520: ebfff94c     	bl	0xa58 <.plt+0x50>       @ imm = #-0x1ad0  // CALL getbytes
    2524: e3500000     	cmp	r0, #0
    2528: e1a06000     	mov	r6, r0
    252c: e58400f0     	str	r0, [r4, #0xf0]
    2530: 0a000039     	beq	0x261c <comport_new+0x2c4> @ imm = #0xe4
    2534: e59fe10c     	ldr	lr, [pc, #0x10c]        @ 0x2648 <comport_new+0x2f0>  // u32=0xc4; f32?=2.74654499e-43
    2538: e3a06000     	mov	r6, #0
    253c: e584a0f8     	str	r10, [r4, #0xf8]
    2540: e58460fc     	str	r6, [r4, #0xfc]
    2544: e1c46cb0     	strh	r6, [r4, #192]
    2548: e798800e     	ldr	r8, [r8, lr]
    254c: e1a00005     	mov	r0, r5
    2550: e1a01008     	mov	r1, r8
    2554: ebfff97e     	bl	0xb54 <.plt+0x14c>      @ imm = #-0x1a08  // CALL outlet_new
    2558: e1a01008     	mov	r1, r8
    255c: e59f90e8     	ldr	r9, [pc, #0xe8]         @ 0x264c <comport_new+0x2f4>  // u32=0x40240000; f32?=2.5625
    2560: e2857d43     	add	r7, r5, #4288
    2564: e3a08000     	mov	r8, #0
    2568: e58400e4     	str	r0, [r4, #0xe4]
    256c: e1a00005     	mov	r0, r5
    2570: ebfff977     	bl	0xb54 <.plt+0x14c>      @ imm = #-0x1a24  // CALL outlet_new
    2574: e59f30d4     	ldr	r3, [pc, #0xd4]         @ 0x2650 <comport_new+0x2f8>  // u32=0x3e8; f32?=1.40129846e-42
    2578: e3a0c00a     	mov	r12, #10
    257c: e58460c8     	str	r6, [r4, #0xc8]
    2580: e08f1003     	add	r1, pc, r3
    2584: e58400e8     	str	r0, [r4, #0xe8]
    2588: e1c781f8     	strd	r8, r9, [r7, #24]
    258c: e1a00005     	mov	r0, r5
    2590: e584c0cc     	str	r12, [r4, #0xcc]
    2594: ebfff94d     	bl	0xad0 <.plt+0xc8>       @ imm = #-0x1acc  // CALL clock_new
    2598: ed970b06     	vldr	d0, [r7, #24]
    259c: e58400c4     	str	r0, [r4, #0xc4]
    25a0: ebfff938     	bl	0xa88 <.plt+0x80>       @ imm = #-0x1b20  // CALL clock_delay
    25a4: e1a00005     	mov	r0, r5
    25a8: e58460e0     	str	r6, [r4, #0xe0]
    25ac: e28ddc11     	add	sp, sp, #4352
    25b0: ecbd8b02     	vpop	{d8}
    25b4: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    25b8: eeb77ac8     	vcvt.f64.f32	d7, s16
    25bc: e59f7090     	ldr	r7, [pc, #0x90]         @ 0x2654 <comport_new+0x2fc>  // u32=0x229c; f32?=1.24155044e-41
    25c0: e1a00005     	mov	r0, r5
    25c4: e08f1007     	add	r1, pc, r7
    25c8: ec532b17     	vmov	r2, r3, d7
    25cc: ebfff984     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x19f0  // CALL pd_error
    25d0: eaffffc9     	b	0x24fc <comport_new+0x1a4> @ imm = #-0xdc
    25d4: e3a05000     	mov	r5, #0
    25d8: ed9f8a15     	vldr	s16, [pc, #84]          @ 0x2634 <comport_new+0x2dc>  // f32=0
    25dc: ed9f0a15     	vldr	s0, [pc, #84]           @ 0x2638 <comport_new+0x2e0>  // f32=9600
    25e0: eaffff6f     	b	0x23a4 <comport_new+0x4c> @ imm = #-0x244
    25e4: ed9f0a13     	vldr	s0, [pc, #76]           @ 0x2638 <comport_new+0x2e0>  // f32=9600
    25e8: eaffff6d     	b	0x23a4 <comport_new+0x4c> @ imm = #-0x24c
    25ec: e3a00000     	mov	r0, #0
    25f0: ebfff972     	bl	0xbc0 <.plt+0x1b8>      @ imm = #-0x1a38  // CALL atom_getfloatarg
    25f4: eefc7ac0     	vcvt.u32.f32	s15, s0
    25f8: eeb08a40     	vmov.f32	s16, s0
    25fc: ee175a90     	vmov	r5, s15
    2600: eaffff61     	b	0x238c <comport_new+0x34> @ imm = #-0x27c
    2604: e59f404c     	ldr	r4, [pc, #0x4c]         @ 0x2658 <comport_new+0x300>  // u32=0x2280; f32?=1.2376268e-41
    2608: e1a00005     	mov	r0, r5
    260c: e08f1004     	add	r1, pc, r4
    2610: ebfff973     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x1a34  // CALL pd_error
    2614: e1a00009     	mov	r0, r9
    2618: eaffffe3     	b	0x25ac <comport_new+0x254> @ imm = #-0x74
    261c: e1a00005     	mov	r0, r5
    2620: e59f5034     	ldr	r5, [pc, #0x34]         @ 0x265c <comport_new+0x304>  // u32=0x2294; f32?=1.2404294e-41
    2624: e08f1005     	add	r1, pc, r5
    2628: ebfff96d     	bl	0xbe4 <.plt+0x1dc>      @ imm = #-0x1a4c  // CALL pd_error
    262c: e1a00006     	mov	r0, r6
    2630: eaffffdd     	b	0x25ac <comport_new+0x254> @ imm = #-0x8c
    2634: 00 00 00 00  	.word	0x00000000
    2638: 00 00 16 46  	.word	0x46160000
    263c: 90 2c 01 00  	.word	0x00012c90
    2640: a4 24 00 00  	.word	0x000024a4
    2644: bc 00 00 00  	.word	0x000000bc
    2648: c4 00 00 00  	.word	0x000000c4
    264c: 00 00 24 40  	.word	0x40240000
    2650: e8 03 00 00  	.word	0x000003e8
    2654: 9c 22 00 00  	.word	0x0000229c
    2658: 80 22 00 00  	.word	0x00002280
    265c: 94 22 00 00  	.word	0x00002294

