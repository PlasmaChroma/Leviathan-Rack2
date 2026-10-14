0000e7b8 <_setStereoSwitch>:
    e7b8: e5d0203c     	ldrb	r2, [r0, #0x3c]
    e7bc: e3520003     	cmp	r2, #3
    e7c0: 0a000004     	beq	0xe7d8 <_setStereoSwitch+0x20> @ imm = #0x10
    e7c4: e59f3180     	ldr	r3, [pc, #0x180]        @ 0xe94c <_setStereoSwitch+0x194>  // u32=0x18be4; f32?=1.42018797e-40
    e7c8: e3a01000     	mov	r1, #0
    e7cc: e08f0003     	add	r0, pc, r3
    e7d0: e5c011a6     	strb	r1, [r0, #0x1a6]
    e7d4: e12fff1e     	bx	lr
    e7d8: eddf7a59     	vldr	s15, [pc, #356]         @ 0xe944 <_setStereoSwitch+0x18c>  // f32=3800
    e7dc: ed9f7a59     	vldr	s14, [pc, #356]         @ 0xe948 <_setStereoSwitch+0x190>  // f32=300
    e7e0: eeb40ae7     	vcmpe.f32	s0, s15
    e7e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e7e8: eeb40ac7     	vcmpe.f32	s0, s14
    e7ec: 43a0c001     	movmi	r12, #1
    e7f0: 53a0c000     	movpl	r12, #0
    e7f4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e7f8: c20cc001     	andgt	r12, r12, #1
    e7fc: d3a0c000     	movle	r12, #0
    e800: e35c0000     	cmp	r12, #0
    e804: 1a000028     	bne	0xe8ac <_setStereoSwitch+0xf4> @ imm = #0xa0
    e808: e92d4070     	push	{r4, r5, r6, lr}
    e80c: eeb40ae7     	vcmpe.f32	s0, s15
    e810: e59f5138     	ldr	r5, [pc, #0x138]        @ 0xe950 <_setStereoSwitch+0x198>  // u32=0x18b94; f32?=1.41906693e-40
    e814: e24dd010     	sub	sp, sp, #16
    e818: e1a04000     	mov	r4, r0
    e81c: e08f6005     	add	r6, pc, r5
    e820: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e824: e5d631a6     	ldrb	r3, [r6, #0x1a6]
    e828: ba000024     	blt	0xe8c0 <_setStereoSwitch+0x108> @ imm = #0x90
    e82c: e3530001     	cmp	r3, #1
    e830: 0a00003b     	beq	0xe924 <_setStereoSwitch+0x16c> @ imm = #0xec
    e834: e3530000     	cmp	r3, #0
    e838: 1a000019     	bne	0xe8a4 <_setStereoSwitch+0xec> @ imm = #0x64
    e83c: e59f2110     	ldr	r2, [pc, #0x110]        @ 0xe954 <_setStereoSwitch+0x19c>  // u32=0x18aa0; f32?=1.41564776e-40
    e840: e59f1110     	ldr	r1, [pc, #0x110]        @ 0xe958 <_setStereoSwitch+0x1a0>  // u32=0x18b68; f32?=1.41845036e-40
    e844: e08f0002     	add	r0, pc, r2
    e848: e08f5001     	add	r5, pc, r1
    e84c: e5d0c079     	ldrb	r12, [r0, #0x79]
    e850: e5d561a7     	ldrb	r6, [r5, #0x1a7]
    e854: e15c0006     	cmp	r12, r6
    e858: 0a000011     	beq	0xe8a4 <_setStereoSwitch+0xec> @ imm = #0x44
    e85c: e59fe0f8     	ldr	lr, [pc, #0xf8]         @ 0xe95c <_setStereoSwitch+0x1a4>  // u32=0x717c; f32?=4.0710523e-41
    e860: e3a06002     	mov	r6, #2
    e864: e58d6000     	str	r6, [sp]
    e868: e08f000e     	add	r0, pc, lr
    e86c: ebffd3ad     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xb14c  // CALL gensym
    e870: e5d5c1a7     	ldrb	r12, [r5, #0x1a7]
    e874: e5d41030     	ldrb	r1, [r4, #0x30]
    e878: e3a02001     	mov	r2, #1
    e87c: e58d2008     	str	r2, [sp, #0x8]
    e880: ee00ca10     	vmov	s0, r12
    e884: e3510062     	cmp	r1, #98
    e888: eef80a40     	vcvt.f32.u32	s1, s0
    e88c: edcd0a03     	vstr	s1, [sp, #12]
    e890: e58d0004     	str	r0, [sp, #0x4]
    e894: 9a000016     	bls	0xe8f4 <_setStereoSwitch+0x13c> @ imm = #0x58
    e898: e59f00c0     	ldr	r0, [pc, #0xc0]         @ 0xe960 <_setStereoSwitch+0x1a8>  // u32=0x18a48; f32?=1.41441462e-40
    e89c: e08f5000     	add	r5, pc, r0
    e8a0: e5c5c079     	strb	r12, [r5, #0x79]
    e8a4: e28dd010     	add	sp, sp, #16
    e8a8: e8bd8070     	pop	{r4, r5, r6, pc}
    e8ac: e59f20b0     	ldr	r2, [pc, #0xb0]         @ 0xe964 <_setStereoSwitch+0x1ac>  // u32=0x18afc; f32?=1.41693696e-40
    e8b0: e3a03001     	mov	r3, #1
    e8b4: e08f1002     	add	r1, pc, r2
    e8b8: e5c131a6     	strb	r3, [r1, #0x1a6]
    e8bc: e12fff1e     	bx	lr
    e8c0: eeb40ac7     	vcmpe.f32	s0, s14
    e8c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    e8c8: 8affffd9     	bhi	0xe834 <_setStereoSwitch+0x7c> @ imm = #-0x9c
    e8cc: e3530001     	cmp	r3, #1
    e8d0: 1affffd7     	bne	0xe834 <_setStereoSwitch+0x7c> @ imm = #-0xa4
    e8d4: e5c0304c     	strb	r3, [r0, #0x4c]
    e8d8: e1a02003     	mov	r2, r3
    e8dc: e3a01028     	mov	r1, #40
    e8e0: e5c631a7     	strb	r3, [r6, #0x1a7]
    e8e4: e5c6c1a6     	strb	r12, [r6, #0x1a6]
    e8e8: ebffd47e     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xae08  // CALL writeToSharedMem
    e8ec: e5d631a6     	ldrb	r3, [r6, #0x1a6]
    e8f0: eaffffcf     	b	0xe834 <_setStereoSwitch+0x7c> @ imm = #-0xc4
    e8f4: e59f306c     	ldr	r3, [pc, #0x6c]         @ 0xe968 <_setStereoSwitch+0x1b0>  // u32=0x61b8; f32?=3.50548824e-41
    e8f8: e2844a01     	add	r4, r4, #4096
    e8fc: e08f0003     	add	r0, pc, r3
    e900: e5944db4     	ldr	r4, [r4, #0xdb4]
    e904: ebffd387     	bl	0x3728 <.plt+0x2c>      @ imm = #-0xb1e4  // CALL gensym
    e908: e1a02006     	mov	r2, r6
    e90c: e1a0300d     	mov	r3, sp
    e910: e1a01000     	mov	r1, r0
    e914: e1a00004     	mov	r0, r4
    e918: ebffd4d5     	bl	0x3c74 <.plt+0x578>     @ imm = #-0xacac  // CALL outlet_list
    e91c: e5d5c1a7     	ldrb	r12, [r5, #0x1a7]
    e920: eaffffdc     	b	0xe898 <_setStereoSwitch+0xe0> @ imm = #-0x90
    e924: e5c0c04c     	strb	r12, [r0, #0x4c]
    e928: e1a0200c     	mov	r2, r12
    e92c: e3a01028     	mov	r1, #40
    e930: e5c6c1a7     	strb	r12, [r6, #0x1a7]
    e934: e5c6c1a6     	strb	r12, [r6, #0x1a6]
    e938: ebffd46a     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0xae58  // CALL writeToSharedMem
    e93c: e5d631a6     	ldrb	r3, [r6, #0x1a6]
    e940: eaffffbb     	b	0xe834 <_setStereoSwitch+0x7c> @ imm = #-0x114
    e944: 00 80 6d 45  	.word	0x456d8000
    e948: 00 00 96 43  	.word	0x43960000
    e94c: e4 8b 01 00  	.word	0x00018be4
    e950: 94 8b 01 00  	.word	0x00018b94
    e954: a0 8a 01 00  	.word	0x00018aa0
    e958: 68 8b 01 00  	.word	0x00018b68
    e95c: 7c 71 00 00  	.word	0x0000717c
    e960: 48 8a 01 00  	.word	0x00018a48
    e964: fc 8a 01 00  	.word	0x00018afc
    e968: b8 61 00 00  	.word	0x000061b8

