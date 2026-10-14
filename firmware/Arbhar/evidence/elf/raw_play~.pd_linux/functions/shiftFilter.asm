00002a40 <shiftFilter>:
    2a40: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2aa8 <shiftFilter+0x68>
    2a44: e1a03000     	mov	r3, r0
    2a48: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2aac <shiftFilter+0x6c>
    2a4c: e08f1001     	add	r1, pc, r1
    2a50: e08f0000     	add	r0, pc, r0
    2a54: e5d1c000     	ldrb	r12, [r1]
    2a58: e5902004     	ldr	r2, [r0, #0x4]
    2a5c: e15c0003     	cmp	r12, r3
    2a60: 0a00000a     	beq	0x2a90 <shiftFilter+0x50> @ imm = #0x28
    2a64: e3520019     	cmp	r2, #25
    2a68: 9a000008     	bls	0x2a90 <shiftFilter+0x50> @ imm = #0x20
    2a6c: e3530000     	cmp	r3, #0
    2a70: 1a000003     	bne	0x2a84 <shiftFilter+0x44> @ imm = #0xc
    2a74: e5803004     	str	r3, [r0, #0x4]
    2a78: e3a00001     	mov	r0, #1
    2a7c: e5c13000     	strb	r3, [r1]
    2a80: e12fff1e     	bx	lr
    2a84: e3a00000     	mov	r0, #0
    2a88: e5c13000     	strb	r3, [r1]
    2a8c: e12fff1e     	bx	lr
    2a90: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2ab0 <shiftFilter+0x70>
    2a94: e282c001     	add	r12, r2, #1
    2a98: e3a000ff     	mov	r0, #255
    2a9c: e08f3001     	add	r3, pc, r1
    2aa0: e583c004     	str	r12, [r3, #0x4]
    2aa4: e12fff1e     	bx	lr
    2aa8: 90 56 01 00  	.word	0x00015690
    2aac: cc 56 01 00  	.word	0x000156cc
    2ab0: 80 56 01 00  	.word	0x00015680

