; void std::__insertion_sort<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x1bbbc size 208

   1bbbc: e1500001     	cmp	r0, r1
   1bbc0: 012fff1e     	bxeq	lr
   1bbc4: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   1bbc8: e2804010     	add	r4, r0, #16
   1bbcc: e1a06000     	mov	r6, r0
   1bbd0: e24dd014     	sub	sp, sp, #20
   1bbd4: e1a09001     	mov	r9, r1
   1bbd8: e1510004     	cmp	r1, r4
   1bbdc: 0a000026     	beq	0x1bc7c
   1bbe0: e1a0500d     	mov	r5, sp
   1bbe4: ea00000c     	b	0x1bc1c
   1bbe8: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1bbec: e1560004     	cmp	r6, r4
   1bbf0: e885000f     	stm	r5, {r0, r1, r2, r3}
   1bbf4: 0a000003     	beq	0x1bc08
   1bbf8: e0442006     	sub	r2, r4, r6
   1bbfc: e1a01006     	mov	r1, r6
   1bc00: e2860010     	add	r0, r6, #16
   1bc04: ebffe776     	bl	0x159e4    @ imm = #-0x6228 ; memmove
   1bc08: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1bc0c: e2844010     	add	r4, r4, #16
   1bc10: e1590004     	cmp	r9, r4
   1bc14: e886000f     	stm	r6, {r0, r1, r2, r3}
   1bc18: 0a000017     	beq	0x1bc7c
   1bc1c: e5947008     	ldr	r7, [r4, #0x8]
   1bc20: e5963008     	ldr	r3, [r6, #0x8]
   1bc24: e1570003     	cmp	r7, r3
   1bc28: baffffee     	blt	0x1bbe8
   1bc2c: e514c008     	ldr	r12, [r4, #-0x8]
   1bc30: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1bc34: e157000c     	cmp	r7, r12
   1bc38: e885000f     	stm	r5, {r0, r1, r2, r3}
   1bc3c: aa000010     	bge	0x1bc84
   1bc40: e244c010     	sub	r12, r4, #16
   1bc44: e28ce010     	add	lr, r12, #16
   1bc48: e1a0800c     	mov	r8, r12
   1bc4c: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1bc50: e24cc010     	sub	r12, r12, #16
   1bc54: e88e000f     	stm	lr, {r0, r1, r2, r3}
   1bc58: e59c3008     	ldr	r3, [r12, #0x8]
   1bc5c: e1570003     	cmp	r7, r3
   1bc60: bafffff7     	blt	0x1bc44
   1bc64: e58d7008     	str	r7, [sp, #0x8]
   1bc68: e2844010     	add	r4, r4, #16
   1bc6c: e1590004     	cmp	r9, r4
   1bc70: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1bc74: e888000f     	stm	r8, {r0, r1, r2, r3}
   1bc78: 1affffe7     	bne	0x1bc1c
   1bc7c: e28dd014     	add	sp, sp, #20
   1bc80: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   1bc84: e1a08004     	mov	r8, r4
   1bc88: eafffff5     	b	0x1bc64
