00002124 <fftwObject_tilde_perform>:
    2124: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    2128: e1a02000     	mov	r2, r0
    212c: ed2d8b0a     	vpush	{d8, d9, d10, d11, d12}
    2130: e5903014     	ldr	r3, [r0, #0x14]
    2134: e5905004     	ldr	r5, [r0, #0x4]
    2138: e3530000     	cmp	r3, #0
    213c: e2431001     	sub	r1, r3, #1
    2140: e24dd044     	sub	sp, sp, #68
    2144: e5924010     	ldr	r4, [r2, #0x10]
    2148: e58d0030     	str	r0, [sp, #0x30]
    214c: e58d1004     	str	r1, [sp, #0x4]
    2150: e5901008     	ldr	r1, [r0, #0x8]
    2154: e590000c     	ldr	r0, [r0, #0xc]
    2158: 0a000032     	beq	0x2228 <fftwObject_tilde_perform+0x104> @ imm = #0xc8
    215c: eeb79a00     	vmov.f32	s18, #1.000000e+00
    2160: e2856866     	add	r6, r5, #6684672
    2164: e304c678     	movw	r12, #0x4678
    2168: e340c033     	movt	r12, #0x33
    216c: e5962cf4     	ldr	r2, [r6, #0xcf4]
    2170: e0407001     	sub	r7, r0, r1
    2174: e0448001     	sub	r8, r4, r1
    2178: e085900c     	add	r9, r5, r12
    217c: e30aacb8     	movw	r10, #0xacb8
    2180: e58d700c     	str	r7, [sp, #0xc]
    2184: e340a065     	movt	r10, #0x65
    2188: e58d8010     	str	r8, [sp, #0x10]
    218c: e58d902c     	str	r9, [sp, #0x2c]
    2190: e58da034     	str	r10, [sp, #0x34]
    2194: eeb78b00     	vmov.f64	d8, #1.000000e+00
    2198: e1a07001     	mov	r7, r1
    219c: e59d000c     	ldr	r0, [sp, #0xc]
    21a0: e497e004     	ldr	lr, [r7], #4
    21a4: e0853102     	add	r3, r5, r2, lsl #2
    21a8: e0814000     	add	r4, r1, r0
    21ac: e2839865     	add	r9, r3, #6619136
    21b0: e2838901     	add	r8, r3, #16384
    21b4: e289ab2b     	add	r10, r9, #44032
    21b8: e583e020     	str	lr, [r3, #0x20]
    21bc: e2893b33     	add	r3, r9, #52224
    21c0: e594e000     	ldr	lr, [r4]
    21c4: e30007ff     	movw	r0, #0x7ff
    21c8: ed9f7a1b     	vldr	s14, [pc, #108]         @ 0x223c <fftwObject_tilde_perform+0x118>  // f32=5.96046448e-08
    21cc: e1520000     	cmp	r2, r0
    21d0: e59dc010     	ldr	r12, [sp, #0x10]
    21d4: e588e020     	str	lr, [r8, #0x20]
    21d8: e28ae0b8     	add	lr, r10, #184
    21dc: edda7a2e     	vldr	s15, [r10, #184]
    21e0: e081100c     	add	r1, r1, r12
    21e4: edd36a2e     	vldr	s13, [r3, #184]
    21e8: ee370aa6     	vadd.f32	s0, s15, s13
    21ec: ee600a07     	vmul.f32	s1, s0, s14
    21f0: edc10a00     	vstr	s1, [r1]
    21f4: 0a000015     	beq	0x2250 <fftwObject_tilde_perform+0x12c> @ imm = #0x54
    21f8: e2822001     	add	r2, r2, #1
    21fc: e59d4004     	ldr	r4, [sp, #0x4]
    2200: e2721000     	rsbs	r1, r2, #0
    2204: e7ea2052     	ubfx	r2, r2, #0x0, #0xb
    2208: e244a001     	sub	r10, r4, #1
    220c: e7ea9051     	ubfx	r9, r1, #0x0, #0xb
    2210: 52692000     	rsbpl	r2, r9, #0
    2214: e37a0001     	cmn	r10, #1
    2218: e58da004     	str	r10, [sp, #0x4]
    221c: e1a01007     	mov	r1, r7
    2220: e5862cf4     	str	r2, [r6, #0xcf4]
    2224: 1affffdb     	bne	0x2198 <fftwObject_tilde_perform+0x74> @ imm = #-0x94
    2228: e59d5030     	ldr	r5, [sp, #0x30]
    222c: e2850018     	add	r0, r5, #24
    2230: e28dd044     	add	sp, sp, #68
    2234: ecbd8b0a     	vpop	{d8, d9, d10, d11, d12}
    2238: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    223c: 00 00 80 33  	.word	0x33800000
    2240: 48 14 00 00  	.word	0x00001448
    2244: 38 14 00 00  	.word	0x00001438
    2248: 2c 14 00 00  	.word	0x0000142c
    224c: 00 00 80 45  	.word	0x45800000
    2250: e5960cb8     	ldr	r0, [r6, #0xcb8]
    2254: ebfff909     	bl	0x680 <.plt+0x68>       @ imm = #-0x1bdc  // CALL fftwf_execute
    2258: e5960cbc     	ldr	r0, [r6, #0xcbc]
    225c: ebfff907     	bl	0x680 <.plt+0x68>       @ imm = #-0x1be4  // CALL fftwf_execute
    2260: e5d62d0c     	ldrb	r2, [r6, #0xd0c]
    2264: e3520000     	cmp	r2, #0
    2268: 1a0002f2     	bne	0x2e38 <fftwObject_tilde_perform+0xd14> @ imm = #0xbc8
    226c: e5d64d0d     	ldrb	r4, [r6, #0xd0d]
    2270: e3540000     	cmp	r4, #0
    2274: 1a0001c7     	bne	0x2998 <fftwObject_tilde_perform+0x874> @ imm = #0x71c
    2278: e596ed10     	ldr	lr, [r6, #0xd10]
    227c: e5968d00     	ldr	r8, [r6, #0xd00]
    2280: e5962cd0     	ldr	r2, [r6, #0xcd0]
    2284: e35e0000     	cmp	lr, #0
    2288: e58de008     	str	lr, [sp, #0x8]
    228c: e5868d08     	str	r8, [r6, #0xd08]
    2290: e5862ccc     	str	r2, [r6, #0xccc]
    2294: da00007a     	ble	0x2484 <fftwObject_tilde_perform+0x360> @ imm = #0x1e8
    2298: e2853902     	add	r3, r5, #32768
    229c: e5964d04     	ldr	r4, [r6, #0xd04]
    22a0: e5961d18     	ldr	r1, [r6, #0xd18]
    22a4: e2859903     	add	r9, r5, #49152
    22a8: e283c020     	add	r12, r3, #32
    22ac: e289a028     	add	r10, r9, #40
    22b0: e3a03000     	mov	r3, #0
    22b4: e58d7038     	str	r7, [sp, #0x38]
    22b8: e58d603c     	str	r6, [sp, #0x3c]
    22bc: e1a07002     	mov	r7, r2
    22c0: e1a06003     	mov	r6, r3
    22c4: e3080346     	movw	r0, #0x8346
    22c8: e24ee001     	sub	lr, lr, #1
    22cc: e3400019     	movt	r0, #0x19
    22d0: e58d4018     	str	r4, [sp, #0x18]
    22d4: e58dc014     	str	r12, [sp, #0x14]
    22d8: e58d101c     	str	r1, [sp, #0x1c]
    22dc: e58de024     	str	lr, [sp, #0x24]
    22e0: e58d0028     	str	r0, [sp, #0x28]
    22e4: e59d2028     	ldr	r2, [sp, #0x28]
    22e8: e088c588     	add	r12, r8, r8, lsl #11
    22ec: e59d4024     	ldr	r4, [sp, #0x24]
    22f0: e0889002     	add	r9, r8, r2
    22f4: e1560004     	cmp	r6, r4
    22f8: e0851109     	add	r1, r5, r9, lsl #2
    22fc: 03a04001     	moveq	r4, #1
    2300: 13a04000     	movne	r4, #0
    2304: ed91ba01     	vldr	s22, [r1, #4]
    2308: e58d4020     	str	r4, [sp, #0x20]
    230c: 0a00010d     	beq	0x2748 <fftwObject_tilde_perform+0x624> @ imm = #0x434
    2310: e287ba66     	add	r11, r7, #417792
    2314: e28cca02     	add	r12, r12, #8192
    2318: e28b00ce     	add	r0, r11, #206
    231c: e28ce006     	add	lr, r12, #6
    2320: e59dc02c     	ldr	r12, [sp, #0x2c]
    2324: e1a09180     	lsl	r9, r0, #3
    2328: e085318e     	add	r3, r5, lr, lsl #3
    232c: e2494833     	sub	r4, r9, #3342336
    2330: e0852009     	add	r2, r5, r9
    2334: e244be67     	sub	r11, r4, #1648
    2338: e289e004     	add	lr, r9, #4
    233c: e08cb00b     	add	r11, r12, r11
    2340: e59d1014     	ldr	r1, [sp, #0x14]
    2344: e04b9002     	sub	r9, r11, r2
    2348: e085000e     	add	r0, r5, lr
    234c: e3190008     	tst	r9, #8
    2350: 0a000014     	beq	0x23a8 <fftwObject_tilde_perform+0x284> @ imm = #0x50
    2354: edd11a01     	vldr	s3, [r1, #4]
    2358: e1a04001     	mov	r4, r1
    235c: e2822008     	add	r2, r2, #8
    2360: e2811008     	add	r1, r1, #8
    2364: e2833008     	add	r3, r3, #8
    2368: e2800008     	add	r0, r0, #8
    236c: ed135a01     	vldr	s10, [r3, #-4]
    2370: ed534a02     	vldr	s9, [r3, #-8]
    2374: ee657a21     	vmul.f32	s15, s10, s3
    2378: edd45a00     	vldr	s11, [r4]
    237c: ed101a03     	vldr	s2, [r0, #-12]
    2380: ee547aa5     	vnmls.f32	s15, s9, s11
    2384: ee61aaa4     	vmul.f32	s21, s3, s9
    2388: ee071a8b     	vmla.f32	s2, s15, s22
    238c: ee45aa25     	vmla.f32	s21, s10, s11
    2390: ed001a03     	vstr	s2, [r0, #-12]
    2394: ed127a01     	vldr	s14, [r2, #-4]
    2398: ee0a7a8b     	vmla.f32	s14, s21, s22
    239c: ed027a01     	vstr	s14, [r2, #-4]
    23a0: e15b0002     	cmp	r11, r2
    23a4: 0a000023     	beq	0x2438 <fftwObject_tilde_perform+0x314> @ imm = #0x8c
    23a8: edd19a01     	vldr	s19, [r1, #4]
    23ac: e280e008     	add	lr, r0, #8
    23b0: e282c008     	add	r12, r2, #8
    23b4: e2822010     	add	r2, r2, #16
    23b8: e2800010     	add	r0, r0, #16
    23bc: e2811010     	add	r1, r1, #16
    23c0: ed93aa01     	vldr	s20, [r3, #4]
    23c4: e2833010     	add	r3, r3, #16
    23c8: ed110a04     	vldr	s0, [r1, #-16]
    23cc: ee2a6a29     	vmul.f32	s12, s20, s19
    23d0: ed530a04     	vldr	s1, [r3, #-16]
    23d4: ed10ca05     	vldr	s24, [r0, #-20]
    23d8: ee106a80     	vnmls.f32	s12, s1, s0
    23dc: ee292aa0     	vmul.f32	s4, s19, s1
    23e0: ee06ca0b     	vmla.f32	s24, s12, s22
    23e4: ee0a2a00     	vmla.f32	s4, s20, s0
    23e8: ed00ca05     	vstr	s24, [r0, #-20]
    23ec: ed127a03     	vldr	s14, [r2, #-12]
    23f0: ee027a0b     	vmla.f32	s14, s4, s22
    23f4: ed027a03     	vstr	s14, [r2, #-12]
    23f8: e15b0002     	cmp	r11, r2
    23fc: ed113a01     	vldr	s6, [r1, #-4]
    2400: ed533a01     	vldr	s7, [r3, #-4]
    2404: ed114a02     	vldr	s8, [r1, #-8]
    2408: ee63ca83     	vmul.f32	s25, s7, s6
    240c: ed531a02     	vldr	s3, [r3, #-8]
    2410: ed5e6a01     	vldr	s13, [lr, #-4]
    2414: ee51ca84     	vnmls.f32	s25, s3, s8
    2418: ee235a21     	vmul.f32	s10, s6, s3
    241c: ee4c6a8b     	vmla.f32	s13, s25, s22
    2420: ee035a84     	vmla.f32	s10, s7, s8
    2424: ed4e6a01     	vstr	s13, [lr, #-4]
    2428: ed9c7a01     	vldr	s14, [r12, #4]
    242c: ee057a0b     	vmla.f32	s14, s10, s22
    2430: ed8c7a01     	vstr	s14, [r12, #4]
    2434: 1affffdb     	bne	0x23a8 <fftwObject_tilde_perform+0x284> @ imm = #-0x94
    2438: e2880001     	add	r0, r8, #1
    243c: e59d1018     	ldr	r1, [sp, #0x18]
    2440: eb00076f     	bl	0x4204 <__aeabi_idivmod> @ imm = #0x1dbc
    2444: e2877b02     	add	r7, r7, #2048
    2448: e2870001     	add	r0, r7, #1
    244c: e2866001     	add	r6, r6, #1
    2450: e1a08001     	mov	r8, r1
    2454: e59d101c     	ldr	r1, [sp, #0x1c]
    2458: eb000769     	bl	0x4204 <__aeabi_idivmod> @ imm = #0x1da4
    245c: e59d9008     	ldr	r9, [sp, #0x8]
    2460: e1560009     	cmp	r6, r9
    2464: e1a07001     	mov	r7, r1
    2468: 1affff9d     	bne	0x22e4 <fftwObject_tilde_perform+0x1c0> @ imm = #-0x18c
    246c: e59d603c     	ldr	r6, [sp, #0x3c]
    2470: e59d4020     	ldr	r4, [sp, #0x20]
    2474: e59d7038     	ldr	r7, [sp, #0x38]
    2478: e5861ccc     	str	r1, [r6, #0xccc]
    247c: e5c64cd8     	strb	r4, [r6, #0xcd8]
    2480: e5868d08     	str	r8, [r6, #0xd08]
    2484: e5d61d0e     	ldrb	r1, [r6, #0xd0e]
    2488: e3510000     	cmp	r1, #0
    248c: 0a000124     	beq	0x2924 <fftwObject_tilde_perform+0x800> @ imm = #0x490
    2490: e596acf8     	ldr	r10, [r6, #0xcf8]
    2494: e286eecd     	add	lr, r6, #3280
    2498: e5969d00     	ldr	r9, [r6, #0xd00]
    249c: e3082346     	movw	r2, #0x8346
    24a0: eddd7a02     	vldr	s15, [sp, #8]
    24a4: e3402019     	movt	r2, #0x19
    24a8: e04a4009     	sub	r4, r10, r9
    24ac: e59d0008     	ldr	r0, [sp, #0x8]
    24b0: e5963cd0     	ldr	r3, [r6, #0xcd0]
    24b4: ee0b4a10     	vmov	s22, r4
    24b8: eeb81ae7     	vcvt.f32.s32	s2, s15
    24bc: e2838a66     	add	r8, r3, #417792
    24c0: ed9e0a03     	vldr	s0, [lr, #12]
    24c4: e288c0ce     	add	r12, r8, #206
    24c8: e1a0e18c     	lsl	lr, r12, #3
    24cc: ed1faaa2     	vldr	s20, [pc, #-648]        @ 0x224c <fftwObject_tilde_perform+0x128>  // f32=4096
    24d0: e24e8833     	sub	r8, lr, #3342336
    24d4: e248ce67     	sub	r12, r8, #1648
    24d8: e085300e     	add	r3, r5, lr
    24dc: eef84acb     	vcvt.f32.s32	s9, s22
    24e0: eec4aa81     	vdiv.f32	s21, s9, s2
    24e4: ee790a40     	vsub.f32	s1, s18, s0
    24e8: ee202a8a     	vmul.f32	s4, s1, s20
    24ec: ee6aba81     	vmul.f32	s23, s21, s2
    24f0: eefd9aeb     	vcvt.s32.f32	s19, s23
    24f4: ee191a90     	vmov	r1, s19
    24f8: e1510000     	cmp	r1, r0
    24fc: d1c11fc1     	bicle	r1, r1, r1, asr #31
    2500: d58d1008     	strle	r1, [sp, #0x8]
    2504: e59d4008     	ldr	r4, [sp, #0x8]
    2508: e2861ece     	add	r1, r6, #3296
    250c: e0840002     	add	r0, r4, r2
    2510: e59d402c     	ldr	r4, [sp, #0x2c]
    2514: ed916a00     	vldr	s12, [r1]
    2518: e084e00c     	add	lr, r4, r12
    251c: e0852100     	add	r2, r5, r0, lsl #2
    2520: e04e0003     	sub	r0, lr, r3
    2524: e08ac58a     	add	r12, r10, r10, lsl #11
    2528: e2408008     	sub	r8, r0, #8
    252c: ed92ca01     	vldr	s24, [r2, #4]
    2530: e28c2a02     	add	r2, r12, #8192
    2534: e586cce8     	str	r12, [r6, #0xce8]
    2538: e1a041a8     	lsr	r4, r8, #3
    253c: e2828006     	add	r8, r2, #6
    2540: e2841001     	add	r1, r4, #1
    2544: e285c865     	add	r12, r5, #6619136
    2548: e2110003     	ands	r0, r1, #3
    254c: e0852188     	add	r2, r5, r8, lsl #3
    2550: e28c4ecb     	add	r4, r12, #3248
    2554: ee602a06     	vmul.f32	s5, s0, s12
    2558: ee223a0c     	vmul.f32	s6, s4, s24
    255c: 0a00002c     	beq	0x2614 <fftwObject_tilde_perform+0x4f0> @ imm = #0xb0
    2560: e3500001     	cmp	r0, #1
    2564: 0a00001b     	beq	0x25d8 <fftwObject_tilde_perform+0x4b4> @ imm = #0x6c
    2568: e3500002     	cmp	r0, #2
    256c: 0a00000c     	beq	0x25a4 <fftwObject_tilde_perform+0x480> @ imm = #0x30
    2570: edd23a00     	vldr	s7, [r2]
    2574: e2833008     	add	r3, r3, #8
    2578: e2822008     	add	r2, r2, #8
    257c: e2844008     	add	r4, r4, #8
    2580: ed134a02     	vldr	s8, [r3, #-8]
    2584: ee63ca23     	vmul.f32	s25, s6, s7
    2588: ee42ca84     	vmla.f32	s25, s5, s8
    258c: ed44ca02     	vstr	s25, [r4, #-8]
    2590: ed521a01     	vldr	s3, [r2, #-4]
    2594: ed135a01     	vldr	s10, [r3, #-4]
    2598: ee636a21     	vmul.f32	s13, s6, s3
    259c: ee426a85     	vmla.f32	s13, s5, s10
    25a0: ed446a01     	vstr	s13, [r4, #-4]
    25a4: e2822008     	add	r2, r2, #8
    25a8: e2833008     	add	r3, r3, #8
    25ac: e2844008     	add	r4, r4, #8
    25b0: ed127a02     	vldr	s14, [r2, #-8]
    25b4: ed535a02     	vldr	s11, [r3, #-8]
    25b8: ee23ba07     	vmul.f32	s22, s6, s14
    25bc: ee02baa5     	vmla.f32	s22, s5, s11
    25c0: ed04ba02     	vstr	s22, [r4, #-8]
    25c4: ed524a01     	vldr	s9, [r2, #-4]
    25c8: ed537a01     	vldr	s15, [r3, #-4]
    25cc: ee231a24     	vmul.f32	s2, s6, s9
    25d0: ee021aa7     	vmla.f32	s2, s5, s15
    25d4: ed041a01     	vstr	s2, [r4, #-4]
    25d8: e2833008     	add	r3, r3, #8
    25dc: e2822008     	add	r2, r2, #8
    25e0: e2844008     	add	r4, r4, #8
    25e4: ed52aa02     	vldr	s21, [r2, #-8]
    25e8: ed53ba02     	vldr	s23, [r3, #-8]
    25ec: ee639a2a     	vmul.f32	s19, s6, s21
    25f0: ee429aab     	vmla.f32	s19, s5, s23
    25f4: ed449a02     	vstr	s19, [r4, #-8]
    25f8: ed12aa01     	vldr	s20, [r2, #-4]
    25fc: ed130a01     	vldr	s0, [r3, #-4]
    2600: e153000e     	cmp	r3, lr
    2604: ee630a0a     	vmul.f32	s1, s6, s20
    2608: ee420a80     	vmla.f32	s1, s5, s0
    260c: ed440a01     	vstr	s1, [r4, #-4]
    2610: 0a00002c     	beq	0x26c8 <fftwObject_tilde_perform+0x5a4> @ imm = #0xb0
    2614: e2833020     	add	r3, r3, #32
    2618: e2822020     	add	r2, r2, #32
    261c: e2844020     	add	r4, r4, #32
    2620: ed12ca08     	vldr	s24, [r2, #-32]
    2624: ed136a08     	vldr	s12, [r3, #-32]
    2628: ee232a0c     	vmul.f32	s4, s6, s24
    262c: ee022a86     	vmla.f32	s4, s5, s12
    2630: ed042a08     	vstr	s4, [r4, #-32]
    2634: ed523a07     	vldr	s7, [r2, #-28]
    2638: ed134a07     	vldr	s8, [r3, #-28]
    263c: ee63ca23     	vmul.f32	s25, s6, s7
    2640: ee42ca84     	vmla.f32	s25, s5, s8
    2644: ed44ca07     	vstr	s25, [r4, #-28]
    2648: ed521a06     	vldr	s3, [r2, #-24]
    264c: ed135a06     	vldr	s10, [r3, #-24]
    2650: ee636a21     	vmul.f32	s13, s6, s3
    2654: ee426a85     	vmla.f32	s13, s5, s10
    2658: ed446a06     	vstr	s13, [r4, #-24]
    265c: ed127a05     	vldr	s14, [r2, #-20]
    2660: ed535a05     	vldr	s11, [r3, #-20]
    2664: ee23ba07     	vmul.f32	s22, s6, s14
    2668: ee02baa5     	vmla.f32	s22, s5, s11
    266c: ed04ba05     	vstr	s22, [r4, #-20]
    2670: ed524a04     	vldr	s9, [r2, #-16]
    2674: ed537a04     	vldr	s15, [r3, #-16]
    2678: ee231a24     	vmul.f32	s2, s6, s9
    267c: ee021aa7     	vmla.f32	s2, s5, s15
    2680: ed041a04     	vstr	s2, [r4, #-16]
    2684: ed52aa03     	vldr	s21, [r2, #-12]
    2688: ed53ba03     	vldr	s23, [r3, #-12]
    268c: ee639a2a     	vmul.f32	s19, s6, s21
    2690: ee429aab     	vmla.f32	s19, s5, s23
    2694: ed449a03     	vstr	s19, [r4, #-12]
    2698: ed12aa02     	vldr	s20, [r2, #-8]
    269c: ed130a02     	vldr	s0, [r3, #-8]
    26a0: ee630a0a     	vmul.f32	s1, s6, s20
    26a4: ee420a80     	vmla.f32	s1, s5, s0
    26a8: ed440a02     	vstr	s1, [r4, #-8]
    26ac: ed12ca01     	vldr	s24, [r2, #-4]
    26b0: ed136a01     	vldr	s12, [r3, #-4]
    26b4: e153000e     	cmp	r3, lr
    26b8: ee232a0c     	vmul.f32	s4, s6, s24
    26bc: ee022a86     	vmla.f32	s4, s5, s12
    26c0: ed042a01     	vstr	s4, [r4, #-4]
    26c4: 1affffd2     	bne	0x2614 <fftwObject_tilde_perform+0x4f0> @ imm = #-0xb8
    26c8: e3041cb8     	movw	r1, #0x4cb8
    26cc: e3401065     	movt	r1, #0x65
    26d0: e596ed04     	ldr	lr, [r6, #0xd04]
    26d4: e0858001     	add	r8, r5, r1
    26d8: e28aa001     	add	r10, r10, #1
    26dc: e586acf8     	str	r10, [r6, #0xcf8]
    26e0: e15a000e     	cmp	r10, lr
    26e4: c5869cf8     	strgt	r9, [r6, #0xcf8]
    26e8: e59d0034     	ldr	r0, [sp, #0x34]
    26ec: e3069cb8     	movw	r9, #0x6cb8
    26f0: e3409065     	movt	r9, #0x65
    26f4: e3a02a02     	mov	r2, #8192
    26f8: e0851009     	add	r1, r5, r9
    26fc: e0850000     	add	r0, r5, r0
    2700: ebfff7d8     	bl	0x668 <.plt+0x50>       @ imm = #-0x20a0  // CALL memcpy
    2704: e5960cc0     	ldr	r0, [r6, #0xcc0]
    2708: ebfff7dc     	bl	0x680 <.plt+0x68>       @ imm = #-0x2090  // CALL fftwf_execute
    270c: e30cccb8     	movw	r12, #0xccb8
    2710: e340c065     	movt	r12, #0x65
    2714: e1a01008     	mov	r1, r8
    2718: e3a02a02     	mov	r2, #8192
    271c: e085000c     	add	r0, r5, r12
    2720: ebfff7d0     	bl	0x668 <.plt+0x50>       @ imm = #-0x20c0  // CALL memcpy
    2724: e5968cd4     	ldr	r8, [r6, #0xcd4]
    2728: e5961d10     	ldr	r1, [r6, #0xd10]
    272c: e2880001     	add	r0, r8, #1
    2730: eb0006b3     	bl	0x4204 <__aeabi_idivmod> @ imm = #0x1acc
    2734: e5962cf4     	ldr	r2, [r6, #0xcf4]
    2738: e0813581     	add	r3, r1, r1, lsl #11
    273c: e5861cd4     	str	r1, [r6, #0xcd4]
    2740: e5863cd0     	str	r3, [r6, #0xcd0]
    2744: eafffeab     	b	0x21f8 <fftwObject_tilde_perform+0xd4> @ imm = #-0x554
    2748: e59d3014     	ldr	r3, [sp, #0x14]
    274c: e28c2a02     	add	r2, r12, #8192
    2750: e287ea66     	add	lr, r7, #417792
    2754: e2824006     	add	r4, r2, #6
    2758: e04a0003     	sub	r0, r10, r3
    275c: e28ec0ce     	add	r12, lr, #206
    2760: e2409008     	sub	r9, r0, #8
    2764: e0852184     	add	r2, r5, r4, lsl #3
    2768: e085418c     	add	r4, r5, r12, lsl #3
    276c: e1a011a9     	lsr	r1, r9, #3
    2770: e2810001     	add	r0, r1, #1
    2774: e210e003     	ands	lr, r0, #3
    2778: 0a000033     	beq	0x284c <fftwObject_tilde_perform+0x728> @ imm = #0xcc
    277c: e35e0001     	cmp	lr, #1
    2780: 0a000020     	beq	0x2808 <fftwObject_tilde_perform+0x6e4> @ imm = #0x80
    2784: e35e0002     	cmp	lr, #2
    2788: 0a00000f     	beq	0x27cc <fftwObject_tilde_perform+0x6a8> @ imm = #0x3c
    278c: e1a09003     	mov	r9, r3
    2790: ed93aa00     	vldr	s20, [r3]
    2794: e2822008     	add	r2, r2, #8
    2798: e2833008     	add	r3, r3, #8
    279c: e2844008     	add	r4, r4, #8
    27a0: ed120a01     	vldr	s0, [r2, #-4]
    27a4: edd90a01     	vldr	s1, [r9, #4]
    27a8: ee6a6a00     	vmul.f32	s13, s20, s0
    27ac: ed12ca02     	vldr	s24, [r2, #-8]
    27b0: ee207a80     	vmul.f32	s14, s1, s0
    27b4: ee4c6a20     	vmla.f32	s13, s24, s1
    27b8: ee1a7a0c     	vnmls.f32	s14, s20, s24
    27bc: ee262a8b     	vmul.f32	s4, s13, s22
    27c0: ee672a0b     	vmul.f32	s5, s14, s22
    27c4: ed042a01     	vstr	s4, [r4, #-4]
    27c8: ed442a02     	vstr	s5, [r4, #-8]
    27cc: e2833008     	add	r3, r3, #8
    27d0: e2822008     	add	r2, r2, #8
    27d4: e2844008     	add	r4, r4, #8
    27d8: ed133a02     	vldr	s6, [r3, #-8]
    27dc: ed523a01     	vldr	s7, [r2, #-4]
    27e0: ed134a01     	vldr	s8, [r3, #-4]
    27e4: ee63ca23     	vmul.f32	s25, s6, s7
    27e8: ed524a02     	vldr	s9, [r2, #-8]
    27ec: ee245a23     	vmul.f32	s10, s8, s7
    27f0: ee44ca84     	vmla.f32	s25, s9, s8
    27f4: ee135a24     	vnmls.f32	s10, s6, s9
    27f8: ee6c1a8b     	vmul.f32	s3, s25, s22
    27fc: ee655a0b     	vmul.f32	s11, s10, s22
    2800: ed441a01     	vstr	s3, [r4, #-4]
    2804: ed445a02     	vstr	s11, [r4, #-8]
    2808: e2833008     	add	r3, r3, #8
    280c: e2822008     	add	r2, r2, #8
    2810: e2844008     	add	r4, r4, #8
    2814: ed537a02     	vldr	s15, [r3, #-8]
    2818: ed53aa01     	vldr	s21, [r3, #-4]
    281c: e15a0003     	cmp	r10, r3
    2820: ed121a01     	vldr	s2, [r2, #-4]
    2824: ed126a02     	vldr	s12, [r2, #-8]
    2828: ee67ba81     	vmul.f32	s23, s15, s2
    282c: ee6a9a81     	vmul.f32	s19, s21, s2
    2830: ee46ba2a     	vmla.f32	s23, s12, s21
    2834: ee579a86     	vnmls.f32	s19, s15, s12
    2838: ee2baa8b     	vmul.f32	s20, s23, s22
    283c: ee290a8b     	vmul.f32	s0, s19, s22
    2840: ed04aa01     	vstr	s20, [r4, #-4]
    2844: ed040a02     	vstr	s0, [r4, #-8]
    2848: 0afffefa     	beq	0x2438 <fftwObject_tilde_perform+0x314> @ imm = #-0x418
    284c: e2833020     	add	r3, r3, #32
    2850: e2822020     	add	r2, r2, #32
    2854: e2844020     	add	r4, r4, #32
    2858: ed530a08     	vldr	s1, [r3, #-32]
    285c: ed132a07     	vldr	s4, [r3, #-28]
    2860: ed12ca07     	vldr	s24, [r2, #-28]
    2864: ed522a08     	vldr	s5, [r2, #-32]
    2868: ee606a8c     	vmul.f32	s13, s1, s24
    286c: ee227a0c     	vmul.f32	s14, s4, s24
    2870: ee426a82     	vmla.f32	s13, s5, s4
    2874: ee107aa2     	vnmls.f32	s14, s1, s5
    2878: ee263a8b     	vmul.f32	s6, s13, s22
    287c: ee673a0b     	vmul.f32	s7, s14, s22
    2880: ed043a07     	vstr	s6, [r4, #-28]
    2884: ed443a08     	vstr	s7, [r4, #-32]
    2888: ed134a05     	vldr	s8, [r3, #-20]
    288c: ed52ca05     	vldr	s25, [r2, #-20]
    2890: ed534a06     	vldr	s9, [r3, #-24]
    2894: ee245a2c     	vmul.f32	s10, s8, s25
    2898: ed521a06     	vldr	s3, [r2, #-24]
    289c: ee645aac     	vmul.f32	s11, s9, s25
    28a0: ee145aa1     	vnmls.f32	s10, s9, s3
    28a4: ee415a84     	vmla.f32	s11, s3, s8
    28a8: ee657a0b     	vmul.f32	s15, s10, s22
    28ac: ee251a8b     	vmul.f32	s2, s11, s22
    28b0: ed447a06     	vstr	s15, [r4, #-24]
    28b4: ed041a05     	vstr	s2, [r4, #-20]
    28b8: ed53aa04     	vldr	s21, [r3, #-16]
    28bc: ed53ba03     	vldr	s23, [r3, #-12]
    28c0: ed126a03     	vldr	s12, [r2, #-12]
    28c4: ed529a04     	vldr	s19, [r2, #-16]
    28c8: ee2baa86     	vmul.f32	s20, s23, s12
    28cc: ee2a0a86     	vmul.f32	s0, s21, s12
    28d0: ee1aaaa9     	vnmls.f32	s20, s21, s19
    28d4: ee090aab     	vmla.f32	s0, s19, s23
    28d8: ee6a0a0b     	vmul.f32	s1, s20, s22
    28dc: ee20ca0b     	vmul.f32	s24, s0, s22
    28e0: ed440a04     	vstr	s1, [r4, #-16]
    28e4: ed04ca03     	vstr	s24, [r4, #-12]
    28e8: ed132a02     	vldr	s4, [r3, #-8]
    28ec: ed133a01     	vldr	s6, [r3, #-4]
    28f0: e15a0003     	cmp	r10, r3
    28f4: ed522a01     	vldr	s5, [r2, #-4]
    28f8: ed523a02     	vldr	s7, [r2, #-8]
    28fc: ee626a22     	vmul.f32	s13, s4, s5
    2900: ee237a22     	vmul.f32	s14, s6, s5
    2904: ee436a83     	vmla.f32	s13, s7, s6
    2908: ee127a23     	vnmls.f32	s14, s4, s7
    290c: ee264a8b     	vmul.f32	s8, s13, s22
    2910: ee67ca0b     	vmul.f32	s25, s14, s22
    2914: ed044a01     	vstr	s8, [r4, #-4]
    2918: ed44ca02     	vstr	s25, [r4, #-8]
    291c: 1affffca     	bne	0x284c <fftwObject_tilde_perform+0x728> @ imm = #-0xd8
    2920: eafffec4     	b	0x2438 <fftwObject_tilde_perform+0x314> @ imm = #-0x4f0
    2924: e2853865     	add	r3, r5, #6619136
    2928: e304ecb8     	movw	lr, #0x4cb8
    292c: e2832ecb     	add	r2, r3, #3248
    2930: e340e065     	movt	lr, #0x65
    2934: e282c008     	add	r12, r2, #8
    2938: e085800e     	add	r8, r5, lr
    293c: e3a00000     	mov	r0, #0
    2940: e5820000     	str	r0, [r2]
    2944: e5820004     	str	r0, [r2, #0x4]
    2948: e58c0000     	str	r0, [r12]
    294c: e28cc040     	add	r12, r12, #64
    2950: e50c003c     	str	r0, [r12, #-0x3c]
    2954: e50c0038     	str	r0, [r12, #-0x38]
    2958: e50c0034     	str	r0, [r12, #-0x34]
    295c: e50c0030     	str	r0, [r12, #-0x30]
    2960: e50c002c     	str	r0, [r12, #-0x2c]
    2964: e50c0028     	str	r0, [r12, #-0x28]
    2968: e50c0024     	str	r0, [r12, #-0x24]
    296c: e50c0020     	str	r0, [r12, #-0x20]
    2970: e50c001c     	str	r0, [r12, #-0x1c]
    2974: e50c0018     	str	r0, [r12, #-0x18]
    2978: e50c0014     	str	r0, [r12, #-0x14]
    297c: e50c0010     	str	r0, [r12, #-0x10]
    2980: e50c000c     	str	r0, [r12, #-0xc]
    2984: e50c0008     	str	r0, [r12, #-0x8]
    2988: e50c0004     	str	r0, [r12, #-0x4]
    298c: e158000c     	cmp	r8, r12
    2990: 1affffec     	bne	0x2948 <fftwObject_tilde_perform+0x824> @ imm = #-0x50
    2994: eaffff53     	b	0x26e8 <fftwObject_tilde_perform+0x5c4> @ imm = #-0x2b4
    2998: e596ccfc     	ldr	r12, [r6, #0xcfc]
    299c: e2861ece     	add	r1, r6, #3296
    29a0: e286aecf     	add	r10, r6, #3312
    29a4: e5c62d0d     	strb	r2, [r6, #0xd0d]
    29a8: ed912a03     	vldr	s4, [r1, #12]
    29ac: e24c8001     	sub	r8, r12, #1
    29b0: e2863a01     	add	r3, r6, #4096
    29b4: e586cd14     	str	r12, [r6, #0xd14]
    29b8: ee01ca10     	vmov	s2, r12
    29bc: e593e03c     	ldr	lr, [r3, #0x3c]
    29c0: edda3a00     	vldr	s7, [r10]
    29c4: eef81ac1     	vcvt.f32.s32	s3, s2
    29c8: ed93ba12     	vldr	s22, [r3, #72]
    29cc: ed93aa13     	vldr	s20, [r3, #76]
    29d0: ee612a82     	vmul.f32	s5, s3, s4
    29d4: ee214aa3     	vmul.f32	s8, s3, s7
    29d8: eebd3ae2     	vcvt.s32.f32	s6, s5
    29dc: eefd4ac4     	vcvt.s32.f32	s9, s8
    29e0: ee139a10     	vmov	r9, s6
    29e4: ee142a90     	vmov	r2, s9
    29e8: e1590008     	cmp	r9, r8
    29ec: e5938040     	ldr	r8, [r3, #0x40]
    29f0: a24c9002     	subge	r9, r12, #2
    29f4: e5869d00     	str	r9, [r6, #0xd00]
    29f8: e3520002     	cmp	r2, #2
    29fc: b3a02002     	movlt	r2, #2
    2a00: e0890002     	add	r0, r9, r2
    2a04: e150000c     	cmp	r0, r12
    2a08: a1a0000c     	movge	r0, r12
    2a0c: e0404009     	sub	r4, r0, r9
    2a10: e1540002     	cmp	r4, r2
    2a14: e5860d04     	str	r0, [r6, #0xd04]
    2a18: a1a04002     	movge	r4, r2
    2a1c: e154000e     	cmp	r4, lr
    2a20: e5864d10     	str	r4, [r6, #0xd10]
    2a24: e084c584     	add	r12, r4, r4, lsl #11
    2a28: b1a0b004     	movlt	r11, r4
    2a2c: a1a0b00e     	movge	r11, lr
    2a30: e1540008     	cmp	r4, r8
    2a34: e586cd18     	str	r12, [r6, #0xd18]
    2a38: e583b03c     	str	r11, [r3, #0x3c]
    2a3c: b1a02004     	movlt	r2, r4
    2a40: a1a02008     	movge	r2, r8
    2a44: e35b0000     	cmp	r11, #0
    2a48: e0441002     	sub	r1, r4, r2
    2a4c: e5832040     	str	r2, [r3, #0x40]
    2a50: e58d1008     	str	r1, [sp, #0x8]
    2a54: da00027e     	ble	0x3454 <fftwObject_tilde_perform+0x1330> @ imm = #0x9f8
    2a58: ee05ea10     	vmov	s10, lr
    2a5c: e21b3003     	ands	r3, r11, #3
    2a60: e3009d1c     	movw	r9, #0xd1c
    2a64: e3409066     	movt	r9, #0x66
    2a68: eef85ac5     	vcvt.f32.s32	s11, s10
    2a6c: e0859009     	add	r9, r5, r9
    2a70: e3a0a000     	mov	r10, #0
    2a74: eec9aa25     	vdiv.f32	s21, s18, s11
    2a78: eeb7cacb     	vcvt.f64.f32	d12, s22
    2a7c: 0a000017     	beq	0x2ae0 <fftwObject_tilde_perform+0x9bc> @ imm = #0x5c
    2a80: e3530001     	cmp	r3, #1
    2a84: 0a00000a     	beq	0x2ab4 <fftwObject_tilde_perform+0x990> @ imm = #0x28
    2a88: e3530002     	cmp	r3, #2
    2a8c: 1a000269     	bne	0x3438 <fftwObject_tilde_perform+0x1314> @ imm = #0x9a4
    2a90: ee09aa90     	vmov	s19, r10
    2a94: e28aa001     	add	r10, r10, #1
    2a98: eeb01b4c     	vmov.f64	d1, d12
    2a9c: eef8bae9     	vcvt.f32.s32	s23, s19
    2aa0: ee2b7aaa     	vmul.f32	s14, s23, s21
    2aa4: eeb70ac7     	vcvt.f64.f32	d0, s14
    2aa8: ebfff715     	bl	0x704 <.plt+0xec>       @ imm = #-0x23ac  // CALL __pow_finite
    2aac: eef77bc0     	vcvt.f32.f64	s15, d0
    2ab0: ece97a01     	vstmia	r9!, {s15}
    2ab4: ee06aa90     	vmov	s13, r10
    2ab8: e28aa001     	add	r10, r10, #1
    2abc: eeb01b4c     	vmov.f64	d1, d12
    2ac0: eeb80ae6     	vcvt.f32.s32	s0, s13
    2ac4: ee600a2a     	vmul.f32	s1, s0, s21
    2ac8: eeb70ae0     	vcvt.f64.f32	d0, s1
    2acc: ebfff70c     	bl	0x704 <.plt+0xec>       @ imm = #-0x23d0  // CALL __pow_finite
    2ad0: e15b000a     	cmp	r11, r10
    2ad4: eeb71bc0     	vcvt.f32.f64	s2, d0
    2ad8: eca91a01     	vstmia	r9!, {s2}
    2adc: 0a00002c     	beq	0x2b94 <fftwObject_tilde_perform+0xa70> @ imm = #0xb0
    2ae0: e58d6014     	str	r6, [sp, #0x14]
    2ae4: e1a06004     	mov	r6, r4
    2ae8: e28a4001     	add	r4, r10, #1
    2aec: ee01aa90     	vmov	s3, r10
    2af0: ee0b4a10     	vmov	s22, r4
    2af4: e1a04009     	mov	r4, r9
    2af8: eeb82ae1     	vcvt.f32.s32	s4, s3
    2afc: e2899010     	add	r9, r9, #16
    2b00: ee622a2a     	vmul.f32	s5, s4, s21
    2b04: eeb01b4c     	vmov.f64	d1, d12
    2b08: eeb70ae2     	vcvt.f64.f32	d0, s5
    2b0c: ebfff6fc     	bl	0x704 <.plt+0xec>       @ imm = #-0x2410  // CALL __pow_finite
    2b10: eeb83acb     	vcvt.f32.s32	s6, s22
    2b14: ee633a2a     	vmul.f32	s7, s6, s21
    2b18: eeb01b4c     	vmov.f64	d1, d12
    2b1c: eeb74bc0     	vcvt.f32.f64	s8, d0
    2b20: eeb70ae3     	vcvt.f64.f32	d0, s7
    2b24: eca44a01     	vstmia	r4!, {s8}
    2b28: ebfff6f5     	bl	0x704 <.plt+0xec>       @ imm = #-0x242c  // CALL __pow_finite
    2b2c: ee1b2a10     	vmov	r2, s22
    2b30: eeb01b4c     	vmov.f64	d1, d12
    2b34: e2820001     	add	r0, r2, #1
    2b38: ee040a90     	vmov	s9, r0
    2b3c: eeb85ae4     	vcvt.f32.s32	s10, s9
    2b40: ee655a2a     	vmul.f32	s11, s10, s21
    2b44: eeb76bc0     	vcvt.f32.f64	s12, d0
    2b48: eeb70ae5     	vcvt.f64.f32	d0, s11
    2b4c: ed096a03     	vstr	s12, [r9, #-12]
    2b50: ebfff6eb     	bl	0x704 <.plt+0xec>       @ imm = #-0x2454  // CALL __pow_finite
    2b54: e28ac003     	add	r12, r10, #3
    2b58: e28aa004     	add	r10, r10, #4
    2b5c: ee09ca90     	vmov	s19, r12
    2b60: eef8bae9     	vcvt.f32.s32	s23, s19
    2b64: ee6b7aaa     	vmul.f32	s15, s23, s21
    2b68: eeb01b4c     	vmov.f64	d1, d12
    2b6c: eeb77bc0     	vcvt.f32.f64	s14, d0
    2b70: eeb70ae7     	vcvt.f64.f32	d0, s15
    2b74: ed847a01     	vstr	s14, [r4, #4]
    2b78: ebfff6e1     	bl	0x704 <.plt+0xec>       @ imm = #-0x247c  // CALL __pow_finite
    2b7c: e15b000a     	cmp	r11, r10
    2b80: eef76bc0     	vcvt.f32.f64	s13, d0
    2b84: ed496a01     	vstr	s13, [r9, #-4]
    2b88: 1affffd6     	bne	0x2ae8 <fftwObject_tilde_perform+0x9c4> @ imm = #-0xa8
    2b8c: e1a04006     	mov	r4, r6
    2b90: e59d6014     	ldr	r6, [sp, #0x14]
    2b94: e59de008     	ldr	lr, [sp, #0x8]
    2b98: e15e000b     	cmp	lr, r11
    2b9c: da00002d     	ble	0x2c58 <fftwObject_tilde_perform+0xb34> @ imm = #0xb4
    2ba0: e3081347     	movw	r1, #0x8347
    2ba4: e3401019     	movt	r1, #0x19
    2ba8: e08b3001     	add	r3, r11, r1
    2bac: e3002d1c     	movw	r2, #0xd1c
    2bb0: e3402066     	movt	r2, #0x66
    2bb4: e0850002     	add	r0, r5, r2
    2bb8: e0853103     	add	r3, r5, r3, lsl #2
    2bbc: e080c10e     	add	r12, r0, lr, lsl #2
    2bc0: e04c9003     	sub	r9, r12, r3
    2bc4: e249e004     	sub	lr, r9, #4
    2bc8: e1a0a12e     	lsr	r10, lr, #2
    2bcc: e28a1001     	add	r1, r10, #1
    2bd0: e2112007     	ands	r2, r1, #7
    2bd4: 0a000013     	beq	0x2c28 <fftwObject_tilde_perform+0xb04> @ imm = #0x4c
    2bd8: e3520001     	cmp	r2, #1
    2bdc: 0a00000e     	beq	0x2c1c <fftwObject_tilde_perform+0xaf8> @ imm = #0x38
    2be0: e3520002     	cmp	r2, #2
    2be4: 0a00000b     	beq	0x2c18 <fftwObject_tilde_perform+0xaf4> @ imm = #0x2c
    2be8: e3520003     	cmp	r2, #3
    2bec: 0a000008     	beq	0x2c14 <fftwObject_tilde_perform+0xaf0> @ imm = #0x20
    2bf0: e3520004     	cmp	r2, #4
    2bf4: 0a000005     	beq	0x2c10 <fftwObject_tilde_perform+0xaec> @ imm = #0x14
    2bf8: e3520005     	cmp	r2, #5
    2bfc: 0a000002     	beq	0x2c0c <fftwObject_tilde_perform+0xae8> @ imm = #0x8
    2c00: e3520006     	cmp	r2, #6
    2c04: 1ca39a01     	vstmiane	r3!, {s18}
    2c08: eca39a01     	vstmia	r3!, {s18}
    2c0c: eca39a01     	vstmia	r3!, {s18}
    2c10: eca39a01     	vstmia	r3!, {s18}
    2c14: eca39a01     	vstmia	r3!, {s18}
    2c18: eca39a01     	vstmia	r3!, {s18}
    2c1c: eca39a01     	vstmia	r3!, {s18}
    2c20: e15c0003     	cmp	r12, r3
    2c24: 0a00000b     	beq	0x2c58 <fftwObject_tilde_perform+0xb34> @ imm = #0x2c
    2c28: e1a00003     	mov	r0, r3
    2c2c: e2833020     	add	r3, r3, #32
    2c30: eca09a01     	vstmia	r0!, {s18}
    2c34: ed039a07     	vstr	s18, [r3, #-28]
    2c38: ed809a01     	vstr	s18, [r0, #4]
    2c3c: ed039a05     	vstr	s18, [r3, #-20]
    2c40: ed039a04     	vstr	s18, [r3, #-16]
    2c44: ed039a03     	vstr	s18, [r3, #-12]
    2c48: ed039a02     	vstr	s18, [r3, #-8]
    2c4c: ed039a01     	vstr	s18, [r3, #-4]
    2c50: e15c0003     	cmp	r12, r3
    2c54: 1afffff3     	bne	0x2c28 <fftwObject_tilde_perform+0xb04> @ imm = #-0x34
    2c58: e3580000     	cmp	r8, #0
    2c5c: da00005e     	ble	0x2ddc <fftwObject_tilde_perform+0xcb8> @ imm = #0x178
    2c60: ee0c8a90     	vmov	s25, r8
    2c64: e59d9008     	ldr	r9, [sp, #0x8]
    2c68: e308c347     	movw	r12, #0x8347
    2c6c: e340c019     	movt	r12, #0x19
    2c70: eef8aaec     	vcvt.f32.s32	s21, s25
    2c74: e089100c     	add	r1, r9, r12
    2c78: e218e003     	ands	lr, r8, #3
    2c7c: e3a0a000     	mov	r10, #0
    2c80: e0859101     	add	r9, r5, r1, lsl #2
    2c84: eef80bec     	vcvt.f64.s32	d16, s25
    2c88: eec99a2a     	vdiv.f32	s19, s18, s21
    2c8c: ee80bba0     	vdiv.f64	d11, d16, d16
    2c90: eeb7aaca     	vcvt.f64.f32	d10, s20
    2c94: 0a00001b     	beq	0x2d08 <fftwObject_tilde_perform+0xbe4> @ imm = #0x6c
    2c98: e35e0001     	cmp	lr, #1
    2c9c: 0a00000c     	beq	0x2cd4 <fftwObject_tilde_perform+0xbb0> @ imm = #0x30
    2ca0: e35e0002     	cmp	lr, #2
    2ca4: 1a0001da     	bne	0x3414 <fftwObject_tilde_perform+0x12f0> @ imm = #0x768
    2ca8: ee00aa90     	vmov	s1, r10
    2cac: e28aa001     	add	r10, r10, #1
    2cb0: eeb01b4a     	vmov.f64	d1, d10
    2cb4: eeb82ae0     	vcvt.f32.s32	s4, s1
    2cb8: ee622a29     	vmul.f32	s5, s4, s19
    2cbc: eeb70ae2     	vcvt.f64.f32	d0, s5
    2cc0: ebfff68f     	bl	0x704 <.plt+0xec>       @ imm = #-0x25c4  // CALL __pow_finite
    2cc4: ee783b40     	vsub.f64	d19, d8, d0
    2cc8: ee6b4b23     	vmul.f64	d20, d11, d19
    2ccc: eeb71be4     	vcvt.f32.f64	s2, d20
    2cd0: eca91a01     	vstmia	r9!, {s2}
    2cd4: ee01aa90     	vmov	s3, r10
    2cd8: e28aa001     	add	r10, r10, #1
    2cdc: eeb83ae1     	vcvt.f32.s32	s6, s3
    2ce0: ee633a29     	vmul.f32	s7, s6, s19
    2ce4: eeb01b4a     	vmov.f64	d1, d10
    2ce8: eeb70ae3     	vcvt.f64.f32	d0, s7
    2cec: ebfff684     	bl	0x704 <.plt+0xec>       @ imm = #-0x25f0  // CALL __pow_finite
    2cf0: e158000a     	cmp	r8, r10
    2cf4: ee785b40     	vsub.f64	d21, d8, d0
    2cf8: ee6b6b25     	vmul.f64	d22, d11, d21
    2cfc: eeb74be6     	vcvt.f32.f64	s8, d22
    2d00: eca94a01     	vstmia	r9!, {s8}
    2d04: 0a000034     	beq	0x2ddc <fftwObject_tilde_perform+0xcb8> @ imm = #0xd0
    2d08: e1a0b006     	mov	r11, r6
    2d0c: e1a06004     	mov	r6, r4
    2d10: e28a4001     	add	r4, r10, #1
    2d14: ee04aa90     	vmov	s9, r10
    2d18: eeb01b4a     	vmov.f64	d1, d10
    2d1c: ee0c4a90     	vmov	s25, r4
    2d20: e1a04009     	mov	r4, r9
    2d24: e2899010     	add	r9, r9, #16
    2d28: eeb85ae4     	vcvt.f32.s32	s10, s9
    2d2c: ee655a29     	vmul.f32	s11, s10, s19
    2d30: eeb70ae5     	vcvt.f64.f32	d0, s11
    2d34: ebfff672     	bl	0x704 <.plt+0xec>       @ imm = #-0x2638  // CALL __pow_finite
    2d38: eeb86aec     	vcvt.f32.s32	s12, s25
    2d3c: ee667a29     	vmul.f32	s15, s12, s19
    2d40: eeb01b4a     	vmov.f64	d1, d10
    2d44: ee787b40     	vsub.f64	d23, d8, d0
    2d48: ee6b8b27     	vmul.f64	d24, d11, d23
    2d4c: eeb70ae7     	vcvt.f64.f32	d0, s15
    2d50: eeb77be8     	vcvt.f32.f64	s14, d24
    2d54: eca47a01     	vstmia	r4!, {s14}
    2d58: ebfff669     	bl	0x704 <.plt+0xec>       @ imm = #-0x265c  // CALL __pow_finite
    2d5c: ee1c2a90     	vmov	r2, s25
    2d60: eeb01b4a     	vmov.f64	d1, d10
    2d64: e2820001     	add	r0, r2, #1
    2d68: ee060a90     	vmov	s13, r0
    2d6c: eeb8cae6     	vcvt.f32.s32	s24, s13
    2d70: ee789b40     	vsub.f64	d25, d8, d0
    2d74: ee6bab29     	vmul.f64	d26, d11, d25
    2d78: ee2c2a29     	vmul.f32	s4, s24, s19
    2d7c: eef72bea     	vcvt.f32.f64	s5, d26
    2d80: eeb70ac2     	vcvt.f64.f32	d0, s4
    2d84: ed492a03     	vstr	s5, [r9, #-12]
    2d88: ebfff65d     	bl	0x704 <.plt+0xec>       @ imm = #-0x268c  // CALL __pow_finite
    2d8c: e28a3003     	add	r3, r10, #3
    2d90: e28aa004     	add	r10, r10, #4
    2d94: ee013a10     	vmov	s2, r3
    2d98: eeb83ac1     	vcvt.f32.s32	s6, s2
    2d9c: ee633a29     	vmul.f32	s7, s6, s19
    2da0: eeb01b4a     	vmov.f64	d1, d10
    2da4: ee78bb40     	vsub.f64	d27, d8, d0
    2da8: ee6bcb2b     	vmul.f64	d28, d11, d27
    2dac: eeb70ae3     	vcvt.f64.f32	d0, s7
    2db0: eeb74bec     	vcvt.f32.f64	s8, d28
    2db4: ed844a01     	vstr	s8, [r4, #4]
    2db8: ebfff651     	bl	0x704 <.plt+0xec>       @ imm = #-0x26bc  // CALL __pow_finite
    2dbc: e158000a     	cmp	r8, r10
    2dc0: ee78db40     	vsub.f64	d29, d8, d0
    2dc4: ee6beb2d     	vmul.f64	d30, d11, d29
    2dc8: eeb70bee     	vcvt.f32.f64	s0, d30
    2dcc: ed090a01     	vstr	s0, [r9, #-4]
    2dd0: 1affffce     	bne	0x2d10 <fftwObject_tilde_perform+0xbec> @ imm = #-0xc8
    2dd4: e1a04006     	mov	r4, r6
    2dd8: e1a0600b     	mov	r6, r11
    2ddc: e51f8ba4     	ldr	r8, [pc, #-0xba4]       @ 0x2240 <fftwObject_tilde_perform+0x11c>  // u32=0x1448; f32?=7.27554163e-42
    2de0: ee0b4a90     	vmov	s23, r4
    2de4: eeb80aeb     	vcvt.f32.s32	s0, s23
    2de8: ebfff64e     	bl	0x728 <.plt+0x110>      @ imm = #-0x26c8  // CALL postfloat
    2dec: e08f0008     	add	r0, pc, r8
    2df0: ebfff634     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0x2730  // CALL post
    2df4: e596cd00     	ldr	r12, [r6, #0xd00]
    2df8: ee0aca90     	vmov	s21, r12
    2dfc: eeb80aea     	vcvt.f32.s32	s0, s21
    2e00: ebfff648     	bl	0x728 <.plt+0x110>      @ imm = #-0x26e0  // CALL postfloat
    2e04: e51f1bc8     	ldr	r1, [pc, #-0xbc8]       @ 0x2244 <fftwObject_tilde_perform+0x120>  // u32=0x1438; f32?=7.25312085e-42
    2e08: e08f0001     	add	r0, pc, r1
    2e0c: ebfff62d     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0x274c  // CALL post
    2e10: e596ad04     	ldr	r10, [r6, #0xd04]
    2e14: ee09aa90     	vmov	s19, r10
    2e18: eeb80ae9     	vcvt.f32.s32	s0, s19
    2e1c: ebfff641     	bl	0x728 <.plt+0x110>      @ imm = #-0x26fc  // CALL postfloat
    2e20: e51f2be0     	ldr	r2, [pc, #-0xbe0]       @ 0x2248 <fftwObject_tilde_perform+0x124>  // u32=0x142c; f32?=7.23630527e-42
    2e24: e08f0002     	add	r0, pc, r2
    2e28: ebfff626     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0x2768  // CALL post
    2e2c: e3a00000     	mov	r0, #0
    2e30: e5860cfc     	str	r0, [r6, #0xcfc]
    2e34: eafffd0f     	b	0x2278 <fftwObject_tilde_perform+0x154> @ imm = #-0xbc4
    2e38: e5969cfc     	ldr	r9, [r6, #0xcfc]
    2e3c: e2854903     	add	r4, r5, #49152
    2e40: e2840028     	add	r0, r4, #40
    2e44: e2858801     	add	r8, r5, #65536
    2e48: e594c028     	ldr	r12, [r4, #0x28]
    2e4c: e3a0e001     	mov	lr, #1
    2e50: e0893589     	add	r3, r9, r9, lsl #11
    2e54: e5863ce4     	str	r3, [r6, #0xce4]
    2e58: e2831a02     	add	r1, r3, #8192
    2e5c: e2843030     	add	r3, r4, #48
    2e60: e281a006     	add	r10, r1, #6
    2e64: e5c6ed0d     	strb	lr, [r6, #0xd0d]
    2e68: e288e030     	add	lr, r8, #48
    2e6c: e085418a     	add	r4, r5, r10, lsl #3
    2e70: e2842008     	add	r2, r4, #8
    2e74: e584c000     	str	r12, [r4]
    2e78: e5908004     	ldr	r8, [r0, #0x4]
    2e7c: e5848004     	str	r8, [r4, #0x4]
    2e80: e593c000     	ldr	r12, [r3]
    2e84: e2833040     	add	r3, r3, #64
    2e88: e2822040     	add	r2, r2, #64
    2e8c: e502c040     	str	r12, [r2, #-0x40]
    2e90: e513103c     	ldr	r1, [r3, #-0x3c]
    2e94: e502103c     	str	r1, [r2, #-0x3c]
    2e98: e513a038     	ldr	r10, [r3, #-0x38]
    2e9c: e502a038     	str	r10, [r2, #-0x38]
    2ea0: e5130034     	ldr	r0, [r3, #-0x34]
    2ea4: e5020034     	str	r0, [r2, #-0x34]
    2ea8: e5134030     	ldr	r4, [r3, #-0x30]
    2eac: e5024030     	str	r4, [r2, #-0x30]
    2eb0: e513802c     	ldr	r8, [r3, #-0x2c]
    2eb4: e502802c     	str	r8, [r2, #-0x2c]
    2eb8: e513c028     	ldr	r12, [r3, #-0x28]
    2ebc: e502c028     	str	r12, [r2, #-0x28]
    2ec0: e5131024     	ldr	r1, [r3, #-0x24]
    2ec4: e5021024     	str	r1, [r2, #-0x24]
    2ec8: e513a020     	ldr	r10, [r3, #-0x20]
    2ecc: e502a020     	str	r10, [r2, #-0x20]
    2ed0: e513001c     	ldr	r0, [r3, #-0x1c]
    2ed4: e502001c     	str	r0, [r2, #-0x1c]
    2ed8: e5134018     	ldr	r4, [r3, #-0x18]
    2edc: e5024018     	str	r4, [r2, #-0x18]
    2ee0: e5138014     	ldr	r8, [r3, #-0x14]
    2ee4: e5028014     	str	r8, [r2, #-0x14]
    2ee8: e513c010     	ldr	r12, [r3, #-0x10]
    2eec: e502c010     	str	r12, [r2, #-0x10]
    2ef0: e513100c     	ldr	r1, [r3, #-0xc]
    2ef4: e502100c     	str	r1, [r2, #-0xc]
    2ef8: e513a008     	ldr	r10, [r3, #-0x8]
    2efc: e502a008     	str	r10, [r2, #-0x8]
    2f00: e5130004     	ldr	r0, [r3, #-0x4]
    2f04: e15e0003     	cmp	lr, r3
    2f08: e5020004     	str	r0, [r2, #-0x4]
    2f0c: 1affffdb     	bne	0x2e80 <fftwObject_tilde_perform+0xd5c> @ imm = #-0x94
    2f10: e2899001     	add	r9, r9, #1
    2f14: e5869cfc     	str	r9, [r6, #0xcfc]
    2f18: e35900c8     	cmp	r9, #200
    2f1c: dafffcd5     	ble	0x2278 <fftwObject_tilde_perform+0x154> @ imm = #-0xcac
    2f20: e286eece     	add	lr, r6, #3296
    2f24: e2864ecf     	add	r4, r6, #3312
    2f28: eddf0afc     	vldr	s1, [pc, #1008]         @ 0x3320 <fftwObject_tilde_perform+0x11fc>  // f32=200
    2f2c: e286ca01     	add	r12, r6, #4096
    2f30: e3008d0c     	movw	r8, #0xd0c
    2f34: e3a09000     	mov	r9, #0
    2f38: e59ca03c     	ldr	r10, [r12, #0x3c]
    2f3c: e3a010c8     	mov	r1, #200
    2f40: ed9eba03     	vldr	s22, [lr, #12]
    2f44: e18690b8     	strh	r9, [r6, r8]
    2f48: e59c8040     	ldr	r8, [r12, #0x40]
    2f4c: edd41a00     	vldr	s3, [r4]
    2f50: e5861d14     	str	r1, [r6, #0xd14]
    2f54: e5869cfc     	str	r9, [r6, #0xcfc]
    2f58: ee6b4a20     	vmul.f32	s9, s22, s1
    2f5c: ed9c5a12     	vldr	s10, [r12, #72]
    2f60: ed9caa13     	vldr	s20, [r12, #76]
    2f64: ee61caa0     	vmul.f32	s25, s3, s1
    2f68: eefd5ae4     	vcvt.s32.f32	s11, s9
    2f6c: eebd6aec     	vcvt.s32.f32	s12, s25
    2f70: ee150a90     	vmov	r0, s11
    2f74: ee163a10     	vmov	r3, s12
    2f78: e35000c6     	cmp	r0, #198
    2f7c: a3a000c6     	movge	r0, #198
    2f80: e5860d00     	str	r0, [r6, #0xd00]
    2f84: e3530002     	cmp	r3, #2
    2f88: b3a03002     	movlt	r3, #2
    2f8c: e0832000     	add	r2, r3, r0
    2f90: e35200c8     	cmp	r2, #200
    2f94: a3a020c8     	movge	r2, #200
    2f98: e0424000     	sub	r4, r2, r0
    2f9c: e1540003     	cmp	r4, r3
    2fa0: e5862d04     	str	r2, [r6, #0xd04]
    2fa4: a1a04003     	movge	r4, r3
    2fa8: e154000a     	cmp	r4, r10
    2fac: e5864d10     	str	r4, [r6, #0xd10]
    2fb0: e084e584     	add	lr, r4, r4, lsl #11
    2fb4: b1a0b004     	movlt	r11, r4
    2fb8: a1a0b00a     	movge	r11, r10
    2fbc: e1540008     	cmp	r4, r8
    2fc0: e586ed18     	str	lr, [r6, #0xd18]
    2fc4: e58cb03c     	str	r11, [r12, #0x3c]
    2fc8: b1a03004     	movlt	r3, r4
    2fcc: a1a03008     	movge	r3, r8
    2fd0: e15b0009     	cmp	r11, r9
    2fd4: e0441003     	sub	r1, r4, r3
    2fd8: e58c3040     	str	r3, [r12, #0x40]
    2fdc: e58d1008     	str	r1, [sp, #0x8]
    2fe0: da00011d     	ble	0x345c <fftwObject_tilde_perform+0x1338> @ imm = #0x474
    2fe4: ee07aa90     	vmov	s15, r10
    2fe8: e21bc003     	ands	r12, r11, #3
    2fec: e300ad1c     	movw	r10, #0xd1c
    2ff0: e340a066     	movt	r10, #0x66
    2ff4: eeb87ae7     	vcvt.f32.s32	s14, s15
    2ff8: e085a00a     	add	r10, r5, r10
    2ffc: ee89ca07     	vdiv.f32	s24, s18, s14
    3000: eeb7bac5     	vcvt.f64.f32	d11, s10
    3004: 0a0000d4     	beq	0x335c <fftwObject_tilde_perform+0x1238> @ imm = #0x350
    3008: e35c0001     	cmp	r12, #1
    300c: 0a000010     	beq	0x3054 <fftwObject_tilde_perform+0xf30> @ imm = #0x40
    3010: e35c0002     	cmp	r12, #2
    3014: 0a000005     	beq	0x3030 <fftwObject_tilde_perform+0xf0c> @ imm = #0x14
    3018: eeb01b4b     	vmov.f64	d1, d11
    301c: e3a09001     	mov	r9, #1
    3020: ed9f0bbc     	vldr	d0, [pc, #752]          @ 0x3318 <fftwObject_tilde_perform+0x11f4>  // f64=0
    3024: ebfff5b6     	bl	0x704 <.plt+0xec>       @ imm = #-0x2928  // CALL __pow_finite
    3028: eef76bc0     	vcvt.f32.f64	s13, d0
    302c: ecea6a01     	vstmia	r10!, {s13}
    3030: ee029a10     	vmov	s4, r9
    3034: e2899001     	add	r9, r9, #1
    3038: eeb01b4b     	vmov.f64	d1, d11
    303c: eef82ac2     	vcvt.f32.s32	s5, s4
    3040: ee223a8c     	vmul.f32	s6, s5, s24
    3044: eeb70ac3     	vcvt.f64.f32	d0, s6
    3048: ebfff5ad     	bl	0x704 <.plt+0xec>       @ imm = #-0x294c  // CALL __pow_finite
    304c: eeb71bc0     	vcvt.f32.f64	s2, d0
    3050: ecaa1a01     	vstmia	r10!, {s2}
    3054: ee039a90     	vmov	s7, r9
    3058: e2899001     	add	r9, r9, #1
    305c: eeb01b4b     	vmov.f64	d1, d11
    3060: eeb84ae3     	vcvt.f32.s32	s8, s7
    3064: ee240a0c     	vmul.f32	s0, s8, s24
    3068: eeb70ac0     	vcvt.f64.f32	d0, s0
    306c: ebfff5a4     	bl	0x704 <.plt+0xec>       @ imm = #-0x2970  // CALL __pow_finite
    3070: e15b0009     	cmp	r11, r9
    3074: eef7abc0     	vcvt.f32.f64	s21, d0
    3078: eceaaa01     	vstmia	r10!, {s21}
    307c: 1a0000b6     	bne	0x335c <fftwObject_tilde_perform+0x1238> @ imm = #0x2d8
    3080: e59de008     	ldr	lr, [sp, #0x8]
    3084: e15e000b     	cmp	lr, r11
    3088: da00002d     	ble	0x3144 <fftwObject_tilde_perform+0x1020> @ imm = #0xb4
    308c: e3081347     	movw	r1, #0x8347
    3090: e3401019     	movt	r1, #0x19
    3094: e08b9001     	add	r9, r11, r1
    3098: e3000d1c     	movw	r0, #0xd1c
    309c: e3400066     	movt	r0, #0x66
    30a0: e0852000     	add	r2, r5, r0
    30a4: e0853109     	add	r3, r5, r9, lsl #2
    30a8: e082a10e     	add	r10, r2, lr, lsl #2
    30ac: e04ae003     	sub	lr, r10, r3
    30b0: e24ec004     	sub	r12, lr, #4
    30b4: e1a0112c     	lsr	r1, r12, #2
    30b8: e2819001     	add	r9, r1, #1
    30bc: e2190007     	ands	r0, r9, #7
    30c0: 0a000013     	beq	0x3114 <fftwObject_tilde_perform+0xff0> @ imm = #0x4c
    30c4: e3500001     	cmp	r0, #1
    30c8: 0a00000e     	beq	0x3108 <fftwObject_tilde_perform+0xfe4> @ imm = #0x38
    30cc: e3500002     	cmp	r0, #2
    30d0: 0a00000b     	beq	0x3104 <fftwObject_tilde_perform+0xfe0> @ imm = #0x2c
    30d4: e3500003     	cmp	r0, #3
    30d8: 0a000008     	beq	0x3100 <fftwObject_tilde_perform+0xfdc> @ imm = #0x20
    30dc: e3500004     	cmp	r0, #4
    30e0: 0a000005     	beq	0x30fc <fftwObject_tilde_perform+0xfd8> @ imm = #0x14
    30e4: e3500005     	cmp	r0, #5
    30e8: 0a000002     	beq	0x30f8 <fftwObject_tilde_perform+0xfd4> @ imm = #0x8
    30ec: e3500006     	cmp	r0, #6
    30f0: 1ca39a01     	vstmiane	r3!, {s18}
    30f4: eca39a01     	vstmia	r3!, {s18}
    30f8: eca39a01     	vstmia	r3!, {s18}
    30fc: eca39a01     	vstmia	r3!, {s18}
    3100: eca39a01     	vstmia	r3!, {s18}
    3104: eca39a01     	vstmia	r3!, {s18}
    3108: eca39a01     	vstmia	r3!, {s18}
    310c: e15a0003     	cmp	r10, r3
    3110: 0a00000b     	beq	0x3144 <fftwObject_tilde_perform+0x1020> @ imm = #0x2c
    3114: e1a02003     	mov	r2, r3
    3118: e2833020     	add	r3, r3, #32
    311c: eca29a01     	vstmia	r2!, {s18}
    3120: ed039a07     	vstr	s18, [r3, #-28]
    3124: ed829a01     	vstr	s18, [r2, #4]
    3128: ed039a05     	vstr	s18, [r3, #-20]
    312c: ed039a04     	vstr	s18, [r3, #-16]
    3130: ed039a03     	vstr	s18, [r3, #-12]
    3134: ed039a02     	vstr	s18, [r3, #-8]
    3138: ed039a01     	vstr	s18, [r3, #-4]
    313c: e15a0003     	cmp	r10, r3
    3140: 1afffff3     	bne	0x3114 <fftwObject_tilde_perform+0xff0> @ imm = #-0x34
    3144: e3580000     	cmp	r8, #0
    3148: da000066     	ble	0x32e8 <fftwObject_tilde_perform+0x11c4> @ imm = #0x198
    314c: ee0b8a90     	vmov	s23, r8
    3150: e59dc008     	ldr	r12, [sp, #0x8]
    3154: e308a347     	movw	r10, #0x8347
    3158: e340a019     	movt	r10, #0x19
    315c: eeb8caeb     	vcvt.f32.s32	s24, s23
    3160: e08c100a     	add	r1, r12, r10
    3164: e218e003     	ands	lr, r8, #3
    3168: e3a0a000     	mov	r10, #0
    316c: e0859101     	add	r9, r5, r1, lsl #2
    3170: eef8fbeb     	vcvt.f64.s32	d31, s23
    3174: eec99a0c     	vdiv.f32	s19, s18, s24
    3178: ee8fbbaf     	vdiv.f64	d11, d31, d31
    317c: eeb7aaca     	vcvt.f64.f32	d10, s20
    3180: 0a000023     	beq	0x3214 <fftwObject_tilde_perform+0x10f0> @ imm = #0x8c
    3184: e35e0001     	cmp	lr, #1
    3188: 0a000014     	beq	0x31e0 <fftwObject_tilde_perform+0x10bc> @ imm = #0x50
    318c: e35e0002     	cmp	lr, #2
    3190: 0a000007     	beq	0x31b4 <fftwObject_tilde_perform+0x1090> @ imm = #0x1c
    3194: eeb01b4a     	vmov.f64	d1, d10
    3198: e3a0a001     	mov	r10, #1
    319c: ed9f0b5d     	vldr	d0, [pc, #372]          @ 0x3318 <fftwObject_tilde_perform+0x11f4>  // f64=0
    31a0: ebfff557     	bl	0x704 <.plt+0xec>       @ imm = #-0x2aa4  // CALL __pow_finite
    31a4: ee780b40     	vsub.f64	d16, d8, d0
    31a8: ee6b1b20     	vmul.f64	d17, d11, d16
    31ac: eeb71be1     	vcvt.f32.f64	s2, d17
    31b0: eca91a01     	vstmia	r9!, {s2}
    31b4: ee00aa10     	vmov	s0, r10
    31b8: e28aa001     	add	r10, r10, #1
    31bc: eeb01b4a     	vmov.f64	d1, d10
    31c0: eef80ac0     	vcvt.f32.s32	s1, s0
    31c4: ee60caa9     	vmul.f32	s25, s1, s19
    31c8: eeb70aec     	vcvt.f64.f32	d0, s25
    31cc: ebfff54c     	bl	0x704 <.plt+0xec>       @ imm = #-0x2ad0  // CALL __pow_finite
    31d0: ee782b40     	vsub.f64	d18, d8, d0
    31d4: ee6b3b22     	vmul.f64	d19, d11, d18
    31d8: eef74be3     	vcvt.f32.f64	s9, d19
    31dc: ece94a01     	vstmia	r9!, {s9}
    31e0: ee05aa10     	vmov	s10, r10
    31e4: e28aa001     	add	r10, r10, #1
    31e8: eeb01b4a     	vmov.f64	d1, d10
    31ec: eef85ac5     	vcvt.f32.s32	s11, s10
    31f0: ee256aa9     	vmul.f32	s12, s11, s19
    31f4: eeb70ac6     	vcvt.f64.f32	d0, s12
    31f8: ebfff541     	bl	0x704 <.plt+0xec>       @ imm = #-0x2afc  // CALL __pow_finite
    31fc: e158000a     	cmp	r8, r10
    3200: ee784b40     	vsub.f64	d20, d8, d0
    3204: ee6b5b24     	vmul.f64	d21, d11, d20
    3208: eef77be5     	vcvt.f32.f64	s15, d21
    320c: ece97a01     	vstmia	r9!, {s15}
    3210: 0a000034     	beq	0x32e8 <fftwObject_tilde_perform+0x11c4> @ imm = #0xd0
    3214: e1a0b006     	mov	r11, r6
    3218: e1a06004     	mov	r6, r4
    321c: e28a4001     	add	r4, r10, #1
    3220: ee07aa10     	vmov	s14, r10
    3224: eeb01b4a     	vmov.f64	d1, d10
    3228: ee0c4a10     	vmov	s24, r4
    322c: e1a04009     	mov	r4, r9
    3230: e2899010     	add	r9, r9, #16
    3234: eef86ac7     	vcvt.f32.s32	s13, s14
    3238: ee262aa9     	vmul.f32	s4, s13, s19
    323c: eeb70ac2     	vcvt.f64.f32	d0, s4
    3240: ebfff52f     	bl	0x704 <.plt+0xec>       @ imm = #-0x2b44  // CALL __pow_finite
    3244: eef82acc     	vcvt.f32.s32	s5, s24
    3248: ee223aa9     	vmul.f32	s6, s5, s19
    324c: eeb01b4a     	vmov.f64	d1, d10
    3250: ee786b40     	vsub.f64	d22, d8, d0
    3254: ee6b7b26     	vmul.f64	d23, d11, d22
    3258: eeb70ac3     	vcvt.f64.f32	d0, s6
    325c: eef73be7     	vcvt.f32.f64	s7, d23
    3260: ece43a01     	vstmia	r4!, {s7}
    3264: ebfff526     	bl	0x704 <.plt+0xec>       @ imm = #-0x2b68  // CALL __pow_finite
    3268: ee1c0a10     	vmov	r0, s24
    326c: eeb01b4a     	vmov.f64	d1, d10
    3270: e2802001     	add	r2, r0, #1
    3274: ee042a10     	vmov	s8, r2
    3278: eef8cac4     	vcvt.f32.s32	s25, s8
    327c: ee788b40     	vsub.f64	d24, d8, d0
    3280: ee6b9b28     	vmul.f64	d25, d11, d24
    3284: ee6c4aa9     	vmul.f32	s9, s25, s19
    3288: eeb75be9     	vcvt.f32.f64	s10, d25
    328c: eeb70ae4     	vcvt.f64.f32	d0, s9
    3290: ed095a03     	vstr	s10, [r9, #-12]
    3294: ebfff51a     	bl	0x704 <.plt+0xec>       @ imm = #-0x2b98  // CALL __pow_finite
    3298: e28a3003     	add	r3, r10, #3
    329c: e28aa004     	add	r10, r10, #4
    32a0: ee013a90     	vmov	s3, r3
    32a4: eef85ae1     	vcvt.f32.s32	s11, s3
    32a8: ee256aa9     	vmul.f32	s12, s11, s19
    32ac: eeb01b4a     	vmov.f64	d1, d10
    32b0: ee78ab40     	vsub.f64	d26, d8, d0
    32b4: ee6bbb2a     	vmul.f64	d27, d11, d26
    32b8: eeb70ac6     	vcvt.f64.f32	d0, s12
    32bc: eef77beb     	vcvt.f32.f64	s15, d27
    32c0: edc47a01     	vstr	s15, [r4, #4]
    32c4: ebfff50e     	bl	0x704 <.plt+0xec>       @ imm = #-0x2bc8  // CALL __pow_finite
    32c8: e158000a     	cmp	r8, r10
    32cc: ee78cb40     	vsub.f64	d28, d8, d0
    32d0: ee6bdb2c     	vmul.f64	d29, d11, d28
    32d4: eeb71bed     	vcvt.f32.f64	s2, d29
    32d8: ed091a01     	vstr	s2, [r9, #-4]
    32dc: 1affffce     	bne	0x321c <fftwObject_tilde_perform+0x10f8> @ imm = #-0xc8
    32e0: e1a04006     	mov	r4, r6
    32e4: e1a0600b     	mov	r6, r11
    32e8: e59f8034     	ldr	r8, [pc, #0x34]         @ 0x3324 <fftwObject_tilde_perform+0x1200>  // u32=0xf3c; f32?=5.46506401e-42
    32ec: ee0a4a90     	vmov	s21, r4
    32f0: eeb80aea     	vcvt.f32.s32	s0, s21
    32f4: ebfff50b     	bl	0x728 <.plt+0x110>      @ imm = #-0x2bd4  // CALL postfloat
    32f8: e08f0008     	add	r0, pc, r8
    32fc: ebfff4f1     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0x2c3c  // CALL post
    3300: e596cd00     	ldr	r12, [r6, #0xd00]
    3304: ee0bca90     	vmov	s23, r12
    3308: eeb80aeb     	vcvt.f32.s32	s0, s23
    330c: ebfff505     	bl	0x728 <.plt+0x110>      @ imm = #-0x2bec  // CALL postfloat
    3310: ea000006     	b	0x3330 <fftwObject_tilde_perform+0x120c> @ imm = #0x18
    3314: e320f000     	nop
    3318: 00 00 00 00  	.word	0x00000000
    331c: 00 00 00 00  	.word	0x00000000
    3320: 00 00 48 43  	.word	0x43480000
    3324: 3c 0f 00 00  	.word	0x00000f3c
    3328: 0c 0f 00 00  	.word	0x00000f0c
    332c: 00 0f 00 00  	.word	0x00000f00
    3330: e51f1010     	ldr	r1, [pc, #-0x10]        @ 0x3328 <fftwObject_tilde_perform+0x1204>  // u32=0xf0c; f32?=5.39780168e-42
    3334: e08f0001     	add	r0, pc, r1
    3338: ebfff4e2     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0x2c78  // CALL post
    333c: e596ad04     	ldr	r10, [r6, #0xd04]
    3340: ee09aa90     	vmov	s19, r10
    3344: eeb80ae9     	vcvt.f32.s32	s0, s19
    3348: ebfff4f6     	bl	0x728 <.plt+0x110>      @ imm = #-0x2c28  // CALL postfloat
    334c: e51f0028     	ldr	r0, [pc, #-0x28]        @ 0x332c <fftwObject_tilde_perform+0x1208>  // u32=0xf00; f32?=5.3809861e-42
    3350: e08f0000     	add	r0, pc, r0
    3354: ebfff4db     	bl	0x6c8 <.plt+0xb0>       @ imm = #-0x2c94  // CALL post
    3358: eafffbc6     	b	0x2278 <fftwObject_tilde_perform+0x154> @ imm = #-0x10e8
    335c: e58d6014     	str	r6, [sp, #0x14]
    3360: e1a06004     	mov	r6, r4
    3364: ee099a90     	vmov	s19, r9
    3368: e2894001     	add	r4, r9, #1
    336c: eeb01b4b     	vmov.f64	d1, d11
    3370: ee0c4a90     	vmov	s25, r4
    3374: e1a0400a     	mov	r4, r10
    3378: e28aa010     	add	r10, r10, #16
    337c: eef80ae9     	vcvt.f32.s32	s1, s19
    3380: ee604a8c     	vmul.f32	s9, s1, s24
    3384: eeb70ae4     	vcvt.f64.f32	d0, s9
    3388: ebfff4dd     	bl	0x704 <.plt+0xec>       @ imm = #-0x2c8c  // CALL __pow_finite
    338c: eeb85aec     	vcvt.f32.s32	s10, s25
    3390: ee655a0c     	vmul.f32	s11, s10, s24
    3394: eeb01b4b     	vmov.f64	d1, d11
    3398: eeb76bc0     	vcvt.f32.f64	s12, d0
    339c: eeb70ae5     	vcvt.f64.f32	d0, s11
    33a0: eca46a01     	vstmia	r4!, {s12}
    33a4: ebfff4d6     	bl	0x704 <.plt+0xec>       @ imm = #-0x2ca8  // CALL __pow_finite
    33a8: ee1c0a90     	vmov	r0, s25
    33ac: eeb01b4b     	vmov.f64	d1, d11
    33b0: e2803001     	add	r3, r0, #1
    33b4: ee073a90     	vmov	s15, r3
    33b8: eeb87ae7     	vcvt.f32.s32	s14, s15
    33bc: ee676a0c     	vmul.f32	s13, s14, s24
    33c0: eeb72bc0     	vcvt.f32.f64	s4, d0
    33c4: eeb70ae6     	vcvt.f64.f32	d0, s13
    33c8: ed0a2a03     	vstr	s4, [r10, #-12]
    33cc: ebfff4cc     	bl	0x704 <.plt+0xec>       @ imm = #-0x2cd0  // CALL __pow_finite
    33d0: e2892003     	add	r2, r9, #3
    33d4: e2899004     	add	r9, r9, #4
    33d8: ee022a90     	vmov	s5, r2
    33dc: eeb83ae2     	vcvt.f32.s32	s6, s5
    33e0: ee633a0c     	vmul.f32	s7, s6, s24
    33e4: eeb01b4b     	vmov.f64	d1, d11
    33e8: eeb74bc0     	vcvt.f32.f64	s8, d0
    33ec: eeb70ae3     	vcvt.f64.f32	d0, s7
    33f0: ed844a01     	vstr	s8, [r4, #4]
    33f4: ebfff4c2     	bl	0x704 <.plt+0xec>       @ imm = #-0x2cf8  // CALL __pow_finite
    33f8: e15b0009     	cmp	r11, r9
    33fc: eef71bc0     	vcvt.f32.f64	s3, d0
    3400: ed4a1a01     	vstr	s3, [r10, #-4]
    3404: 1affffd6     	bne	0x3364 <fftwObject_tilde_perform+0x1240> @ imm = #-0xa8
    3408: e1a04006     	mov	r4, r6
    340c: e59d6014     	ldr	r6, [sp, #0x14]
    3410: eaffff1a     	b	0x3080 <fftwObject_tilde_perform+0xf5c> @ imm = #-0x398
    3414: eeb01b4a     	vmov.f64	d1, d10
    3418: e3a0a001     	mov	r10, #1
    341c: ed9f0b11     	vldr	d0, [pc, #68]           @ 0x3468 <fftwObject_tilde_perform+0x1344>  // f64=0
    3420: ebfff4b7     	bl	0x704 <.plt+0xec>       @ imm = #-0x2d24  // CALL __pow_finite
    3424: ee781b40     	vsub.f64	d17, d8, d0
    3428: ee6b2b21     	vmul.f64	d18, d11, d17
    342c: eeb70be2     	vcvt.f32.f64	s0, d18
    3430: eca90a01     	vstmia	r9!, {s0}
    3434: eafffe1b     	b	0x2ca8 <fftwObject_tilde_perform+0xb84> @ imm = #-0x794
    3438: eeb01b4c     	vmov.f64	d1, d12
    343c: e3a0a001     	mov	r10, #1
    3440: ed9f0b08     	vldr	d0, [pc, #32]           @ 0x3468 <fftwObject_tilde_perform+0x1344>  // f64=0
    3444: ebfff4ae     	bl	0x704 <.plt+0xec>       @ imm = #-0x2d48  // CALL __pow_finite
    3448: eeb76bc0     	vcvt.f32.f64	s12, d0
    344c: eca96a01     	vstmia	r9!, {s12}
    3450: eafffd8e     	b	0x2a90 <fftwObject_tilde_perform+0x96c> @ imm = #-0x9c8
    3454: e3a0b000     	mov	r11, #0
    3458: eafffdcd     	b	0x2b94 <fftwObject_tilde_perform+0xa70> @ imm = #-0x8cc
    345c: e1a0b009     	mov	r11, r9
    3460: eaffff06     	b	0x3080 <fftwObject_tilde_perform+0xf5c> @ imm = #-0x3e8
    3464: e320f000     	nop
    3468: 00 00 00 00  	.word	0x00000000
    346c: 00 00 00 00  	.word	0x00000000

