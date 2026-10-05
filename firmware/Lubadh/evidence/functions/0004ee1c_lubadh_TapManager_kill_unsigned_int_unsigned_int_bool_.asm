; lubadh::TapManager::kill(unsigned int, unsigned int, bool)
; VA 0x4ee1c size 140

   4ee1c: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   4ee20: e3a04faa     	mov	r4, #680
   4ee24: e1a06002     	mov	r6, r2
   4ee28: e24dd008     	sub	sp, sp, #8
   4ee2c: e1a05003     	mov	r5, r3
   4ee30: e0240194     	mla	r4, r4, r1, r0
   4ee34: e3a08000     	mov	r8, #0
   4ee38: e2847fa5     	add	r7, r4, #660
   4ee3c: e5d43000     	ldrb	r3, [r4]
   4ee40: e3530000     	cmp	r3, #0
   4ee44: 0a000012     	beq	0x4ee94
   4ee48: e5d4304c     	ldrb	r3, [r4, #0x4c]
   4ee4c: e3530000     	cmp	r3, #0
   4ee50: 1a00000f     	bne	0x4ee94
   4ee54: e5d43014     	ldrb	r3, [r4, #0x14]
   4ee58: e1a01005     	mov	r1, r5
   4ee5c: e5942004     	ldr	r2, [r4, #0x4]
   4ee60: e3530000     	cmp	r3, #0
   4ee64: 0a000004     	beq	0x4ee7c
   4ee68: edd47a06     	vldr	s15, [r4, #24]
   4ee6c: eef57ac0     	vcmpe.f32	s15, #0
   4ee70: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ee74: a3a01001     	movge	r1, #1
   4ee78: b3a01000     	movlt	r1, #0
   4ee7c: e58d1004     	str	r1, [sp, #0x4]
   4ee80: e1a03006     	mov	r3, r6
   4ee84: e3a01002     	mov	r1, #2
   4ee88: e1a00004     	mov	r0, r4
   4ee8c: e58d8000     	str	r8, [sp]
   4ee90: ebfffd75     	bl	0x4e46c
   4ee94: e2844084     	add	r4, r4, #132
   4ee98: e1570004     	cmp	r7, r4
   4ee9c: 1affffe6     	bne	0x4ee3c
   4eea0: e28dd008     	add	sp, sp, #8
   4eea4: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
