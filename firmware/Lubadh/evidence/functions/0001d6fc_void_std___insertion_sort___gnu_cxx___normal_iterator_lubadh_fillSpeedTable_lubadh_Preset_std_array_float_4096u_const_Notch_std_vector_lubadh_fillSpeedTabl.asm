; void std::__insertion_sort<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x1d6fc size 208

   1d6fc: e1500001     	cmp	r0, r1
   1d700: 012fff1e     	bxeq	lr
   1d704: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   1d708: e2804010     	add	r4, r0, #16
   1d70c: e1a06000     	mov	r6, r0
   1d710: e24dd014     	sub	sp, sp, #20
   1d714: e1a09001     	mov	r9, r1
   1d718: e1510004     	cmp	r1, r4
   1d71c: 0a000026     	beq	0x1d7bc
   1d720: e1a0500d     	mov	r5, sp
   1d724: ea00000c     	b	0x1d75c
   1d728: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1d72c: e1560004     	cmp	r6, r4
   1d730: e885000f     	stm	r5, {r0, r1, r2, r3}
   1d734: 0a000003     	beq	0x1d748
   1d738: e0442006     	sub	r2, r4, r6
   1d73c: e1a01006     	mov	r1, r6
   1d740: e2860010     	add	r0, r6, #16
   1d744: ebffe0a6     	bl	0x159e4    @ imm = #-0x7d68 ; memmove
   1d748: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1d74c: e2844010     	add	r4, r4, #16
   1d750: e1590004     	cmp	r9, r4
   1d754: e886000f     	stm	r6, {r0, r1, r2, r3}
   1d758: 0a000017     	beq	0x1d7bc
   1d75c: e5947008     	ldr	r7, [r4, #0x8]
   1d760: e5963008     	ldr	r3, [r6, #0x8]
   1d764: e1570003     	cmp	r7, r3
   1d768: baffffee     	blt	0x1d728
   1d76c: e514c008     	ldr	r12, [r4, #-0x8]
   1d770: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1d774: e157000c     	cmp	r7, r12
   1d778: e885000f     	stm	r5, {r0, r1, r2, r3}
   1d77c: aa000010     	bge	0x1d7c4
   1d780: e244c010     	sub	r12, r4, #16
   1d784: e28ce010     	add	lr, r12, #16
   1d788: e1a0800c     	mov	r8, r12
   1d78c: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1d790: e24cc010     	sub	r12, r12, #16
   1d794: e88e000f     	stm	lr, {r0, r1, r2, r3}
   1d798: e59c3008     	ldr	r3, [r12, #0x8]
   1d79c: e1570003     	cmp	r7, r3
   1d7a0: bafffff7     	blt	0x1d784
   1d7a4: e58d7008     	str	r7, [sp, #0x8]
   1d7a8: e2844010     	add	r4, r4, #16
   1d7ac: e1590004     	cmp	r9, r4
   1d7b0: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1d7b4: e888000f     	stm	r8, {r0, r1, r2, r3}
   1d7b8: 1affffe7     	bne	0x1d75c
   1d7bc: e28dd014     	add	sp, sp, #20
   1d7c0: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   1d7c4: e1a08004     	mov	r8, r4
   1d7c8: eafffff5     	b	0x1d7a4
