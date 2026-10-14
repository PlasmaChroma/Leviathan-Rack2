0000e5cc <_setPhaseSwitch>:
    e5cc: e5d0203c     	ldrb	r2, [r0, #0x3c]
    e5d0: e3520003     	cmp	r2, #3
    e5d4: 0a000004     	beq	0xe5ec <_setPhaseSwitch+0x20> @ imm = #0x10
    e5d8: e59f31ac     	ldr	r3, [pc, #0x1ac]        @ 0xe78c <_setPhaseSwitch+0x1c0>  // u32=0x18dd0; f32?=1.42708236e-40
    e5dc: e3a01000     	mov	r1, #0
    e5e0: e08f0003     	add	r0, pc, r3
    e5e4: e5c011a4     	strb	r1, [r0, #0x1a4]
    e5e8: e12fff1e     	bx	lr
    e5ec: eddf7a64     	vldr	s15, [pc, #400]         @ 0xe784 <_setPhaseSwitch+0x1b8>  // f32=3800
    e5f0: ed9f7a64     	vldr	s14, [pc, #400]         @ 0xe788 <_setPhaseSwitch+0x1bc>  // f32=300
    e5f4: eeb40ae7     	vcmpe.f32	s0, s15
    e5f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e5fc: eeb40ac7     	vcmpe.f32	s0, s14
    e600: 43a0c001     	movmi	r12, #1
    e604: 53a0c000     	movpl	r12, #0
    e608: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e60c: c20cc001     	andgt	r12, r12, #1
    e610: d3a0c000     	movle	r12, #0
    e614: e35c0000     	cmp	r12, #0
    e618: 1a00002d     	bne	0xe6d4 <_setPhaseSwitch+0x108> @ imm = #0xb4
    e61c: e92d4070     	push	{r4, r5, r6, lr}
    e620: eeb40ae7     	vcmpe.f32	s0, s15
    e624: e59f5164     	ldr	r5, [pc, #0x164]        @ 0xe790 <_setPhaseSwitch+0x1c4>  // u32=0x18d80; f32?=1.42596132e-40
    e628: e24dd010     	sub	sp, sp, #16
    e62c: e1a04000     	mov	r4, r0
    e630: e08f6005     	add	r6, pc, r5
    e634: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e638: e5d631a4     	ldrb	r3, [r6, #0x1a4]
    e63c: ba000029     	blt	0xe6e8 <_setPhaseSwitch+0x11c> @ imm = #0xa4
    e640: e3530001     	cmp	r3, #1
    e644: 0a000043     	beq	0xe758 <_setPhaseSwitch+0x18c> @ imm = #0x10c
    e648: e3530000     	cmp	r3, #0
    e64c: 1a00001e     	bne	0xe6cc <_setPhaseSwitch+0x100> @ imm = #0x78
    e650: e59f113c     	ldr	r1, [pc, #0x13c]        @ 0xe794 <_setPhaseSwitch+0x1c8>  // u32=0x18c8c; f32?=1.42254215e-40
    e654: e59f513c     	ldr	r5, [pc, #0x13c]        @ 0xe798 <_setPhaseSwitch+0x1cc>  // u32=0x18d54; f32?=1.42534475e-40
    e658: e08fc001     	add	r12, pc, r1
    e65c: e08f5005     	add	r5, pc, r5
    e660: e5dc6078     	ldrb	r6, [r12, #0x78]
    e664: e5d521a5     	ldrb	r2, [r5, #0x1a5]
    e668: e1560002     	cmp	r6, r2
    e66c: 0a000016     	beq	0xe6cc <_setPhaseSwitch+0x100> @ imm = #0x58
    e670: e59fe124     	ldr	lr, [pc, #0x124]        @ 0xe79c <_setPhaseSwitch+0x1d0>  // u32=0x7348; f32?=4.13551203e-41
    e674: e3a06002     	mov	r6, #2
    e678: e58d6000     	str	r6, [sp]
    e67c: e08f000e     	add	r0, pc, lr
    e680: ebffd428     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xaf60  // CALL gensym
    e684: e59f3114     	ldr	r3, [pc, #0x114]        @ 0xe7a0 <_setPhaseSwitch+0x1d4>  // u32=0x7348; f32?=4.13551203e-41
    e688: e58d6000     	str	r6, [sp]
    e68c: e58d0004     	str	r0, [sp, #0x4]
    e690: e08f0003     	add	r0, pc, r3
    e694: ebffd423     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xaf74  // CALL gensym
    e698: e5d531a5     	ldrb	r3, [r5, #0x1a5]
    e69c: e5d4c030     	ldrb	r12, [r4, #0x30]
    e6a0: e3a01001     	mov	r1, #1
    e6a4: e58d1008     	str	r1, [sp, #0x8]
    e6a8: ee003a10     	vmov	s0, r3
    e6ac: e35c0062     	cmp	r12, #98
    e6b0: eef80a40     	vcvt.f32.u32	s1, s0
    e6b4: edcd0a03     	vstr	s1, [sp, #12]
    e6b8: e58d0004     	str	r0, [sp, #0x4]
    e6bc: 9a000019     	bls	0xe728 <_setPhaseSwitch+0x15c> @ imm = #0x64
    e6c0: e59f50dc     	ldr	r5, [pc, #0xdc]         @ 0xe7a4 <_setPhaseSwitch+0x1d8>  // u32=0x18c20; f32?=1.42102875e-40
    e6c4: e08f2005     	add	r2, pc, r5
    e6c8: e5c23078     	strb	r3, [r2, #0x78]
    e6cc: e28dd010     	add	sp, sp, #16
    e6d0: e8bd8070     	pop	{r4, r5, r6, pc}
    e6d4: e59fc0cc     	ldr	r12, [pc, #0xcc]        @ 0xe7a8 <_setPhaseSwitch+0x1dc>  // u32=0x18cd4; f32?=1.42355108e-40
    e6d8: e3a00001     	mov	r0, #1
    e6dc: e08f100c     	add	r1, pc, r12
    e6e0: e5c101a4     	strb	r0, [r1, #0x1a4]
    e6e4: e12fff1e     	bx	lr
    e6e8: eeb40ac7     	vcmpe.f32	s0, s14
    e6ec: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e6f0: 8affffd4     	bhi	0xe648 <_setPhaseSwitch+0x7c> @ imm = #-0xb0
    e6f4: e3530001     	cmp	r3, #1
    e6f8: 1affffd2     	bne	0xe648 <_setPhaseSwitch+0x7c> @ imm = #-0xb8
    e6fc: e1a02003     	mov	r2, r3
    e700: e3a01025     	mov	r1, #37
    e704: e5c631a5     	strb	r3, [r6, #0x1a5]
    e708: e5c6c1a4     	strb	r12, [r6, #0x1a4]
    e70c: ebffd4f5     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xac2c  // CALL writeToSharedMem
    e710: e59f2094     	ldr	r2, [pc, #0x94]         @ 0xe7ac <_setPhaseSwitch+0x1e0>  // u32=0x7294; f32?=4.11028866e-41
    e714: e5d611a5     	ldrb	r1, [r6, #0x1a5]
    e718: e08f0002     	add	r0, pc, r2
    e71c: ebffd515     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xabac  // CALL post
    e720: e5d631a4     	ldrb	r3, [r6, #0x1a4]
    e724: eaffffc7     	b	0xe648 <_setPhaseSwitch+0x7c> @ imm = #-0xe4
    e728: e59f0080     	ldr	r0, [pc, #0x80]         @ 0xe7b0 <_setPhaseSwitch+0x1e4>  // u32=0x6384; f32?=3.56994797e-41
    e72c: e2844a01     	add	r4, r4, #4096
    e730: e08f0000     	add	r0, pc, r0
    e734: e5944db4     	ldr	r4, [r4, #0xdb4]
    e738: ebffd3fa     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xb018  // CALL gensym
    e73c: e1a0300d     	mov	r3, sp
    e740: e1a02006     	mov	r2, r6
    e744: e1a01000     	mov	r1, r0
    e748: e1a00004     	mov	r0, r4
    e74c: ebffd548     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xaae0  // CALL outlet_list
    e750: e5d531a5     	ldrb	r3, [r5, #0x1a5]
    e754: eaffffd9     	b	0xe6c0 <_setPhaseSwitch+0xf4> @ imm = #-0x9c
    e758: e1a0200c     	mov	r2, r12
    e75c: e3a01025     	mov	r1, #37
    e760: e5c6c1a5     	strb	r12, [r6, #0x1a5]
    e764: e5c6c1a4     	strb	r12, [r6, #0x1a4]
    e768: ebffd4de     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xac88  // CALL writeToSharedMem
    e76c: e59f0040     	ldr	r0, [pc, #0x40]         @ 0xe7b4 <_setPhaseSwitch+0x1e8>  // u32=0x7238; f32?=4.09739671e-41
    e770: e5d611a5     	ldrb	r1, [r6, #0x1a5]
    e774: e08f0000     	add	r0, pc, r0
    e778: ebffd4fe     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xac08  // CALL post
    e77c: e5d631a4     	ldrb	r3, [r6, #0x1a4]
    e780: eaffffb0     	b	0xe648 <_setPhaseSwitch+0x7c> @ imm = #-0x140
    e784: 00 80 6d 45  	.word	0x456d8000
    e788: 00 00 96 43  	.word	0x43960000
    e78c: d0 8d 01 00  	.word	0x00018dd0
    e790: 80 8d 01 00  	.word	0x00018d80
    e794: 8c 8c 01 00  	.word	0x00018c8c
    e798: 54 8d 01 00  	.word	0x00018d54
    e79c: 48 73 00 00  	.word	0x00007348
    e7a0: 48 73 00 00  	.word	0x00007348
    e7a4: 20 8c 01 00  	.word	0x00018c20
    e7a8: d4 8c 01 00  	.word	0x00018cd4
    e7ac: 94 72 00 00  	.word	0x00007294
    e7b0: 84 63 00 00  	.word	0x00006384
    e7b4: 38 72 00 00  	.word	0x00007238

