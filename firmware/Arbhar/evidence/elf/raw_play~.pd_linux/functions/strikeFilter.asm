00002b28 <strikeFilter>:
    2b28: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2b90 <strikeFilter+0x68>
    2b2c: e1a03000     	mov	r3, r0
    2b30: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2b94 <strikeFilter+0x6c>
    2b34: e08f1001     	add	r1, pc, r1
    2b38: e08f0000     	add	r0, pc, r0
    2b3c: e5d1c002     	ldrb	r12, [r1, #0x2]
    2b40: e590200c     	ldr	r2, [r0, #0xc]
    2b44: e15c0003     	cmp	r12, r3
    2b48: 0a00000a     	beq	0x2b78 <strikeFilter+0x50> @ imm = #0x28
    2b4c: e3520019     	cmp	r2, #25
    2b50: 9a000008     	bls	0x2b78 <strikeFilter+0x50> @ imm = #0x20
    2b54: e3530000     	cmp	r3, #0
    2b58: 1a000003     	bne	0x2b6c <strikeFilter+0x44> @ imm = #0xc
    2b5c: e580300c     	str	r3, [r0, #0xc]
    2b60: e3a00001     	mov	r0, #1
    2b64: e5c13002     	strb	r3, [r1, #0x2]
    2b68: e12fff1e     	bx	lr
    2b6c: e3a00000     	mov	r0, #0
    2b70: e5c13002     	strb	r3, [r1, #0x2]
    2b74: e12fff1e     	bx	lr
    2b78: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2b98 <strikeFilter+0x70>
    2b7c: e282c001     	add	r12, r2, #1
    2b80: e3a000ff     	mov	r0, #255
    2b84: e08f3001     	add	r3, pc, r1
    2b88: e583c00c     	str	r12, [r3, #0xc]
    2b8c: e12fff1e     	bx	lr
    2b90: a8 55 01 00  	.word	0x000155a8
    2b94: e4 55 01 00  	.word	0x000155e4
    2b98: 98 55 01 00  	.word	0x00015598

