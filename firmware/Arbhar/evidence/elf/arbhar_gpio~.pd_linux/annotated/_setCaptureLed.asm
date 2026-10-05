00005dc0 <_setCaptureLed>:
    5dc0: e2801a01     	add	r1, r0, #4096
    5dc4: e59f0048     	ldr	r0, [pc, #0x48]         @ 0x5e14 <_setCaptureLed+0x54>  // u32=0xece4; f32?=8.49803441e-41
    5dc8: e92d4010     	push	{r4, lr}
    5dcc: e24dd010     	sub	sp, sp, #16
    5dd0: e08f0000     	add	r0, pc, r0
    5dd4: e3a03001     	mov	r3, #1
    5dd8: e5914dac     	ldr	r4, [r1, #0xdac]
    5ddc: e3a02000     	mov	r2, #0
    5de0: ed8d0a01     	vstr	s0, [sp, #4]
    5de4: e3442310     	movt	r2, #0x4310
    5de8: e58d3000     	str	r3, [sp]
    5dec: e58d200c     	str	r2, [sp, #0xc]
    5df0: e58d3008     	str	r3, [sp, #0x8]
    5df4: ebfff64b     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x26d4  // CALL gensym
    5df8: e1a0300d     	mov	r3, sp
    5dfc: e3a02002     	mov	r2, #2
    5e00: e1a01000     	mov	r1, r0
    5e04: e1a00004     	mov	r0, r4
    5e08: ebfff799     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x219c  // CALL outlet_list
    5e0c: e28dd010     	add	sp, sp, #16
    5e10: e8bd8010     	pop	{r4, pc}
    5e14: e4 ec 00 00  	.word	0x0000ece4

