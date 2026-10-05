00005a4c <captureFilter>:
    5a4c: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x5ab4 <captureFilter+0x68>
    5a50: e1a03000     	mov	r3, r0
    5a54: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x5ab8 <captureFilter+0x6c>
    5a58: e08f1001     	add	r1, pc, r1
    5a5c: e08f0000     	add	r0, pc, r0
    5a60: e5d1c005     	ldrb	r12, [r1, #0x5]
    5a64: e5902018     	ldr	r2, [r0, #0x18]
    5a68: e15c0003     	cmp	r12, r3
    5a6c: 0a00000a     	beq	0x5a9c <captureFilter+0x50> @ imm = #0x28
    5a70: e3520019     	cmp	r2, #25
    5a74: 9a000008     	bls	0x5a9c <captureFilter+0x50> @ imm = #0x20
    5a78: e3530000     	cmp	r3, #0
    5a7c: 1a000003     	bne	0x5a90 <captureFilter+0x44> @ imm = #0xc
    5a80: e5803018     	str	r3, [r0, #0x18]
    5a84: e3a00001     	mov	r0, #1
    5a88: e5c13005     	strb	r3, [r1, #0x5]
    5a8c: e12fff1e     	bx	lr
    5a90: e3a00000     	mov	r0, #0
    5a94: e5c13005     	strb	r3, [r1, #0x5]
    5a98: e12fff1e     	bx	lr
    5a9c: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x5abc <captureFilter+0x70>
    5aa0: e282c001     	add	r12, r2, #1
    5aa4: e3a000ff     	mov	r0, #255
    5aa8: e08f3001     	add	r3, pc, r1
    5aac: e583c018     	str	r12, [r3, #0x18]
    5ab0: e12fff1e     	bx	lr
    5ab4: f8 86 01 00  	.word	0x000186f8
    5ab8: 38 87 01 00  	.word	0x00018738
    5abc: ec 86 01 00  	.word	0x000186ec

