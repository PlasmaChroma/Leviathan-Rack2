00002814 <captureFilter>:
    2814: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x287c <captureFilter+0x68>
    2818: e1a03000     	mov	r3, r0
    281c: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2880 <captureFilter+0x6c>
    2820: e08f1001     	add	r1, pc, r1
    2824: e08f0000     	add	r0, pc, r0
    2828: e5d1c001     	ldrb	r12, [r1, #0x1]
    282c: e5902008     	ldr	r2, [r0, #0x8]
    2830: e15c0003     	cmp	r12, r3
    2834: 0a00000a     	beq	0x2864 <captureFilter+0x50> @ imm = #0x28
    2838: e3520019     	cmp	r2, #25
    283c: 9a000008     	bls	0x2864 <captureFilter+0x50> @ imm = #0x20
    2840: e3530000     	cmp	r3, #0
    2844: 1a000003     	bne	0x2858 <captureFilter+0x44> @ imm = #0xc
    2848: e5803008     	str	r3, [r0, #0x8]
    284c: e3a00001     	mov	r0, #1
    2850: e5c13001     	strb	r3, [r1, #0x1]
    2854: e12fff1e     	bx	lr
    2858: e3a00000     	mov	r0, #0
    285c: e5c13001     	strb	r3, [r1, #0x1]
    2860: e12fff1e     	bx	lr
    2864: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2884 <captureFilter+0x70>
    2868: e282c001     	add	r12, r2, #1
    286c: e3a000ff     	mov	r0, #255
    2870: e08f3001     	add	r3, pc, r1
    2874: e583c008     	str	r12, [r3, #0x8]
    2878: e12fff1e     	bx	lr
    287c: b0 58 01 00  	.word	0x000158b0
    2880: ec 58 01 00  	.word	0x000158ec
    2884: a0 58 01 00  	.word	0x000158a0

