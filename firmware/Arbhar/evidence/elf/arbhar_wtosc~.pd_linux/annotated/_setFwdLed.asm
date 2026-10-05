00002d5c <_setFwdLed>:
    2d5c: eefc7ac0     	vcvt.u32.f32	s15, s0
    2d60: e3a00025     	mov	r0, #37
    2d64: e92d4010     	push	{r4, lr}
    2d68: e24dd008     	sub	sp, sp, #8
    2d6c: edcd7a01     	vstr	s15, [sp, #4]
    2d70: e5dd4004     	ldrb	r4, [sp, #0x4]
    2d74: e2041001     	and	r1, r4, #1
    2d78: ebfffde5     	bl	0x2514 <.plt+0x230>     @ imm = #-0x86c  // CALL bcm2835_gpio_write
    2d7c: e1a010a4     	lsr	r1, r4, #1
    2d80: e3a00016     	mov	r0, #22
    2d84: e28dd008     	add	sp, sp, #8
    2d88: e8bd4010     	pop	{r4, lr}
    2d8c: eafffde0     	b	0x2514 <.plt+0x230>     @ imm = #-0x880  // CALL bcm2835_gpio_write

