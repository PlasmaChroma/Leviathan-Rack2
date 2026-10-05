000033f4 <captureFilter>:
    33f4: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x345c <captureFilter+0x68>  // u32=0x16d68; f32?=1.31083064e-40
    33f8: e1a03000     	mov	r3, r0
    33fc: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x3460 <captureFilter+0x6c>  // u32=0x16da4; f32?=1.31167141e-40
    3400: e08f1001     	add	r1, pc, r1
    3404: e08f0000     	add	r0, pc, r0
    3408: e5d1c001     	ldrb	r12, [r1, #0x1]
    340c: e5902004     	ldr	r2, [r0, #0x4]
    3410: e15c0003     	cmp	r12, r3
    3414: 0a00000a     	beq	0x3444 <captureFilter+0x50> @ imm = #0x28
    3418: e3520019     	cmp	r2, #25
    341c: 9a000008     	bls	0x3444 <captureFilter+0x50> @ imm = #0x20
    3420: e3530000     	cmp	r3, #0
    3424: 1a000003     	bne	0x3438 <captureFilter+0x44> @ imm = #0xc
    3428: e5803004     	str	r3, [r0, #0x4]
    342c: e3a00001     	mov	r0, #1
    3430: e5c13001     	strb	r3, [r1, #0x1]
    3434: e12fff1e     	bx	lr
    3438: e3a00000     	mov	r0, #0
    343c: e5c13001     	strb	r3, [r1, #0x1]
    3440: e12fff1e     	bx	lr
    3444: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x3464 <captureFilter+0x70>  // u32=0x16d58; f32?=1.31060643e-40
    3448: e282c001     	add	r12, r2, #1
    344c: e3a000ff     	mov	r0, #255
    3450: e08f3001     	add	r3, pc, r1
    3454: e583c004     	str	r12, [r3, #0x4]
    3458: e12fff1e     	bx	lr
    345c: 68 6d 01 00  	.word	0x00016d68
    3460: a4 6d 01 00  	.word	0x00016da4
    3464: 58 6d 01 00  	.word	0x00016d58

