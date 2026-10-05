00002e6c <captureFilter>:
    2e6c: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2ed4 <captureFilter+0x68>  // u32=0x162ac; f32?=1.27232295e-40
    2e70: e1a03000     	mov	r3, r0
    2e74: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2ed8 <captureFilter+0x6c>  // u32=0x162e8; f32?=1.27316373e-40
    2e78: e08f1001     	add	r1, pc, r1
    2e7c: e08f0000     	add	r0, pc, r0
    2e80: e5d1c001     	ldrb	r12, [r1, #0x1]
    2e84: e5902004     	ldr	r2, [r0, #0x4]
    2e88: e15c0003     	cmp	r12, r3
    2e8c: 0a00000a     	beq	0x2ebc <captureFilter+0x50> @ imm = #0x28
    2e90: e3520019     	cmp	r2, #25
    2e94: 9a000008     	bls	0x2ebc <captureFilter+0x50> @ imm = #0x20
    2e98: e3530000     	cmp	r3, #0
    2e9c: 1a000003     	bne	0x2eb0 <captureFilter+0x44> @ imm = #0xc
    2ea0: e5803004     	str	r3, [r0, #0x4]
    2ea4: e3a00001     	mov	r0, #1
    2ea8: e5c13001     	strb	r3, [r1, #0x1]
    2eac: e12fff1e     	bx	lr
    2eb0: e3a00000     	mov	r0, #0
    2eb4: e5c13001     	strb	r3, [r1, #0x1]
    2eb8: e12fff1e     	bx	lr
    2ebc: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2edc <captureFilter+0x70>  // u32=0x1629c; f32?=1.27209875e-40
    2ec0: e282c001     	add	r12, r2, #1
    2ec4: e3a000ff     	mov	r0, #255
    2ec8: e08f3001     	add	r3, pc, r1
    2ecc: e583c004     	str	r12, [r3, #0x4]
    2ed0: e12fff1e     	bx	lr
    2ed4: ac 62 01 00  	.word	0x000162ac
    2ed8: e8 62 01 00  	.word	0x000162e8
    2edc: 9c 62 01 00  	.word	0x0001629c

