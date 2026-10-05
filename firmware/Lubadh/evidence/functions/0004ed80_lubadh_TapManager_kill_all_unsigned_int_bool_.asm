; lubadh::TapManager::kill_all(unsigned int, bool)
; VA 0x4ed80 size 156

   4ed80: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   4ed84: e2807ed3     	add	r7, r0, #3376
   4ed88: e1a09001     	mov	r9, r1
   4ed8c: e24dd00c     	sub	sp, sp, #12
   4ed90: e1a06002     	mov	r6, r2
   4ed94: e2877004     	add	r7, r7, #4
   4ed98: e2805fa5     	add	r5, r0, #660
   4ed9c: e3a08000     	mov	r8, #0
   4eda0: e2454fa5     	sub	r4, r5, #660
   4eda4: e5d40000     	ldrb	r0, [r4]
   4eda8: e3500000     	cmp	r0, #0
   4edac: 0a000012     	beq	0x4edfc
   4edb0: e5d4004c     	ldrb	r0, [r4, #0x4c]
   4edb4: e3500000     	cmp	r0, #0
   4edb8: 1a00000f     	bne	0x4edfc
   4edbc: e5d43014     	ldrb	r3, [r4, #0x14]
   4edc0: e1a00006     	mov	r0, r6
   4edc4: e5942004     	ldr	r2, [r4, #0x4]
   4edc8: e3530000     	cmp	r3, #0
   4edcc: 0a000004     	beq	0x4ede4
   4edd0: edd47a06     	vldr	s15, [r4, #24]
   4edd4: eef57ac0     	vcmpe.f32	s15, #0
   4edd8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4eddc: a3a00001     	movge	r0, #1
   4ede0: b3a00000     	movlt	r0, #0
   4ede4: e58d0004     	str	r0, [sp, #0x4]
   4ede8: e1a03009     	mov	r3, r9
   4edec: e3a01002     	mov	r1, #2
   4edf0: e1a00004     	mov	r0, r4
   4edf4: e58d8000     	str	r8, [sp]
   4edf8: ebfffd9b     	bl	0x4e46c
   4edfc: e2844084     	add	r4, r4, #132
   4ee00: e1540005     	cmp	r4, r5
   4ee04: 1affffe6     	bne	0x4eda4
   4ee08: e2845faa     	add	r5, r4, #680
   4ee0c: e1570005     	cmp	r7, r5
   4ee10: 1affffe2     	bne	0x4eda0
   4ee14: e28dd00c     	add	sp, sp, #12
   4ee18: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
