00002df8 <shiftFilter>:
    2df8: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2e60 <shiftFilter+0x68>  // u32=0x16320; f32?=1.27394846e-40
    2dfc: e1a03000     	mov	r3, r0
    2e00: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2e64 <shiftFilter+0x6c>  // u32=0x1635c; f32?=1.27478924e-40
    2e04: e08f1001     	add	r1, pc, r1
    2e08: e08f0000     	add	r0, pc, r0
    2e0c: e5d1c000     	ldrb	r12, [r1]
    2e10: e5902000     	ldr	r2, [r0]
    2e14: e15c0003     	cmp	r12, r3
    2e18: 0a00000a     	beq	0x2e48 <shiftFilter+0x50> @ imm = #0x28
    2e1c: e3520019     	cmp	r2, #25
    2e20: 9a000008     	bls	0x2e48 <shiftFilter+0x50> @ imm = #0x20
    2e24: e3530000     	cmp	r3, #0
    2e28: 1a000003     	bne	0x2e3c <shiftFilter+0x44> @ imm = #0xc
    2e2c: e5803000     	str	r3, [r0]
    2e30: e3a00001     	mov	r0, #1
    2e34: e5c13000     	strb	r3, [r1]
    2e38: e12fff1e     	bx	lr
    2e3c: e3a00000     	mov	r0, #0
    2e40: e5c13000     	strb	r3, [r1]
    2e44: e12fff1e     	bx	lr
    2e48: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2e68 <shiftFilter+0x70>  // u32=0x16310; f32?=1.27372425e-40
    2e4c: e282c001     	add	r12, r2, #1
    2e50: e3a000ff     	mov	r0, #255
    2e54: e08f3001     	add	r3, pc, r1
    2e58: e583c000     	str	r12, [r3]
    2e5c: e12fff1e     	bx	lr
    2e60: 20 63 01 00  	.word	0x00016320
    2e64: 5c 63 01 00  	.word	0x0001635c
    2e68: 10 63 01 00  	.word	0x00016310

