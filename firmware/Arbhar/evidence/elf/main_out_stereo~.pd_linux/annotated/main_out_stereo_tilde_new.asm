000024b4 <main_out_stereo_tilde_new>:
    24b4: e59f3074     	ldr	r3, [pc, #0x74]         @ 0x2530 <main_out_stereo_tilde_new+0x7c>  // u32=0x15c54; f32?=1.24956587e-40
    24b8: e92d4070     	push	{r4, r5, r6, lr}
    24bc: e08f0003     	add	r0, pc, r3
    24c0: ed2d8b02     	vpush	{d8}
    24c4: e5900000     	ldr	r0, [r0]
    24c8: e59f5064     	ldr	r5, [pc, #0x64]         @ 0x2534 <main_out_stereo_tilde_new+0x80>  // u32=0x15b28; f32?=1.24536197e-40
    24cc: eef08a40     	vmov.f32	s17, s0
    24d0: e08f5005     	add	r5, pc, r5
    24d4: eeb08a60     	vmov.f32	s16, s1
    24d8: ebfffec1     	bl	0x1fe4 <.plt+0x8c>      @ imm = #-0x4fc  // CALL pd_new
    24dc: e59f2054     	ldr	r2, [pc, #0x54]         @ 0x2538 <main_out_stereo_tilde_new+0x84>  // u32=0xcc; f32?=2.85864887e-43
    24e0: edc08a07     	vstr	s17, [r0, #28]
    24e4: e1a01000     	mov	r1, r0
    24e8: e1a04000     	mov	r4, r0
    24ec: ed808a08     	vstr	s16, [r0, #32]
    24f0: e7956002     	ldr	r6, [r5, r2]
    24f4: e1a03006     	mov	r3, r6
    24f8: e1a02006     	mov	r2, r6
    24fc: ebfffec1     	bl	0x2008 <.plt+0xb0>      @ imm = #-0x4fc  // CALL inlet_new
    2500: e1a01006     	mov	r1, r6
    2504: e5840028     	str	r0, [r4, #0x28]
    2508: e1a00004     	mov	r0, r4
    250c: ebfffef0     	bl	0x20d4 <.plt+0x17c>     @ imm = #-0x440  // CALL outlet_new
    2510: e1a01006     	mov	r1, r6
    2514: e584002c     	str	r0, [r4, #0x2c]
    2518: e1a00004     	mov	r0, r4
    251c: ebfffeec     	bl	0x20d4 <.plt+0x17c>     @ imm = #-0x450  // CALL outlet_new
    2520: ecbd8b02     	vpop	{d8}
    2524: e5840030     	str	r0, [r4, #0x30]
    2528: e1a00004     	mov	r0, r4
    252c: e8bd8070     	pop	{r4, r5, r6, pc}
    2530: 54 5c 01 00  	.word	0x00015c54
    2534: 28 5b 01 00  	.word	0x00015b28
    2538: cc 00 00 00  	.word	0x000000cc

