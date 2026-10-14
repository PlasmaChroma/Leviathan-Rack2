00002d90 <_setRevLed>:
    2d90: eefc7ac0     	vcvt.u32.f32	s15, s0
    2d94: e3a00023     	mov	r0, #35
    2d98: e92d4010     	push	{r4, lr}
    2d9c: e24dd008     	sub	sp, sp, #8
    2da0: edcd7a01     	vstr	s15, [sp, #4]
    2da4: e5dd4004     	ldrb	r4, [sp, #0x4]
    2da8: e2041001     	and	r1, r4, #1
    2dac: ebfffdd8     	bl	0x2514 <.plt+0x230>     @ imm = #-0x8a0  // CALL bcm2835_gpio_write
    2db0: e1a010a4     	lsr	r1, r4, #1
    2db4: e3a0000d     	mov	r0, #13
    2db8: e28dd008     	add	sp, sp, #8
    2dbc: e8bd4010     	pop	{r4, lr}
    2dc0: eafffdd3     	b	0x2514 <.plt+0x230>     @ imm = #-0x8b4  // CALL bcm2835_gpio_write

