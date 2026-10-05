; void std::__introsort_loop<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>)
; VA 0x1a284 size 536

   1a284: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1a288: e1a05000     	mov	r5, r0
   1a28c: e0410000     	sub	r0, r1, r0
   1a290: e24dd02c     	sub	sp, sp, #44
   1a294: e3500c01     	cmp	r0, #256
   1a298: da00007d     	ble	0x1a494
   1a29c: e1a08002     	mov	r8, r2
   1a2a0: e3520000     	cmp	r2, #0
   1a2a4: 0a000057     	beq	0x1a408
   1a2a8: e1a0a001     	mov	r10, r1
   1a2ac: e28d4018     	add	r4, sp, #24
   1a2b0: e1a09003     	mov	r9, r3
   1a2b4: e2853010     	add	r3, r5, #16
   1a2b8: e58d3010     	str	r3, [sp, #0x10]
   1a2bc: e2853020     	add	r3, r5, #32
   1a2c0: e58d3014     	str	r3, [sp, #0x14]
   1a2c4: e04ac005     	sub	r12, r10, r5
   1a2c8: e5952018     	ldr	r2, [r5, #0x18]
   1a2cc: e1a0300c     	mov	r3, r12
   1a2d0: e51a1008     	ldr	r1, [r10, #-0x8]
   1a2d4: e1a0cfac     	lsr	r12, r12, #31
   1a2d8: e2488001     	sub	r8, r8, #1
   1a2dc: e08cc243     	add	r12, r12, r3, asr #4
   1a2e0: e1a0c0cc     	asr	r12, r12, #1
   1a2e4: e085c20c     	add	r12, r5, r12, lsl #4
   1a2e8: e59c3008     	ldr	r3, [r12, #0x8]
   1a2ec: e1520003     	cmp	r2, r3
   1a2f0: aa000037     	bge	0x1a3d4
   1a2f4: e1530001     	cmp	r3, r1
   1a2f8: ba000002     	blt	0x1a308
   1a2fc: e1520001     	cmp	r2, r1
   1a300: aa000038     	bge	0x1a3e8
   1a304: e24ac010     	sub	r12, r10, #16
   1a308: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1a30c: e884000f     	stm	r4, {r0, r1, r2, r3}
   1a310: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1a314: e885000f     	stm	r5, {r0, r1, r2, r3}
   1a318: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1a31c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1a320: e5953008     	ldr	r3, [r5, #0x8]
   1a324: e51a2008     	ldr	r2, [r10, #-0x8]
   1a328: e1a0e00a     	mov	lr, r10
   1a32c: e59d7010     	ldr	r7, [sp, #0x10]
   1a330: e59db014     	ldr	r11, [sp, #0x14]
   1a334: e51b1008     	ldr	r1, [r11, #-0x8]
   1a338: e1a06007     	mov	r6, r7
   1a33c: e1510003     	cmp	r1, r3
   1a340: ba000014     	blt	0x1a398
   1a344: e1520003     	cmp	r2, r3
   1a348: e24ec010     	sub	r12, lr, #16
   1a34c: da000005     	ble	0x1a368
   1a350: e24ee020     	sub	lr, lr, #32
   1a354: e1a0c00e     	mov	r12, lr
   1a358: e24ee010     	sub	lr, lr, #16
   1a35c: e59e2018     	ldr	r2, [lr, #0x18]
   1a360: e1520003     	cmp	r2, r3
   1a364: cafffffa     	bgt	0x1a354
   1a368: e157000c     	cmp	r7, r12
   1a36c: 2a00000c     	bhs	0x1a3a4
   1a370: e24b6010     	sub	r6, r11, #16
   1a374: e1a0e00c     	mov	lr, r12
   1a378: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1a37c: e884000f     	stm	r4, {r0, r1, r2, r3}
   1a380: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1a384: e886000f     	stm	r6, {r0, r1, r2, r3}
   1a388: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1a38c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1a390: e51c2008     	ldr	r2, [r12, #-0x8]
   1a394: e5953008     	ldr	r3, [r5, #0x8]
   1a398: e2877010     	add	r7, r7, #16
   1a39c: e28bb010     	add	r11, r11, #16
   1a3a0: eaffffe3     	b	0x1a334
   1a3a4: e1a00007     	mov	r0, r7
   1a3a8: e1a0100a     	mov	r1, r10
   1a3ac: e1a03009     	mov	r3, r9
   1a3b0: e1a02008     	mov	r2, r8
   1a3b4: ebffffb2     	bl	0x1a284
   1a3b8: e0470005     	sub	r0, r7, r5
   1a3bc: e3500c01     	cmp	r0, #256
   1a3c0: da000033     	ble	0x1a494
   1a3c4: e3580000     	cmp	r8, #0
   1a3c8: 0a000010     	beq	0x1a410
   1a3cc: e1a0a007     	mov	r10, r7
   1a3d0: eaffffbb     	b	0x1a2c4
   1a3d4: e1520001     	cmp	r2, r1
   1a3d8: ba000002     	blt	0x1a3e8
   1a3dc: e1530001     	cmp	r3, r1
   1a3e0: aaffffc8     	bge	0x1a308
   1a3e4: eaffffc6     	b	0x1a304
   1a3e8: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1a3ec: e884000f     	stm	r4, {r0, r1, r2, r3}
   1a3f0: e59dc010     	ldr	r12, [sp, #0x10]
   1a3f4: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1a3f8: e885000f     	stm	r5, {r0, r1, r2, r3}
   1a3fc: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1a400: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1a404: eaffffc5     	b	0x1a320
   1a408: e1a06001     	mov	r6, r1
   1a40c: e28d4018     	add	r4, sp, #24
   1a410: e1a08240     	asr	r8, r0, #4
   1a414: e28d701c     	add	r7, sp, #28
   1a418: e2489002     	sub	r9, r8, #2
   1a41c: e1a090c9     	asr	r9, r9, #1
   1a420: ea000000     	b	0x1a428
   1a424: e2499001     	sub	r9, r9, #1
   1a428: e0853209     	add	r3, r5, r9, lsl #4
   1a42c: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1a430: e884000f     	stm	r4, {r0, r1, r2, r3}
   1a434: e8970007     	ldm	r7, {r0, r1, r2}
   1a438: e88d0007     	stm	sp, {r0, r1, r2}
   1a43c: e59d3018     	ldr	r3, [sp, #0x18]
   1a440: e1a02008     	mov	r2, r8
   1a444: e1a01009     	mov	r1, r9
   1a448: e1a00005     	mov	r0, r5
   1a44c: ebfffe8d     	bl	0x19e88
   1a450: e3590000     	cmp	r9, #0
   1a454: 1afffff2     	bne	0x1a424
   1a458: e2466010     	sub	r6, r6, #16
   1a45c: e0468005     	sub	r8, r6, r5
   1a460: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1a464: e884000f     	stm	r4, {r0, r1, r2, r3}
   1a468: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1a46c: e886000f     	stm	r6, {r0, r1, r2, r3}
   1a470: e59d3018     	ldr	r3, [sp, #0x18]
   1a474: e8970007     	ldm	r7, {r0, r1, r2}
   1a478: e88d0007     	stm	sp, {r0, r1, r2}
   1a47c: e1a02248     	asr	r2, r8, #4
   1a480: e3a01000     	mov	r1, #0
   1a484: e1a00005     	mov	r0, r5
   1a488: ebfffe7e     	bl	0x19e88
   1a48c: e3580010     	cmp	r8, #16
   1a490: cafffff0     	bgt	0x1a458
   1a494: e28dd02c     	add	sp, sp, #44
   1a498: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
