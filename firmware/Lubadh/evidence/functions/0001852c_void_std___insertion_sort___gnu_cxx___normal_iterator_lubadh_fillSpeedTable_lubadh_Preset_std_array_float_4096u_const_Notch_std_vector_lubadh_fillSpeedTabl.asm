; void std::__insertion_sort<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x1852c size 208

   1852c: e1500001     	cmp	r0, r1
   18530: 012fff1e     	bxeq	lr
   18534: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   18538: e2804010     	add	r4, r0, #16
   1853c: e1a06000     	mov	r6, r0
   18540: e24dd014     	sub	sp, sp, #20
   18544: e1a09001     	mov	r9, r1
   18548: e1510004     	cmp	r1, r4
   1854c: 0a000026     	beq	0x185ec
   18550: e1a0500d     	mov	r5, sp
   18554: ea00000c     	b	0x1858c
   18558: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1855c: e1560004     	cmp	r6, r4
   18560: e885000f     	stm	r5, {r0, r1, r2, r3}
   18564: 0a000003     	beq	0x18578
   18568: e0442006     	sub	r2, r4, r6
   1856c: e1a01006     	mov	r1, r6
   18570: e2860010     	add	r0, r6, #16
   18574: ebfff51a     	bl	0x159e4    @ imm = #-0x2b98 ; memmove
   18578: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1857c: e2844010     	add	r4, r4, #16
   18580: e1590004     	cmp	r9, r4
   18584: e886000f     	stm	r6, {r0, r1, r2, r3}
   18588: 0a000017     	beq	0x185ec
   1858c: e5947008     	ldr	r7, [r4, #0x8]
   18590: e5963008     	ldr	r3, [r6, #0x8]
   18594: e1570003     	cmp	r7, r3
   18598: baffffee     	blt	0x18558
   1859c: e514c008     	ldr	r12, [r4, #-0x8]
   185a0: e894000f     	ldm	r4, {r0, r1, r2, r3}
   185a4: e157000c     	cmp	r7, r12
   185a8: e885000f     	stm	r5, {r0, r1, r2, r3}
   185ac: aa000010     	bge	0x185f4
   185b0: e244c010     	sub	r12, r4, #16
   185b4: e28ce010     	add	lr, r12, #16
   185b8: e1a0800c     	mov	r8, r12
   185bc: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   185c0: e24cc010     	sub	r12, r12, #16
   185c4: e88e000f     	stm	lr, {r0, r1, r2, r3}
   185c8: e59c3008     	ldr	r3, [r12, #0x8]
   185cc: e1570003     	cmp	r7, r3
   185d0: bafffff7     	blt	0x185b4
   185d4: e58d7008     	str	r7, [sp, #0x8]
   185d8: e2844010     	add	r4, r4, #16
   185dc: e1590004     	cmp	r9, r4
   185e0: e895000f     	ldm	r5, {r0, r1, r2, r3}
   185e4: e888000f     	stm	r8, {r0, r1, r2, r3}
   185e8: 1affffe7     	bne	0x1858c
   185ec: e28dd014     	add	sp, sp, #20
   185f0: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   185f4: e1a08004     	mov	r8, r4
   185f8: eafffff5     	b	0x185d4
