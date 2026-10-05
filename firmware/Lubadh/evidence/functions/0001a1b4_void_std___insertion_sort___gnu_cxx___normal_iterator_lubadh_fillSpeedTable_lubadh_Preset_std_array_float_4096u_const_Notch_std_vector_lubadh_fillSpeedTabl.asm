; void std::__insertion_sort<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x1a1b4 size 208

   1a1b4: e1500001     	cmp	r0, r1
   1a1b8: 012fff1e     	bxeq	lr
   1a1bc: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   1a1c0: e2804010     	add	r4, r0, #16
   1a1c4: e1a06000     	mov	r6, r0
   1a1c8: e24dd014     	sub	sp, sp, #20
   1a1cc: e1a09001     	mov	r9, r1
   1a1d0: e1510004     	cmp	r1, r4
   1a1d4: 0a000026     	beq	0x1a274
   1a1d8: e1a0500d     	mov	r5, sp
   1a1dc: ea00000c     	b	0x1a214
   1a1e0: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1a1e4: e1560004     	cmp	r6, r4
   1a1e8: e885000f     	stm	r5, {r0, r1, r2, r3}
   1a1ec: 0a000003     	beq	0x1a200
   1a1f0: e0442006     	sub	r2, r4, r6
   1a1f4: e1a01006     	mov	r1, r6
   1a1f8: e2860010     	add	r0, r6, #16
   1a1fc: ebffedf8     	bl	0x159e4    @ imm = #-0x4820 ; memmove
   1a200: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1a204: e2844010     	add	r4, r4, #16
   1a208: e1590004     	cmp	r9, r4
   1a20c: e886000f     	stm	r6, {r0, r1, r2, r3}
   1a210: 0a000017     	beq	0x1a274
   1a214: e5947008     	ldr	r7, [r4, #0x8]
   1a218: e5963008     	ldr	r3, [r6, #0x8]
   1a21c: e1570003     	cmp	r7, r3
   1a220: baffffee     	blt	0x1a1e0
   1a224: e514c008     	ldr	r12, [r4, #-0x8]
   1a228: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1a22c: e157000c     	cmp	r7, r12
   1a230: e885000f     	stm	r5, {r0, r1, r2, r3}
   1a234: aa000010     	bge	0x1a27c
   1a238: e244c010     	sub	r12, r4, #16
   1a23c: e28ce010     	add	lr, r12, #16
   1a240: e1a0800c     	mov	r8, r12
   1a244: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1a248: e24cc010     	sub	r12, r12, #16
   1a24c: e88e000f     	stm	lr, {r0, r1, r2, r3}
   1a250: e59c3008     	ldr	r3, [r12, #0x8]
   1a254: e1570003     	cmp	r7, r3
   1a258: bafffff7     	blt	0x1a23c
   1a25c: e58d7008     	str	r7, [sp, #0x8]
   1a260: e2844010     	add	r4, r4, #16
   1a264: e1590004     	cmp	r9, r4
   1a268: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1a26c: e888000f     	stm	r8, {r0, r1, r2, r3}
   1a270: 1affffe7     	bne	0x1a214
   1a274: e28dd014     	add	sp, sp, #20
   1a278: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   1a27c: e1a08004     	mov	r8, r4
   1a280: eafffff5     	b	0x1a25c
