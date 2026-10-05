00007c14 <_setPlayAndRecLayerInOmega>:
    7c14: e92d4070     	push	{r4, r5, r6, lr}
    7c18: e1a02001     	mov	r2, r1
    7c1c: e1a04000     	mov	r4, r0
    7c20: e5c01036     	strb	r1, [r0, #0x36]
    7c24: e1a05001     	mov	r5, r1
    7c28: e3a0101e     	mov	r1, #30
    7c2c: ebffefad     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x414c  // CALL writeToSharedMem
    7c30: e5d43031     	ldrb	r3, [r4, #0x31]
    7c34: e3530000     	cmp	r3, #0
    7c38: 08bd8070     	popeq	{r4, r5, r6, pc}
    7c3c: e1a02005     	mov	r2, r5
    7c40: e1a00004     	mov	r0, r4
    7c44: e3a0101f     	mov	r1, #31
    7c48: e8bd4070     	pop	{r4, r5, r6, lr}
    7c4c: eaffefa5     	b	0x3ae8 <.plt+0x3ec>     @ imm = #-0x416c  // CALL writeToSharedMem

