; void std::__adjust_heap<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, int, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>) [clone .isra.0]
; VA 0x19e88 size 396

   19e88: e24dd008     	sub	sp, sp, #8
   19e8c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   19e90: e2429001     	sub	r9, r2, #1
   19e94: e1a0a001     	mov	r10, r1
   19e98: e24dd01c     	sub	sp, sp, #28
   19e9c: e0899fa9     	add	r9, r9, r9, lsr #31
   19ea0: e1a04000     	mov	r4, r0
   19ea4: e15100c9     	cmp	r1, r9, asr #1
   19ea8: e58d3044     	str	r3, [sp, #0x44]
   19eac: e2023001     	and	r3, r2, #1
   19eb0: e59db04c     	ldr	r11, [sp, #0x4c]
   19eb4: e58d2004     	str	r2, [sp, #0x4]
   19eb8: e58d3000     	str	r3, [sp]
   19ebc: aa000031     	bge	0x19f88
   19ec0: e1a090c9     	asr	r9, r9, #1
   19ec4: e1a05001     	mov	r5, r1
   19ec8: e285c001     	add	r12, r5, #1
   19ecc: e0848205     	add	r8, r4, r5, lsl #4
   19ed0: e1a0508c     	lsl	r5, r12, #1
   19ed4: e084c28c     	add	r12, r4, r12, lsl #5
   19ed8: e2457001     	sub	r7, r5, #1
   19edc: e0846207     	add	r6, r4, r7, lsl #4
   19ee0: e59c3008     	ldr	r3, [r12, #0x8]
   19ee4: e5962008     	ldr	r2, [r6, #0x8]
   19ee8: e1520003     	cmp	r2, r3
   19eec: ca000035     	bgt	0x19fc8
   19ef0: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   19ef4: e1590005     	cmp	r9, r5
   19ef8: e888000f     	stm	r8, {r0, r1, r2, r3}
   19efc: cafffff1     	bgt	0x19ec8
   19f00: e59d3000     	ldr	r3, [sp]
   19f04: e3530000     	cmp	r3, #0
   19f08: 0a000022     	beq	0x19f98
   19f0c: e28d3044     	add	r3, sp, #68
   19f10: e2456001     	sub	r6, r5, #1
   19f14: e28d7008     	add	r7, sp, #8
   19f18: e155000a     	cmp	r5, r10
   19f1c: e0866fa6     	add	r6, r6, r6, lsr #31
   19f20: e893000f     	ldm	r3, {r0, r1, r2, r3}
   19f24: e1a060c6     	asr	r6, r6, #1
   19f28: e887000f     	stm	r7, {r0, r1, r2, r3}
   19f2c: ca000005     	bgt	0x19f48
   19f30: ea00000d     	b	0x19f6c
   19f34: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   19f38: e15a0006     	cmp	r10, r6
   19f3c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   19f40: e1a06008     	mov	r6, r8
   19f44: aa000025     	bge	0x19fe0
   19f48: e084e206     	add	lr, r4, r6, lsl #4
   19f4c: e2463001     	sub	r3, r6, #1
   19f50: e084c205     	add	r12, r4, r5, lsl #4
   19f54: e1a05006     	mov	r5, r6
   19f58: e0833fa3     	add	r3, r3, r3, lsr #31
   19f5c: e59e2008     	ldr	r2, [lr, #0x8]
   19f60: e1a080c3     	asr	r8, r3, #1
   19f64: e152000b     	cmp	r2, r11
   19f68: bafffff1     	blt	0x19f34
   19f6c: e58db010     	str	r11, [sp, #0x10]
   19f70: e897000f     	ldm	r7, {r0, r1, r2, r3}
   19f74: e88c000f     	stm	r12, {r0, r1, r2, r3}
   19f78: e28dd01c     	add	sp, sp, #28
   19f7c: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   19f80: e28dd008     	add	sp, sp, #8
   19f84: e12fff1e     	bx	lr
   19f88: e3530000     	cmp	r3, #0
   19f8c: e080c201     	add	r12, r0, r1, lsl #4
   19f90: 1a00001a     	bne	0x1a000
   19f94: e1a0500a     	mov	r5, r10
   19f98: e59d3004     	ldr	r3, [sp, #0x4]
   19f9c: e2432002     	sub	r2, r3, #2
   19fa0: e0822fa2     	add	r2, r2, r2, lsr #31
   19fa4: e15500c2     	cmp	r5, r2, asr #1
   19fa8: 1affffd7     	bne	0x19f0c
   19fac: e1a05085     	lsl	r5, r5, #1
   19fb0: e2855001     	add	r5, r5, #1
   19fb4: e084e205     	add	lr, r4, r5, lsl #4
   19fb8: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   19fbc: e88c000f     	stm	r12, {r0, r1, r2, r3}
   19fc0: e1a0c00e     	mov	r12, lr
   19fc4: eaffffd0     	b	0x19f0c
   19fc8: e896000f     	ldm	r6, {r0, r1, r2, r3}
   19fcc: e1590007     	cmp	r9, r7
   19fd0: e888000f     	stm	r8, {r0, r1, r2, r3}
   19fd4: da000003     	ble	0x19fe8
   19fd8: e1a05007     	mov	r5, r7
   19fdc: eaffffb9     	b	0x19ec8
   19fe0: e1a0c00e     	mov	r12, lr
   19fe4: eaffffe0     	b	0x19f6c
   19fe8: e59d3000     	ldr	r3, [sp]
   19fec: e1a0c006     	mov	r12, r6
   19ff0: e1a05007     	mov	r5, r7
   19ff4: e3530000     	cmp	r3, #0
   19ff8: 1affffc3     	bne	0x19f0c
   19ffc: eaffffe5     	b	0x19f98
   1a000: e28d3044     	add	r3, sp, #68
   1a004: e28d7008     	add	r7, sp, #8
   1a008: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1a00c: e887000f     	stm	r7, {r0, r1, r2, r3}
   1a010: eaffffd5     	b	0x19f6c
