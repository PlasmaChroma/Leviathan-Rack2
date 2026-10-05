00004f88 <shiftFilter>:
    4f88: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x4ff0 <shiftFilter+0x68>
    4f8c: e1a03000     	mov	r3, r0
    4f90: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x4ff4 <shiftFilter+0x6c>
    4f94: e08f1001     	add	r1, pc, r1
    4f98: e08f0000     	add	r0, pc, r0
    4f9c: e5d1c000     	ldrb	r12, [r1]
    4fa0: e5902004     	ldr	r2, [r0, #0x4]
    4fa4: e15c0003     	cmp	r12, r3
    4fa8: 0a00000a     	beq	0x4fd8 <shiftFilter+0x50> @ imm = #0x28
    4fac: e3520019     	cmp	r2, #25
    4fb0: 9a000008     	bls	0x4fd8 <shiftFilter+0x50> @ imm = #0x20
    4fb4: e3530000     	cmp	r3, #0
    4fb8: 1a000003     	bne	0x4fcc <shiftFilter+0x44> @ imm = #0xc
    4fbc: e5803004     	str	r3, [r0, #0x4]
    4fc0: e3a00001     	mov	r0, #1
    4fc4: e5c13000     	strb	r3, [r1]
    4fc8: e12fff1e     	bx	lr
    4fcc: e3a00000     	mov	r0, #0
    4fd0: e5c13000     	strb	r3, [r1]
    4fd4: e12fff1e     	bx	lr
    4fd8: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x4ff8 <shiftFilter+0x70>
    4fdc: e282c001     	add	r12, r2, #1
    4fe0: e3a000ff     	mov	r0, #255
    4fe4: e08f3001     	add	r3, pc, r1
    4fe8: e583c004     	str	r12, [r3, #0x4]
    4fec: e12fff1e     	bx	lr
    4ff0: 50 23 02 00  	.word	0x00022350
    4ff4: 18 24 02 00  	.word	0x00022418
    4ff8: cc 23 02 00  	.word	0x000223cc

