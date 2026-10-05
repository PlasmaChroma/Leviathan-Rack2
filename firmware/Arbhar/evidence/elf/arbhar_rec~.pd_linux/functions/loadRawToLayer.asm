00002cdc <loadRawToLayer>:
    2cdc: e92d40f0     	push	{r4, r5, r6, r7, lr}
    2ce0: e24dd06c     	sub	sp, sp, #108
    2ce4: e28d4004     	add	r4, sp, #4
    2ce8: e3a02064     	mov	r2, #100
    2cec: e1a05000     	mov	r5, r0
    2cf0: e1a00003     	mov	r0, r3
    2cf4: e1a01004     	mov	r1, r4
    2cf8: e1a06003     	mov	r6, r3
    2cfc: ebfffedf     	bl	0x2880 <.plt+0x380>     @ imm = #-0x484
    2d00: e1a00004     	mov	r0, r4
    2d04: ebfffe92     	bl	0x2754 <.plt+0x254>     @ imm = #-0x5b8
    2d08: e2860008     	add	r0, r6, #8
    2d0c: ebfffe87     	bl	0x2730 <.plt+0x230>     @ imm = #-0x5e4
    2d10: e59f109c     	ldr	r1, [pc, #0x9c]         @ 0x2db4 <loadRawToLayer+0xd8>
    2d14: e08f1001     	add	r1, pc, r1
    2d18: e1a06000     	mov	r6, r0
    2d1c: e1a00004     	mov	r0, r4
    2d20: ebfffe10     	bl	0x2568 <.plt+0x68>      @ imm = #-0x7c0
    2d24: e2507000     	subs	r7, r0, #0
    2d28: 0a000003     	beq	0x2d3c <loadRawToLayer+0x60> @ imm = #0xc
    2d2c: e0855106     	add	r5, r5, r6, lsl #2
    2d30: e595321c     	ldr	r3, [r5, #0x21c]
    2d34: e3530000     	cmp	r3, #0
    2d38: ca000001     	bgt	0x2d44 <loadRawToLayer+0x68> @ imm = #0x4
    2d3c: e28dd06c     	add	sp, sp, #108
    2d40: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    2d44: e59f006c     	ldr	r0, [pc, #0x6c]         @ 0x2db8 <loadRawToLayer+0xdc>
    2d48: e08f0000     	add	r0, pc, r0
    2d4c: ebfffe80     	bl	0x2754 <.plt+0x254>     @ imm = #-0x600
    2d50: e3a02000     	mov	r2, #0
    2d54: e1a01002     	mov	r1, r2
    2d58: e1a00007     	mov	r0, r7
    2d5c: ebfffeb2     	bl	0x282c <.plt+0x32c>     @ imm = #-0x538
    2d60: e3500000     	cmp	r0, #0
    2d64: 0a000009     	beq	0x2d90 <loadRawToLayer+0xb4> @ imm = #0x24
    2d68: e59fc04c     	ldr	r12, [pc, #0x4c]        @ 0x2dbc <loadRawToLayer+0xe0>
    2d6c: e1a02006     	mov	r2, r6
    2d70: e595321c     	ldr	r3, [r5, #0x21c]
    2d74: e1a01004     	mov	r1, r4
    2d78: e08f000c     	add	r0, pc, r12
    2d7c: ebfffe74     	bl	0x2754 <.plt+0x254>     @ imm = #-0x630
    2d80: e1a00007     	mov	r0, r7
    2d84: ebfffe7e     	bl	0x2784 <.plt+0x284>     @ imm = #-0x608
    2d88: e28dd06c     	add	sp, sp, #108
    2d8c: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
    2d90: e59f2028     	ldr	r2, [pc, #0x28]         @ 0x2dc0 <loadRawToLayer+0xe4>
    2d94: e08f0002     	add	r0, pc, r2
    2d98: ebfffe6d     	bl	0x2754 <.plt+0x254>     @ imm = #-0x64c
    2d9c: e1a03007     	mov	r3, r7
    2da0: e595221c     	ldr	r2, [r5, #0x21c]
    2da4: e3a01004     	mov	r1, #4
    2da8: e59501e8     	ldr	r0, [r5, #0x1e8]
    2dac: ebfffe29     	bl	0x2658 <.plt+0x158>     @ imm = #-0x75c
    2db0: eaffffec     	b	0x2d68 <loadRawToLayer+0x8c> @ imm = #-0x50
    2db4: c0 66 00 00  	.word	0x000066c0
    2db8: f0 66 00 00  	.word	0x000066f0
    2dbc: 08 67 00 00  	.word	0x00006708
    2dc0: d4 66 00 00  	.word	0x000066d4

