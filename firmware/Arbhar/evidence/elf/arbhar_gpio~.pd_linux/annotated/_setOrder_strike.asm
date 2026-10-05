0000b0f8 <_setOrder_strike>:
    b0f8: e5d0202b     	ldrb	r2, [r0, #0x2b]
    b0fc: e1a03000     	mov	r3, r0
    b100: e5d00026     	ldrb	r0, [r0, #0x26]
    b104: e3520000     	cmp	r2, #0
    b108: 0a000006     	beq	0xb128 <_setOrder_strike+0x30> @ imm = #0x18
    b10c: e5d3c025     	ldrb	r12, [r3, #0x25]
    b110: e35c0000     	cmp	r12, #0
    b114: 15d30005     	ldrbne	r0, [r3, #0x5]
    b118: 15d31015     	ldrbne	r1, [r3, #0x15]
    b11c: 108c0000     	addne	r0, r12, r0
    b120: 16e10070     	uxtabne	r0, r1, r0
    b124: 16ef0070     	uxtbne	r0, r0
    b128: e5c30026     	strb	r0, [r3, #0x26]
    b12c: e12fff1e     	bx	lr

