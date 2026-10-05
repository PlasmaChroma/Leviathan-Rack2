; void std::__introsort_loop<__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}> >(__gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, __gnu_cxx::__normal_iterator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch*, std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> > >, int, __gnu_cxx::__ops::_Iter_comp_iter<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::{lambda(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&, lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch const&)#1}>)
; VA 0x1bc8c size 536

   1bc8c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1bc90: e1a05000     	mov	r5, r0
   1bc94: e0410000     	sub	r0, r1, r0
   1bc98: e24dd02c     	sub	sp, sp, #44
   1bc9c: e3500c01     	cmp	r0, #256
   1bca0: da00007d     	ble	0x1be9c
   1bca4: e1a08002     	mov	r8, r2
   1bca8: e3520000     	cmp	r2, #0
   1bcac: 0a000057     	beq	0x1be10
   1bcb0: e1a0a001     	mov	r10, r1
   1bcb4: e28d4018     	add	r4, sp, #24
   1bcb8: e1a09003     	mov	r9, r3
   1bcbc: e2853010     	add	r3, r5, #16
   1bcc0: e58d3010     	str	r3, [sp, #0x10]
   1bcc4: e2853020     	add	r3, r5, #32
   1bcc8: e58d3014     	str	r3, [sp, #0x14]
   1bccc: e04ac005     	sub	r12, r10, r5
   1bcd0: e5952018     	ldr	r2, [r5, #0x18]
   1bcd4: e1a0300c     	mov	r3, r12
   1bcd8: e51a1008     	ldr	r1, [r10, #-0x8]
   1bcdc: e1a0cfac     	lsr	r12, r12, #31
   1bce0: e2488001     	sub	r8, r8, #1
   1bce4: e08cc243     	add	r12, r12, r3, asr #4
   1bce8: e1a0c0cc     	asr	r12, r12, #1
   1bcec: e085c20c     	add	r12, r5, r12, lsl #4
   1bcf0: e59c3008     	ldr	r3, [r12, #0x8]
   1bcf4: e1520003     	cmp	r2, r3
   1bcf8: aa000037     	bge	0x1bddc
   1bcfc: e1530001     	cmp	r3, r1
   1bd00: ba000002     	blt	0x1bd10
   1bd04: e1520001     	cmp	r2, r1
   1bd08: aa000038     	bge	0x1bdf0
   1bd0c: e24ac010     	sub	r12, r10, #16
   1bd10: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1bd14: e884000f     	stm	r4, {r0, r1, r2, r3}
   1bd18: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1bd1c: e885000f     	stm	r5, {r0, r1, r2, r3}
   1bd20: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1bd24: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1bd28: e5953008     	ldr	r3, [r5, #0x8]
   1bd2c: e51a2008     	ldr	r2, [r10, #-0x8]
   1bd30: e1a0e00a     	mov	lr, r10
   1bd34: e59d7010     	ldr	r7, [sp, #0x10]
   1bd38: e59db014     	ldr	r11, [sp, #0x14]
   1bd3c: e51b1008     	ldr	r1, [r11, #-0x8]
   1bd40: e1a06007     	mov	r6, r7
   1bd44: e1510003     	cmp	r1, r3
   1bd48: ba000014     	blt	0x1bda0
   1bd4c: e1520003     	cmp	r2, r3
   1bd50: e24ec010     	sub	r12, lr, #16
   1bd54: da000005     	ble	0x1bd70
   1bd58: e24ee020     	sub	lr, lr, #32
   1bd5c: e1a0c00e     	mov	r12, lr
   1bd60: e24ee010     	sub	lr, lr, #16
   1bd64: e59e2018     	ldr	r2, [lr, #0x18]
   1bd68: e1520003     	cmp	r2, r3
   1bd6c: cafffffa     	bgt	0x1bd5c
   1bd70: e157000c     	cmp	r7, r12
   1bd74: 2a00000c     	bhs	0x1bdac
   1bd78: e24b6010     	sub	r6, r11, #16
   1bd7c: e1a0e00c     	mov	lr, r12
   1bd80: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1bd84: e884000f     	stm	r4, {r0, r1, r2, r3}
   1bd88: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1bd8c: e886000f     	stm	r6, {r0, r1, r2, r3}
   1bd90: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1bd94: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1bd98: e51c2008     	ldr	r2, [r12, #-0x8]
   1bd9c: e5953008     	ldr	r3, [r5, #0x8]
   1bda0: e2877010     	add	r7, r7, #16
   1bda4: e28bb010     	add	r11, r11, #16
   1bda8: eaffffe3     	b	0x1bd3c
   1bdac: e1a00007     	mov	r0, r7
   1bdb0: e1a0100a     	mov	r1, r10
   1bdb4: e1a03009     	mov	r3, r9
   1bdb8: e1a02008     	mov	r2, r8
   1bdbc: ebffffb2     	bl	0x1bc8c
   1bdc0: e0470005     	sub	r0, r7, r5
   1bdc4: e3500c01     	cmp	r0, #256
   1bdc8: da000033     	ble	0x1be9c
   1bdcc: e3580000     	cmp	r8, #0
   1bdd0: 0a000010     	beq	0x1be18
   1bdd4: e1a0a007     	mov	r10, r7
   1bdd8: eaffffbb     	b	0x1bccc
   1bddc: e1520001     	cmp	r2, r1
   1bde0: ba000002     	blt	0x1bdf0
   1bde4: e1530001     	cmp	r3, r1
   1bde8: aaffffc8     	bge	0x1bd10
   1bdec: eaffffc6     	b	0x1bd0c
   1bdf0: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1bdf4: e884000f     	stm	r4, {r0, r1, r2, r3}
   1bdf8: e59dc010     	ldr	r12, [sp, #0x10]
   1bdfc: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1be00: e885000f     	stm	r5, {r0, r1, r2, r3}
   1be04: e894000f     	ldm	r4, {r0, r1, r2, r3}
   1be08: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1be0c: eaffffc5     	b	0x1bd28
   1be10: e1a06001     	mov	r6, r1
   1be14: e28d4018     	add	r4, sp, #24
   1be18: e1a08240     	asr	r8, r0, #4
   1be1c: e28d701c     	add	r7, sp, #28
   1be20: e2489002     	sub	r9, r8, #2
   1be24: e1a090c9     	asr	r9, r9, #1
   1be28: ea000000     	b	0x1be30
   1be2c: e2499001     	sub	r9, r9, #1
   1be30: e0853209     	add	r3, r5, r9, lsl #4
   1be34: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1be38: e884000f     	stm	r4, {r0, r1, r2, r3}
   1be3c: e8970007     	ldm	r7, {r0, r1, r2}
   1be40: e88d0007     	stm	sp, {r0, r1, r2}
   1be44: e59d3018     	ldr	r3, [sp, #0x18]
   1be48: e1a02008     	mov	r2, r8
   1be4c: e1a01009     	mov	r1, r9
   1be50: e1a00005     	mov	r0, r5
   1be54: ebfffeb9     	bl	0x1b940
   1be58: e3590000     	cmp	r9, #0
   1be5c: 1afffff2     	bne	0x1be2c
   1be60: e2466010     	sub	r6, r6, #16
   1be64: e0468005     	sub	r8, r6, r5
   1be68: e896000f     	ldm	r6, {r0, r1, r2, r3}
   1be6c: e884000f     	stm	r4, {r0, r1, r2, r3}
   1be70: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1be74: e886000f     	stm	r6, {r0, r1, r2, r3}
   1be78: e59d3018     	ldr	r3, [sp, #0x18]
   1be7c: e8970007     	ldm	r7, {r0, r1, r2}
   1be80: e88d0007     	stm	sp, {r0, r1, r2}
   1be84: e1a02248     	asr	r2, r8, #4
   1be88: e3a01000     	mov	r1, #0
   1be8c: e1a00005     	mov	r0, r5
   1be90: ebfffeaa     	bl	0x1b940
   1be94: e3580010     	cmp	r8, #16
   1be98: cafffff0     	bgt	0x1be60
   1be9c: e28dd02c     	add	sp, sp, #44
   1bea0: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
