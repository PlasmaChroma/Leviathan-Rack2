; lubadh::Channel::killTaps(bool)
; VA 0x38434 size 228

   38434: e92d40f0     	push	{r4, r5, r6, r7, lr}
   38438: e2804fda     	add	r4, r0, #872
   3843c: e1a06000     	mov	r6, r0
   38440: e24dd00c     	sub	sp, sp, #12
   38444: e1a00004     	mov	r0, r4
   38448: e1a07001     	mov	r7, r1
   3844c: eb005676     	bl	0x4de2c
   38450: e1a05000     	mov	r5, r0
   38454: e1a00004     	mov	r0, r4
   38458: e1a01005     	mov	r1, r5
   3845c: eb0055e3     	bl	0x4dbf0
   38460: e3500000     	cmp	r0, #0
   38464: 0a000020     	beq	0x384ec
   38468: e3570000     	cmp	r7, #0
   3846c: 1a000020     	bne	0x384f4
   38470: e3a01faa     	mov	r1, #680
   38474: e3a07000     	mov	r7, #0
   38478: e0254591     	mla	r5, r1, r5, r4
   3847c: e2854e29     	add	r4, r5, #656
   38480: e2855fa9     	add	r5, r5, #676
   38484: e5b40004     	ldr	r0, [r4, #0x4]!
   38488: e3a01002     	mov	r1, #2
   3848c: e3500000     	cmp	r0, #0
   38490: 0a000013     	beq	0x384e4
   38494: eb004eb4     	bl	0x4bf6c
   38498: e3a01002     	mov	r1, #2
   3849c: e3500000     	cmp	r0, #0
   384a0: 1a00000f     	bne	0x384e4
   384a4: e594c000     	ldr	r12, [r4]
   384a8: e596e0e8     	ldr	lr, [r6, #0xe8]
   384ac: e1a0000c     	mov	r0, r12
   384b0: e5dc3014     	ldrb	r3, [r12, #0x14]
   384b4: e59c2004     	ldr	r2, [r12, #0x4]
   384b8: e3530000     	cmp	r3, #0
   384bc: edde7a00     	vldr	s15, [lr]
   384c0: e59e3064     	ldr	r3, [lr, #0x64]
   384c4: 1ddc7a06     	vldrne	s15, [r12, #24]
   384c8: e58d7000     	str	r7, [sp]
   384cc: eef57ac0     	vcmpe.f32	s15, #0
   384d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   384d4: a3a0c001     	movge	r12, #1
   384d8: b3a0c000     	movlt	r12, #0
   384dc: e58dc004     	str	r12, [sp, #0x4]
   384e0: eb0057e1     	bl	0x4e46c
   384e4: e1540005     	cmp	r4, r5
   384e8: 1affffe5     	bne	0x38484
   384ec: e28dd00c     	add	sp, sp, #12
   384f0: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   384f4: e1a00004     	mov	r0, r4
   384f8: eb005634     	bl	0x4ddd0
   384fc: e3500002     	cmp	r0, #2
   38500: 1affffda     	bne	0x38470
   38504: eddf0b01     	vldr	d16, [pc, #4]           @ 0x38510 ; float 6.36598738204e-313
   38508: edc60ba4     	vstr	d16, [r6, #656]
   3850c: eaffffd7     	b	0x38470
   38510: b9 00 00 00  	.word	0x000000b9
   38514: 1e 00 00 00  	.word	0x0000001e
