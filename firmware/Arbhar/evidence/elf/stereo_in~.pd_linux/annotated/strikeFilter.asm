00002808 <strikeFilter>:
    2808: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2870 <strikeFilter+0x68>  // u32=0x158d0; f32?=1.23695418e-40
    280c: e1a03000     	mov	r3, r0
    2810: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2874 <strikeFilter+0x6c>  // u32=0x1590c; f32?=1.23779496e-40
    2814: e08f1001     	add	r1, pc, r1
    2818: e08f0000     	add	r0, pc, r0
    281c: e5d1c002     	ldrb	r12, [r1, #0x2]
    2820: e590200c     	ldr	r2, [r0, #0xc]
    2824: e15c0003     	cmp	r12, r3
    2828: 0a00000a     	beq	0x2858 <strikeFilter+0x50> @ imm = #0x28
    282c: e3520019     	cmp	r2, #25
    2830: 9a000008     	bls	0x2858 <strikeFilter+0x50> @ imm = #0x20
    2834: e3530000     	cmp	r3, #0
    2838: 1a000003     	bne	0x284c <strikeFilter+0x44> @ imm = #0xc
    283c: e580300c     	str	r3, [r0, #0xc]
    2840: e3a00001     	mov	r0, #1
    2844: e5c13002     	strb	r3, [r1, #0x2]
    2848: e12fff1e     	bx	lr
    284c: e3a00000     	mov	r0, #0
    2850: e5c13002     	strb	r3, [r1, #0x2]
    2854: e12fff1e     	bx	lr
    2858: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2878 <strikeFilter+0x70>  // u32=0x158c0; f32?=1.23672997e-40
    285c: e282c001     	add	r12, r2, #1
    2860: e3a000ff     	mov	r0, #255
    2864: e08f3001     	add	r3, pc, r1
    2868: e583c00c     	str	r12, [r3, #0xc]
    286c: e12fff1e     	bx	lr
    2870: d0 58 01 00  	.word	0x000158d0
    2874: 0c 59 01 00  	.word	0x0001590c
    2878: c0 58 01 00  	.word	0x000158c0

