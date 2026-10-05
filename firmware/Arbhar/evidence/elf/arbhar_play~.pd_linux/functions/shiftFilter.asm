000059d8 <shiftFilter>:
    59d8: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x5a40 <shiftFilter+0x68>
    59dc: e1a03000     	mov	r3, r0
    59e0: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x5a44 <shiftFilter+0x6c>
    59e4: e08f1001     	add	r1, pc, r1
    59e8: e08f0000     	add	r0, pc, r0
    59ec: e5d1c004     	ldrb	r12, [r1, #0x4]
    59f0: e5902014     	ldr	r2, [r0, #0x14]
    59f4: e15c0003     	cmp	r12, r3
    59f8: 0a00000a     	beq	0x5a28 <shiftFilter+0x50> @ imm = #0x28
    59fc: e3520019     	cmp	r2, #25
    5a00: 9a000008     	bls	0x5a28 <shiftFilter+0x50> @ imm = #0x20
    5a04: e3530000     	cmp	r3, #0
    5a08: 1a000003     	bne	0x5a1c <shiftFilter+0x44> @ imm = #0xc
    5a0c: e5803014     	str	r3, [r0, #0x14]
    5a10: e3a00001     	mov	r0, #1
    5a14: e5c13004     	strb	r3, [r1, #0x4]
    5a18: e12fff1e     	bx	lr
    5a1c: e3a00000     	mov	r0, #0
    5a20: e5c13004     	strb	r3, [r1, #0x4]
    5a24: e12fff1e     	bx	lr
    5a28: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x5a48 <shiftFilter+0x70>
    5a2c: e282c001     	add	r12, r2, #1
    5a30: e3a000ff     	mov	r0, #255
    5a34: e08f3001     	add	r3, pc, r1
    5a38: e583c014     	str	r12, [r3, #0x14]
    5a3c: e12fff1e     	bx	lr
    5a40: 6c 87 01 00  	.word	0x0001876c
    5a44: ac 87 01 00  	.word	0x000187ac
    5a48: 60 87 01 00  	.word	0x00018760

