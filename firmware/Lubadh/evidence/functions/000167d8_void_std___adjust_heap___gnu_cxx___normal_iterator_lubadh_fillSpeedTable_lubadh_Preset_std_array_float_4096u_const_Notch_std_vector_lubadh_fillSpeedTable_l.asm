; void std::__adjust_heap<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x167d8 size 396

   167d8: e24dd008     	sub	sp, sp, #8
   167dc: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   167e0: e2429001     	sub	r9, r2, #1
   167e4: e1a0a001     	mov	r10, r1
   167e8: e24dd01c     	sub	sp, sp, #28
   167ec: e0899fa9     	add	r9, r9, r9, lsr #31
   167f0: e1a04000     	mov	r4, r0
   167f4: e15100c9     	cmp	r1, r9, asr #1
   167f8: e58d3044     	str	r3, [sp, #0x44]
   167fc: e2023001     	and	r3, r2, #1
   16800: e59db04c     	ldr	r11, [sp, #0x4c]
   16804: e58d2004     	str	r2, [sp, #0x4]
   16808: e58d3000     	str	r3, [sp]
   1680c: aa000031     	bge	0x168d8
   16810: e1a090c9     	asr	r9, r9, #1
   16814: e1a05001     	mov	r5, r1
   16818: e285c001     	add	r12, r5, #1
   1681c: e0848205     	add	r8, r4, r5, lsl #4
   16820: e1a0508c     	lsl	r5, r12, #1
   16824: e084c28c     	add	r12, r4, r12, lsl #5
   16828: e2457001     	sub	r7, r5, #1
   1682c: e0846207     	add	r6, r4, r7, lsl #4
   16830: e59c3008     	ldr	r3, [r12, #0x8]
   16834: e5962008     	ldr	r2, [r6, #0x8]
   16838: e1520003     	cmp	r2, r3
   1683c: ca000035     	bgt	0x16918
   16840: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   16844: e1590005     	cmp	r9, r5
   16848: e888000f     	stm	r8, {r0, r1, r2, r3}
   1684c: cafffff1     	bgt	0x16818
   16850: e59d3000     	ldr	r3, [sp]
   16854: e3530000     	cmp	r3, #0
   16858: 0a000022     	beq	0x168e8
   1685c: e28d3044     	add	r3, sp, #68
   16860: e2456001     	sub	r6, r5, #1
   16864: e28d7008     	add	r7, sp, #8
   16868: e155000a     	cmp	r5, r10
   1686c: e0866fa6     	add	r6, r6, r6, lsr #31
   16870: e893000f     	ldm	r3, {r0, r1, r2, r3}
   16874: e1a060c6     	asr	r6, r6, #1
   16878: e887000f     	stm	r7, {r0, r1, r2, r3}
   1687c: ca000005     	bgt	0x16898
   16880: ea00000d     	b	0x168bc
   16884: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   16888: e15a0006     	cmp	r10, r6
   1688c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   16890: e1a06008     	mov	r6, r8
   16894: aa000025     	bge	0x16930
   16898: e084e206     	add	lr, r4, r6, lsl #4
   1689c: e2463001     	sub	r3, r6, #1
   168a0: e084c205     	add	r12, r4, r5, lsl #4
   168a4: e1a05006     	mov	r5, r6
   168a8: e0833fa3     	add	r3, r3, r3, lsr #31
   168ac: e59e2008     	ldr	r2, [lr, #0x8]
   168b0: e1a080c3     	asr	r8, r3, #1
   168b4: e152000b     	cmp	r2, r11
   168b8: bafffff1     	blt	0x16884
   168bc: e58db010     	str	r11, [sp, #0x10]
   168c0: e897000f     	ldm	r7, {r0, r1, r2, r3}
   168c4: e88c000f     	stm	r12, {r0, r1, r2, r3}
   168c8: e28dd01c     	add	sp, sp, #28
   168cc: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   168d0: e28dd008     	add	sp, sp, #8
   168d4: e12fff1e     	bx	lr
   168d8: e3530000     	cmp	r3, #0
   168dc: e080c201     	add	r12, r0, r1, lsl #4
   168e0: 1a00001a     	bne	0x16950
   168e4: e1a0500a     	mov	r5, r10
   168e8: e59d3004     	ldr	r3, [sp, #0x4]
   168ec: e2432002     	sub	r2, r3, #2
   168f0: e0822fa2     	add	r2, r2, r2, lsr #31
   168f4: e15500c2     	cmp	r5, r2, asr #1
   168f8: 1affffd7     	bne	0x1685c
   168fc: e1a05085     	lsl	r5, r5, #1
   16900: e2855001     	add	r5, r5, #1
   16904: e084e205     	add	lr, r4, r5, lsl #4
   16908: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1690c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   16910: e1a0c00e     	mov	r12, lr
   16914: eaffffd0     	b	0x1685c
   16918: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1691c: e1590007     	cmp	r9, r7
   16920: e888000f     	stm	r8, {r0, r1, r2, r3}
   16924: da000003     	ble	0x16938
   16928: e1a05007     	mov	r5, r7
   1692c: eaffffb9     	b	0x16818
   16930: e1a0c00e     	mov	r12, lr
   16934: eaffffe0     	b	0x168bc
   16938: e59d3000     	ldr	r3, [sp]
   1693c: e1a0c006     	mov	r12, r6
   16940: e1a05007     	mov	r5, r7
   16944: e3530000     	cmp	r3, #0
   16948: 1affffc3     	bne	0x1685c
   1694c: eaffffe5     	b	0x168e8
   16950: e28d3044     	add	r3, sp, #68
   16954: e28d7008     	add	r7, sp, #8
   16958: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1695c: e887000f     	stm	r7, {r0, r1, r2, r3}
   16960: eaffffd5     	b	0x168bc
