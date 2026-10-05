00007784 <_controlLayerCoupling>:
    7784: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    7788: e24dd010     	sub	sp, sp, #16
    778c: e5d0603c     	ldrb	r6, [r0, #0x3c]
    7790: e3560001     	cmp	r6, #1
    7794: 0a000002     	beq	0x77a4 <_controlLayerCoupling+0x20> @ imm = #0x8
    7798: e3a00000     	mov	r0, #0
    779c: e28dd010     	add	sp, sp, #16
    77a0: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    77a4: eefd7ac0     	vcvt.s32.f32	s15, s0
    77a8: e5d02037     	ldrb	r2, [r0, #0x37]
    77ac: e5d03032     	ldrb	r3, [r0, #0x32]
    77b0: e1a04000     	mov	r4, r0
    77b4: ee175a90     	vmov	r5, s15
    77b8: e1530005     	cmp	r3, r5
    77bc: 01530002     	cmpeq	r3, r2
    77c0: 0a000022     	beq	0x7850 <_controlLayerCoupling+0xcc> @ imm = #0x88
    77c4: e3550005     	cmp	r5, #5
    77c8: ca00001a     	bgt	0x7838 <_controlLayerCoupling+0xb4> @ imm = #0x68
    77cc: e1530005     	cmp	r3, r5
    77d0: 0a00000f     	beq	0x7814 <_controlLayerCoupling+0x90> @ imm = #0x3c
    77d4: e5d01038     	ldrb	r1, [r0, #0x38]
    77d8: e3510001     	cmp	r1, #1
    77dc: 0a00000c     	beq	0x7814 <_controlLayerCoupling+0x90> @ imm = #0x30
    77e0: e59f70d0     	ldr	r7, [pc, #0xd0]         @ 0x78b8 <_controlLayerCoupling+0x134>  // u32=0x1faf8; f32?=1.8186612e-40
    77e4: e3a08000     	mov	r8, #0
    77e8: e5c06038     	strb	r6, [r0, #0x38]
    77ec: e08f7007     	add	r7, pc, r7
    77f0: e5c08031     	strb	r8, [r0, #0x31]
    77f4: e5d7c003     	ldrb	r12, [r7, #0x3]
    77f8: e35c0001     	cmp	r12, #1
    77fc: 1a000017     	bne	0x7860 <_controlLayerCoupling+0xdc> @ imm = #0x5c
    7800: e1a02005     	mov	r2, r5
    7804: e3a0101f     	mov	r1, #31
    7808: e1a00004     	mov	r0, r4
    780c: ebfff0b5     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x3d2c  // CALL writeToSharedMem
    7810: e5c45037     	strb	r5, [r4, #0x37]
    7814: e5d46031     	ldrb	r6, [r4, #0x31]
    7818: e3560000     	cmp	r6, #0
    781c: 1affffdd     	bne	0x7798 <_controlLayerCoupling+0x14> @ imm = #-0x8c
    7820: e1a02005     	mov	r2, r5
    7824: e3a0101f     	mov	r1, #31
    7828: e1a00004     	mov	r0, r4
    782c: ebfff0ad     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x3d4c  // CALL writeToSharedMem
    7830: e5c45037     	strb	r5, [r4, #0x37]
    7834: eaffffd7     	b	0x7798 <_controlLayerCoupling+0x14> @ imm = #-0xa4
    7838: e3a00001     	mov	r0, #1
    783c: e5c43037     	strb	r3, [r4, #0x37]
    7840: e5c40031     	strb	r0, [r4, #0x31]
    7844: e3a00000     	mov	r0, #0
    7848: e28dd010     	add	sp, sp, #16
    784c: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    7850: e3520005     	cmp	r2, #5
    7854: cafffff7     	bgt	0x7838 <_controlLayerCoupling+0xb4> @ imm = #-0x24
    7858: e1a05002     	mov	r5, r2
    785c: eaffffec     	b	0x7814 <_controlLayerCoupling+0x90> @ imm = #-0x50
    7860: e59fe054     	ldr	lr, [pc, #0x54]         @ 0x78bc <_controlLayerCoupling+0x138>  // u32=0xd54c; f32?=7.65165013e-41
    7864: e1a01006     	mov	r1, r6
    7868: e08f000e     	add	r0, pc, lr
    786c: ebfff0c1     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x3cfc  // CALL post
    7870: e59f0048     	ldr	r0, [pc, #0x48]         @ 0x78c0 <_controlLayerCoupling+0x13c>  // u32=0xd238; f32?=7.54122782e-41
    7874: e3a035fe     	mov	r3, #1065353216
    7878: e5948070     	ldr	r8, [r4, #0x70]
    787c: e08f0000     	add	r0, pc, r0
    7880: e58d3004     	str	r3, [sp, #0x4]
    7884: e3a02000     	mov	r2, #0
    7888: e58d6000     	str	r6, [sp]
    788c: e344231d     	movt	r2, #0x431d
    7890: e58d6008     	str	r6, [sp, #0x8]
    7894: e58d200c     	str	r2, [sp, #0xc]
    7898: ebffefa2     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x4178  // CALL gensym
    789c: e1a0300d     	mov	r3, sp
    78a0: e3a02002     	mov	r2, #2
    78a4: e1a01000     	mov	r1, r0
    78a8: e1a00008     	mov	r0, r8
    78ac: ebfff0f0     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x3c40  // CALL outlet_list
    78b0: e5c76003     	strb	r6, [r7, #0x3]
    78b4: eaffffd1     	b	0x7800 <_controlLayerCoupling+0x7c> @ imm = #-0xbc
    78b8: f8 fa 01 00  	.word	0x0001faf8
    78bc: 4c d5 00 00  	.word	0x0000d54c
    78c0: 38 d2 00 00  	.word	0x0000d238

