0000b058 <_setOrder_button>:
    b058: e5d13001     	ldrb	r3, [r1, #0x1]
    b05c: e3530000     	cmp	r3, #0
    b060: 0a000005     	beq	0xb07c <_setOrder_button+0x24> @ imm = #0x14
    b064: e5d02005     	ldrb	r2, [r0, #0x5]
    b068: e5d03015     	ldrb	r3, [r0, #0x15]
    b06c: e5d00025     	ldrb	r0, [r0, #0x25]
    b070: e082c003     	add	r12, r2, r3
    b074: e6e0207c     	uxtab	r2, r0, r12
    b078: e6ef3072     	uxtb	r3, r2
    b07c: e5c13002     	strb	r3, [r1, #0x2]
    b080: e1a00003     	mov	r0, r3
    b084: e12fff1e     	bx	lr

