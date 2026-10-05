; lubadh::Channel::doResync()
; VA 0x382b4 size 384

   382b4: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   382b8: e1a07000     	mov	r7, r0
   382bc: e3a0b001     	mov	r11, #1
   382c0: e2809fda     	add	r9, r0, #872
   382c4: e24dd014     	sub	sp, sp, #20
   382c8: e59730e8     	ldr	r3, [r7, #0xe8]
   382cc: e1a00009     	mov	r0, r9
   382d0: e5931064     	ldr	r1, [r3, #0x64]
   382d4: edd37a00     	vldr	s15, [r3]
   382d8: eef57ac0     	vcmpe.f32	s15, #0
   382dc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   382e0: a1a0200b     	movge	r2, r11
   382e4: b3a02000     	movlt	r2, #0
   382e8: eb005aa4     	bl	0x4ed80
   382ec: e597a01c     	ldr	r10, [r7, #0x1c]
   382f0: e28a8ee1     	add	r8, r10, #3600
   382f4: e28aac0e     	add	r10, r10, #3584
   382f8: e2888004     	add	r8, r8, #4
   382fc: e28aa004     	add	r10, r10, #4
   38300: e4185004     	ldr	r5, [r8], #-4
   38304: e3550000     	cmp	r5, #0
   38308: 12854faa     	addne	r4, r5, #680
   3830c: 13a00001     	movne	r0, #1
   38310: 12855fa5     	addne	r5, r5, #660
   38314: 0a000040     	beq	0x3841c
   38318: e5143004     	ldr	r3, [r4, #-0x4]
   3831c: e3530000     	cmp	r3, #0
   38320: 0a000039     	beq	0x3840c
   38324: e5931004     	ldr	r1, [r3, #0x4]
   38328: e3500000     	cmp	r0, #0
   3832c: 0a00003e     	beq	0x3842c
   38330: e5d3c014     	ldrb	r12, [r3, #0x14]
   38334: e2446004     	sub	r6, r4, #4
   38338: e59700e8     	ldr	r0, [r7, #0xe8]
   3833c: e35c0000     	cmp	r12, #0
   38340: e5902064     	ldr	r2, [r0, #0x64]
   38344: edd07a00     	vldr	s15, [r0]
   38348: e1a00009     	mov	r0, r9
   3834c: 1dd37a06     	vldrne	s15, [r3, #24]
   38350: eef57ac0     	vcmpe.f32	s15, #0
   38354: eef1fa10     	vmrs	APSR_nzcv, fpscr
   38358: a3a03001     	movge	r3, #1
   3835c: b3a03000     	movlt	r3, #0
   38360: eb005ad0     	bl	0x4eea8
   38364: e3500000     	cmp	r0, #0
   38368: 0a000020     	beq	0x383f0
   3836c: e5141004     	ldr	r1, [r4, #-0x4]
   38370: e58d000c     	str	r0, [sp, #0xc]
   38374: eb004e7c     	bl	0x4bd6c
   38378: e59d000c     	ldr	r0, [sp, #0xc]
   3837c: e59710e8     	ldr	r1, [r7, #0xe8]
   38380: e5d0c014     	ldrb	r12, [r0, #0x14]
   38384: e5913064     	ldr	r3, [r1, #0x64]
   38388: e35c0000     	cmp	r12, #0
   3838c: edd17a00     	vldr	s15, [r1]
   38390: e5902004     	ldr	r2, [r0, #0x4]
   38394: e3a01003     	mov	r1, #3
   38398: 1dd07a06     	vldrne	s15, [r0, #24]
   3839c: e58db000     	str	r11, [sp]
   383a0: eef57ac0     	vcmpe.f32	s15, #0
   383a4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   383a8: a3a0c001     	movge	r12, #1
   383ac: b3a0c000     	movlt	r12, #0
   383b0: e58dc004     	str	r12, [sp, #0x4]
   383b4: eb00582c     	bl	0x4e46c
   383b8: e1550006     	cmp	r5, r6
   383bc: 0a000016     	beq	0x3841c
   383c0: e5140008     	ldr	r0, [r4, #-0x8]
   383c4: e3500000     	cmp	r0, #0
   383c8: 0a000010     	beq	0x38410
   383cc: e5901004     	ldr	r1, [r0, #0x4]
   383d0: e5983004     	ldr	r3, [r8, #0x4]
   383d4: e1a00009     	mov	r0, r9
   383d8: e1a04006     	mov	r4, r6
   383dc: e2446004     	sub	r6, r4, #4
   383e0: e593207c     	ldr	r2, [r3, #0x7c]
   383e4: eb00581d     	bl	0x4e460
   383e8: e3500000     	cmp	r0, #0
   383ec: 1affffde     	bne	0x3836c
   383f0: e1550006     	cmp	r5, r6
   383f4: 0a000008     	beq	0x3841c
   383f8: e5143008     	ldr	r3, [r4, #-0x8]
   383fc: e3530000     	cmp	r3, #0
   38400: 0a000002     	beq	0x38410
   38404: e5931004     	ldr	r1, [r3, #0x4]
   38408: eafffff0     	b	0x383d0
   3840c: e1a06004     	mov	r6, r4
   38410: e2464004     	sub	r4, r6, #4
   38414: e1540005     	cmp	r4, r5
   38418: 1affffbe     	bne	0x38318
   3841c: e158000a     	cmp	r8, r10
   38420: 1affffb6     	bne	0x38300
   38424: e28dd014     	add	sp, sp, #20
   38428: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   3842c: e1a06004     	mov	r6, r4
   38430: eaffffe6     	b	0x383d0
