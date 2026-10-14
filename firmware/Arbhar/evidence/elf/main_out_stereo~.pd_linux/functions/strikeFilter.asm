00002830 <strikeFilter>:
    2830: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2898 <strikeFilter+0x68>
    2834: e1a03000     	mov	r3, r0
    2838: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x289c <strikeFilter+0x6c>
    283c: e08f1001     	add	r1, pc, r1
    2840: e08f0000     	add	r0, pc, r0
    2844: e5d1c002     	ldrb	r12, [r1, #0x2]
    2848: e590200c     	ldr	r2, [r0, #0xc]
    284c: e15c0003     	cmp	r12, r3
    2850: 0a00000a     	beq	0x2880 <strikeFilter+0x50> @ imm = #0x28
    2854: e3520019     	cmp	r2, #25
    2858: 9a000008     	bls	0x2880 <strikeFilter+0x50> @ imm = #0x20
    285c: e3530000     	cmp	r3, #0
    2860: 1a000003     	bne	0x2874 <strikeFilter+0x44> @ imm = #0xc
    2864: e580300c     	str	r3, [r0, #0xc]
    2868: e3a00001     	mov	r0, #1
    286c: e5c13002     	strb	r3, [r1, #0x2]
    2870: e12fff1e     	bx	lr
    2874: e3a00000     	mov	r0, #0
    2878: e5c13002     	strb	r3, [r1, #0x2]
    287c: e12fff1e     	bx	lr
    2880: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x28a0 <strikeFilter+0x70>
    2884: e282c001     	add	r12, r2, #1
    2888: e3a000ff     	mov	r0, #255
    288c: e08f3001     	add	r3, pc, r1
    2890: e583c00c     	str	r12, [r3, #0xc]
    2894: e12fff1e     	bx	lr
    2898: 94 58 01 00  	.word	0x00015894
    289c: d0 58 01 00  	.word	0x000158d0
    28a0: 84 58 01 00  	.word	0x00015884

