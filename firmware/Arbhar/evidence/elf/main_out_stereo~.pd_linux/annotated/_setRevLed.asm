000026e0 <_setRevLed>:
    26e0: eefc7ac0     	vcvt.u32.f32	s15, s0
    26e4: e3a00023     	mov	r0, #35
    26e8: e92d4010     	push	{r4, lr}
    26ec: e24dd008     	sub	sp, sp, #8
    26f0: edcd7a01     	vstr	s15, [sp, #4]
    26f4: e5dd4004     	ldrb	r4, [sp, #0x4]
    26f8: e2041001     	and	r1, r4, #1
    26fc: ebfffe6e     	bl	0x20bc <.plt+0x164>     @ imm = #-0x648  // CALL bcm2835_gpio_write
    2700: e1a010a4     	lsr	r1, r4, #1
    2704: e3a0000d     	mov	r0, #13
    2708: e28dd008     	add	sp, sp, #8
    270c: e8bd4010     	pop	{r4, lr}
    2710: eafffe69     	b	0x20bc <.plt+0x164>     @ imm = #-0x65c  // CALL bcm2835_gpio_write

