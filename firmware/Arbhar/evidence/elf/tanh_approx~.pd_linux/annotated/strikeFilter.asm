00002888 <strikeFilter>:
    2888: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x28f0 <strikeFilter+0x68>  // u32=0x1583c; f32?=1.23488026e-40
    288c: e1a03000     	mov	r3, r0
    2890: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x28f4 <strikeFilter+0x6c>  // u32=0x15878; f32?=1.23572104e-40
    2894: e08f1001     	add	r1, pc, r1
    2898: e08f0000     	add	r0, pc, r0
    289c: e5d1c002     	ldrb	r12, [r1, #0x2]
    28a0: e590200c     	ldr	r2, [r0, #0xc]
    28a4: e15c0003     	cmp	r12, r3
    28a8: 0a00000a     	beq	0x28d8 <strikeFilter+0x50> @ imm = #0x28
    28ac: e3520019     	cmp	r2, #25
    28b0: 9a000008     	bls	0x28d8 <strikeFilter+0x50> @ imm = #0x20
    28b4: e3530000     	cmp	r3, #0
    28b8: 1a000003     	bne	0x28cc <strikeFilter+0x44> @ imm = #0xc
    28bc: e580300c     	str	r3, [r0, #0xc]
    28c0: e3a00001     	mov	r0, #1
    28c4: e5c13002     	strb	r3, [r1, #0x2]
    28c8: e12fff1e     	bx	lr
    28cc: e3a00000     	mov	r0, #0
    28d0: e5c13002     	strb	r3, [r1, #0x2]
    28d4: e12fff1e     	bx	lr
    28d8: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x28f8 <strikeFilter+0x70>  // u32=0x1582c; f32?=1.23465605e-40
    28dc: e282c001     	add	r12, r2, #1
    28e0: e3a000ff     	mov	r0, #255
    28e4: e08f3001     	add	r3, pc, r1
    28e8: e583c00c     	str	r12, [r3, #0xc]
    28ec: e12fff1e     	bx	lr
    28f0: 3c 58 01 00  	.word	0x0001583c
    28f4: 78 58 01 00  	.word	0x00015878
    28f8: 2c 58 01 00  	.word	0x0001582c

