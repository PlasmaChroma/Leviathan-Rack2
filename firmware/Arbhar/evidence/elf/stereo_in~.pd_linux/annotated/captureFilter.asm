00002794 <captureFilter>:
    2794: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x27fc <captureFilter+0x68>  // u32=0x15944; f32?=1.23857969e-40
    2798: e1a03000     	mov	r3, r0
    279c: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2800 <captureFilter+0x6c>  // u32=0x15980; f32?=1.23942047e-40
    27a0: e08f1001     	add	r1, pc, r1
    27a4: e08f0000     	add	r0, pc, r0
    27a8: e5d1c001     	ldrb	r12, [r1, #0x1]
    27ac: e5902008     	ldr	r2, [r0, #0x8]
    27b0: e15c0003     	cmp	r12, r3
    27b4: 0a00000a     	beq	0x27e4 <captureFilter+0x50> @ imm = #0x28
    27b8: e3520019     	cmp	r2, #25
    27bc: 9a000008     	bls	0x27e4 <captureFilter+0x50> @ imm = #0x20
    27c0: e3530000     	cmp	r3, #0
    27c4: 1a000003     	bne	0x27d8 <captureFilter+0x44> @ imm = #0xc
    27c8: e5803008     	str	r3, [r0, #0x8]
    27cc: e3a00001     	mov	r0, #1
    27d0: e5c13001     	strb	r3, [r1, #0x1]
    27d4: e12fff1e     	bx	lr
    27d8: e3a00000     	mov	r0, #0
    27dc: e5c13001     	strb	r3, [r1, #0x1]
    27e0: e12fff1e     	bx	lr
    27e4: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2804 <captureFilter+0x70>  // u32=0x15934; f32?=1.23835548e-40
    27e8: e282c001     	add	r12, r2, #1
    27ec: e3a000ff     	mov	r0, #255
    27f0: e08f3001     	add	r3, pc, r1
    27f4: e583c008     	str	r12, [r3, #0x8]
    27f8: e12fff1e     	bx	lr
    27fc: 44 59 01 00  	.word	0x00015944
    2800: 80 59 01 00  	.word	0x00015980
    2804: 34 59 01 00  	.word	0x00015934

