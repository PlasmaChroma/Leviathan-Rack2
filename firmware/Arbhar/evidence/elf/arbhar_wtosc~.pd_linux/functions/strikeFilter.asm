00002ee0 <strikeFilter>:
    2ee0: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2f48 <strikeFilter+0x68>
    2ee4: e1a03000     	mov	r3, r0
    2ee8: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2f4c <strikeFilter+0x6c>
    2eec: e08f1001     	add	r1, pc, r1
    2ef0: e08f0000     	add	r0, pc, r0
    2ef4: e5d1c002     	ldrb	r12, [r1, #0x2]
    2ef8: e5902008     	ldr	r2, [r0, #0x8]
    2efc: e15c0003     	cmp	r12, r3
    2f00: 0a00000a     	beq	0x2f30 <strikeFilter+0x50> @ imm = #0x28
    2f04: e3520019     	cmp	r2, #25
    2f08: 9a000008     	bls	0x2f30 <strikeFilter+0x50> @ imm = #0x20
    2f0c: e3530000     	cmp	r3, #0
    2f10: 1a000003     	bne	0x2f24 <strikeFilter+0x44> @ imm = #0xc
    2f14: e5803008     	str	r3, [r0, #0x8]
    2f18: e3a00001     	mov	r0, #1
    2f1c: e5c13002     	strb	r3, [r1, #0x2]
    2f20: e12fff1e     	bx	lr
    2f24: e3a00000     	mov	r0, #0
    2f28: e5c13002     	strb	r3, [r1, #0x2]
    2f2c: e12fff1e     	bx	lr
    2f30: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2f50 <strikeFilter+0x70>
    2f34: e282c001     	add	r12, r2, #1
    2f38: e3a000ff     	mov	r0, #255
    2f3c: e08f3001     	add	r3, pc, r1
    2f40: e583c008     	str	r12, [r3, #0x8]
    2f44: e12fff1e     	bx	lr
    2f48: 38 62 01 00  	.word	0x00016238
    2f4c: 74 62 01 00  	.word	0x00016274
    2f50: 28 62 01 00  	.word	0x00016228

