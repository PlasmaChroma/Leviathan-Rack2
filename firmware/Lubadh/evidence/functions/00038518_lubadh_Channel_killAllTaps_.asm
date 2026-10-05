; lubadh::Channel::killAllTaps()
; VA 0x38518 size 180

   38518: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   3851c: e2803ee6     	add	r3, r0, #3680
   38520: e1a06000     	mov	r6, r0
   38524: e24dd010     	sub	sp, sp, #16
   38528: e3a07000     	mov	r7, #0
   3852c: e28d8008     	add	r8, sp, #8
   38530: e28d9010     	add	r9, sp, #16
   38534: e2833008     	add	r3, r3, #8
   38538: e58d300c     	str	r3, [sp, #0xc]
   3853c: e2803fda     	add	r3, r0, #872
   38540: e58d3008     	str	r3, [sp, #0x8]
   38544: e4980004     	ldr	r0, [r8], #4
   38548: eb00550d     	bl	0x4d984
   3854c: e2404004     	sub	r4, r0, #4
   38550: e280504c     	add	r5, r0, #76
   38554: e5b40004     	ldr	r0, [r4, #0x4]!
   38558: e3a01002     	mov	r1, #2
   3855c: e3500000     	cmp	r0, #0
   38560: 0a000013     	beq	0x385b4
   38564: eb004e80     	bl	0x4bf6c
   38568: e3a01002     	mov	r1, #2
   3856c: e3500000     	cmp	r0, #0
   38570: e30031eb     	movw	r3, #0x1eb
   38574: 1a00000e     	bne	0x385b4
   38578: e594c000     	ldr	r12, [r4]
   3857c: e596e0e8     	ldr	lr, [r6, #0xe8]
   38580: e1a0000c     	mov	r0, r12
   38584: e5dca014     	ldrb	r10, [r12, #0x14]
   38588: e59c2004     	ldr	r2, [r12, #0x4]
   3858c: e35a0000     	cmp	r10, #0
   38590: edde7a00     	vldr	s15, [lr]
   38594: 1ddc7a06     	vldrne	s15, [r12, #24]
   38598: e58d7000     	str	r7, [sp]
   3859c: eef57ac0     	vcmpe.f32	s15, #0
   385a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   385a4: a3a0c001     	movge	r12, #1
   385a8: b3a0c000     	movlt	r12, #0
   385ac: e58dc004     	str	r12, [sp, #0x4]
   385b0: eb0057ad     	bl	0x4e46c
   385b4: e1540005     	cmp	r4, r5
   385b8: 1affffe5     	bne	0x38554
   385bc: e1580009     	cmp	r8, r9
   385c0: 1affffdf     	bne	0x38544
   385c4: e28dd010     	add	sp, sp, #16
   385c8: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
