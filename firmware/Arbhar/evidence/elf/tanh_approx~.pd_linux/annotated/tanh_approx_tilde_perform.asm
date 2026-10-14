00002268 <tanh_approx_tilde_perform>:
    2268: e590c010     	ldr	r12, [r0, #0x10]
    226c: e5901004     	ldr	r1, [r0, #0x4]
    2270: e35c0000     	cmp	r12, #0
    2274: e5903008     	ldr	r3, [r0, #0x8]
    2278: e92d4070     	push	{r4, r5, r6, lr}
    227c: e3a0e000     	mov	lr, #0
    2280: e590200c     	ldr	r2, [r0, #0xc]
    2284: edd15a07     	vldr	s11, [r1, #28]
    2288: e581e024     	str	lr, [r1, #0x24]
    228c: da000088     	ble	0x24b4 <tanh_approx_tilde_perform+0x24c> @ imm = #0x220
    2290: e083c10c     	add	r12, r3, r12, lsl #2
    2294: e0422003     	sub	r2, r2, r3
    2298: e04c1003     	sub	r1, r12, r3
    229c: eeb96a08     	vmov.f32	s12, #-6.000000e+00
    22a0: e2411004     	sub	r1, r1, #4
    22a4: e1a01121     	lsr	r1, r1, #2
    22a8: e2811001     	add	r1, r1, #1
    22ac: e2111003     	ands	r1, r1, #3
    22b0: eef16a08     	vmov.f32	s13, #6.000000e+00
    22b4: eeb37a0b     	vmov.f32	s14, #2.700000e+01
    22b8: eeb25a02     	vmov.f32	s10, #9.000000e+00
    22bc: 0a000038     	beq	0x23a4 <tanh_approx_tilde_perform+0x13c> @ imm = #0xe0
    22c0: e3510001     	cmp	r1, #1
    22c4: 0a000023     	beq	0x2358 <tanh_approx_tilde_perform+0xf0> @ imm = #0x8c
    22c8: e3510002     	cmp	r1, #2
    22cc: 0a000010     	beq	0x2314 <tanh_approx_tilde_perform+0xac> @ imm = #0x40
    22d0: edd37a00     	vldr	s15, [r3]
    22d4: e0821003     	add	r1, r2, r3
    22d8: e2833004     	add	r3, r3, #4
    22dc: eef03a47     	vmov.f32	s7, s14
    22e0: eef47ac6     	vcmpe.f32	s15, s12
    22e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    22e8: bef07a46     	vmovlt.f32	s15, s12
    22ec: eef47ae6     	vcmpe.f32	s15, s13
    22f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    22f4: 8ef07a66     	vmovhi.f32	s15, s13
    22f8: ee274aa7     	vmul.f32	s8, s15, s15
    22fc: ee744a07     	vadd.f32	s9, s8, s14
    2300: ee443a05     	vmla.f32	s7, s8, s10
    2304: ee240aa7     	vmul.f32	s0, s9, s15
    2308: eec00a23     	vdiv.f32	s1, s0, s7
    230c: ee201aa5     	vmul.f32	s2, s1, s11
    2310: ed811a00     	vstr	s2, [r1]
    2314: e0821003     	add	r1, r2, r3
    2318: e2833004     	add	r3, r3, #4
    231c: ed531a01     	vldr	s3, [r3, #-4]
    2320: eeb04a47     	vmov.f32	s8, s14
    2324: eef41ac6     	vcmpe.f32	s3, s12
    2328: eef1fa10     	vmrs	APSR_nzcv, fpscr
    232c: bef01a46     	vmovlt.f32	s3, s12
    2330: eef41ae6     	vcmpe.f32	s3, s13
    2334: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2338: 8ef01a66     	vmovhi.f32	s3, s13
    233c: ee212aa1     	vmul.f32	s4, s3, s3
    2340: ee722a07     	vadd.f32	s5, s4, s14
    2344: ee024a05     	vmla.f32	s8, s4, s10
    2348: ee624aa1     	vmul.f32	s9, s5, s3
    234c: eec47a84     	vdiv.f32	s15, s9, s8
    2350: ee673aa5     	vmul.f32	s7, s15, s11
    2354: edc13a00     	vstr	s7, [r1]
    2358: e0821003     	add	r1, r2, r3
    235c: e2833004     	add	r3, r3, #4
    2360: ed130a01     	vldr	s0, [r3, #-4]
    2364: eeb04a47     	vmov.f32	s8, s14
    2368: eeb40ac6     	vcmpe.f32	s0, s12
    236c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2370: beb00a46     	vmovlt.f32	s0, s12
    2374: eeb40ae6     	vcmpe.f32	s0, s13
    2378: eef1fa10     	vmrs	APSR_nzcv, fpscr
    237c: 8eb00a66     	vmovhi.f32	s0, s13
    2380: e15c0003     	cmp	r12, r3
    2384: ee600a00     	vmul.f32	s1, s0, s0
    2388: ee301a87     	vadd.f32	s2, s1, s14
    238c: ee004a85     	vmla.f32	s8, s1, s10
    2390: ee212a00     	vmul.f32	s4, s2, s0
    2394: eec22a04     	vdiv.f32	s5, s4, s8
    2398: ee223aa5     	vmul.f32	s6, s5, s11
    239c: ed813a00     	vstr	s6, [r1]
    23a0: 0a000043     	beq	0x24b4 <tanh_approx_tilde_perform+0x24c> @ imm = #0x10c
    23a4: e0826003     	add	r6, r2, r3
    23a8: e2834004     	add	r4, r3, #4
    23ac: e0825004     	add	r5, r2, r4
    23b0: e2833010     	add	r3, r3, #16
    23b4: ed537a04     	vldr	s15, [r3, #-16]
    23b8: e1a0e006     	mov	lr, r6
    23bc: e1a04005     	mov	r4, r5
    23c0: eeb04a47     	vmov.f32	s8, s14
    23c4: eef47ac6     	vcmpe.f32	s15, s12
    23c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    23cc: bef07a46     	vmovlt.f32	s15, s12
    23d0: eef47ae6     	vcmpe.f32	s15, s13
    23d4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    23d8: 8ef07a66     	vmovhi.f32	s15, s13
    23dc: ee270aa7     	vmul.f32	s0, s15, s15
    23e0: ee704a07     	vadd.f32	s9, s0, s14
    23e4: ee004a05     	vmla.f32	s8, s0, s10
    23e8: ee640aa7     	vmul.f32	s1, s9, s15
    23ec: ee801a84     	vdiv.f32	s2, s1, s8
    23f0: eeb04a47     	vmov.f32	s8, s14
    23f4: ee611a25     	vmul.f32	s3, s2, s11
    23f8: edc61a00     	vstr	s3, [r6]
    23fc: ed132a03     	vldr	s4, [r3, #-12]
    2400: eeb42ac6     	vcmpe.f32	s4, s12
    2404: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2408: beb02a46     	vmovlt.f32	s4, s12
    240c: eeb42ae6     	vcmpe.f32	s4, s13
    2410: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2414: 8eb02a66     	vmovhi.f32	s4, s13
    2418: ee622a02     	vmul.f32	s5, s4, s4
    241c: ee323a87     	vadd.f32	s6, s5, s14
    2420: ee024a85     	vmla.f32	s8, s5, s10
    2424: ee634a02     	vmul.f32	s9, s6, s4
    2428: eec47a84     	vdiv.f32	s15, s9, s8
    242c: eeb04a47     	vmov.f32	s8, s14
    2430: ee673aa5     	vmul.f32	s7, s15, s11
    2434: edc53a00     	vstr	s7, [r5]
    2438: ed530a02     	vldr	s1, [r3, #-8]
    243c: eef40ac6     	vcmpe.f32	s1, s12
    2440: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2444: bef00a46     	vmovlt.f32	s1, s12
    2448: eef40ae6     	vcmpe.f32	s1, s13
    244c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2450: 8ef00a66     	vmovhi.f32	s1, s13
    2454: ee201aa0     	vmul.f32	s2, s1, s1
    2458: ee711a07     	vadd.f32	s3, s2, s14
    245c: ee014a05     	vmla.f32	s8, s2, s10
    2460: ee612aa0     	vmul.f32	s5, s3, s1
    2464: ee823a84     	vdiv.f32	s6, s5, s8
    2468: eeb04a47     	vmov.f32	s8, s14
    246c: ee230a25     	vmul.f32	s0, s6, s11
    2470: ed850a01     	vstr	s0, [r5, #4]
    2474: ed537a01     	vldr	s15, [r3, #-4]
    2478: eef47ac6     	vcmpe.f32	s15, s12
    247c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2480: bef07a46     	vmovlt.f32	s15, s12
    2484: eef47ae6     	vcmpe.f32	s15, s13
    2488: eef1fa10     	vmrs	APSR_nzcv, fpscr
    248c: 8ef07a66     	vmovhi.f32	s15, s13
    2490: e15c0003     	cmp	r12, r3
    2494: ee670aa7     	vmul.f32	s1, s15, s15
    2498: ee704a87     	vadd.f32	s9, s1, s14
    249c: ee004a85     	vmla.f32	s8, s1, s10
    24a0: ee241aa7     	vmul.f32	s2, s9, s15
    24a4: eec11a04     	vdiv.f32	s3, s2, s8
    24a8: ee212aa5     	vmul.f32	s4, s3, s11
    24ac: ed862a03     	vstr	s4, [r6, #12]
    24b0: 1affffbb     	bne	0x23a4 <tanh_approx_tilde_perform+0x13c> @ imm = #-0x114
    24b4: e2800014     	add	r0, r0, #20
    24b8: e8bd8070     	pop	{r4, r5, r6, pc}

