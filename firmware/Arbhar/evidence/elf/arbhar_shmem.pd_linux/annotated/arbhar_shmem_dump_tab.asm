000023cc <arbhar_shmem_dump_tab>:
    23cc: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
    23d0: e24dd010     	sub	sp, sp, #16
    23d4: e1a08000     	mov	r8, r0
    23d8: e59f0210     	ldr	r0, [pc, #0x210]        @ 0x25f0 <arbhar_shmem_dump_tab+0x224>  // u32=0x9d8; f32?=3.53127213e-42
    23dc: e59d6030     	ldr	r6, [sp, #0x30]
    23e0: e1a09002     	mov	r9, r2
    23e4: e08f0000     	add	r0, pc, r0
    23e8: e5922000     	ldr	r2, [r2]
    23ec: e1a0a001     	mov	r10, r1
    23f0: e1a05003     	mov	r5, r3
    23f4: e58d6000     	str	r6, [sp]
    23f8: ebfff9c4     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x18f0  // CALL post
    23fc: e59f31f0     	ldr	r3, [pc, #0x1f0]        @ 0x25f4 <arbhar_shmem_dump_tab+0x228>  // u32=0x10bf0; f32?=9.61178643e-41
    2400: e59f21f0     	ldr	r2, [pc, #0x1f0]        @ 0x25f8 <arbhar_shmem_dump_tab+0x22c>  // u32=0xd0; f32?=2.91470081e-43
    2404: e1a00009     	mov	r0, r9
    2408: e08f3003     	add	r3, pc, r3
    240c: e7931002     	ldr	r1, [r3, r2]
    2410: e5911000     	ldr	r1, [r1]
    2414: ebfff9b1     	bl	0xae0 <.plt+0xec>       @ imm = #-0x193c  // CALL pd_findbyclass
    2418: e2507000     	subs	r7, r0, #0
    241c: 0a000067     	beq	0x25c0 <arbhar_shmem_dump_tab+0x1f4> @ imm = #0x19c
    2420: e28d2008     	add	r2, sp, #8
    2424: e28d100c     	add	r1, sp, #12
    2428: ebfff9a3     	bl	0xabc <.plt+0xc8>       @ imm = #-0x1974  // CALL garray_getfloatwords
    242c: e2504000     	subs	r4, r0, #0
    2430: 0a000058     	beq	0x2598 <arbhar_shmem_dump_tab+0x1cc> @ imm = #0x160
    2434: e088910a     	add	r9, r8, r10, lsl #2
    2438: e59f41bc     	ldr	r4, [pc, #0x1bc]        @ 0x25fc <arbhar_shmem_dump_tab+0x230>  // u32=0x9b8; f32?=3.48643058e-42
    243c: e599201c     	ldr	r2, [r9, #0x1c]
    2440: e08f0004     	add	r0, pc, r4
    2444: e599108c     	ldr	r1, [r9, #0x8c]
    2448: ebfff9b0     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x1940  // CALL post
    244c: e599c08c     	ldr	r12, [r9, #0x8c]
    2450: e59d800c     	ldr	r8, [sp, #0xc]
    2454: e04c0005     	sub	r0, r12, r5
    2458: eddd7a0d     	vldr	s15, [sp, #52]
    245c: e048a006     	sub	r10, r8, r6
    2460: ee070a10     	vmov	s14, r0
    2464: ee00aa10     	vmov	s0, r10
    2468: eef80ac7     	vcvt.f32.s32	s1, s14
    246c: eeb81ac0     	vcvt.f32.s32	s2, s0
    2470: eef40ac1     	vcmpe.f32	s1, s2
    2474: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2478: 9eb01a60     	vmovls.f32	s2, s1
    247c: eefd1ac1     	vcvt.s32.f32	s3, s2
    2480: eef86ae7     	vcvt.f32.s32	s13, s15
    2484: eeb82ae1     	vcvt.f32.s32	s4, s3
    2488: eeb42ae6     	vcmpe.f32	s4, s13
    248c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2490: 8eb02a66     	vmovhi.f32	s4, s13
    2494: eefd2ac2     	vcvt.s32.f32	s5, s4
    2498: ee124a90     	vmov	r4, s5
    249c: e3540000     	cmp	r4, #0
    24a0: da000050     	ble	0x25e8 <arbhar_shmem_dump_tab+0x21c> @ imm = #0x140
    24a4: e599e058     	ldr	lr, [r9, #0x58]
    24a8: e0841005     	add	r1, r4, r5
    24ac: e59d2008     	ldr	r2, [sp, #0x8]
    24b0: e08e3105     	add	r3, lr, r5, lsl #2
    24b4: e08e5101     	add	r5, lr, r1, lsl #2
    24b8: e0459003     	sub	r9, r5, r3
    24bc: e0822106     	add	r2, r2, r6, lsl #2
    24c0: e2496004     	sub	r6, r9, #4
    24c4: e1a0c126     	lsr	r12, r6, #2
    24c8: e28c8001     	add	r8, r12, #1
    24cc: e2180007     	ands	r0, r8, #7
    24d0: 0a000019     	beq	0x253c <arbhar_shmem_dump_tab+0x170> @ imm = #0x64
    24d4: e3500001     	cmp	r0, #1
    24d8: 0a000013     	beq	0x252c <arbhar_shmem_dump_tab+0x160> @ imm = #0x4c
    24dc: e3500002     	cmp	r0, #2
    24e0: 0a00000f     	beq	0x2524 <arbhar_shmem_dump_tab+0x158> @ imm = #0x3c
    24e4: e3500003     	cmp	r0, #3
    24e8: 0a00000b     	beq	0x251c <arbhar_shmem_dump_tab+0x150> @ imm = #0x2c
    24ec: e3500004     	cmp	r0, #4
    24f0: 0a000007     	beq	0x2514 <arbhar_shmem_dump_tab+0x148> @ imm = #0x1c
    24f4: e3500005     	cmp	r0, #5
    24f8: 0a000003     	beq	0x250c <arbhar_shmem_dump_tab+0x140> @ imm = #0xc
    24fc: e3500006     	cmp	r0, #6
    2500: 1a000035     	bne	0x25dc <arbhar_shmem_dump_tab+0x210> @ imm = #0xd4
    2504: e493e004     	ldr	lr, [r3], #4
    2508: e482e004     	str	lr, [r2], #4
    250c: e4931004     	ldr	r1, [r3], #4
    2510: e4821004     	str	r1, [r2], #4
    2514: e4939004     	ldr	r9, [r3], #4
    2518: e4829004     	str	r9, [r2], #4
    251c: e4936004     	ldr	r6, [r3], #4
    2520: e4826004     	str	r6, [r2], #4
    2524: e493c004     	ldr	r12, [r3], #4
    2528: e482c004     	str	r12, [r2], #4
    252c: e4938004     	ldr	r8, [r3], #4
    2530: e1550003     	cmp	r5, r3
    2534: e4828004     	str	r8, [r2], #4
    2538: 0a00001b     	beq	0x25ac <arbhar_shmem_dump_tab+0x1e0> @ imm = #0x6c
    253c: e1a00003     	mov	r0, r3
    2540: e1a0a002     	mov	r10, r2
    2544: e490e004     	ldr	lr, [r0], #4
    2548: e2833020     	add	r3, r3, #32
    254c: e2822020     	add	r2, r2, #32
    2550: e48ae004     	str	lr, [r10], #4
    2554: e513101c     	ldr	r1, [r3, #-0x1c]
    2558: e502101c     	str	r1, [r2, #-0x1c]
    255c: e5909004     	ldr	r9, [r0, #0x4]
    2560: e58a9004     	str	r9, [r10, #0x4]
    2564: e5136014     	ldr	r6, [r3, #-0x14]
    2568: e5026014     	str	r6, [r2, #-0x14]
    256c: e513c010     	ldr	r12, [r3, #-0x10]
    2570: e502c010     	str	r12, [r2, #-0x10]
    2574: e513800c     	ldr	r8, [r3, #-0xc]
    2578: e502800c     	str	r8, [r2, #-0xc]
    257c: e5130008     	ldr	r0, [r3, #-0x8]
    2580: e5020008     	str	r0, [r2, #-0x8]
    2584: e513a004     	ldr	r10, [r3, #-0x4]
    2588: e1550003     	cmp	r5, r3
    258c: e502a004     	str	r10, [r2, #-0x4]
    2590: 1affffe9     	bne	0x253c <arbhar_shmem_dump_tab+0x170> @ imm = #-0x5c
    2594: ea000004     	b	0x25ac <arbhar_shmem_dump_tab+0x1e0> @ imm = #0x10
    2598: e59f5060     	ldr	r5, [pc, #0x60]         @ 0x2600 <arbhar_shmem_dump_tab+0x234>  // u32=0x834; f32?=2.94272678e-42
    259c: e1a00008     	mov	r0, r8
    25a0: e5992000     	ldr	r2, [r9]
    25a4: e08f1005     	add	r1, pc, r5
    25a8: ebfff97c     	bl	0xba0 <.plt+0x1ac>      @ imm = #-0x1a10  // CALL pd_error
    25ac: e1a00007     	mov	r0, r7
    25b0: ebfff926     	bl	0xa50 <.plt+0x5c>       @ imm = #-0x1b68  // CALL garray_redraw
    25b4: e1a00004     	mov	r0, r4
    25b8: e28dd010     	add	sp, sp, #16
    25bc: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    25c0: e59f403c     	ldr	r4, [pc, #0x3c]         @ 0x2604 <arbhar_shmem_dump_tab+0x238>  // u32=0x714; f32?=2.53915282e-42
    25c4: e1a00008     	mov	r0, r8
    25c8: e5992000     	ldr	r2, [r9]
    25cc: e08f1004     	add	r1, pc, r4
    25d0: e1a04007     	mov	r4, r7
    25d4: ebfff971     	bl	0xba0 <.plt+0x1ac>      @ imm = #-0x1a3c  // CALL pd_error
    25d8: eafffff3     	b	0x25ac <arbhar_shmem_dump_tab+0x1e0> @ imm = #-0x34
    25dc: e493a004     	ldr	r10, [r3], #4
    25e0: e482a004     	str	r10, [r2], #4
    25e4: eaffffc6     	b	0x2504 <arbhar_shmem_dump_tab+0x138> @ imm = #-0xe8
    25e8: e3a04000     	mov	r4, #0
    25ec: eaffffee     	b	0x25ac <arbhar_shmem_dump_tab+0x1e0> @ imm = #-0x48
    25f0: d8 09 00 00  	.word	0x000009d8
    25f4: f0 0b 01 00  	.word	0x00010bf0
    25f8: d0 00 00 00  	.word	0x000000d0
    25fc: b8 09 00 00  	.word	0x000009b8
    2600: 34 08 00 00  	.word	0x00000834
    2604: 14 07 00 00  	.word	0x00000714

