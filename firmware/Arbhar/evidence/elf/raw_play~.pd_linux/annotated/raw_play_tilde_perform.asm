000022dc <raw_play_tilde_perform>:
    22dc: e590c004     	ldr	r12, [r0, #0x4]
    22e0: e5901014     	ldr	r1, [r0, #0x14]
    22e4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    22e8: e3510000     	cmp	r1, #0
    22ec: e28c4923     	add	r4, r12, #573440
    22f0: e24dd00c     	sub	sp, sp, #12
    22f4: e590300c     	ldr	r3, [r0, #0xc]
    22f8: ed9c7a07     	vldr	s14, [r12, #28]
    22fc: e5942a28     	ldr	r2, [r4, #0xa28]
    2300: e5905010     	ldr	r5, [r0, #0x10]
    2304: eddc7a08     	vldr	s15, [r12, #32]
    2308: da00004b     	ble	0x243c <raw_play_tilde_perform+0x160> @ imm = #0x12c
    230c: e594ea2c     	ldr	lr, [r4, #0xa2c]
    2310: e0836101     	add	r6, r3, r1, lsl #2
    2314: e35e0000     	cmp	lr, #0
    2318: 1a00004b     	bne	0x244c <raw_play_tilde_perform+0x170> @ imm = #0x12c
    231c: e046e003     	sub	lr, r6, r3
    2320: e045c003     	sub	r12, r5, r3
    2324: e24e7004     	sub	r7, lr, #4
    2328: e3a0b000     	mov	r11, #0
    232c: e1a09127     	lsr	r9, r7, #2
    2330: e289a001     	add	r10, r9, #1
    2334: e21a8007     	ands	r8, r10, #7
    2338: 0a000028     	beq	0x23e0 <raw_play_tilde_perform+0x104> @ imm = #0xa0
    233c: e3580001     	cmp	r8, #1
    2340: 0a000020     	beq	0x23c8 <raw_play_tilde_perform+0xec> @ imm = #0x80
    2344: e3580002     	cmp	r8, #2
    2348: 0a00001a     	beq	0x23b8 <raw_play_tilde_perform+0xdc> @ imm = #0x68
    234c: e3580003     	cmp	r8, #3
    2350: 0a000014     	beq	0x23a8 <raw_play_tilde_perform+0xcc> @ imm = #0x50
    2354: e3580004     	cmp	r8, #4
    2358: 0a00000e     	beq	0x2398 <raw_play_tilde_perform+0xbc> @ imm = #0x38
    235c: e3580005     	cmp	r8, #5
    2360: 0a000008     	beq	0x2388 <raw_play_tilde_perform+0xac> @ imm = #0x20
    2364: e3580006     	cmp	r8, #6
    2368: 108c1003     	addne	r1, r12, r3
    236c: 1583b000     	strne	r11, [r3]
    2370: 12833004     	addne	r3, r3, #4
    2374: 1581b000     	strne	r11, [r1]
    2378: e08c5003     	add	r5, r12, r3
    237c: e583b000     	str	r11, [r3]
    2380: e2833004     	add	r3, r3, #4
    2384: e585b000     	str	r11, [r5]
    2388: e08ce003     	add	lr, r12, r3
    238c: e583b000     	str	r11, [r3]
    2390: e2833004     	add	r3, r3, #4
    2394: e58eb000     	str	r11, [lr]
    2398: e08c7003     	add	r7, r12, r3
    239c: e583b000     	str	r11, [r3]
    23a0: e2833004     	add	r3, r3, #4
    23a4: e587b000     	str	r11, [r7]
    23a8: e08c9003     	add	r9, r12, r3
    23ac: e583b000     	str	r11, [r3]
    23b0: e2833004     	add	r3, r3, #4
    23b4: e589b000     	str	r11, [r9]
    23b8: e08ca003     	add	r10, r12, r3
    23bc: e583b000     	str	r11, [r3]
    23c0: e2833004     	add	r3, r3, #4
    23c4: e58ab000     	str	r11, [r10]
    23c8: e583b000     	str	r11, [r3]
    23cc: e08ce003     	add	lr, r12, r3
    23d0: e2833004     	add	r3, r3, #4
    23d4: e1560003     	cmp	r6, r3
    23d8: e58eb000     	str	r11, [lr]
    23dc: 0a000016     	beq	0x243c <raw_play_tilde_perform+0x160> @ imm = #0x58
    23e0: e2838004     	add	r8, r3, #4
    23e4: e08c1003     	add	r1, r12, r3
    23e8: e08c5008     	add	r5, r12, r8
    23ec: e583b000     	str	r11, [r3]
    23f0: e581b000     	str	r11, [r1]
    23f4: e2833020     	add	r3, r3, #32
    23f8: e503b01c     	str	r11, [r3, #-0x1c]
    23fc: e1a0e001     	mov	lr, r1
    2400: e585b000     	str	r11, [r5]
    2404: e503b018     	str	r11, [r3, #-0x18]
    2408: e585b004     	str	r11, [r5, #0x4]
    240c: e503b014     	str	r11, [r3, #-0x14]
    2410: e581b00c     	str	r11, [r1, #0xc]
    2414: e503b010     	str	r11, [r3, #-0x10]
    2418: e581b010     	str	r11, [r1, #0x10]
    241c: e503b00c     	str	r11, [r3, #-0xc]
    2420: e581b014     	str	r11, [r1, #0x14]
    2424: e503b008     	str	r11, [r3, #-0x8]
    2428: e581b018     	str	r11, [r1, #0x18]
    242c: e503b004     	str	r11, [r3, #-0x4]
    2430: e1560003     	cmp	r6, r3
    2434: e581b01c     	str	r11, [r1, #0x1c]
    2438: 1affffe8     	bne	0x23e0 <raw_play_tilde_perform+0x104> @ imm = #-0x60
    243c: e2800018     	add	r0, r0, #24
    2440: e5842a28     	str	r2, [r4, #0xa28]
    2444: e28dd00c     	add	sp, sp, #12
    2448: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    244c: e0468003     	sub	r8, r6, r3
    2450: e303e27f     	movw	lr, #0x327f
    2454: e2489004     	sub	r9, r8, #4
    2458: e0457003     	sub	r7, r5, r3
    245c: e340e002     	movt	lr, #0x2
    2460: e1a0a129     	lsr	r10, r9, #2
    2464: e28ab001     	add	r11, r10, #1
    2468: e21b1003     	ands	r1, r11, #3
    246c: 0a00001b     	beq	0x24e0 <raw_play_tilde_perform+0x204> @ imm = #0x6c
    2470: e3510001     	cmp	r1, #1
    2474: 0a00000c     	beq	0x24ac <raw_play_tilde_perform+0x1d0> @ imm = #0x30
    2478: e3510002     	cmp	r1, #2
    247c: 1a000044     	bne	0x2594 <raw_play_tilde_perform+0x2b8> @ imm = #0x110
    2480: e08c9102     	add	r9, r12, r2, lsl #2
    2484: e087a003     	add	r10, r7, r3
    2488: e2822001     	add	r2, r2, #1
    248c: e2833004     	add	r3, r3, #4
    2490: edd90a0a     	vldr	s1, [r9, #40]
    2494: e152000e     	cmp	r2, lr
    2498: 83a02000     	movhi	r2, #0
    249c: ee271a20     	vmul.f32	s2, s14, s1
    24a0: ee671aa0     	vmul.f32	s3, s15, s1
    24a4: ed031a01     	vstr	s2, [r3, #-4]
    24a8: edca1a00     	vstr	s3, [r10]
    24ac: e08cb102     	add	r11, r12, r2, lsl #2
    24b0: e2822001     	add	r2, r2, #1
    24b4: e152000e     	cmp	r2, lr
    24b8: e0871003     	add	r1, r7, r3
    24bc: ed9b2a0a     	vldr	s4, [r11, #40]
    24c0: e2833004     	add	r3, r3, #4
    24c4: 83a02000     	movhi	r2, #0
    24c8: ee672a02     	vmul.f32	s5, s14, s4
    24cc: ee273a82     	vmul.f32	s6, s15, s4
    24d0: ed432a01     	vstr	s5, [r3, #-4]
    24d4: e1530006     	cmp	r3, r6
    24d8: ed813a00     	vstr	s6, [r1]
    24dc: 0affffd6     	beq	0x243c <raw_play_tilde_perform+0x160> @ imm = #-0xa8
    24e0: e88d0011     	stm	sp, {r0, r4}
    24e4: e08c4102     	add	r4, r12, r2, lsl #2
    24e8: e2825001     	add	r5, r2, #1
    24ec: e087b003     	add	r11, r7, r3
    24f0: e155000e     	cmp	r5, lr
    24f4: edd43a0a     	vldr	s7, [r4, #40]
    24f8: e2830004     	add	r0, r3, #4
    24fc: 83a05000     	movhi	r5, #0
    2500: e0879000     	add	r9, r7, r0
    2504: e08ca105     	add	r10, r12, r5, lsl #2
    2508: e2852001     	add	r2, r5, #1
    250c: e152000e     	cmp	r2, lr
    2510: e2833010     	add	r3, r3, #16
    2514: ee274a23     	vmul.f32	s8, s14, s7
    2518: 83a02000     	movhi	r2, #0
    251c: e2821001     	add	r1, r2, #1
    2520: e08c8102     	add	r8, r12, r2, lsl #2
    2524: e151000e     	cmp	r1, lr
    2528: 83a01000     	movhi	r1, #0
    252c: e2812001     	add	r2, r1, #1
    2530: e08c0101     	add	r0, r12, r1, lsl #2
    2534: e152000e     	cmp	r2, lr
    2538: 83a02000     	movhi	r2, #0
    253c: ee674aa3     	vmul.f32	s9, s15, s7
    2540: ed034a04     	vstr	s8, [r3, #-16]
    2544: edcb4a00     	vstr	s9, [r11]
    2548: ed9a5a0a     	vldr	s10, [r10, #40]
    254c: ee675a05     	vmul.f32	s11, s14, s10
    2550: ee676a85     	vmul.f32	s13, s15, s10
    2554: ed435a03     	vstr	s11, [r3, #-12]
    2558: edc96a00     	vstr	s13, [r9]
    255c: ed980a0a     	vldr	s0, [r8, #40]
    2560: ee276a00     	vmul.f32	s12, s14, s0
    2564: ee670a80     	vmul.f32	s1, s15, s0
    2568: ed036a02     	vstr	s12, [r3, #-8]
    256c: edc90a01     	vstr	s1, [r9, #4]
    2570: ed901a0a     	vldr	s2, [r0, #40]
    2574: ee671a01     	vmul.f32	s3, s14, s2
    2578: ee272a81     	vmul.f32	s4, s15, s2
    257c: ed431a01     	vstr	s3, [r3, #-4]
    2580: e1530006     	cmp	r3, r6
    2584: ed8b2a03     	vstr	s4, [r11, #12]
    2588: 1affffd5     	bne	0x24e4 <raw_play_tilde_perform+0x208> @ imm = #-0xac
    258c: e89d0011     	ldm	sp, {r0, r4}
    2590: eaffffa9     	b	0x243c <raw_play_tilde_perform+0x160> @ imm = #-0x15c
    2594: e08c5102     	add	r5, r12, r2, lsl #2
    2598: e2822001     	add	r2, r2, #1
    259c: e0878003     	add	r8, r7, r3
    25a0: e152000e     	cmp	r2, lr
    25a4: edd56a0a     	vldr	s13, [r5, #40]
    25a8: e2833004     	add	r3, r3, #4
    25ac: 83a02000     	movhi	r2, #0
    25b0: ee276a26     	vmul.f32	s12, s14, s13
    25b4: ee270aa6     	vmul.f32	s0, s15, s13
    25b8: ed036a01     	vstr	s12, [r3, #-4]
    25bc: ed880a00     	vstr	s0, [r8]
    25c0: eaffffae     	b	0x2480 <raw_play_tilde_perform+0x1a4> @ imm = #-0x148

