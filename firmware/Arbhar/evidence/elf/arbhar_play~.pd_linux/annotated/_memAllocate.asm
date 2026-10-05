00006074 <_memAllocate>:
    6074: eefd7ac0     	vcvt.s32.f32	s15, s0
    6078: e92d4030     	push	{r4, r5, lr}
    607c: e24dd05c     	sub	sp, sp, #92
    6080: ee175a90     	vmov	r5, s15
    6084: e3550000     	cmp	r5, #0
    6088: da00003b     	ble	0x617c <_memAllocate+0x108> @ imm = #0xec
    608c: eefd0ae0     	vcvt.s32.f32	s1, s1
    6090: e0800101     	add	r0, r0, r1, lsl #2
    6094: e2804a02     	add	r4, r0, #8192
    6098: e5940894     	ldr	r0, [r4, #0x894]
    609c: e3500000     	cmp	r0, #0
    60a0: ee103a90     	vmov	r3, s1
    60a4: e58438c8     	str	r3, [r4, #0x8c8]
    60a8: 0a000002     	beq	0x60b8 <_memAllocate+0x44> @ imm = #0x8
    60ac: ebfff163     	bl	0x2640 <.plt+0x260>     @ imm = #-0x3a74  // CALL shmdt
    60b0: e3a01000     	mov	r1, #0
    60b4: e5841894     	str	r1, [r4, #0x894]
    60b8: e5940860     	ldr	r0, [r4, #0x860]
    60bc: e3700001     	cmn	r0, #1
    60c0: 1a000015     	bne	0x611c <_memAllocate+0xa8> @ imm = #0x54
    60c4: e594c8c8     	ldr	r12, [r4, #0x8c8]
    60c8: e3002386     	movw	r2, #0x386
    60cc: e1a00005     	mov	r0, r5
    60d0: e1a0110c     	lsl	r1, r12, #2
    60d4: ebfff0f3     	bl	0x24a8 <.plt+0xc8>      @ imm = #-0x3c34  // CALL shmget
    60d8: e3700001     	cmn	r0, #1
    60dc: e5840860     	str	r0, [r4, #0x860]
    60e0: 0a000013     	beq	0x6134 <_memAllocate+0xc0> @ imm = #0x4c
    60e4: e3a02000     	mov	r2, #0
    60e8: e1a01002     	mov	r1, r2
    60ec: ebfff0f9     	bl	0x24d8 <.plt+0xf8>      @ imm = #-0x3c1c  // CALL shmat
    60f0: e28d2004     	add	r2, sp, #4
    60f4: e3a01002     	mov	r1, #2
    60f8: e5840894     	str	r0, [r4, #0x894]
    60fc: e5940860     	ldr	r0, [r4, #0x860]
    6100: ebfff10c     	bl	0x2538 <.plt+0x158>     @ imm = #-0x3bd0  // CALL shmctl
    6104: e59428c8     	ldr	r2, [r4, #0x8c8]
    6108: e59d0028     	ldr	r0, [sp, #0x28]
    610c: e1500102     	cmp	r0, r2, lsl #2
    6110: ba000010     	blt	0x6158 <_memAllocate+0xe4> @ imm = #0x40
    6114: e28dd05c     	add	sp, sp, #92
    6118: e8bd8030     	pop	{r4, r5, pc}
    611c: e3a02000     	mov	r2, #0
    6120: e1a01002     	mov	r1, r2
    6124: ebfff103     	bl	0x2538 <.plt+0x158>     @ imm = #-0x3bf4  // CALL shmctl
    6128: e3e02000     	mvn	r2, #0
    612c: e5842860     	str	r2, [r4, #0x860]
    6130: eaffffe3     	b	0x60c4 <_memAllocate+0x50> @ imm = #-0x74
    6134: e59fe054     	ldr	lr, [pc, #0x54]         @ 0x6190 <_memAllocate+0x11c>  // u32=0x6b7c; f32?=3.85581285e-41
    6138: e1a01005     	mov	r1, r5
    613c: e59428c8     	ldr	r2, [r4, #0x8c8]
    6140: e3a05000     	mov	r5, #0
    6144: e08f000e     	add	r0, pc, lr
    6148: ebfff0fd     	bl	0x2544 <.plt+0x164>     @ imm = #-0x3c0c  // CALL error
    614c: e58458c8     	str	r5, [r4, #0x8c8]
    6150: e28dd05c     	add	sp, sp, #92
    6154: e8bd8030     	pop	{r4, r5, pc}
    6158: e59fc034     	ldr	r12, [pc, #0x34]        @ 0x6194 <_memAllocate+0x120>  // u32=0x6b60; f32?=3.85188922e-41
    615c: e1a01005     	mov	r1, r5
    6160: e08f000c     	add	r0, pc, r12
    6164: ebfff0f6     	bl	0x2544 <.plt+0x164>     @ imm = #-0x3c28  // CALL error
    6168: e3a01000     	mov	r1, #0
    616c: e58418c8     	str	r1, [r4, #0x8c8]
    6170: e5841894     	str	r1, [r4, #0x894]
    6174: e28dd05c     	add	sp, sp, #92
    6178: e8bd8030     	pop	{r4, r5, pc}
    617c: e59f4014     	ldr	r4, [pc, #0x14]         @ 0x6198 <_memAllocate+0x124>  // u32=0x6b2c; f32?=3.84460247e-41
    6180: e08f0004     	add	r0, pc, r4
    6184: ebfff0ee     	bl	0x2544 <.plt+0x164>     @ imm = #-0x3c48  // CALL error
    6188: e28dd05c     	add	sp, sp, #92
    618c: e8bd8030     	pop	{r4, r5, pc}
    6190: 7c 6b 00 00  	.word	0x00006b7c
    6194: 60 6b 00 00  	.word	0x00006b60
    6198: 2c 6b 00 00  	.word	0x00006b2c

