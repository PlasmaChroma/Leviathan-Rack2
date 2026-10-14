00002748 <shiftFilter>:
    2748: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x27b0 <shiftFilter+0x68>
    274c: e1a03000     	mov	r3, r0
    2750: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x27b4 <shiftFilter+0x6c>
    2754: e08f1001     	add	r1, pc, r1
    2758: e08f0000     	add	r0, pc, r0
    275c: e5d1c000     	ldrb	r12, [r1]
    2760: e5902004     	ldr	r2, [r0, #0x4]
    2764: e15c0003     	cmp	r12, r3
    2768: 0a00000a     	beq	0x2798 <shiftFilter+0x50> @ imm = #0x28
    276c: e3520019     	cmp	r2, #25
    2770: 9a000008     	bls	0x2798 <shiftFilter+0x50> @ imm = #0x20
    2774: e3530000     	cmp	r3, #0
    2778: 1a000003     	bne	0x278c <shiftFilter+0x44> @ imm = #0xc
    277c: e5803004     	str	r3, [r0, #0x4]
    2780: e3a00001     	mov	r0, #1
    2784: e5c13000     	strb	r3, [r1]
    2788: e12fff1e     	bx	lr
    278c: e3a00000     	mov	r0, #0
    2790: e5c13000     	strb	r3, [r1]
    2794: e12fff1e     	bx	lr
    2798: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x27b8 <shiftFilter+0x70>
    279c: e282c001     	add	r12, r2, #1
    27a0: e3a000ff     	mov	r0, #255
    27a4: e08f3001     	add	r3, pc, r1
    27a8: e583c004     	str	r12, [r3, #0x4]
    27ac: e12fff1e     	bx	lr
    27b0: 7c 59 01 00  	.word	0x0001597c
    27b4: b8 59 01 00  	.word	0x000159b8
    27b8: 6c 59 01 00  	.word	0x0001596c

