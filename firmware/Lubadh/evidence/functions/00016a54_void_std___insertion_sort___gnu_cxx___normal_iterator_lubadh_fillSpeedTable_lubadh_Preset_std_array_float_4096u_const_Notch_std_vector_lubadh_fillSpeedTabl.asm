; void std::__insertion_sort<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x16a54 size 208

   16a54: e1500001     	cmp	r0, r1
   16a58: 012fff1e     	bxeq	lr
   16a5c: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   16a60: e2804010     	add	r4, r0, #16
   16a64: e1a06000     	mov	r6, r0
   16a68: e24dd014     	sub	sp, sp, #20
   16a6c: e1a09001     	mov	r9, r1
   16a70: e1510004     	cmp	r1, r4
   16a74: 0a000026     	beq	0x16b14
   16a78: e1a0500d     	mov	r5, sp
   16a7c: ea00000c     	b	0x16ab4
   16a80: e894000f     	ldm	r4, {r0, r1, r2, r3}
   16a84: e1560004     	cmp	r6, r4
   16a88: e885000f     	stm	r5, {r0, r1, r2, r3}
   16a8c: 0a000003     	beq	0x16aa0
   16a90: e0442006     	sub	r2, r4, r6
   16a94: e1a01006     	mov	r1, r6
   16a98: e2860010     	add	r0, r6, #16
   16a9c: ebfffbd0     	bl	0x159e4    @ imm = #-0x10c0 ; memmove
   16aa0: e895000f     	ldm	r5, {r0, r1, r2, r3}
   16aa4: e2844010     	add	r4, r4, #16
   16aa8: e1590004     	cmp	r9, r4
   16aac: e886000f     	stm	r6, {r0, r1, r2, r3}
   16ab0: 0a000017     	beq	0x16b14
   16ab4: e5947008     	ldr	r7, [r4, #0x8]
   16ab8: e5963008     	ldr	r3, [r6, #0x8]
   16abc: e1570003     	cmp	r7, r3
   16ac0: baffffee     	blt	0x16a80
   16ac4: e514c008     	ldr	r12, [r4, #-0x8]
   16ac8: e894000f     	ldm	r4, {r0, r1, r2, r3}
   16acc: e157000c     	cmp	r7, r12
   16ad0: e885000f     	stm	r5, {r0, r1, r2, r3}
   16ad4: aa000010     	bge	0x16b1c
   16ad8: e244c010     	sub	r12, r4, #16
   16adc: e28ce010     	add	lr, r12, #16
   16ae0: e1a0800c     	mov	r8, r12
   16ae4: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   16ae8: e24cc010     	sub	r12, r12, #16
   16aec: e88e000f     	stm	lr, {r0, r1, r2, r3}
   16af0: e59c3008     	ldr	r3, [r12, #0x8]
   16af4: e1570003     	cmp	r7, r3
   16af8: bafffff7     	blt	0x16adc
   16afc: e58d7008     	str	r7, [sp, #0x8]
   16b00: e2844010     	add	r4, r4, #16
   16b04: e1590004     	cmp	r9, r4
   16b08: e895000f     	ldm	r5, {r0, r1, r2, r3}
   16b0c: e888000f     	stm	r8, {r0, r1, r2, r3}
   16b10: 1affffe7     	bne	0x16ab4
   16b14: e28dd014     	add	sp, sp, #20
   16b18: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   16b1c: e1a08004     	mov	r8, r4
   16b20: eafffff5     	b	0x16afc
