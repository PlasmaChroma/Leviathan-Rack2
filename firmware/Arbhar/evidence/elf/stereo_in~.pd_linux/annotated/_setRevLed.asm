000026b8 <_setRevLed>:
    26b8: eefc7ac0     	vcvt.u32.f32	s15, s0
    26bc: e3a00023     	mov	r0, #35
    26c0: e92d4010     	push	{r4, lr}
    26c4: e24dd008     	sub	sp, sp, #8
    26c8: edcd7a01     	vstr	s15, [sp, #4]
    26cc: e5dd4004     	ldrb	r4, [sp, #0x4]
    26d0: e2041001     	and	r1, r4, #1
    26d4: ebfffebe     	bl	0x21d4 <.plt+0x170>     @ imm = #-0x508  // CALL bcm2835_gpio_write
    26d8: e1a010a4     	lsr	r1, r4, #1
    26dc: e3a0000d     	mov	r0, #13
    26e0: e28dd008     	add	sp, sp, #8
    26e4: e8bd4010     	pop	{r4, lr}
    26e8: eafffeb9     	b	0x21d4 <.plt+0x170>     @ imm = #-0x51c  // CALL bcm2835_gpio_write

