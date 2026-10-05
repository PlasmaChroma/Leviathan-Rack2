; void std::__introsort_loop<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>)
; VA 0x1d7cc size 536

   1d7cc: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1d7d0: e1a05000     	mov	r5, r0
   1d7d4: e0410000     	sub	r0, r1, r0
   1d7d8: e24dd02c     	sub	sp, sp, #44
   1d7dc: e3500c01     	cmp	r0, #256
   1d7e0: da00007d     	ble	0x1d9dc
   1d7e4: e1a08002     	mov	r8, r2
   1d7e8: e3520000     	cmp	r2, #0
   1d7ec: 0a000057     	beq	0x1d950
   1d7f0: e1a0a001     	mov	r10, r1
   1d7f4: e28d4018     	add	r4, sp, #24
   1d7f8: e1a09003     	mov	r9, r3
   1d7fc: e2853010     	add	r3, r5, #16
   1d800: e58d3010     	str	r3, [sp, #0x10]
   1d804: e2853020     	add	r3, r5, #32
   1d808: e58d3014     	str	r3, [sp, #0x14]
   1d80c: e04ac005     	sub	r12, r10, r5
   1d810: e5952018     	ldr	r2, [r5, #0x18]
   1d814: e1a0300c     	mov	r3, r12
   1d818: e51a1008     	ldr	r1, [r10, #-0x8]
   1d81c: e1a0cfac     	lsr	r12, r12, #31
   1d820: e2488001     	sub	r8, r8, #1
   1d824: e08cc243     	add	r12, r12, r3, asr #4
   1d828: e1a0c0cc     	asr	r12, r12, #1
   1d82c: e085c20c     	add	r12, r5, r12, lsl #4
   1d830: e59c3008     	ldr	r3, [r12, #0x8]
   1d834: e1520003     	cmp	r2, r3
   1d838: aa000037     	bge	0x1d91c
   1d83c: e1530001     	cmp	r3, r1
   1d840: ba000002     	blt	0x1d850
   1d844: e1520001     	cmp	r2, r1
   1d848: aa000038     	bge	0x1d930
   1d84c: e24ac010     	sub	r12, r10, #16
   1d850: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1d854: e884000f     	stm	r4, {r0, r1, r2, r3}
   1d858: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1d85c: e885000f     	stm	r5, {r0, r1, r2, r3}
   1d860: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1d864: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1d868: e5953008     	ldr	r3, [r5, #0x8]
   1d86c: e51a2008     	ldr	r2, [r10, #-0x8]
   1d870: e1a0e00a     	mov	lr, r10
   1d874: e59d7010     	ldr	r7, [sp, #0x10]
   1d878: e59db014     	ldr	r11, [sp, #0x14]
   1d87c: e51b1008     	ldr	r1, [r11, #-0x8]
   1d880: e1a06007     	mov	r6, r7
   1d884: e1510003     	cmp	r1, r3
   1d888: ba000014     	blt	0x1d8e0
   1d88c: e1520003     	cmp	r2, r3
   1d890: e24ec010     	sub	r12, lr, #16
   1d894: da000005     	ble	0x1d8b0
   1d898: e24ee020     	sub	lr, lr, #32
   1d89c: e1a0c00e     	mov	r12, lr
   1d8a0: e24ee010     	sub	lr, lr, #16
   1d8a4: e59e2018     	ldr	r2, [lr, #0x18]
   1d8a8: e1520003     	cmp	r2, r3
   1d8ac: cafffffa     	bgt	0x1d89c
   1d8b0: e157000c     	cmp	r7, r12
   1d8b4: 2a00000c     	bhs	0x1d8ec
   1d8b8: e24b6010     	sub	r6, r11, #16
   1d8bc: e1a0e00c     	mov	lr, r12
   1d8c0: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1d8c4: e884000f     	stm	r4, {r0, r1, r2, r3}
   1d8c8: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1d8cc: e886000f     	stm	r6, {r0, r1, r2, r3}
   1d8d0: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1d8d4: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1d8d8: e51c2008     	ldr	r2, [r12, #-0x8]
   1d8dc: e5953008     	ldr	r3, [r5, #0x8]
   1d8e0: e2877010     	add	r7, r7, #16
   1d8e4: e28bb010     	add	r11, r11, #16
   1d8e8: eaffffe3     	b	0x1d87c
   1d8ec: e1a00007     	mov	r0, r7
   1d8f0: e1a0100a     	mov	r1, r10
   1d8f4: e1a03009     	mov	r3, r9
   1d8f8: e1a02008     	mov	r2, r8
   1d8fc: ebffffb2     	bl	0x1d7cc
   1d900: e0470005     	sub	r0, r7, r5
   1d904: e3500c01     	cmp	r0, #256
   1d908: da000033     	ble	0x1d9dc
   1d90c: e3580000     	cmp	r8, #0
   1d910: 0a000010     	beq	0x1d958
   1d914: e1a0a007     	mov	r10, r7
   1d918: eaffffbb     	b	0x1d80c
   1d91c: e1520001     	cmp	r2, r1
   1d920: ba000002     	blt	0x1d930
   1d924: e1530001     	cmp	r3, r1
   1d928: aaffffc8     	bge	0x1d850
   1d92c: eaffffc6     	b	0x1d84c
   1d930: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1d934: e884000f     	stm	r4, {r0, r1, r2, r3}
   1d938: e59dc010     	ldr	r12, [sp, #0x10]
   1d93c: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1d940: e885000f     	stm	r5, {r0, r1, r2, r3}
   1d944: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1d948: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1d94c: eaffffc5     	b	0x1d868
   1d950: e1a06001     	mov	r6, r1
   1d954: e28d4018     	add	r4, sp, #24
   1d958: e1a08240     	asr	r8, r0, #4
   1d95c: e28d701c     	add	r7, sp, #28
   1d960: e2489002     	sub	r9, r8, #2
   1d964: e1a090c9     	asr	r9, r9, #1
   1d968: ea000000     	b	0x1d970
   1d96c: e2499001     	sub	r9, r9, #1
   1d970: e0853209     	add	r3, r5, r9, lsl #4
   1d974: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1d978: e884000f     	stm	r4, {r0, r1, r2, r3}
   1d97c: e8970007     	ldm	r7, {r0, r1, r2}
   1d980: e88d0007     	stm	sp, {r0, r1, r2}
   1d984: e59d3018     	ldr	r3, [sp, #0x18]
   1d988: e1a02008     	mov	r2, r8
   1d98c: e1a01009     	mov	r1, r9
   1d990: e1a00005     	mov	r0, r5
   1d994: ebfffe8d     	bl	0x1d3d0
   1d998: e3590000     	cmp	r9, #0
   1d99c: 1afffff2     	bne	0x1d96c
   1d9a0: e2466010     	sub	r6, r6, #16
   1d9a4: e0468005     	sub	r8, r6, r5
   1d9a8: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1d9ac: e884000f     	stm	r4, {r0, r1, r2, r3}
   1d9b0: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1d9b4: e886000f     	stm	r6, {r0, r1, r2, r3}
   1d9b8: e59d3018     	ldr	r3, [sp, #0x18]
   1d9bc: e8970007     	ldm	r7, {r0, r1, r2}
   1d9c0: e88d0007     	stm	sp, {r0, r1, r2}
   1d9c4: e1a02248     	asr	r2, r8, #4
   1d9c8: e3a01000     	mov	r1, #0
   1d9cc: e1a00005     	mov	r0, r5
   1d9d0: ebfffe7e     	bl	0x1d3d0
   1d9d4: e3580010     	cmp	r8, #16
   1d9d8: cafffff0     	bgt	0x1d9a0
   1d9dc: e28dd02c     	add	sp, sp, #44
   1d9e0: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
