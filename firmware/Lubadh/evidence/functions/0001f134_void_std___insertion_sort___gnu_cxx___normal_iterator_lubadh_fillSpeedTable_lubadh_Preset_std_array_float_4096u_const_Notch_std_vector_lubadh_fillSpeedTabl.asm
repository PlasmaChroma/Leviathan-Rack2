; void std::__insertion_sort<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x1f134 size 208

   1f134: e1500001     	cmp	r0, r1
   1f138: 012fff1e     	bxeq	lr
   1f13c: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   1f140: e2804010     	add	r4, r0, #16
   1f144: e1a06000     	mov	r6, r0
   1f148: e24dd014     	sub	sp, sp, #20
   1f14c: e1a09001     	mov	r9, r1
   1f150: e1510004     	cmp	r1, r4
   1f154: 0a000026     	beq	0x1f1f4
   1f158: e1a0500d     	mov	r5, sp
   1f15c: ea00000c     	b	0x1f194
   1f160: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1f164: e1560004     	cmp	r6, r4
   1f168: e885000f     	stm	r5, {r0, r1, r2, r3}
   1f16c: 0a000003     	beq	0x1f180
   1f170: e0442006     	sub	r2, r4, r6
   1f174: e1a01006     	mov	r1, r6
   1f178: e2860010     	add	r0, r6, #16
   1f17c: ebffda18     	bl	0x159e4    @ imm = #-0x97a0 ; memmove
   1f180: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1f184: e2844010     	add	r4, r4, #16
   1f188: e1590004     	cmp	r9, r4
   1f18c: e886000f     	stm	r6, {r0, r1, r2, r3}
   1f190: 0a000017     	beq	0x1f1f4
   1f194: e5947008     	ldr	r7, [r4, #0x8]
   1f198: e5963008     	ldr	r3, [r6, #0x8]
   1f19c: e1570003     	cmp	r7, r3
   1f1a0: baffffee     	blt	0x1f160
   1f1a4: e514c008     	ldr	r12, [r4, #-0x8]
   1f1a8: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1f1ac: e157000c     	cmp	r7, r12
   1f1b0: e885000f     	stm	r5, {r0, r1, r2, r3}
   1f1b4: aa000010     	bge	0x1f1fc
   1f1b8: e244c010     	sub	r12, r4, #16
   1f1bc: e28ce010     	add	lr, r12, #16
   1f1c0: e1a0800c     	mov	r8, r12
   1f1c4: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1f1c8: e24cc010     	sub	r12, r12, #16
   1f1cc: e88e000f     	stm	lr, {r0, r1, r2, r3}
   1f1d0: e59c3008     	ldr	r3, [r12, #0x8]
   1f1d4: e1570003     	cmp	r7, r3
   1f1d8: bafffff7     	blt	0x1f1bc
   1f1dc: e58d7008     	str	r7, [sp, #0x8]
   1f1e0: e2844010     	add	r4, r4, #16
   1f1e4: e1590004     	cmp	r9, r4
   1f1e8: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1f1ec: e888000f     	stm	r8, {r0, r1, r2, r3}
   1f1f0: 1affffe7     	bne	0x1f194
   1f1f4: e28dd014     	add	sp, sp, #20
   1f1f8: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   1f1fc: e1a08004     	mov	r8, r4
   1f200: eafffff5     	b	0x1f1dc
