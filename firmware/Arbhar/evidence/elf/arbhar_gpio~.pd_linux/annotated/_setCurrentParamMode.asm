0000638c <_setCurrentParamMode>:
    638c: e92d4030     	push	{r4, r5, lr}
    6390: eeb70ac0     	vcvt.f64.f32	d0, s0
    6394: e59f407c     	ldr	r4, [pc, #0x7c]         @ 0x6418 <_setCurrentParamMode+0x8c>  // u32=0x20f40; f32?=1.89141662e-40
    6398: e24dd014     	sub	sp, sp, #20
    639c: e1a05000     	mov	r5, r0
    63a0: ebfff5eb     	bl	0x3b54 <.plt+0x458>     @ imm = #-0x2854  // CALL led_onsetModeControl
    63a4: e08f4004     	add	r4, pc, r4
    63a8: e5d43003     	ldrb	r3, [r4, #0x3]
    63ac: e3530000     	cmp	r3, #0
    63b0: 0a000016     	beq	0x6410 <_setCurrentParamMode+0x84> @ imm = #0x58
    63b4: e59f0060     	ldr	r0, [pc, #0x60]         @ 0x641c <_setCurrentParamMode+0x90>  // u32=0xe9f8; f32?=8.39321728e-41
    63b8: e3a01000     	mov	r1, #0
    63bc: e08f0000     	add	r0, pc, r0
    63c0: ebfff5ec     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x2850  // CALL post
    63c4: e59f2054     	ldr	r2, [pc, #0x54]         @ 0x6420 <_setCurrentParamMode+0x94>  // u32=0xe6e4; f32?=8.28279496e-41
    63c8: e3a0c001     	mov	r12, #1
    63cc: e3a01000     	mov	r1, #0
    63d0: e08f0002     	add	r0, pc, r2
    63d4: e5955070     	ldr	r5, [r5, #0x70]
    63d8: e58d1004     	str	r1, [sp, #0x4]
    63dc: e3a03000     	mov	r3, #0
    63e0: e58dc000     	str	r12, [sp]
    63e4: e344331d     	movt	r3, #0x431d
    63e8: e58dc008     	str	r12, [sp, #0x8]
    63ec: e58d300c     	str	r3, [sp, #0xc]
    63f0: ebfff4cc     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x2cd0  // CALL gensym
    63f4: e1a0300d     	mov	r3, sp
    63f8: e3a02002     	mov	r2, #2
    63fc: e1a01000     	mov	r1, r0
    6400: e1a00005     	mov	r0, r5
    6404: ebfff61a     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x2798  // CALL outlet_list
    6408: e3a00000     	mov	r0, #0
    640c: e5c40003     	strb	r0, [r4, #0x3]
    6410: e28dd014     	add	sp, sp, #20
    6414: e8bd8030     	pop	{r4, r5, pc}
    6418: 40 0f 02 00  	.word	0x00020f40
    641c: f8 e9 00 00  	.word	0x0000e9f8
    6420: e4 e6 00 00  	.word	0x0000e6e4

