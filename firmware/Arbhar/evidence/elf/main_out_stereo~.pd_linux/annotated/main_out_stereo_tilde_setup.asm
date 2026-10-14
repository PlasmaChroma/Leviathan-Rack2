00002d38 <main_out_stereo_tilde_setup>:
    2d38: e59f008c     	ldr	r0, [pc, #0x8c]         @ 0x2dcc <main_out_stereo_tilde_setup+0x94>  // u32=0x44e4; f32?=2.47132997e-41
    2d3c: e92d40f0     	push	{r4, r5, r6, r7, lr}
    2d40: e08f0000     	add	r0, pc, r0
    2d44: e24dd014     	sub	sp, sp, #20
    2d48: e59f4080     	ldr	r4, [pc, #0x80]         @ 0x2dd0 <main_out_stereo_tilde_setup+0x98>  // u32=0x152a0; f32?=1.21475761e-40
    2d4c: ebfffc86     	bl	0x1f6c <.plt+0x14>      @ imm = #-0xde8  // CALL gensym
    2d50: e59f207c     	ldr	r2, [pc, #0x7c]         @ 0x2dd4 <main_out_stereo_tilde_setup+0x9c>  // u32=0xc8; f32?=2.80259693e-43
    2d54: e59f107c     	ldr	r1, [pc, #0x7c]         @ 0x2dd8 <main_out_stereo_tilde_setup+0xa0>  // u32=0xb4; f32?=2.52233724e-43
    2d58: e08f4004     	add	r4, pc, r4
    2d5c: e3a0c006     	mov	r12, #6
    2d60: e3a06000     	mov	r6, #0
    2d64: e7942002     	ldr	r2, [r4, r2]
    2d68: e3a03034     	mov	r3, #52
    2d6c: e7941001     	ldr	r1, [r4, r1]
    2d70: e58dc008     	str	r12, [sp, #0x8]
    2d74: e58dc004     	str	r12, [sp, #0x4]
    2d78: e58d600c     	str	r6, [sp, #0xc]
    2d7c: e58d6000     	str	r6, [sp]
    2d80: ebfffcdf     	bl	0x2104 <.plt+0x1ac>     @ imm = #-0xc84  // CALL class_new
    2d84: e59f5050     	ldr	r5, [pc, #0x50]         @ 0x2ddc <main_out_stereo_tilde_setup+0xa4>  // u32=0x15384; f32?=1.21795257e-40
    2d88: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x2de0 <main_out_stereo_tilde_setup+0xa8>  // u32=0x44a4; f32?=2.46236166e-41
    2d8c: e08f5005     	add	r5, pc, r5
    2d90: e1a07000     	mov	r7, r0
    2d94: e08f0003     	add	r0, pc, r3
    2d98: e5857000     	str	r7, [r5]
    2d9c: ebfffc72     	bl	0x1f6c <.plt+0x14>      @ imm = #-0xe38  // CALL gensym
    2da0: e59f203c     	ldr	r2, [pc, #0x3c]         @ 0x2de4 <main_out_stereo_tilde_setup+0xac>  // u32=0xc4; f32?=2.74654499e-43
    2da4: e1a03006     	mov	r3, r6
    2da8: e7941002     	ldr	r1, [r4, r2]
    2dac: e1a02000     	mov	r2, r0
    2db0: e1a00007     	mov	r0, r7
    2db4: ebfffcdb     	bl	0x2128 <.plt+0x1d0>     @ imm = #-0xc94  // CALL class_addmethod
    2db8: e5950000     	ldr	r0, [r5]
    2dbc: e3a01024     	mov	r1, #36
    2dc0: e28dd014     	add	sp, sp, #20
    2dc4: e8bd40f0     	pop	{r4, r5, r6, r7, lr}
    2dc8: eafffcd0     	b	0x2110 <.plt+0x1b8>     @ imm = #-0xcc0  // CALL class_domainsignalin
    2dcc: e4 44 00 00  	.word	0x000044e4
    2dd0: a0 52 01 00  	.word	0x000152a0
    2dd4: c8 00 00 00  	.word	0x000000c8
    2dd8: b4 00 00 00  	.word	0x000000b4
    2ddc: 84 53 01 00  	.word	0x00015384
    2de0: a4 44 00 00  	.word	0x000044a4
    2de4: c4 00 00 00  	.word	0x000000c4

