000024bc <tanh_approx_tilde_dsp>:
    24bc: e59f3044     	ldr	r3, [pc, #0x44]         @ 0x2508 <tanh_approx_tilde_dsp+0x4c>  // u32=0x15b30; f32?=1.24547408e-40
    24c0: e1a02000     	mov	r2, r0
    24c4: e92d4030     	push	{r4, r5, lr}
    24c8: e08f5003     	add	r5, pc, r3
    24cc: e591e000     	ldr	lr, [r1]
    24d0: e24dd00c     	sub	sp, sp, #12
    24d4: e5914004     	ldr	r4, [r1, #0x4]
    24d8: e1a00005     	mov	r0, r5
    24dc: e59fc028     	ldr	r12, [pc, #0x28]        @ 0x250c <tanh_approx_tilde_dsp+0x50>  // u32=0xd0; f32?=2.91470081e-43
    24e0: e3a01004     	mov	r1, #4
    24e4: e59e5000     	ldr	r5, [lr]
    24e8: e59e3004     	ldr	r3, [lr, #0x4]
    24ec: e790000c     	ldr	r0, [r0, r12]
    24f0: e58d5004     	str	r5, [sp, #0x4]
    24f4: e594e004     	ldr	lr, [r4, #0x4]
    24f8: e58de000     	str	lr, [sp]
    24fc: ebfffef3     	bl	0x20d0 <.plt+0x188>     @ imm = #-0x434  // CALL dsp_add
    2500: e28dd00c     	add	sp, sp, #12
    2504: e8bd8030     	pop	{r4, r5, pc}
    2508: 30 5b 01 00  	.word	0x00015b30
    250c: d0 00 00 00  	.word	0x000000d0

