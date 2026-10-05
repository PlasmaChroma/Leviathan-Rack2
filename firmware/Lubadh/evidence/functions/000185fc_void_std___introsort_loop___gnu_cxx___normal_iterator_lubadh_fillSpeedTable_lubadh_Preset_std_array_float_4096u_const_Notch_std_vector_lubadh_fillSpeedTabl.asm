; void std::__introsort_loop<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>)
; VA 0x185fc size 536

   185fc: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   18600: e1a05000     	mov	r5, r0
   18604: e0410000     	sub	r0, r1, r0
   18608: e24dd02c     	sub	sp, sp, #44
   1860c: e3500c01     	cmp	r0, #256
   18610: da00007d     	ble	0x1880c
   18614: e1a08002     	mov	r8, r2
   18618: e3520000     	cmp	r2, #0
   1861c: 0a000057     	beq	0x18780
   18620: e1a0a001     	mov	r10, r1
   18624: e28d4018     	add	r4, sp, #24
   18628: e1a09003     	mov	r9, r3
   1862c: e2853010     	add	r3, r5, #16
   18630: e58d3010     	str	r3, [sp, #0x10]
   18634: e2853020     	add	r3, r5, #32
   18638: e58d3014     	str	r3, [sp, #0x14]
   1863c: e04ac005     	sub	r12, r10, r5
   18640: e5952018     	ldr	r2, [r5, #0x18]
   18644: e1a0300c     	mov	r3, r12
   18648: e51a1008     	ldr	r1, [r10, #-0x8]
   1864c: e1a0cfac     	lsr	r12, r12, #31
   18650: e2488001     	sub	r8, r8, #1
   18654: e08cc243     	add	r12, r12, r3, asr #4
   18658: e1a0c0cc     	asr	r12, r12, #1
   1865c: e085c20c     	add	r12, r5, r12, lsl #4
   18660: e59c3008     	ldr	r3, [r12, #0x8]
   18664: e1520003     	cmp	r2, r3
   18668: aa000037     	bge	0x1874c
   1866c: e1530001     	cmp	r3, r1
   18670: ba000002     	blt	0x18680
   18674: e1520001     	cmp	r2, r1
   18678: aa000038     	bge	0x18760
   1867c: e24ac010     	sub	r12, r10, #16
   18680: e895000f     	ldm	r5, {r0, r1, r2, r3}
   18684: e884000f     	stm	r4, {r0, r1, r2, r3}
   18688: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1868c: e885000f     	stm	r5, {r0, r1, r2, r3}
   18690: e894000f     	ldm	r4, {r0, r1, r2, r3}
   18694: e88c000f     	stm	r12, {r0, r1, r2, r3}
   18698: e5953008     	ldr	r3, [r5, #0x8]
   1869c: e51a2008     	ldr	r2, [r10, #-0x8]
   186a0: e1a0e00a     	mov	lr, r10
   186a4: e59d7010     	ldr	r7, [sp, #0x10]
   186a8: e59db014     	ldr	r11, [sp, #0x14]
   186ac: e51b1008     	ldr	r1, [r11, #-0x8]
   186b0: e1a06007     	mov	r6, r7
   186b4: e1510003     	cmp	r1, r3
   186b8: ba000014     	blt	0x18710
   186bc: e1520003     	cmp	r2, r3
   186c0: e24ec010     	sub	r12, lr, #16
   186c4: da000005     	ble	0x186e0
   186c8: e24ee020     	sub	lr, lr, #32
   186cc: e1a0c00e     	mov	r12, lr
   186d0: e24ee010     	sub	lr, lr, #16
   186d4: e59e2018     	ldr	r2, [lr, #0x18]
   186d8: e1520003     	cmp	r2, r3
   186dc: cafffffa     	bgt	0x186cc
   186e0: e157000c     	cmp	r7, r12
   186e4: 2a00000c     	bhs	0x1871c
   186e8: e24b6010     	sub	r6, r11, #16
   186ec: e1a0e00c     	mov	lr, r12
   186f0: e896000f     	ldm	r6, {r0, r1, r2, r3}
   186f4: e884000f     	stm	r4, {r0, r1, r2, r3}
   186f8: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   186fc: e886000f     	stm	r6, {r0, r1, r2, r3}
   18700: e894000f     	ldm	r4, {r0, r1, r2, r3}
   18704: e88c000f     	stm	r12, {r0, r1, r2, r3}
   18708: e51c2008     	ldr	r2, [r12, #-0x8]
   1870c: e5953008     	ldr	r3, [r5, #0x8]
   18710: e2877010     	add	r7, r7, #16
   18714: e28bb010     	add	r11, r11, #16
   18718: eaffffe3     	b	0x186ac
   1871c: e1a00007     	mov	r0, r7
   18720: e1a0100a     	mov	r1, r10
   18724: e1a03009     	mov	r3, r9
   18728: e1a02008     	mov	r2, r8
   1872c: ebffffb2     	bl	0x185fc
   18730: e0470005     	sub	r0, r7, r5
   18734: e3500c01     	cmp	r0, #256
   18738: da000033     	ble	0x1880c
   1873c: e3580000     	cmp	r8, #0
   18740: 0a000010     	beq	0x18788
   18744: e1a0a007     	mov	r10, r7
   18748: eaffffbb     	b	0x1863c
   1874c: e1520001     	cmp	r2, r1
   18750: ba000002     	blt	0x18760
   18754: e1530001     	cmp	r3, r1
   18758: aaffffc8     	bge	0x18680
   1875c: eaffffc6     	b	0x1867c
   18760: e895000f     	ldm	r5, {r0, r1, r2, r3}
   18764: e884000f     	stm	r4, {r0, r1, r2, r3}
   18768: e59dc010     	ldr	r12, [sp, #0x10]
   1876c: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   18770: e885000f     	stm	r5, {r0, r1, r2, r3}
   18774: e894000f     	ldm	r4, {r0, r1, r2, r3}
   18778: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1877c: eaffffc5     	b	0x18698
   18780: e1a06001     	mov	r6, r1
   18784: e28d4018     	add	r4, sp, #24
   18788: e1a08240     	asr	r8, r0, #4
   1878c: e28d701c     	add	r7, sp, #28
   18790: e2489002     	sub	r9, r8, #2
   18794: e1a090c9     	asr	r9, r9, #1
   18798: ea000000     	b	0x187a0
   1879c: e2499001     	sub	r9, r9, #1
   187a0: e0853209     	add	r3, r5, r9, lsl #4
   187a4: e893000f     	ldm	r3, {r0, r1, r2, r3}
   187a8: e884000f     	stm	r4, {r0, r1, r2, r3}
   187ac: e8970007     	ldm	r7, {r0, r1, r2}
   187b0: e88d0007     	stm	sp, {r0, r1, r2}
   187b4: e59d3018     	ldr	r3, [sp, #0x18]
   187b8: e1a02008     	mov	r2, r8
   187bc: e1a01009     	mov	r1, r9
   187c0: e1a00005     	mov	r0, r5
   187c4: ebfffe8d     	bl	0x18200
   187c8: e3590000     	cmp	r9, #0
   187cc: 1afffff2     	bne	0x1879c
   187d0: e2466010     	sub	r6, r6, #16
   187d4: e0468005     	sub	r8, r6, r5
   187d8: e896000f     	ldm	r6, {r0, r1, r2, r3}
   187dc: e884000f     	stm	r4, {r0, r1, r2, r3}
   187e0: e895000f     	ldm	r5, {r0, r1, r2, r3}
   187e4: e886000f     	stm	r6, {r0, r1, r2, r3}
   187e8: e59d3018     	ldr	r3, [sp, #0x18]
   187ec: e8970007     	ldm	r7, {r0, r1, r2}
   187f0: e88d0007     	stm	sp, {r0, r1, r2}
   187f4: e1a02248     	asr	r2, r8, #4
   187f8: e3a01000     	mov	r1, #0
   187fc: e1a00005     	mov	r0, r5
   18800: ebfffe7e     	bl	0x18200
   18804: e3580010     	cmp	r8, #16
   18808: cafffff0     	bgt	0x187d0
   1880c: e28dd02c     	add	sp, sp, #44
   18810: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
