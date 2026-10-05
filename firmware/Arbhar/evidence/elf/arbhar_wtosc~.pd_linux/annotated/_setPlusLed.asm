00002dc4 <_setPlusLed>:
    2dc4: eefc7ac0     	vcvt.u32.f32	s15, s0
    2dc8: e3a00028     	mov	r0, #40
    2dcc: e92d4010     	push	{r4, lr}
    2dd0: e24dd008     	sub	sp, sp, #8
    2dd4: edcd7a01     	vstr	s15, [sp, #4]
    2dd8: e5dd4004     	ldrb	r4, [sp, #0x4]
    2ddc: e2041001     	and	r1, r4, #1
    2de0: ebfffdcb     	bl	0x2514 <.plt+0x230>     @ imm = #-0x8d4  // CALL bcm2835_gpio_write
    2de4: e1a010a4     	lsr	r1, r4, #1
    2de8: e3a00029     	mov	r0, #41
    2dec: e28dd008     	add	sp, sp, #8
    2df0: e8bd4010     	pop	{r4, lr}
    2df4: eafffdc6     	b	0x2514 <.plt+0x230>     @ imm = #-0x8e8  // CALL bcm2835_gpio_write

