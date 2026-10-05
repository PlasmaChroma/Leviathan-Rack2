00003380 <shiftFilter>:
    3380: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x33e8 <shiftFilter+0x68>
    3384: e1a03000     	mov	r3, r0
    3388: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x33ec <shiftFilter+0x6c>
    338c: e08f1001     	add	r1, pc, r1
    3390: e08f0000     	add	r0, pc, r0
    3394: e5d1c000     	ldrb	r12, [r1]
    3398: e5902000     	ldr	r2, [r0]
    339c: e15c0003     	cmp	r12, r3
    33a0: 0a00000a     	beq	0x33d0 <shiftFilter+0x50> @ imm = #0x28
    33a4: e3520019     	cmp	r2, #25
    33a8: 9a000008     	bls	0x33d0 <shiftFilter+0x50> @ imm = #0x20
    33ac: e3530000     	cmp	r3, #0
    33b0: 1a000003     	bne	0x33c4 <shiftFilter+0x44> @ imm = #0xc
    33b4: e5803000     	str	r3, [r0]
    33b8: e3a00001     	mov	r0, #1
    33bc: e5c13000     	strb	r3, [r1]
    33c0: e12fff1e     	bx	lr
    33c4: e3a00000     	mov	r0, #0
    33c8: e5c13000     	strb	r3, [r1]
    33cc: e12fff1e     	bx	lr
    33d0: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x33f0 <shiftFilter+0x70>
    33d4: e282c001     	add	r12, r2, #1
    33d8: e3a000ff     	mov	r0, #255
    33dc: e08f3001     	add	r3, pc, r1
    33e0: e583c000     	str	r12, [r3]
    33e4: e12fff1e     	bx	lr
    33e8: dc 6d 01 00  	.word	0x00016ddc
    33ec: 18 6e 01 00  	.word	0x00016e18
    33f0: cc 6d 01 00  	.word	0x00016dcc

