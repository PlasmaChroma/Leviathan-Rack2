00002278 <main_out_stereo_tilde_perform>:
    2278: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    227c: e24dd00c     	sub	sp, sp, #12
    2280: e590e018     	ldr	lr, [r0, #0x18]
    2284: e590c004     	ldr	r12, [r0, #0x4]
    2288: e35e0000     	cmp	lr, #0
    228c: e5903008     	ldr	r3, [r0, #0x8]
    2290: e590100c     	ldr	r1, [r0, #0xc]
    2294: eddc7a07     	vldr	s15, [r12, #28]
    2298: e5902010     	ldr	r2, [r0, #0x10]
    229c: e5904014     	ldr	r4, [r0, #0x14]
    22a0: ed9c7a08     	vldr	s14, [r12, #32]
    22a4: da00005b     	ble	0x2418 <main_out_stereo_tilde_perform+0x1a0> @ imm = #0x16c
    22a8: e083510e     	add	r5, r3, lr, lsl #2
    22ac: e0411003     	sub	r1, r1, r3
    22b0: e0456003     	sub	r6, r5, r3
    22b4: e0422003     	sub	r2, r2, r3
    22b8: e2467004     	sub	r7, r6, #4
    22bc: e0446003     	sub	r6, r4, r3
    22c0: e1a08127     	lsr	r8, r7, #2
    22c4: e2889001     	add	r9, r8, #1
    22c8: e219a003     	ands	r10, r9, #3
    22cc: 0a000023     	beq	0x2360 <main_out_stereo_tilde_perform+0xe8> @ imm = #0x8c
    22d0: e35a0001     	cmp	r10, #1
    22d4: 0a000015     	beq	0x2330 <main_out_stereo_tilde_perform+0xb8> @ imm = #0x54
    22d8: e35a0002     	cmp	r10, #2
    22dc: 0a000009     	beq	0x2308 <main_out_stereo_tilde_perform+0x90> @ imm = #0x24
    22e0: e081b003     	add	r11, r1, r3
    22e4: ed936a00     	vldr	s12, [r3]
    22e8: e082e003     	add	lr, r2, r3
    22ec: e086c003     	add	r12, r6, r3
    22f0: e2833004     	add	r3, r3, #4
    22f4: eddb6a00     	vldr	s13, [r11]
    22f8: ee260a27     	vmul.f32	s0, s12, s15
    22fc: ee670a26     	vmul.f32	s1, s14, s13
    2300: ed8e0a00     	vstr	s0, [lr]
    2304: edcc0a00     	vstr	s1, [r12]
    2308: e0814003     	add	r4, r1, r3
    230c: e0827003     	add	r7, r2, r3
    2310: e0868003     	add	r8, r6, r3
    2314: e2833004     	add	r3, r3, #4
    2318: ed531a01     	vldr	s3, [r3, #-4]
    231c: ed941a00     	vldr	s2, [r4]
    2320: ee612aa7     	vmul.f32	s5, s3, s15
    2324: ee272a01     	vmul.f32	s4, s14, s2
    2328: edc72a00     	vstr	s5, [r7]
    232c: ed882a00     	vstr	s4, [r8]
    2330: e0819003     	add	r9, r1, r3
    2334: e082e003     	add	lr, r2, r3
    2338: e086a003     	add	r10, r6, r3
    233c: e2833004     	add	r3, r3, #4
    2340: ed533a01     	vldr	s7, [r3, #-4]
    2344: e1530005     	cmp	r3, r5
    2348: ed993a00     	vldr	s6, [r9]
    234c: ee634aa7     	vmul.f32	s9, s7, s15
    2350: ee274a03     	vmul.f32	s8, s14, s6
    2354: edce4a00     	vstr	s9, [lr]
    2358: ed8a4a00     	vstr	s8, [r10]
    235c: 0a00002d     	beq	0x2418 <main_out_stereo_tilde_perform+0x1a0> @ imm = #0xb4
    2360: e58d5000     	str	r5, [sp]
    2364: e58d0004     	str	r0, [sp, #0x4]
    2368: e0815003     	add	r5, r1, r3
    236c: ed935a00     	vldr	s10, [r3]
    2370: e0820003     	add	r0, r2, r3
    2374: e086b003     	add	r11, r6, r3
    2378: e283e004     	add	lr, r3, #4
    237c: e283c008     	add	r12, r3, #8
    2380: edd55a00     	vldr	s11, [r5]
    2384: e081a00e     	add	r10, r1, lr
    2388: e082900e     	add	r9, r2, lr
    238c: e086800e     	add	r8, r6, lr
    2390: e081700c     	add	r7, r1, r12
    2394: e082500c     	add	r5, r2, r12
    2398: ee256a27     	vmul.f32	s12, s10, s15
    239c: e086c00c     	add	r12, r6, r12
    23a0: ee676a25     	vmul.f32	s13, s14, s11
    23a4: ed806a00     	vstr	s12, [r0]
    23a8: e283000c     	add	r0, r3, #12
    23ac: e0814000     	add	r4, r1, r0
    23b0: e2833010     	add	r3, r3, #16
    23b4: edcb6a00     	vstr	s13, [r11]
    23b8: e082b000     	add	r11, r2, r0
    23bc: ed130a03     	vldr	s0, [r3, #-12]
    23c0: edda0a00     	vldr	s1, [r10]
    23c4: e086a000     	add	r10, r6, r0
    23c8: ee201a27     	vmul.f32	s2, s0, s15
    23cc: ee671a20     	vmul.f32	s3, s14, s1
    23d0: ed891a00     	vstr	s2, [r9]
    23d4: e59d9000     	ldr	r9, [sp]
    23d8: edc81a00     	vstr	s3, [r8]
    23dc: ed132a02     	vldr	s4, [r3, #-8]
    23e0: edd72a00     	vldr	s5, [r7]
    23e4: ee223a27     	vmul.f32	s6, s4, s15
    23e8: ee673a22     	vmul.f32	s7, s14, s5
    23ec: ed853a00     	vstr	s6, [r5]
    23f0: edcc3a00     	vstr	s7, [r12]
    23f4: ed134a01     	vldr	s8, [r3, #-4]
    23f8: e1530009     	cmp	r3, r9
    23fc: edd44a00     	vldr	s9, [r4]
    2400: ee245a27     	vmul.f32	s10, s8, s15
    2404: ee675a24     	vmul.f32	s11, s14, s9
    2408: ed8b5a00     	vstr	s10, [r11]
    240c: edca5a00     	vstr	s11, [r10]
    2410: 1affffd4     	bne	0x2368 <main_out_stereo_tilde_perform+0xf0> @ imm = #-0xb0
    2414: e59d0004     	ldr	r0, [sp, #0x4]
    2418: e280001c     	add	r0, r0, #28
    241c: e28dd00c     	add	sp, sp, #12
    2420: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}

