00002720 <shiftFilter>:
    2720: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2788 <shiftFilter+0x68>
    2724: e1a03000     	mov	r3, r0
    2728: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x278c <shiftFilter+0x6c>
    272c: e08f1001     	add	r1, pc, r1
    2730: e08f0000     	add	r0, pc, r0
    2734: e5d1c000     	ldrb	r12, [r1]
    2738: e5902004     	ldr	r2, [r0, #0x4]
    273c: e15c0003     	cmp	r12, r3
    2740: 0a00000a     	beq	0x2770 <shiftFilter+0x50> @ imm = #0x28
    2744: e3520019     	cmp	r2, #25
    2748: 9a000008     	bls	0x2770 <shiftFilter+0x50> @ imm = #0x20
    274c: e3530000     	cmp	r3, #0
    2750: 1a000003     	bne	0x2764 <shiftFilter+0x44> @ imm = #0xc
    2754: e5803004     	str	r3, [r0, #0x4]
    2758: e3a00001     	mov	r0, #1
    275c: e5c13000     	strb	r3, [r1]
    2760: e12fff1e     	bx	lr
    2764: e3a00000     	mov	r0, #0
    2768: e5c13000     	strb	r3, [r1]
    276c: e12fff1e     	bx	lr
    2770: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2790 <shiftFilter+0x70>
    2774: e282c001     	add	r12, r2, #1
    2778: e3a000ff     	mov	r0, #255
    277c: e08f3001     	add	r3, pc, r1
    2780: e583c004     	str	r12, [r3, #0x4]
    2784: e12fff1e     	bx	lr
    2788: b8 59 01 00  	.word	0x000159b8
    278c: f4 59 01 00  	.word	0x000159f4
    2790: a8 59 01 00  	.word	0x000159a8

