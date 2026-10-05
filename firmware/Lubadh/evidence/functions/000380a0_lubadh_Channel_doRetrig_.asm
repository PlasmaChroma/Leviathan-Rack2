; lubadh::Channel::doRetrig()
; VA 0x380a0 size 532

   380a0: e92d40f0     	push	{r4, r5, r6, r7, lr}
   380a4: e1a04000     	mov	r4, r0
   380a8: e24dd014     	sub	sp, sp, #20
   380ac: e28d0008     	add	r0, sp, #8
   380b0: ebff77ef     	bl	0x16074    @ imm = #-0x22044 ; _ZNSt6chrono3_V212steady_clock3nowEv
   380b4: e1cd00d8     	ldrd	r0, r1, [sp, #8]
   380b8: e2843f8e     	add	r3, r4, #568
   380bc: e1c320d0     	ldrd	r2, r3, [r3]
   380c0: e28f7e1e     	add	r7, pc, #480
   380c4: e1c760d0     	ldrd	r6, r7, [r7]
   380c8: e0502002     	subs	r2, r0, r2
   380cc: e0c13003     	sbc	r3, r1, r3
   380d0: e1560002     	cmp	r6, r2
   380d4: e0d73003     	sbcs	r3, r7, r3
   380d8: aa000016     	bge	0x38138
   380dc: e59430e8     	ldr	r3, [r4, #0xe8]
   380e0: ed9f7a72     	vldr	s14, [pc, #456]         @ 0x382b0 ; float 0.10000000149
   380e4: e5840238     	str	r0, [r4, #0x238]
   380e8: e584123c     	str	r1, [r4, #0x23c]
   380ec: edd37a00     	vldr	s15, [r3]
   380f0: eef06ae7     	vabs.f32	s13, s15
   380f4: eef46ac7     	vcmpe.f32	s13, s14
   380f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   380fc: 5a000002     	bpl	0x3810c
   38100: e5932094     	ldr	r2, [r3, #0x94]
   38104: e3520001     	cmp	r2, #1
   38108: 0a00000a     	beq	0x38138
   3810c: eef57ac0     	vcmpe.f32	s15, #0
   38110: e5942278     	ldr	r2, [r4, #0x278]
   38114: e5922004     	ldr	r2, [r2, #0x4]
   38118: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3811c: e2422002     	sub	r2, r2, #2
   38120: a5937014     	ldrge	r7, [r3, #0x14]
   38124: a3a06001     	movge	r6, #1
   38128: b5937020     	ldrlt	r7, [r3, #0x20]
   3812c: b3a06000     	movlt	r6, #0
   38130: e3520001     	cmp	r2, #1
   38134: 9a000001     	bls	0x38140
   38138: e28dd014     	add	sp, sp, #20
   3813c: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   38140: e5931090     	ldr	r1, [r3, #0x90]
   38144: e2845fda     	add	r5, r4, #872
   38148: e5932064     	ldr	r2, [r3, #0x64]
   3814c: e3510000     	cmp	r1, #0
   38150: 0a000045     	beq	0x3826c
   38154: e1a00005     	mov	r0, r5
   38158: e1a03006     	mov	r3, r6
   3815c: e1a01007     	mov	r1, r7
   38160: eb005b50     	bl	0x4eea8
   38164: e2505000     	subs	r5, r0, #0
   38168: 0a000012     	beq	0x381b8
   3816c: e59410e8     	ldr	r1, [r4, #0xe8]
   38170: e5913094     	ldr	r3, [r1, #0x94]
   38174: e3530001     	cmp	r3, #1
   38178: 0a000042     	beq	0x38288
   3817c: e5d53014     	ldrb	r3, [r5, #0x14]
   38180: e1a00005     	mov	r0, r5
   38184: edd17a00     	vldr	s15, [r1]
   38188: e3530000     	cmp	r3, #0
   3818c: e5913064     	ldr	r3, [r1, #0x64]
   38190: e3a01001     	mov	r1, #1
   38194: e5952004     	ldr	r2, [r5, #0x4]
   38198: 1dd57a06     	vldrne	s15, [r5, #24]
   3819c: e58d1000     	str	r1, [sp]
   381a0: eef57ac0     	vcmpe.f32	s15, #0
   381a4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   381a8: a1a0c001     	movge	r12, r1
   381ac: b3a0c000     	movlt	r12, #0
   381b0: e58dc004     	str	r12, [sp, #0x4]
   381b4: eb0058ac     	bl	0x4e46c
   381b8: e5943278     	ldr	r3, [r4, #0x278]
   381bc: e5933004     	ldr	r3, [r3, #0x4]
   381c0: e3530002     	cmp	r3, #2
   381c4: 1affffdb     	bne	0x38138
   381c8: e59430e8     	ldr	r3, [r4, #0xe8]
   381cc: e59320a0     	ldr	r2, [r3, #0xa0]
   381d0: e3520000     	cmp	r2, #0
   381d4: 1affffd7     	bne	0x38138
   381d8: e5932090     	ldr	r2, [r3, #0x90]
   381dc: e3520000     	cmp	r2, #0
   381e0: 1affffd4     	bne	0x38138
   381e4: e2845ee6     	add	r5, r4, #3680
   381e8: e5931064     	ldr	r1, [r3, #0x64]
   381ec: e2855008     	add	r5, r5, #8
   381f0: e1a02006     	mov	r2, r6
   381f4: e1a00005     	mov	r0, r5
   381f8: eb005ae0     	bl	0x4ed80
   381fc: e59420e8     	ldr	r2, [r4, #0xe8]
   38200: e1a00005     	mov	r0, r5
   38204: e1a03006     	mov	r3, r6
   38208: e1a01007     	mov	r1, r7
   3820c: e5922064     	ldr	r2, [r2, #0x64]
   38210: eb005b24     	bl	0x4eea8
   38214: e2505000     	subs	r5, r0, #0
   38218: 0affffc6     	beq	0x38138
   3821c: e59410e8     	ldr	r1, [r4, #0xe8]
   38220: e59130a0     	ldr	r3, [r1, #0xa0]
   38224: e3530001     	cmp	r3, #1
   38228: 0a00001a     	beq	0x38298
   3822c: e5d53014     	ldrb	r3, [r5, #0x14]
   38230: e1a00005     	mov	r0, r5
   38234: edd17a00     	vldr	s15, [r1]
   38238: e3530000     	cmp	r3, #0
   3823c: e5913064     	ldr	r3, [r1, #0x64]
   38240: e3a01001     	mov	r1, #1
   38244: e5952004     	ldr	r2, [r5, #0x4]
   38248: 1dd57a06     	vldrne	s15, [r5, #24]
   3824c: e58d1000     	str	r1, [sp]
   38250: eef57ac0     	vcmpe.f32	s15, #0
   38254: eef1fa10     	vmrs	APSR_nzcv, fpscr
   38258: a1a0c001     	movge	r12, r1
   3825c: b3a0c000     	movlt	r12, #0
   38260: e58dc004     	str	r12, [sp, #0x4]
   38264: eb005880     	bl	0x4e46c
   38268: eaffffb2     	b	0x38138
   3826c: e1a01002     	mov	r1, r2
   38270: e1a00005     	mov	r0, r5
   38274: e1a02006     	mov	r2, r6
   38278: eb005ac0     	bl	0x4ed80
   3827c: e59430e8     	ldr	r3, [r4, #0xe8]
   38280: e5932064     	ldr	r2, [r3, #0x64]
   38284: eaffffb2     	b	0x38154
   38288: ed910a02     	vldr	s0, [r1, #8]
   3828c: eb004f22     	bl	0x4bf1c
   38290: e59410e8     	ldr	r1, [r4, #0xe8]
   38294: eaffffb8     	b	0x3817c
   38298: eeb70a00     	vmov.f32	s0, #1.000000e+00
   3829c: eb004f1e     	bl	0x4bf1c
   382a0: e59410e8     	ldr	r1, [r4, #0xe8]
   382a4: eaffffe0     	b	0x3822c
   382a8: ff 2c 31 01  	.word	0x01312cff
   382ac: 00 00 00 00  	.word	0x00000000
   382b0: cd cc cc 3d  	.word	0x3dcccccd
