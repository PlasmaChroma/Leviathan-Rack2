; void std::__introsort_loop<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>)
; VA 0x1f204 size 536

   1f204: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1f208: e1a05000     	mov	r5, r0
   1f20c: e0410000     	sub	r0, r1, r0
   1f210: e24dd02c     	sub	sp, sp, #44
   1f214: e3500c01     	cmp	r0, #256
   1f218: da00007d     	ble	0x1f414
   1f21c: e1a08002     	mov	r8, r2
   1f220: e3520000     	cmp	r2, #0
   1f224: 0a000057     	beq	0x1f388
   1f228: e1a0a001     	mov	r10, r1
   1f22c: e28d4018     	add	r4, sp, #24
   1f230: e1a09003     	mov	r9, r3
   1f234: e2853010     	add	r3, r5, #16
   1f238: e58d3010     	str	r3, [sp, #0x10]
   1f23c: e2853020     	add	r3, r5, #32
   1f240: e58d3014     	str	r3, [sp, #0x14]
   1f244: e04ac005     	sub	r12, r10, r5
   1f248: e5952018     	ldr	r2, [r5, #0x18]
   1f24c: e1a0300c     	mov	r3, r12
   1f250: e51a1008     	ldr	r1, [r10, #-0x8]
   1f254: e1a0cfac     	lsr	r12, r12, #31
   1f258: e2488001     	sub	r8, r8, #1
   1f25c: e08cc243     	add	r12, r12, r3, asr #4
   1f260: e1a0c0cc     	asr	r12, r12, #1
   1f264: e085c20c     	add	r12, r5, r12, lsl #4
   1f268: e59c3008     	ldr	r3, [r12, #0x8]
   1f26c: e1520003     	cmp	r2, r3
   1f270: aa000037     	bge	0x1f354
   1f274: e1530001     	cmp	r3, r1
   1f278: ba000002     	blt	0x1f288
   1f27c: e1520001     	cmp	r2, r1
   1f280: aa000038     	bge	0x1f368
   1f284: e24ac010     	sub	r12, r10, #16
   1f288: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1f28c: e884000f     	stm	r4, {r0, r1, r2, r3}
   1f290: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1f294: e885000f     	stm	r5, {r0, r1, r2, r3}
   1f298: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1f29c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1f2a0: e5953008     	ldr	r3, [r5, #0x8]
   1f2a4: e51a2008     	ldr	r2, [r10, #-0x8]
   1f2a8: e1a0e00a     	mov	lr, r10
   1f2ac: e59d7010     	ldr	r7, [sp, #0x10]
   1f2b0: e59db014     	ldr	r11, [sp, #0x14]
   1f2b4: e51b1008     	ldr	r1, [r11, #-0x8]
   1f2b8: e1a06007     	mov	r6, r7
   1f2bc: e1510003     	cmp	r1, r3
   1f2c0: ba000014     	blt	0x1f318
   1f2c4: e1520003     	cmp	r2, r3
   1f2c8: e24ec010     	sub	r12, lr, #16
   1f2cc: da000005     	ble	0x1f2e8
   1f2d0: e24ee020     	sub	lr, lr, #32
   1f2d4: e1a0c00e     	mov	r12, lr
   1f2d8: e24ee010     	sub	lr, lr, #16
   1f2dc: e59e2018     	ldr	r2, [lr, #0x18]
   1f2e0: e1520003     	cmp	r2, r3
   1f2e4: cafffffa     	bgt	0x1f2d4
   1f2e8: e157000c     	cmp	r7, r12
   1f2ec: 2a00000c     	bhs	0x1f324
   1f2f0: e24b6010     	sub	r6, r11, #16
   1f2f4: e1a0e00c     	mov	lr, r12
   1f2f8: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1f2fc: e884000f     	stm	r4, {r0, r1, r2, r3}
   1f300: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1f304: e886000f     	stm	r6, {r0, r1, r2, r3}
   1f308: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1f30c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1f310: e51c2008     	ldr	r2, [r12, #-0x8]
   1f314: e5953008     	ldr	r3, [r5, #0x8]
   1f318: e2877010     	add	r7, r7, #16
   1f31c: e28bb010     	add	r11, r11, #16
   1f320: eaffffe3     	b	0x1f2b4
   1f324: e1a00007     	mov	r0, r7
   1f328: e1a0100a     	mov	r1, r10
   1f32c: e1a03009     	mov	r3, r9
   1f330: e1a02008     	mov	r2, r8
   1f334: ebffffb2     	bl	0x1f204
   1f338: e0470005     	sub	r0, r7, r5
   1f33c: e3500c01     	cmp	r0, #256
   1f340: da000033     	ble	0x1f414
   1f344: e3580000     	cmp	r8, #0
   1f348: 0a000010     	beq	0x1f390
   1f34c: e1a0a007     	mov	r10, r7
   1f350: eaffffbb     	b	0x1f244
   1f354: e1520001     	cmp	r2, r1
   1f358: ba000002     	blt	0x1f368
   1f35c: e1530001     	cmp	r3, r1
   1f360: aaffffc8     	bge	0x1f288
   1f364: eaffffc6     	b	0x1f284
   1f368: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1f36c: e884000f     	stm	r4, {r0, r1, r2, r3}
   1f370: e59dc010     	ldr	r12, [sp, #0x10]
   1f374: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1f378: e885000f     	stm	r5, {r0, r1, r2, r3}
   1f37c: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1f380: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1f384: eaffffc5     	b	0x1f2a0
   1f388: e1a06001     	mov	r6, r1
   1f38c: e28d4018     	add	r4, sp, #24
   1f390: e1a08240     	asr	r8, r0, #4
   1f394: e28d701c     	add	r7, sp, #28
   1f398: e2489002     	sub	r9, r8, #2
   1f39c: e1a090c9     	asr	r9, r9, #1
   1f3a0: ea000000     	b	0x1f3a8
   1f3a4: e2499001     	sub	r9, r9, #1
   1f3a8: e0853209     	add	r3, r5, r9, lsl #4
   1f3ac: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1f3b0: e884000f     	stm	r4, {r0, r1, r2, r3}
   1f3b4: e8970007     	ldm	r7, {r0, r1, r2}
   1f3b8: e88d0007     	stm	sp, {r0, r1, r2}
   1f3bc: e59d3018     	ldr	r3, [sp, #0x18]
   1f3c0: e1a02008     	mov	r2, r8
   1f3c4: e1a01009     	mov	r1, r9
   1f3c8: e1a00005     	mov	r0, r5
   1f3cc: ebfffeb9     	bl	0x1eeb8
   1f3d0: e3590000     	cmp	r9, #0
   1f3d4: 1afffff2     	bne	0x1f3a4
   1f3d8: e2466010     	sub	r6, r6, #16
   1f3dc: e0468005     	sub	r8, r6, r5
   1f3e0: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1f3e4: e884000f     	stm	r4, {r0, r1, r2, r3}
   1f3e8: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1f3ec: e886000f     	stm	r6, {r0, r1, r2, r3}
   1f3f0: e59d3018     	ldr	r3, [sp, #0x18]
   1f3f4: e8970007     	ldm	r7, {r0, r1, r2}
   1f3f8: e88d0007     	stm	sp, {r0, r1, r2}
   1f3fc: e1a02248     	asr	r2, r8, #4
   1f400: e3a01000     	mov	r1, #0
   1f404: e1a00005     	mov	r0, r5
   1f408: ebfffeaa     	bl	0x1eeb8
   1f40c: e3580010     	cmp	r8, #16
   1f410: cafffff0     	bgt	0x1f3d8
   1f414: e28dd02c     	add	sp, sp, #44
   1f418: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
