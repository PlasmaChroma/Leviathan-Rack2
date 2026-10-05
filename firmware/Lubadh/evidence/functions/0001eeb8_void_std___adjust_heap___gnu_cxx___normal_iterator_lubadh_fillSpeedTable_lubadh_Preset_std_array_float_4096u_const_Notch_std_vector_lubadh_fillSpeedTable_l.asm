; void std::__adjust_heap<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x1eeb8 size 396

   1eeb8: e24dd008     	sub	sp, sp, #8
   1eebc: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1eec0: e2429001     	sub	r9, r2, #1
   1eec4: e1a0a001     	mov	r10, r1
   1eec8: e24dd01c     	sub	sp, sp, #28
   1eecc: e0899fa9     	add	r9, r9, r9, lsr #31
   1eed0: e1a04000     	mov	r4, r0
   1eed4: e15100c9     	cmp	r1, r9, asr #1
   1eed8: e58d3044     	str	r3, [sp, #0x44]
   1eedc: e2023001     	and	r3, r2, #1
   1eee0: e59db04c     	ldr	r11, [sp, #0x4c]
   1eee4: e58d2004     	str	r2, [sp, #0x4]
   1eee8: e58d3000     	str	r3, [sp]
   1eeec: aa000031     	bge	0x1efb8
   1eef0: e1a090c9     	asr	r9, r9, #1
   1eef4: e1a05001     	mov	r5, r1
   1eef8: e285c001     	add	r12, r5, #1
   1eefc: e0848205     	add	r8, r4, r5, lsl #4
   1ef00: e1a0508c     	lsl	r5, r12, #1
   1ef04: e084c28c     	add	r12, r4, r12, lsl #5
   1ef08: e2457001     	sub	r7, r5, #1
   1ef0c: e0846207     	add	r6, r4, r7, lsl #4
   1ef10: e59c3008     	ldr	r3, [r12, #0x8]
   1ef14: e5962008     	ldr	r2, [r6, #0x8]
   1ef18: e1520003     	cmp	r2, r3
   1ef1c: ca000035     	bgt	0x1eff8
   1ef20: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1ef24: e1590005     	cmp	r9, r5
   1ef28: e888000f     	stm	r8, {r0, r1, r2, r3}
   1ef2c: cafffff1     	bgt	0x1eef8
   1ef30: e59d3000     	ldr	r3, [sp]
   1ef34: e3530000     	cmp	r3, #0
   1ef38: 0a000022     	beq	0x1efc8
   1ef3c: e28d3044     	add	r3, sp, #68
   1ef40: e2456001     	sub	r6, r5, #1
   1ef44: e28d7008     	add	r7, sp, #8
   1ef48: e155000a     	cmp	r5, r10
   1ef4c: e0866fa6     	add	r6, r6, r6, lsr #31
   1ef50: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1ef54: e1a060c6     	asr	r6, r6, #1
   1ef58: e887000f     	stm	r7, {r0, r1, r2, r3}
   1ef5c: ca000005     	bgt	0x1ef78
   1ef60: ea00000d     	b	0x1ef9c
   1ef64: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1ef68: e15a0006     	cmp	r10, r6
   1ef6c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1ef70: e1a06008     	mov	r6, r8
   1ef74: aa000025     	bge	0x1f010
   1ef78: e084e206     	add	lr, r4, r6, lsl #4
   1ef7c: e2463001     	sub	r3, r6, #1
   1ef80: e084c205     	add	r12, r4, r5, lsl #4
   1ef84: e1a05006     	mov	r5, r6
   1ef88: e0833fa3     	add	r3, r3, r3, lsr #31
   1ef8c: e59e2008     	ldr	r2, [lr, #0x8]
   1ef90: e1a080c3     	asr	r8, r3, #1
   1ef94: e152000b     	cmp	r2, r11
   1ef98: bafffff1     	blt	0x1ef64
   1ef9c: e58db010     	str	r11, [sp, #0x10]
   1efa0: e897000f     	ldm	r7, {r0, r1, r2, r3}
   1efa4: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1efa8: e28dd01c     	add	sp, sp, #28
   1efac: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1efb0: e28dd008     	add	sp, sp, #8
   1efb4: e12fff1e     	bx	lr
   1efb8: e3530000     	cmp	r3, #0
   1efbc: e080c201     	add	r12, r0, r1, lsl #4
   1efc0: 1a00001a     	bne	0x1f030
   1efc4: e1a0500a     	mov	r5, r10
   1efc8: e59d3004     	ldr	r3, [sp, #0x4]
   1efcc: e2432002     	sub	r2, r3, #2
   1efd0: e0822fa2     	add	r2, r2, r2, lsr #31
   1efd4: e15500c2     	cmp	r5, r2, asr #1
   1efd8: 1affffd7     	bne	0x1ef3c
   1efdc: e1a05085     	lsl	r5, r5, #1
   1efe0: e2855001     	add	r5, r5, #1
   1efe4: e084e205     	add	lr, r4, r5, lsl #4
   1efe8: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1efec: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1eff0: e1a0c00e     	mov	r12, lr
   1eff4: eaffffd0     	b	0x1ef3c
   1eff8: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1effc: e1590007     	cmp	r9, r7
   1f000: e888000f     	stm	r8, {r0, r1, r2, r3}
   1f004: da000003     	ble	0x1f018
   1f008: e1a05007     	mov	r5, r7
   1f00c: eaffffb9     	b	0x1eef8
   1f010: e1a0c00e     	mov	r12, lr
   1f014: eaffffe0     	b	0x1ef9c
   1f018: e59d3000     	ldr	r3, [sp]
   1f01c: e1a0c006     	mov	r12, r6
   1f020: e1a05007     	mov	r5, r7
   1f024: e3530000     	cmp	r3, #0
   1f028: 1affffc3     	bne	0x1ef3c
   1f02c: eaffffe5     	b	0x1efc8
   1f030: e28d3044     	add	r3, sp, #68
   1f034: e28d7008     	add	r7, sp, #8
   1f038: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1f03c: e887000f     	stm	r7, {r0, r1, r2, r3}
   1f040: eaffffd5     	b	0x1ef9c
