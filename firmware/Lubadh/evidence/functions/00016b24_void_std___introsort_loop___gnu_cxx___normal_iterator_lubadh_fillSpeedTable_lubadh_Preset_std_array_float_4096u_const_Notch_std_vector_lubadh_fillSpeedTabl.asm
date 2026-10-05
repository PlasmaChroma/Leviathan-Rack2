; void std::__introsort_loop<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>)
; VA 0x16b24 size 536

   16b24: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   16b28: e1a05000     	mov	r5, r0
   16b2c: e0410000     	sub	r0, r1, r0
   16b30: e24dd02c     	sub	sp, sp, #44
   16b34: e3500c01     	cmp	r0, #256
   16b38: da00007d     	ble	0x16d34
   16b3c: e1a08002     	mov	r8, r2
   16b40: e3520000     	cmp	r2, #0
   16b44: 0a000057     	beq	0x16ca8
   16b48: e1a0a001     	mov	r10, r1
   16b4c: e28d4018     	add	r4, sp, #24
   16b50: e1a09003     	mov	r9, r3
   16b54: e2853010     	add	r3, r5, #16
   16b58: e58d3010     	str	r3, [sp, #0x10]
   16b5c: e2853020     	add	r3, r5, #32
   16b60: e58d3014     	str	r3, [sp, #0x14]
   16b64: e04ac005     	sub	r12, r10, r5
   16b68: e5952018     	ldr	r2, [r5, #0x18]
   16b6c: e1a0300c     	mov	r3, r12
   16b70: e51a1008     	ldr	r1, [r10, #-0x8]
   16b74: e1a0cfac     	lsr	r12, r12, #31
   16b78: e2488001     	sub	r8, r8, #1
   16b7c: e08cc243     	add	r12, r12, r3, asr #4
   16b80: e1a0c0cc     	asr	r12, r12, #1
   16b84: e085c20c     	add	r12, r5, r12, lsl #4
   16b88: e59c3008     	ldr	r3, [r12, #0x8]
   16b8c: e1520003     	cmp	r2, r3
   16b90: aa000037     	bge	0x16c74
   16b94: e1530001     	cmp	r3, r1
   16b98: ba000002     	blt	0x16ba8
   16b9c: e1520001     	cmp	r2, r1
   16ba0: aa000038     	bge	0x16c88
   16ba4: e24ac010     	sub	r12, r10, #16
   16ba8: e895000f     	ldm	r5, {r0, r1, r2, r3}
   16bac: e884000f     	stm	r4, {r0, r1, r2, r3}
   16bb0: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   16bb4: e885000f     	stm	r5, {r0, r1, r2, r3}
   16bb8: e894000f     	ldm	r4, {r0, r1, r2, r3}
   16bbc: e88c000f     	stm	r12, {r0, r1, r2, r3}
   16bc0: e5953008     	ldr	r3, [r5, #0x8]
   16bc4: e51a2008     	ldr	r2, [r10, #-0x8]
   16bc8: e1a0e00a     	mov	lr, r10
   16bcc: e59d7010     	ldr	r7, [sp, #0x10]
   16bd0: e59db014     	ldr	r11, [sp, #0x14]
   16bd4: e51b1008     	ldr	r1, [r11, #-0x8]
   16bd8: e1a06007     	mov	r6, r7
   16bdc: e1510003     	cmp	r1, r3
   16be0: ba000014     	blt	0x16c38
   16be4: e1520003     	cmp	r2, r3
   16be8: e24ec010     	sub	r12, lr, #16
   16bec: da000005     	ble	0x16c08
   16bf0: e24ee020     	sub	lr, lr, #32
   16bf4: e1a0c00e     	mov	r12, lr
   16bf8: e24ee010     	sub	lr, lr, #16
   16bfc: e59e2018     	ldr	r2, [lr, #0x18]
   16c00: e1520003     	cmp	r2, r3
   16c04: cafffffa     	bgt	0x16bf4
   16c08: e157000c     	cmp	r7, r12
   16c0c: 2a00000c     	bhs	0x16c44
   16c10: e24b6010     	sub	r6, r11, #16
   16c14: e1a0e00c     	mov	lr, r12
   16c18: e896000f     	ldm	r6, {r0, r1, r2, r3}
   16c1c: e884000f     	stm	r4, {r0, r1, r2, r3}
   16c20: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   16c24: e886000f     	stm	r6, {r0, r1, r2, r3}
   16c28: e894000f     	ldm	r4, {r0, r1, r2, r3}
   16c2c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   16c30: e51c2008     	ldr	r2, [r12, #-0x8]
   16c34: e5953008     	ldr	r3, [r5, #0x8]
   16c38: e2877010     	add	r7, r7, #16
   16c3c: e28bb010     	add	r11, r11, #16
   16c40: eaffffe3     	b	0x16bd4
   16c44: e1a00007     	mov	r0, r7
   16c48: e1a0100a     	mov	r1, r10
   16c4c: e1a03009     	mov	r3, r9
   16c50: e1a02008     	mov	r2, r8
   16c54: ebffffb2     	bl	0x16b24
   16c58: e0470005     	sub	r0, r7, r5
   16c5c: e3500c01     	cmp	r0, #256
   16c60: da000033     	ble	0x16d34
   16c64: e3580000     	cmp	r8, #0
   16c68: 0a000010     	beq	0x16cb0
   16c6c: e1a0a007     	mov	r10, r7
   16c70: eaffffbb     	b	0x16b64
   16c74: e1520001     	cmp	r2, r1
   16c78: ba000002     	blt	0x16c88
   16c7c: e1530001     	cmp	r3, r1
   16c80: aaffffc8     	bge	0x16ba8
   16c84: eaffffc6     	b	0x16ba4
   16c88: e895000f     	ldm	r5, {r0, r1, r2, r3}
   16c8c: e884000f     	stm	r4, {r0, r1, r2, r3}
   16c90: e59dc010     	ldr	r12, [sp, #0x10]
   16c94: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   16c98: e885000f     	stm	r5, {r0, r1, r2, r3}
   16c9c: e894000f     	ldm	r4, {r0, r1, r2, r3}
   16ca0: e88c000f     	stm	r12, {r0, r1, r2, r3}
   16ca4: eaffffc5     	b	0x16bc0
   16ca8: e1a06001     	mov	r6, r1
   16cac: e28d4018     	add	r4, sp, #24
   16cb0: e1a08240     	asr	r8, r0, #4
   16cb4: e28d701c     	add	r7, sp, #28
   16cb8: e2489002     	sub	r9, r8, #2
   16cbc: e1a090c9     	asr	r9, r9, #1
   16cc0: ea000000     	b	0x16cc8
   16cc4: e2499001     	sub	r9, r9, #1
   16cc8: e0853209     	add	r3, r5, r9, lsl #4
   16ccc: e893000f     	ldm	r3, {r0, r1, r2, r3}
   16cd0: e884000f     	stm	r4, {r0, r1, r2, r3}
   16cd4: e8970007     	ldm	r7, {r0, r1, r2}
   16cd8: e88d0007     	stm	sp, {r0, r1, r2}
   16cdc: e59d3018     	ldr	r3, [sp, #0x18]
   16ce0: e1a02008     	mov	r2, r8
   16ce4: e1a01009     	mov	r1, r9
   16ce8: e1a00005     	mov	r0, r5
   16cec: ebfffeb9     	bl	0x167d8
   16cf0: e3590000     	cmp	r9, #0
   16cf4: 1afffff2     	bne	0x16cc4
   16cf8: e2466010     	sub	r6, r6, #16
   16cfc: e0468005     	sub	r8, r6, r5
   16d00: e896000f     	ldm	r6, {r0, r1, r2, r3}
   16d04: e884000f     	stm	r4, {r0, r1, r2, r3}
   16d08: e895000f     	ldm	r5, {r0, r1, r2, r3}
   16d0c: e886000f     	stm	r6, {r0, r1, r2, r3}
   16d10: e59d3018     	ldr	r3, [sp, #0x18]
   16d14: e8970007     	ldm	r7, {r0, r1, r2}
   16d18: e88d0007     	stm	sp, {r0, r1, r2}
   16d1c: e1a02248     	asr	r2, r8, #4
   16d20: e3a01000     	mov	r1, #0
   16d24: e1a00005     	mov	r0, r5
   16d28: ebfffeaa     	bl	0x167d8
   16d2c: e3580010     	cmp	r8, #16
   16d30: cafffff0     	bgt	0x16cf8
   16d34: e28dd02c     	add	sp, sp, #44
   16d38: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
