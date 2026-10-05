0000de24 <getModeFromOrder>:
    de24: e92d4070     	push	{r4, r5, r6, lr}
    de28: e1a04000     	mov	r4, r0
    de2c: ebffd7b4     	bl	0x3d04 <.plt+0x608>     @ imm = #-0xa130  // CALL _setOrder_shift
    de30: e1a05000     	mov	r5, r0
    de34: e1a00004     	mov	r0, r4
    de38: ebffd739     	bl	0x3b24 <.plt+0x428>     @ imm = #-0xa31c  // CALL _setOrder_capture
    de3c: e1a06000     	mov	r6, r0
    de40: e1a00004     	mov	r0, r4
    de44: ebffd682     	bl	0x3854 <.plt+0x158>     @ imm = #-0xa5f8  // CALL _setOrder_strike
    de48: e2453001     	sub	r3, r5, #1
    de4c: e16f1f13     	clz	r1, r3
    de50: e3560002     	cmp	r6, #2
    de54: 03550001     	cmpeq	r5, #1
    de58: e1a022a1     	lsr	r2, r1, #5
    de5c: 0a000010     	beq	0xdea4 <getModeFromOrder+0x80> @ imm = #0x40
    de60: e3560000     	cmp	r6, #0
    de64: 13a02000     	movne	r2, #0
    de68: e3520000     	cmp	r2, #0
    de6c: 0a000007     	beq	0xde90 <getModeFromOrder+0x6c> @ imm = #0x1c
    de70: e3500000     	cmp	r0, #0
    de74: 18bd8070     	popne	{r4, r5, r6, pc}
    de78: e1a00004     	mov	r0, r4
    de7c: e3a04001     	mov	r4, #1
    de80: e1a01004     	mov	r1, r4
    de84: e4c04004     	strb	r4, [r0], #4
    de88: e8bd4070     	pop	{r4, r5, r6, lr}
    de8c: eaffd7a2     	b	0x3d1c <.plt+0x620>     @ imm = #-0xa178  // CALL setIgnore
    de90: e1855006     	orr	r5, r5, r6
    de94: e1800005     	orr	r0, r0, r5
    de98: e210c0ff     	ands	r12, r0, #255
    de9c: 05c4c000     	strbeq	r12, [r4]
    dea0: e8bd8070     	pop	{r4, r5, r6, pc}
    dea4: e3500003     	cmp	r0, #3
    dea8: 0a00000a     	beq	0xded8 <getModeFromOrder+0xb4> @ imm = #0x28
    deac: e3500000     	cmp	r0, #0
    deb0: 18bd8070     	popne	{r4, r5, r6, pc}
    deb4: e1a00004     	mov	r0, r4
    deb8: e3a05003     	mov	r5, #3
    debc: e3a01001     	mov	r1, #1
    dec0: e4c05004     	strb	r5, [r0], #4
    dec4: ebffd794     	bl	0x3d1c <.plt+0x620>     @ imm = #-0xa1b0  // CALL setIgnore
    dec8: e2840014     	add	r0, r4, #20
    decc: e3a01001     	mov	r1, #1
    ded0: e8bd4070     	pop	{r4, r5, r6, lr}
    ded4: eaffd790     	b	0x3d1c <.plt+0x620>     @ imm = #-0xa1c0  // CALL setIgnore
    ded8: e1a00004     	mov	r0, r4
    dedc: e3a06004     	mov	r6, #4
    dee0: e3a01001     	mov	r1, #1
    dee4: e4c06004     	strb	r6, [r0], #4
    dee8: ebffd78b     	bl	0x3d1c <.plt+0x620>     @ imm = #-0xa1d4  // CALL setIgnore
    deec: e2840014     	add	r0, r4, #20
    def0: e3a01001     	mov	r1, #1
    def4: ebffd788     	bl	0x3d1c <.plt+0x620>     @ imm = #-0xa1e0  // CALL setIgnore
    def8: e2840024     	add	r0, r4, #36
    defc: e3a01001     	mov	r1, #1
    df00: e8bd4070     	pop	{r4, r5, r6, lr}
    df04: eaffd784     	b	0x3d1c <.plt+0x620>     @ imm = #-0xa1f0  // CALL setIgnore

