00002794 <arbhar_wtosc_tilde_set>:
    2794: e59f30a4     	ldr	r3, [pc, #0xa4]         @ 0x2840 <arbhar_wtosc_tilde_set+0xac>  // u32=0x1685c; f32?=1.29272586e-40
    2798: e59f20a4     	ldr	r2, [pc, #0xa4]         @ 0x2844 <arbhar_wtosc_tilde_set+0xb0>  // u32=0x110; f32?=3.81153182e-43
    279c: e08f3003     	add	r3, pc, r3
    27a0: e92d4070     	push	{r4, r5, r6, lr}
    27a4: e1a04000     	mov	r4, r0
    27a8: e5801030     	str	r1, [r0, #0x30]
    27ac: e1a05001     	mov	r5, r1
    27b0: e1a00001     	mov	r0, r1
    27b4: e7931002     	ldr	r1, [r3, r2]
    27b8: e5911000     	ldr	r1, [r1]
    27bc: ebffff39     	bl	0x24a8 <.plt+0x1c4>     @ imm = #-0x31c  // CALL pd_findbyclass
    27c0: e2506000     	subs	r6, r0, #0
    27c4: 0a00000f     	beq	0x2808 <arbhar_wtosc_tilde_set+0x74> @ imm = #0x3c
    27c8: e284202c     	add	r2, r4, #44
    27cc: e2841028     	add	r1, r4, #40
    27d0: ebffff1f     	bl	0x2454 <.plt+0x170>     @ imm = #-0x384  // CALL garray_getfloatwords
    27d4: e2505000     	subs	r5, r0, #0
    27d8: 1a000007     	bne	0x27fc <arbhar_wtosc_tilde_set+0x68> @ imm = #0x1c
    27dc: e594c030     	ldr	r12, [r4, #0x30]
    27e0: e1a00004     	mov	r0, r4
    27e4: e59fe05c     	ldr	lr, [pc, #0x5c]         @ 0x2848 <arbhar_wtosc_tilde_set+0xb4>  // u32=0x5c9c; f32?=3.3221984e-41
    27e8: e59c2000     	ldr	r2, [r12]
    27ec: e08f100e     	add	r1, pc, lr
    27f0: ebffff71     	bl	0x25bc <.plt+0x2d8>     @ imm = #-0x23c  // CALL pd_error
    27f4: e584502c     	str	r5, [r4, #0x2c]
    27f8: e8bd8070     	pop	{r4, r5, r6, pc}
    27fc: e1a00006     	mov	r0, r6
    2800: e8bd4070     	pop	{r4, r5, r6, lr}
    2804: eaffff45     	b	0x2520 <.plt+0x23c>     @ imm = #-0x2ec  // CALL garray_usedindsp
    2808: e5950000     	ldr	r0, [r5]
    280c: e5d03000     	ldrb	r3, [r0]
    2810: e3530000     	cmp	r3, #0
    2814: 1a000002     	bne	0x2824 <arbhar_wtosc_tilde_set+0x90> @ imm = #0x8
    2818: e3a0c000     	mov	r12, #0
    281c: e584c02c     	str	r12, [r4, #0x2c]
    2820: e8bd8070     	pop	{r4, r5, r6, pc}
    2824: e5942030     	ldr	r2, [r4, #0x30]
    2828: e1a00004     	mov	r0, r4
    282c: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x284c <arbhar_wtosc_tilde_set+0xb8>  // u32=0x5c30; f32?=3.30706438e-41
    2830: e5922000     	ldr	r2, [r2]
    2834: e08f1001     	add	r1, pc, r1
    2838: ebffff5f     	bl	0x25bc <.plt+0x2d8>     @ imm = #-0x284  // CALL pd_error
    283c: eafffff5     	b	0x2818 <arbhar_wtosc_tilde_set+0x84> @ imm = #-0x2c
    2840: 5c 68 01 00  	.word	0x0001685c
    2844: 10 01 00 00  	.word	0x00000110
    2848: 9c 5c 00 00  	.word	0x00005c9c
    284c: 30 5c 00 00  	.word	0x00005c30

