00005ac0 <strikeFilter>:
    5ac0: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x5b28 <strikeFilter+0x68>
    5ac4: e1a03000     	mov	r3, r0
    5ac8: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x5b2c <strikeFilter+0x6c>
    5acc: e08f1001     	add	r1, pc, r1
    5ad0: e08f0000     	add	r0, pc, r0
    5ad4: e5d1c006     	ldrb	r12, [r1, #0x6]
    5ad8: e590201c     	ldr	r2, [r0, #0x1c]
    5adc: e15c0003     	cmp	r12, r3
    5ae0: 0a00000a     	beq	0x5b10 <strikeFilter+0x50> @ imm = #0x28
    5ae4: e3520019     	cmp	r2, #25
    5ae8: 9a000008     	bls	0x5b10 <strikeFilter+0x50> @ imm = #0x20
    5aec: e3530000     	cmp	r3, #0
    5af0: 1a000003     	bne	0x5b04 <strikeFilter+0x44> @ imm = #0xc
    5af4: e580301c     	str	r3, [r0, #0x1c]
    5af8: e3a00001     	mov	r0, #1
    5afc: e5c13006     	strb	r3, [r1, #0x6]
    5b00: e12fff1e     	bx	lr
    5b04: e3a00000     	mov	r0, #0
    5b08: e5c13006     	strb	r3, [r1, #0x6]
    5b0c: e12fff1e     	bx	lr
    5b10: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x5b30 <strikeFilter+0x70>
    5b14: e282c001     	add	r12, r2, #1
    5b18: e3a000ff     	mov	r0, #255
    5b1c: e08f3001     	add	r3, pc, r1
    5b20: e583c01c     	str	r12, [r3, #0x1c]
    5b24: e12fff1e     	bx	lr
    5b28: 84 86 01 00  	.word	0x00018684
    5b2c: c4 86 01 00  	.word	0x000186c4
    5b30: 78 86 01 00  	.word	0x00018678

