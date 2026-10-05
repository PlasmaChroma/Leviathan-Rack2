; void std::__adjust_heap<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x18200 size 396

   18200: e24dd008     	sub	sp, sp, #8
   18204: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   18208: e2429001     	sub	r9, r2, #1
   1820c: e1a0a001     	mov	r10, r1
   18210: e24dd01c     	sub	sp, sp, #28
   18214: e0899fa9     	add	r9, r9, r9, lsr #31
   18218: e1a04000     	mov	r4, r0
   1821c: e15100c9     	cmp	r1, r9, asr #1
   18220: e58d3044     	str	r3, [sp, #0x44]
   18224: e2023001     	and	r3, r2, #1
   18228: e59db04c     	ldr	r11, [sp, #0x4c]
   1822c: e58d2004     	str	r2, [sp, #0x4]
   18230: e58d3000     	str	r3, [sp]
   18234: aa000031     	bge	0x18300
   18238: e1a090c9     	asr	r9, r9, #1
   1823c: e1a05001     	mov	r5, r1
   18240: e285c001     	add	r12, r5, #1
   18244: e0848205     	add	r8, r4, r5, lsl #4
   18248: e1a0508c     	lsl	r5, r12, #1
   1824c: e084c28c     	add	r12, r4, r12, lsl #5
   18250: e2457001     	sub	r7, r5, #1
   18254: e0846207     	add	r6, r4, r7, lsl #4
   18258: e59c3008     	ldr	r3, [r12, #0x8]
   1825c: e5962008     	ldr	r2, [r6, #0x8]
   18260: e1520003     	cmp	r2, r3
   18264: ca000035     	bgt	0x18340
   18268: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1826c: e1590005     	cmp	r9, r5
   18270: e888000f     	stm	r8, {r0, r1, r2, r3}
   18274: cafffff1     	bgt	0x18240
   18278: e59d3000     	ldr	r3, [sp]
   1827c: e3530000     	cmp	r3, #0
   18280: 0a000022     	beq	0x18310
   18284: e28d3044     	add	r3, sp, #68
   18288: e2456001     	sub	r6, r5, #1
   1828c: e28d7008     	add	r7, sp, #8
   18290: e155000a     	cmp	r5, r10
   18294: e0866fa6     	add	r6, r6, r6, lsr #31
   18298: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1829c: e1a060c6     	asr	r6, r6, #1
   182a0: e887000f     	stm	r7, {r0, r1, r2, r3}
   182a4: ca000005     	bgt	0x182c0
   182a8: ea00000d     	b	0x182e4
   182ac: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   182b0: e15a0006     	cmp	r10, r6
   182b4: e88c000f     	stm	r12, {r0, r1, r2, r3}
   182b8: e1a06008     	mov	r6, r8
   182bc: aa000025     	bge	0x18358
   182c0: e084e206     	add	lr, r4, r6, lsl #4
   182c4: e2463001     	sub	r3, r6, #1
   182c8: e084c205     	add	r12, r4, r5, lsl #4
   182cc: e1a05006     	mov	r5, r6
   182d0: e0833fa3     	add	r3, r3, r3, lsr #31
   182d4: e59e2008     	ldr	r2, [lr, #0x8]
   182d8: e1a080c3     	asr	r8, r3, #1
   182dc: e152000b     	cmp	r2, r11
   182e0: bafffff1     	blt	0x182ac
   182e4: e58db010     	str	r11, [sp, #0x10]
   182e8: e897000f     	ldm	r7, {r0, r1, r2, r3}
   182ec: e88c000f     	stm	r12, {r0, r1, r2, r3}
   182f0: e28dd01c     	add	sp, sp, #28
   182f4: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   182f8: e28dd008     	add	sp, sp, #8
   182fc: e12fff1e     	bx	lr
   18300: e3530000     	cmp	r3, #0
   18304: e080c201     	add	r12, r0, r1, lsl #4
   18308: 1a00001a     	bne	0x18378
   1830c: e1a0500a     	mov	r5, r10
   18310: e59d3004     	ldr	r3, [sp, #0x4]
   18314: e2432002     	sub	r2, r3, #2
   18318: e0822fa2     	add	r2, r2, r2, lsr #31
   1831c: e15500c2     	cmp	r5, r2, asr #1
   18320: 1affffd7     	bne	0x18284
   18324: e1a05085     	lsl	r5, r5, #1
   18328: e2855001     	add	r5, r5, #1
   1832c: e084e205     	add	lr, r4, r5, lsl #4
   18330: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   18334: e88c000f     	stm	r12, {r0, r1, r2, r3}
   18338: e1a0c00e     	mov	r12, lr
   1833c: eaffffd0     	b	0x18284
   18340: e896000f     	ldm	r6, {r0, r1, r2, r3}
   18344: e1590007     	cmp	r9, r7
   18348: e888000f     	stm	r8, {r0, r1, r2, r3}
   1834c: da000003     	ble	0x18360
   18350: e1a05007     	mov	r5, r7
   18354: eaffffb9     	b	0x18240
   18358: e1a0c00e     	mov	r12, lr
   1835c: eaffffe0     	b	0x182e4
   18360: e59d3000     	ldr	r3, [sp]
   18364: e1a0c006     	mov	r12, r6
   18368: e1a05007     	mov	r5, r7
   1836c: e3530000     	cmp	r3, #0
   18370: 1affffc3     	bne	0x18284
   18374: eaffffe5     	b	0x18310
   18378: e28d3044     	add	r3, sp, #68
   1837c: e28d7008     	add	r7, sp, #8
   18380: e893000f     	ldm	r3, {r0, r1, r2, r3}
   18384: e887000f     	stm	r7, {r0, r1, r2, r3}
   18388: eaffffd5     	b	0x182e4
