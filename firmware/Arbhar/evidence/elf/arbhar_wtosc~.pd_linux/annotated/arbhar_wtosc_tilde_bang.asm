00002850 <arbhar_wtosc_tilde_bang>:
    2850: e59f2038     	ldr	r2, [pc, #0x38]         @ 0x2890 <arbhar_wtosc_tilde_bang+0x40>  // u32=0x167a0; f32?=1.29009142e-40
    2854: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x2894 <arbhar_wtosc_tilde_bang+0x44>  // u32=0x110; f32?=3.81153182e-43
    2858: e08f2002     	add	r2, pc, r2
    285c: e5900030     	ldr	r0, [r0, #0x30]
    2860: e92d4010     	push	{r4, lr}
    2864: e7921003     	ldr	r1, [r2, r3]
    2868: e5911000     	ldr	r1, [r1]
    286c: ebffff0d     	bl	0x24a8 <.plt+0x1c4>     @ imm = #-0x3cc  // CALL pd_findbyclass
    2870: e3500000     	cmp	r0, #0
    2874: 0a000001     	beq	0x2880 <arbhar_wtosc_tilde_bang+0x30> @ imm = #0x4
    2878: e8bd4010     	pop	{r4, lr}
    287c: eafffeca     	b	0x23ac <.plt+0xc8>      @ imm = #-0x4d8  // CALL garray_redraw
    2880: e59f0010     	ldr	r0, [pc, #0x10]         @ 0x2898 <arbhar_wtosc_tilde_bang+0x48>  // u32=0x5c24; f32?=3.30538282e-41
    2884: e8bd4010     	pop	{r4, lr}
    2888: e08f0000     	add	r0, pc, r0
    288c: eafffeba     	b	0x237c <.plt+0x98>      @ imm = #-0x518  // CALL bug
    2890: a0 67 01 00  	.word	0x000167a0
    2894: 10 01 00 00  	.word	0x00000110
    2898: 24 5c 00 00  	.word	0x00005c24

