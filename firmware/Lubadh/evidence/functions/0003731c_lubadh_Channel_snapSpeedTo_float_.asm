; lubadh::Channel::snapSpeedTo(float)
; VA 0x3731c size 72

   3731c: e59030e8     	ldr	r3, [r0, #0xe8]
   37320: e3a01000     	mov	r1, #0
   37324: e3a02000     	mov	r2, #0
   37328: e58012dc     	str	r1, [r0, #0x2dc]
   3732c: ed800a0b     	vstr	s0, [r0, #44]
   37330: e59330b0     	ldr	r3, [r3, #0xb0]
   37334: ed800ab6     	vstr	s0, [r0, #728]
   37338: ed800a09     	vstr	s0, [r0, #36]
   3733c: e5933040     	ldr	r3, [r3, #0x40]
   37340: e58022e0     	str	r2, [r0, #0x2e0]
   37344: e3530003     	cmp	r3, #3
   37348: 112fff1e     	bxne	lr
   3734c: eeb50ac0     	vcmpe.f32	s0, #0
   37350: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37354: b3a03001     	movlt	r3, #1
   37358: a1a03002     	movge	r3, r2
   3735c: e58030ec     	str	r3, [r0, #0xec]
   37360: e12fff1e     	bx	lr
