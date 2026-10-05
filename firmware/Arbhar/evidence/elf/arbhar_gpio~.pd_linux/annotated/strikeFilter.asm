00005070 <strikeFilter>:
    5070: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x50d8 <strikeFilter+0x68>  // u32=0x22268; f32?=1.96013629e-40
    5074: e1a03000     	mov	r3, r0
    5078: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x50dc <strikeFilter+0x6c>  // u32=0x22330; f32?=1.96293889e-40
    507c: e08f1001     	add	r1, pc, r1
    5080: e08f0000     	add	r0, pc, r0
    5084: e5d1c002     	ldrb	r12, [r1, #0x2]
    5088: e590200c     	ldr	r2, [r0, #0xc]
    508c: e15c0003     	cmp	r12, r3
    5090: 0a00000a     	beq	0x50c0 <strikeFilter+0x50> @ imm = #0x28
    5094: e3520019     	cmp	r2, #25
    5098: 9a000008     	bls	0x50c0 <strikeFilter+0x50> @ imm = #0x20
    509c: e3530000     	cmp	r3, #0
    50a0: 1a000003     	bne	0x50b4 <strikeFilter+0x44> @ imm = #0xc
    50a4: e580300c     	str	r3, [r0, #0xc]
    50a8: e3a00001     	mov	r0, #1
    50ac: e5c13002     	strb	r3, [r1, #0x2]
    50b0: e12fff1e     	bx	lr
    50b4: e3a00000     	mov	r0, #0
    50b8: e5c13002     	strb	r3, [r1, #0x2]
    50bc: e12fff1e     	bx	lr
    50c0: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x50e0 <strikeFilter+0x70>  // u32=0x222e4; f32?=1.9618739e-40
    50c4: e282c001     	add	r12, r2, #1
    50c8: e3a000ff     	mov	r0, #255
    50cc: e08f3001     	add	r3, pc, r1
    50d0: e583c00c     	str	r12, [r3, #0xc]
    50d4: e12fff1e     	bx	lr
    50d8: 68 22 02 00  	.word	0x00022268
    50dc: 30 23 02 00  	.word	0x00022330
    50e0: e4 22 02 00  	.word	0x000222e4

