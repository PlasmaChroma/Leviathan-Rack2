; lubadh::TapEngine::kill_all(unsigned int, bool)
; VA 0x4ecf8 size 136

   4ecf8: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   4ecfc: e1a04000     	mov	r4, r0
   4ed00: e1a07001     	mov	r7, r1
   4ed04: e24dd008     	sub	sp, sp, #8
   4ed08: e1a06002     	mov	r6, r2
   4ed0c: e2805fa5     	add	r5, r0, #660
   4ed10: e3a08000     	mov	r8, #0
   4ed14: e5d40000     	ldrb	r0, [r4]
   4ed18: e3500000     	cmp	r0, #0
   4ed1c: 0a000012     	beq	0x4ed6c
   4ed20: e5d4004c     	ldrb	r0, [r4, #0x4c]
   4ed24: e3500000     	cmp	r0, #0
   4ed28: 1a00000f     	bne	0x4ed6c
   4ed2c: e5d41014     	ldrb	r1, [r4, #0x14]
   4ed30: e1a03006     	mov	r3, r6
   4ed34: e5942004     	ldr	r2, [r4, #0x4]
   4ed38: e3510000     	cmp	r1, #0
   4ed3c: 0a000004     	beq	0x4ed54
   4ed40: edd47a06     	vldr	s15, [r4, #24]
   4ed44: eef57ac0     	vcmpe.f32	s15, #0
   4ed48: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ed4c: a3a03001     	movge	r3, #1
   4ed50: b3a03000     	movlt	r3, #0
   4ed54: e58d3004     	str	r3, [sp, #0x4]
   4ed58: e3a01002     	mov	r1, #2
   4ed5c: e1a03007     	mov	r3, r7
   4ed60: e1a00004     	mov	r0, r4
   4ed64: e58d8000     	str	r8, [sp]
   4ed68: ebfffdbf     	bl	0x4e46c
   4ed6c: e2844084     	add	r4, r4, #132
   4ed70: e1550004     	cmp	r5, r4
   4ed74: 1affffe6     	bne	0x4ed14
   4ed78: e28dd008     	add	sp, sp, #8
   4ed7c: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
