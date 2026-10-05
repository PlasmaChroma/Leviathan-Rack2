000027a0 <shiftFilter>:
    27a0: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2808 <shiftFilter+0x68>
    27a4: e1a03000     	mov	r3, r0
    27a8: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x280c <shiftFilter+0x6c>
    27ac: e08f1001     	add	r1, pc, r1
    27b0: e08f0000     	add	r0, pc, r0
    27b4: e5d1c000     	ldrb	r12, [r1]
    27b8: e5902004     	ldr	r2, [r0, #0x4]
    27bc: e15c0003     	cmp	r12, r3
    27c0: 0a00000a     	beq	0x27f0 <shiftFilter+0x50> @ imm = #0x28
    27c4: e3520019     	cmp	r2, #25
    27c8: 9a000008     	bls	0x27f0 <shiftFilter+0x50> @ imm = #0x20
    27cc: e3530000     	cmp	r3, #0
    27d0: 1a000003     	bne	0x27e4 <shiftFilter+0x44> @ imm = #0xc
    27d4: e5803004     	str	r3, [r0, #0x4]
    27d8: e3a00001     	mov	r0, #1
    27dc: e5c13000     	strb	r3, [r1]
    27e0: e12fff1e     	bx	lr
    27e4: e3a00000     	mov	r0, #0
    27e8: e5c13000     	strb	r3, [r1]
    27ec: e12fff1e     	bx	lr
    27f0: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2810 <shiftFilter+0x70>
    27f4: e282c001     	add	r12, r2, #1
    27f8: e3a000ff     	mov	r0, #255
    27fc: e08f3001     	add	r3, pc, r1
    2800: e583c004     	str	r12, [r3, #0x4]
    2804: e12fff1e     	bx	lr
    2808: 24 59 01 00  	.word	0x00015924
    280c: 60 59 01 00  	.word	0x00015960
    2810: 14 59 01 00  	.word	0x00015914

