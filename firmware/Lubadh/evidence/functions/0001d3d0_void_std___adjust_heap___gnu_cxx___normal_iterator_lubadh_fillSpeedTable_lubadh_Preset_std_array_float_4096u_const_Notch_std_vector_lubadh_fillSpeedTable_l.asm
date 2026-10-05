; void std::__adjust_heap<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x1d3d0 size 396

   1d3d0: e24dd008     	sub	sp, sp, #8
   1d3d4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1d3d8: e2429001     	sub	r9, r2, #1
   1d3dc: e1a0a001     	mov	r10, r1
   1d3e0: e24dd01c     	sub	sp, sp, #28
   1d3e4: e0899fa9     	add	r9, r9, r9, lsr #31
   1d3e8: e1a04000     	mov	r4, r0
   1d3ec: e15100c9     	cmp	r1, r9, asr #1
   1d3f0: e58d3044     	str	r3, [sp, #0x44]
   1d3f4: e2023001     	and	r3, r2, #1
   1d3f8: e59db04c     	ldr	r11, [sp, #0x4c]
   1d3fc: e58d2004     	str	r2, [sp, #0x4]
   1d400: e58d3000     	str	r3, [sp]
   1d404: aa000031     	bge	0x1d4d0
   1d408: e1a090c9     	asr	r9, r9, #1
   1d40c: e1a05001     	mov	r5, r1
   1d410: e285c001     	add	r12, r5, #1
   1d414: e0848205     	add	r8, r4, r5, lsl #4
   1d418: e1a0508c     	lsl	r5, r12, #1
   1d41c: e084c28c     	add	r12, r4, r12, lsl #5
   1d420: e2457001     	sub	r7, r5, #1
   1d424: e0846207     	add	r6, r4, r7, lsl #4
   1d428: e59c3008     	ldr	r3, [r12, #0x8]
   1d42c: e5962008     	ldr	r2, [r6, #0x8]
   1d430: e1520003     	cmp	r2, r3
   1d434: ca000035     	bgt	0x1d510
   1d438: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1d43c: e1590005     	cmp	r9, r5
   1d440: e888000f     	stm	r8, {r0, r1, r2, r3}
   1d444: cafffff1     	bgt	0x1d410
   1d448: e59d3000     	ldr	r3, [sp]
   1d44c: e3530000     	cmp	r3, #0
   1d450: 0a000022     	beq	0x1d4e0
   1d454: e28d3044     	add	r3, sp, #68
   1d458: e2456001     	sub	r6, r5, #1
   1d45c: e28d7008     	add	r7, sp, #8
   1d460: e155000a     	cmp	r5, r10
   1d464: e0866fa6     	add	r6, r6, r6, lsr #31
   1d468: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1d46c: e1a060c6     	asr	r6, r6, #1
   1d470: e887000f     	stm	r7, {r0, r1, r2, r3}
   1d474: ca000005     	bgt	0x1d490
   1d478: ea00000d     	b	0x1d4b4
   1d47c: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1d480: e15a0006     	cmp	r10, r6
   1d484: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1d488: e1a06008     	mov	r6, r8
   1d48c: aa000025     	bge	0x1d528
   1d490: e084e206     	add	lr, r4, r6, lsl #4
   1d494: e2463001     	sub	r3, r6, #1
   1d498: e084c205     	add	r12, r4, r5, lsl #4
   1d49c: e1a05006     	mov	r5, r6
   1d4a0: e0833fa3     	add	r3, r3, r3, lsr #31
   1d4a4: e59e2008     	ldr	r2, [lr, #0x8]
   1d4a8: e1a080c3     	asr	r8, r3, #1
   1d4ac: e152000b     	cmp	r2, r11
   1d4b0: bafffff1     	blt	0x1d47c
   1d4b4: e58db010     	str	r11, [sp, #0x10]
   1d4b8: e897000f     	ldm	r7, {r0, r1, r2, r3}
   1d4bc: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1d4c0: e28dd01c     	add	sp, sp, #28
   1d4c4: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1d4c8: e28dd008     	add	sp, sp, #8
   1d4cc: e12fff1e     	bx	lr
   1d4d0: e3530000     	cmp	r3, #0
   1d4d4: e080c201     	add	r12, r0, r1, lsl #4
   1d4d8: 1a00001a     	bne	0x1d548
   1d4dc: e1a0500a     	mov	r5, r10
   1d4e0: e59d3004     	ldr	r3, [sp, #0x4]
   1d4e4: e2432002     	sub	r2, r3, #2
   1d4e8: e0822fa2     	add	r2, r2, r2, lsr #31
   1d4ec: e15500c2     	cmp	r5, r2, asr #1
   1d4f0: 1affffd7     	bne	0x1d454
   1d4f4: e1a05085     	lsl	r5, r5, #1
   1d4f8: e2855001     	add	r5, r5, #1
   1d4fc: e084e205     	add	lr, r4, r5, lsl #4
   1d500: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1d504: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1d508: e1a0c00e     	mov	r12, lr
   1d50c: eaffffd0     	b	0x1d454
   1d510: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1d514: e1590007     	cmp	r9, r7
   1d518: e888000f     	stm	r8, {r0, r1, r2, r3}
   1d51c: da000003     	ble	0x1d530
   1d520: e1a05007     	mov	r5, r7
   1d524: eaffffb9     	b	0x1d410
   1d528: e1a0c00e     	mov	r12, lr
   1d52c: eaffffe0     	b	0x1d4b4
   1d530: e59d3000     	ldr	r3, [sp]
   1d534: e1a0c006     	mov	r12, r6
   1d538: e1a05007     	mov	r5, r7
   1d53c: e3530000     	cmp	r3, #0
   1d540: 1affffc3     	bne	0x1d454
   1d544: eaffffe5     	b	0x1d4e0
   1d548: e28d3044     	add	r3, sp, #68
   1d54c: e28d7008     	add	r7, sp, #8
   1d550: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1d554: e887000f     	stm	r7, {r0, r1, r2, r3}
   1d558: eaffffd5     	b	0x1d4b4
