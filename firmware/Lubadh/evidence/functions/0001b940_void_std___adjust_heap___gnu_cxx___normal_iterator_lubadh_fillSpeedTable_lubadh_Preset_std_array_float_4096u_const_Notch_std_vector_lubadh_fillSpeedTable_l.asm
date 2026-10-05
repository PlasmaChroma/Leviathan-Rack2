; void std::__adjust_heap<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x1b940 size 396

   1b940: e24dd008     	sub	sp, sp, #8
   1b944: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1b948: e2429001     	sub	r9, r2, #1
   1b94c: e1a0a001     	mov	r10, r1
   1b950: e24dd01c     	sub	sp, sp, #28
   1b954: e0899fa9     	add	r9, r9, r9, lsr #31
   1b958: e1a04000     	mov	r4, r0
   1b95c: e15100c9     	cmp	r1, r9, asr #1
   1b960: e58d3044     	str	r3, [sp, #0x44]
   1b964: e2023001     	and	r3, r2, #1
   1b968: e59db04c     	ldr	r11, [sp, #0x4c]
   1b96c: e58d2004     	str	r2, [sp, #0x4]
   1b970: e58d3000     	str	r3, [sp]
   1b974: aa000031     	bge	0x1ba40
   1b978: e1a090c9     	asr	r9, r9, #1
   1b97c: e1a05001     	mov	r5, r1
   1b980: e285c001     	add	r12, r5, #1
   1b984: e0848205     	add	r8, r4, r5, lsl #4
   1b988: e1a0508c     	lsl	r5, r12, #1
   1b98c: e084c28c     	add	r12, r4, r12, lsl #5
   1b990: e2457001     	sub	r7, r5, #1
   1b994: e0846207     	add	r6, r4, r7, lsl #4
   1b998: e59c3008     	ldr	r3, [r12, #0x8]
   1b99c: e5962008     	ldr	r2, [r6, #0x8]
   1b9a0: e1520003     	cmp	r2, r3
   1b9a4: ca000035     	bgt	0x1ba80
   1b9a8: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1b9ac: e1590005     	cmp	r9, r5
   1b9b0: e888000f     	stm	r8, {r0, r1, r2, r3}
   1b9b4: cafffff1     	bgt	0x1b980
   1b9b8: e59d3000     	ldr	r3, [sp]
   1b9bc: e3530000     	cmp	r3, #0
   1b9c0: 0a000022     	beq	0x1ba50
   1b9c4: e28d3044     	add	r3, sp, #68
   1b9c8: e2456001     	sub	r6, r5, #1
   1b9cc: e28d7008     	add	r7, sp, #8
   1b9d0: e155000a     	cmp	r5, r10
   1b9d4: e0866fa6     	add	r6, r6, r6, lsr #31
   1b9d8: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1b9dc: e1a060c6     	asr	r6, r6, #1
   1b9e0: e887000f     	stm	r7, {r0, r1, r2, r3}
   1b9e4: ca000005     	bgt	0x1ba00
   1b9e8: ea00000d     	b	0x1ba24
   1b9ec: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1b9f0: e15a0006     	cmp	r10, r6
   1b9f4: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1b9f8: e1a06008     	mov	r6, r8
   1b9fc: aa000025     	bge	0x1ba98
   1ba00: e084e206     	add	lr, r4, r6, lsl #4
   1ba04: e2463001     	sub	r3, r6, #1
   1ba08: e084c205     	add	r12, r4, r5, lsl #4
   1ba0c: e1a05006     	mov	r5, r6
   1ba10: e0833fa3     	add	r3, r3, r3, lsr #31
   1ba14: e59e2008     	ldr	r2, [lr, #0x8]
   1ba18: e1a080c3     	asr	r8, r3, #1
   1ba1c: e152000b     	cmp	r2, r11
   1ba20: bafffff1     	blt	0x1b9ec
   1ba24: e58db010     	str	r11, [sp, #0x10]
   1ba28: e897000f     	ldm	r7, {r0, r1, r2, r3}
   1ba2c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1ba30: e28dd01c     	add	sp, sp, #28
   1ba34: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1ba38: e28dd008     	add	sp, sp, #8
   1ba3c: e12fff1e     	bx	lr
   1ba40: e3530000     	cmp	r3, #0
   1ba44: e080c201     	add	r12, r0, r1, lsl #4
   1ba48: 1a00001a     	bne	0x1bab8
   1ba4c: e1a0500a     	mov	r5, r10
   1ba50: e59d3004     	ldr	r3, [sp, #0x4]
   1ba54: e2432002     	sub	r2, r3, #2
   1ba58: e0822fa2     	add	r2, r2, r2, lsr #31
   1ba5c: e15500c2     	cmp	r5, r2, asr #1
   1ba60: 1affffd7     	bne	0x1b9c4
   1ba64: e1a05085     	lsl	r5, r5, #1
   1ba68: e2855001     	add	r5, r5, #1
   1ba6c: e084e205     	add	lr, r4, r5, lsl #4
   1ba70: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1ba74: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1ba78: e1a0c00e     	mov	r12, lr
   1ba7c: eaffffd0     	b	0x1b9c4
   1ba80: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1ba84: e1590007     	cmp	r9, r7
   1ba88: e888000f     	stm	r8, {r0, r1, r2, r3}
   1ba8c: da000003     	ble	0x1baa0
   1ba90: e1a05007     	mov	r5, r7
   1ba94: eaffffb9     	b	0x1b980
   1ba98: e1a0c00e     	mov	r12, lr
   1ba9c: eaffffe0     	b	0x1ba24
   1baa0: e59d3000     	ldr	r3, [sp]
   1baa4: e1a0c006     	mov	r12, r6
   1baa8: e1a05007     	mov	r5, r7
   1baac: e3530000     	cmp	r3, #0
   1bab0: 1affffc3     	bne	0x1b9c4
   1bab4: eaffffe5     	b	0x1ba50
   1bab8: e28d3044     	add	r3, sp, #68
   1babc: e28d7008     	add	r7, sp, #8
   1bac0: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1bac4: e887000f     	stm	r7, {r0, r1, r2, r3}
   1bac8: eaffffd5     	b	0x1ba24
