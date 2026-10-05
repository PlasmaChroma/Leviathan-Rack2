00003468 <strikeFilter>:
    3468: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x34d0 <strikeFilter+0x68>
    346c: e1a03000     	mov	r3, r0
    3470: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x34d4 <strikeFilter+0x6c>
    3474: e08f1001     	add	r1, pc, r1
    3478: e08f0000     	add	r0, pc, r0
    347c: e5d1c002     	ldrb	r12, [r1, #0x2]
    3480: e5902008     	ldr	r2, [r0, #0x8]
    3484: e15c0003     	cmp	r12, r3
    3488: 0a00000a     	beq	0x34b8 <strikeFilter+0x50> @ imm = #0x28
    348c: e3520019     	cmp	r2, #25
    3490: 9a000008     	bls	0x34b8 <strikeFilter+0x50> @ imm = #0x20
    3494: e3530000     	cmp	r3, #0
    3498: 1a000003     	bne	0x34ac <strikeFilter+0x44> @ imm = #0xc
    349c: e5803008     	str	r3, [r0, #0x8]
    34a0: e3a00001     	mov	r0, #1
    34a4: e5c13002     	strb	r3, [r1, #0x2]
    34a8: e12fff1e     	bx	lr
    34ac: e3a00000     	mov	r0, #0
    34b0: e5c13002     	strb	r3, [r1, #0x2]
    34b4: e12fff1e     	bx	lr
    34b8: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x34d8 <strikeFilter+0x70>
    34bc: e282c001     	add	r12, r2, #1
    34c0: e3a000ff     	mov	r0, #255
    34c4: e08f3001     	add	r3, pc, r1
    34c8: e583c008     	str	r12, [r3, #0x8]
    34cc: e12fff1e     	bx	lr
    34d0: f4 6c 01 00  	.word	0x00016cf4
    34d4: 30 6d 01 00  	.word	0x00016d30
    34d8: e4 6c 01 00  	.word	0x00016ce4

