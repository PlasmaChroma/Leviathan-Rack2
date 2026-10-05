0000b0c0 <_setOrder_capture>:
    b0c0: e5d0201b     	ldrb	r2, [r0, #0x1b]
    b0c4: e1a03000     	mov	r3, r0
    b0c8: e5d00016     	ldrb	r0, [r0, #0x16]
    b0cc: e3520000     	cmp	r2, #0
    b0d0: 0a000006     	beq	0xb0f0 <_setOrder_capture+0x30> @ imm = #0x18
    b0d4: e5d3c015     	ldrb	r12, [r3, #0x15]
    b0d8: e35c0000     	cmp	r12, #0
    b0dc: 15d30005     	ldrbne	r0, [r3, #0x5]
    b0e0: 15d31025     	ldrbne	r1, [r3, #0x25]
    b0e4: 108c0000     	addne	r0, r12, r0
    b0e8: 16e10070     	uxtabne	r0, r1, r0
    b0ec: 16ef0070     	uxtbne	r0, r0
    b0f0: e5c30016     	strb	r0, [r3, #0x16]
    b0f4: e12fff1e     	bx	lr

