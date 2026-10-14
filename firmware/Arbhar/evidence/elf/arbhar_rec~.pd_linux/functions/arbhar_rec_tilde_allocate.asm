00003970 <arbhar_rec_tilde_allocate>:
    3970: eefd7ac0     	vcvt.s32.f32	s15, s0
    3974: e92d4030     	push	{r4, r5, lr}
    3978: e24dd05c     	sub	sp, sp, #92
    397c: ee175a90     	vmov	r5, s15
    3980: e3550000     	cmp	r5, #0
    3984: da00003a     	ble	0x3a74 <arbhar_rec_tilde_allocate+0x104> @ imm = #0xe8
    3988: e1c11fc1     	bic	r1, r1, r1, asr #31
    398c: eefd0ae0     	vcvt.s32.f32	s1, s1
    3990: e0804101     	add	r4, r0, r1, lsl #2
    3994: e59401e8     	ldr	r0, [r4, #0x1e8]
    3998: e3500000     	cmp	r0, #0
    399c: edc40a87     	vstr	s1, [r4, #540]
    39a0: 0a000002     	beq	0x39b0 <arbhar_rec_tilde_allocate+0x40> @ imm = #0x8
    39a4: ebfffb73     	bl	0x2778 <.plt+0x278>     @ imm = #-0x1234
    39a8: e3a03000     	mov	r3, #0
    39ac: e58431e8     	str	r3, [r4, #0x1e8]
    39b0: e59401b4     	ldr	r0, [r4, #0x1b4]
    39b4: e3700001     	cmn	r0, #1
    39b8: 1a000015     	bne	0x3a14 <arbhar_rec_tilde_allocate+0xa4> @ imm = #0x54
    39bc: e594c21c     	ldr	r12, [r4, #0x21c]
    39c0: e3002386     	movw	r2, #0x386
    39c4: e1a00005     	mov	r0, r5
    39c8: e1a0110c     	lsl	r1, r12, #2
    39cc: ebfffb06     	bl	0x25ec <.plt+0xec>      @ imm = #-0x13e8
    39d0: e3700001     	cmn	r0, #1
    39d4: e58401b4     	str	r0, [r4, #0x1b4]
    39d8: 0a000013     	beq	0x3a2c <arbhar_rec_tilde_allocate+0xbc> @ imm = #0x4c
    39dc: e3a02000     	mov	r2, #0
    39e0: e1a01002     	mov	r1, r2
    39e4: ebfffb09     	bl	0x2610 <.plt+0x110>     @ imm = #-0x13dc
    39e8: e28d2004     	add	r2, sp, #4
    39ec: e3a01002     	mov	r1, #2
    39f0: e58401e8     	str	r0, [r4, #0x1e8]
    39f4: e59401b4     	ldr	r0, [r4, #0x1b4]
    39f8: ebfffb19     	bl	0x2664 <.plt+0x164>     @ imm = #-0x139c
    39fc: e594221c     	ldr	r2, [r4, #0x21c]
    3a00: e59d1028     	ldr	r1, [sp, #0x28]
    3a04: e1510102     	cmp	r1, r2, lsl #2
    3a08: ba000010     	blt	0x3a50 <arbhar_rec_tilde_allocate+0xe0> @ imm = #0x40
    3a0c: e28dd05c     	add	sp, sp, #92
    3a10: e8bd8030     	pop	{r4, r5, pc}
    3a14: e3a02000     	mov	r2, #0
    3a18: e1a01002     	mov	r1, r2
    3a1c: ebfffb10     	bl	0x2664 <.plt+0x164>     @ imm = #-0x13c0
    3a20: e3e00000     	mvn	r0, #0
    3a24: e58401b4     	str	r0, [r4, #0x1b4]
    3a28: eaffffe3     	b	0x39bc <arbhar_rec_tilde_allocate+0x4c> @ imm = #-0x74
    3a2c: e59fe054     	ldr	lr, [pc, #0x54]         @ 0x3a88 <arbhar_rec_tilde_allocate+0x118>
    3a30: e1a01005     	mov	r1, r5
    3a34: e594221c     	ldr	r2, [r4, #0x21c]
    3a38: e3a05000     	mov	r5, #0
    3a3c: e08f000e     	add	r0, pc, lr
    3a40: ebfffb0a     	bl	0x2670 <.plt+0x170>     @ imm = #-0x13d8
    3a44: e584521c     	str	r5, [r4, #0x21c]
    3a48: e28dd05c     	add	sp, sp, #92
    3a4c: e8bd8030     	pop	{r4, r5, pc}
    3a50: e59f3034     	ldr	r3, [pc, #0x34]         @ 0x3a8c <arbhar_rec_tilde_allocate+0x11c>
    3a54: e1a01005     	mov	r1, r5
    3a58: e08f0003     	add	r0, pc, r3
    3a5c: ebfffb03     	bl	0x2670 <.plt+0x170>     @ imm = #-0x13f4
    3a60: e3a00000     	mov	r0, #0
    3a64: e584021c     	str	r0, [r4, #0x21c]
    3a68: e58401e8     	str	r0, [r4, #0x1e8]
    3a6c: e28dd05c     	add	sp, sp, #92
    3a70: e8bd8030     	pop	{r4, r5, pc}
    3a74: e59f4014     	ldr	r4, [pc, #0x14]         @ 0x3a90 <arbhar_rec_tilde_allocate+0x120>
    3a78: e08f0004     	add	r0, pc, r4
    3a7c: ebfffafb     	bl	0x2670 <.plt+0x170>     @ imm = #-0x1414
    3a80: e28dd05c     	add	sp, sp, #92
    3a84: e8bd8030     	pop	{r4, r5, pc}
    3a88: a4 5b 00 00  	.word	0x00005ba4
    3a8c: 88 5b 00 00  	.word	0x00005b88
    3a90: 54 5b 00 00  	.word	0x00005b54

