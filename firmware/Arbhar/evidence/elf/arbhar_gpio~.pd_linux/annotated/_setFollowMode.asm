000076b8 <_setFollowMode>:
    76b8: e59f30b4     	ldr	r3, [pc, #0xb4]         @ 0x7774 <_setFollowMode+0xbc>  // u32=0x1fc28; f32?=1.82292115e-40
    76bc: e08f2003     	add	r2, pc, r3
    76c0: e5d2c010     	ldrb	r12, [r2, #0x10]
    76c4: e15c0001     	cmp	r12, r1
    76c8: 012fff1e     	bxeq	lr
    76cc: e92d4070     	push	{r4, r5, r6, lr}
    76d0: e1a04000     	mov	r4, r0
    76d4: e24dd010     	sub	sp, sp, #16
    76d8: e1a02001     	mov	r2, r1
    76dc: e1a05001     	mov	r5, r1
    76e0: e5c41054     	strb	r1, [r4, #0x54]
    76e4: e3a01024     	mov	r1, #36
    76e8: e3a06002     	mov	r6, #2
    76ec: ebfff0fd     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x3c0c  // CALL writeToSharedMem
    76f0: e3550000     	cmp	r5, #0
    76f4: e59f107c     	ldr	r1, [pc, #0x7c]         @ 0x7778 <_setFollowMode+0xc0>  // u32=0xd7d4; f32?=7.74245428e-41
    76f8: 13a00000     	movne	r0, #0
    76fc: e58d6000     	str	r6, [sp]
    7700: 15840058     	strne	r0, [r4, #0x58]
    7704: e08f0001     	add	r0, pc, r1
    7708: ebfff006     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x3fe8  // CALL gensym
    770c: e5d43054     	ldrb	r3, [r4, #0x54]
    7710: e5d4c030     	ldrb	r12, [r4, #0x30]
    7714: e3a02001     	mov	r2, #1
    7718: e58d2008     	str	r2, [sp, #0x8]
    771c: ee073a90     	vmov	s15, r3
    7720: e35c0062     	cmp	r12, #98
    7724: eeb80a67     	vcvt.f32.u32	s0, s15
    7728: ed8d0a03     	vstr	s0, [sp, #12]
    772c: e58d0004     	str	r0, [sp, #0x4]
    7730: 9a000004     	bls	0x7748 <_setFollowMode+0x90> @ imm = #0x10
    7734: e59f0040     	ldr	r0, [pc, #0x40]         @ 0x777c <_setFollowMode+0xc4>  // u32=0x1fbac; f32?=1.82118354e-40
    7738: e08f1000     	add	r1, pc, r0
    773c: e5c15010     	strb	r5, [r1, #0x10]
    7740: e28dd010     	add	sp, sp, #16
    7744: e8bd8070     	pop	{r4, r5, r6, pc}
    7748: e284ea01     	add	lr, r4, #4096
    774c: e59f402c     	ldr	r4, [pc, #0x2c]         @ 0x7780 <_setFollowMode+0xc8>  // u32=0xd364; f32?=7.58326677e-41
    7750: e08f0004     	add	r0, pc, r4
    7754: e59e4db4     	ldr	r4, [lr, #0xdb4]
    7758: ebffeff2     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x4038  // CALL gensym
    775c: e1a02006     	mov	r2, r6
    7760: e1a0300d     	mov	r3, sp
    7764: e1a01000     	mov	r1, r0
    7768: e1a00004     	mov	r0, r4
    776c: ebfff140     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x3b00  // CALL outlet_list
    7770: eaffffef     	b	0x7734 <_setFollowMode+0x7c> @ imm = #-0x44
    7774: 28 fc 01 00  	.word	0x0001fc28
    7778: d4 d7 00 00  	.word	0x0000d7d4
    777c: ac fb 01 00  	.word	0x0001fbac
    7780: 64 d3 00 00  	.word	0x0000d364

