; lubadh::Channel::getDisplayPosition() const
; VA 0x385cc size 216

   385cc: e5903278     	ldr	r3, [r0, #0x278]
   385d0: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   385d4: e1a04000     	mov	r4, r0
   385d8: e5933004     	ldr	r3, [r3, #0x4]
   385dc: e3530002     	cmp	r3, #2
   385e0: 02805ee6     	addeq	r5, r0, #3680
   385e4: 12805fda     	addne	r5, r0, #872
   385e8: 02855008     	addeq	r5, r5, #8
   385ec: e1a00005     	mov	r0, r5
   385f0: eb005543     	bl	0x4db04
   385f4: e3500000     	cmp	r0, #0
   385f8: 1a00000b     	bne	0x3862c
   385fc: e59430e8     	ldr	r3, [r4, #0xe8]
   38600: edd37a00     	vldr	s15, [r3]
   38604: eef57ac0     	vcmpe.f32	s15, #0
   38608: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3860c: ba000001     	blt	0x38618
   38610: e5930014     	ldr	r0, [r3, #0x14]
   38614: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   38618: e59320a0     	ldr	r2, [r3, #0xa0]
   3861c: e3520001     	cmp	r2, #1
   38620: 0a000012     	beq	0x38670
   38624: e593001c     	ldr	r0, [r3, #0x1c]
   38628: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   3862c: e1a00005     	mov	r0, r5
   38630: eb0055f0     	bl	0x4ddf8
   38634: e1a07000     	mov	r7, r0
   38638: e1a01007     	mov	r1, r7
   3863c: e1a00005     	mov	r0, r5
   38640: eb005493     	bl	0x4d894
   38644: e5d03014     	ldrb	r3, [r0, #0x14]
   38648: e59420e8     	ldr	r2, [r4, #0xe8]
   3864c: e1a06000     	mov	r6, r0
   38650: e3530000     	cmp	r3, #0
   38654: edd27a00     	vldr	s15, [r2]
   38658: 1dd07a06     	vldrne	s15, [r0, #24]
   3865c: eef57ac0     	vcmpe.f32	s15, #0
   38660: eef1fa10     	vmrs	APSR_nzcv, fpscr
   38664: 4a000006     	bmi	0x38684
   38668: e5960004     	ldr	r0, [r6, #0x4]
   3866c: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   38670: e2844ee6     	add	r4, r4, #3680
   38674: e2844008     	add	r4, r4, #8
   38678: e1550004     	cmp	r5, r4
   3867c: 1affffe8     	bne	0x38624
   38680: eaffffe2     	b	0x38610
   38684: eb004e2d     	bl	0x4bf40
   38688: e3500000     	cmp	r0, #0
   3868c: 0afffff5     	beq	0x38668
   38690: e1a01007     	mov	r1, r7
   38694: e1a00005     	mov	r0, r5
   38698: eb0054b0     	bl	0x4d960
   3869c: e5900004     	ldr	r0, [r0, #0x4]
   386a0: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
