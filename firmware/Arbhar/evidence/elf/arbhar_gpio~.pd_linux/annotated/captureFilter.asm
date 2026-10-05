00004ffc <captureFilter>:
    4ffc: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x5064 <captureFilter+0x68>  // u32=0x222dc; f32?=1.9617618e-40
    5000: e1a03000     	mov	r3, r0
    5004: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x5068 <captureFilter+0x6c>  // u32=0x223a4; f32?=1.9645644e-40
    5008: e08f1001     	add	r1, pc, r1
    500c: e08f0000     	add	r0, pc, r0
    5010: e5d1c001     	ldrb	r12, [r1, #0x1]
    5014: e5902008     	ldr	r2, [r0, #0x8]
    5018: e15c0003     	cmp	r12, r3
    501c: 0a00000a     	beq	0x504c <captureFilter+0x50> @ imm = #0x28
    5020: e3520019     	cmp	r2, #25
    5024: 9a000008     	bls	0x504c <captureFilter+0x50> @ imm = #0x20
    5028: e3530000     	cmp	r3, #0
    502c: 1a000003     	bne	0x5040 <captureFilter+0x44> @ imm = #0xc
    5030: e5803008     	str	r3, [r0, #0x8]
    5034: e3a00001     	mov	r0, #1
    5038: e5c13001     	strb	r3, [r1, #0x1]
    503c: e12fff1e     	bx	lr
    5040: e3a00000     	mov	r0, #0
    5044: e5c13001     	strb	r3, [r1, #0x1]
    5048: e12fff1e     	bx	lr
    504c: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x506c <captureFilter+0x70>  // u32=0x22358; f32?=1.96349941e-40
    5050: e282c001     	add	r12, r2, #1
    5054: e3a000ff     	mov	r0, #255
    5058: e08f3001     	add	r3, pc, r1
    505c: e583c008     	str	r12, [r3, #0x8]
    5060: e12fff1e     	bx	lr
    5064: dc 22 02 00  	.word	0x000222dc
    5068: a4 23 02 00  	.word	0x000223a4
    506c: 58 23 02 00  	.word	0x00022358

