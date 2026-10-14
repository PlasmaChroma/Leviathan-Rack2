0000b088 <_setOrder_shift>:
    b088: e5d0200b     	ldrb	r2, [r0, #0xb]
    b08c: e1a03000     	mov	r3, r0
    b090: e5d00006     	ldrb	r0, [r0, #0x6]
    b094: e3520000     	cmp	r2, #0
    b098: 0a000006     	beq	0xb0b8 <_setOrder_shift+0x30> @ imm = #0x18
    b09c: e5d3c005     	ldrb	r12, [r3, #0x5]
    b0a0: e35c0000     	cmp	r12, #0
    b0a4: 15d30015     	ldrbne	r0, [r3, #0x15]
    b0a8: 15d31025     	ldrbne	r1, [r3, #0x25]
    b0ac: 108c0000     	addne	r0, r12, r0
    b0b0: 16e10070     	uxtabne	r0, r1, r0
    b0b4: 16ef0070     	uxtbne	r0, r0
    b0b8: e5c30006     	strb	r0, [r3, #0x6]
    b0bc: e12fff1e     	bx	lr

