000027bc <captureFilter>:
    27bc: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2824 <captureFilter+0x68>  // u32=0x15908; f32?=1.23773891e-40
    27c0: e1a03000     	mov	r3, r0
    27c4: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2828 <captureFilter+0x6c>  // u32=0x15944; f32?=1.23857969e-40
    27c8: e08f1001     	add	r1, pc, r1
    27cc: e08f0000     	add	r0, pc, r0
    27d0: e5d1c001     	ldrb	r12, [r1, #0x1]
    27d4: e5902008     	ldr	r2, [r0, #0x8]
    27d8: e15c0003     	cmp	r12, r3
    27dc: 0a00000a     	beq	0x280c <captureFilter+0x50> @ imm = #0x28
    27e0: e3520019     	cmp	r2, #25
    27e4: 9a000008     	bls	0x280c <captureFilter+0x50> @ imm = #0x20
    27e8: e3530000     	cmp	r3, #0
    27ec: 1a000003     	bne	0x2800 <captureFilter+0x44> @ imm = #0xc
    27f0: e5803008     	str	r3, [r0, #0x8]
    27f4: e3a00001     	mov	r0, #1
    27f8: e5c13001     	strb	r3, [r1, #0x1]
    27fc: e12fff1e     	bx	lr
    2800: e3a00000     	mov	r0, #0
    2804: e5c13001     	strb	r3, [r1, #0x1]
    2808: e12fff1e     	bx	lr
    280c: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x282c <captureFilter+0x70>  // u32=0x158f8; f32?=1.2375147e-40
    2810: e282c001     	add	r12, r2, #1
    2814: e3a000ff     	mov	r0, #255
    2818: e08f3001     	add	r3, pc, r1
    281c: e583c008     	str	r12, [r3, #0x8]
    2820: e12fff1e     	bx	lr
    2824: 08 59 01 00  	.word	0x00015908
    2828: 44 59 01 00  	.word	0x00015944
    282c: f8 58 01 00  	.word	0x000158f8

