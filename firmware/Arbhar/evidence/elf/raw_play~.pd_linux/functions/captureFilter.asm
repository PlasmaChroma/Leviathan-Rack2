00002ab4 <captureFilter>:
    2ab4: e59f1060     	ldr	r1, [pc, #0x60]         @ 0x2b1c <captureFilter+0x68>
    2ab8: e1a03000     	mov	r3, r0
    2abc: e59f005c     	ldr	r0, [pc, #0x5c]         @ 0x2b20 <captureFilter+0x6c>
    2ac0: e08f1001     	add	r1, pc, r1
    2ac4: e08f0000     	add	r0, pc, r0
    2ac8: e5d1c001     	ldrb	r12, [r1, #0x1]
    2acc: e5902008     	ldr	r2, [r0, #0x8]
    2ad0: e15c0003     	cmp	r12, r3
    2ad4: 0a00000a     	beq	0x2b04 <captureFilter+0x50> @ imm = #0x28
    2ad8: e3520019     	cmp	r2, #25
    2adc: 9a000008     	bls	0x2b04 <captureFilter+0x50> @ imm = #0x20
    2ae0: e3530000     	cmp	r3, #0
    2ae4: 1a000003     	bne	0x2af8 <captureFilter+0x44> @ imm = #0xc
    2ae8: e5803008     	str	r3, [r0, #0x8]
    2aec: e3a00001     	mov	r0, #1
    2af0: e5c13001     	strb	r3, [r1, #0x1]
    2af4: e12fff1e     	bx	lr
    2af8: e3a00000     	mov	r0, #0
    2afc: e5c13001     	strb	r3, [r1, #0x1]
    2b00: e12fff1e     	bx	lr
    2b04: e59f1018     	ldr	r1, [pc, #0x18]         @ 0x2b24 <captureFilter+0x70>
    2b08: e282c001     	add	r12, r2, #1
    2b0c: e3a000ff     	mov	r0, #255
    2b10: e08f3001     	add	r3, pc, r1
    2b14: e583c008     	str	r12, [r3, #0x8]
    2b18: e12fff1e     	bx	lr
    2b1c: 1c 56 01 00  	.word	0x0001561c
    2b20: 58 56 01 00  	.word	0x00015658
    2b24: 0c 56 01 00  	.word	0x0001560c

