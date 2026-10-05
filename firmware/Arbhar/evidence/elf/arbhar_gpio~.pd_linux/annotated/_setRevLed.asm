00004164 <_setRevLed>:
    4164: eefc7ac0     	vcvt.u32.f32	s15, s0
    4168: e3a00023     	mov	r0, #35
    416c: e92d4010     	push	{r4, lr}
    4170: e24dd008     	sub	sp, sp, #8
    4174: edcd7a01     	vstr	s15, [sp, #4]
    4178: e5dd4004     	ldrb	r4, [sp, #0x4]
    417c: e2041001     	and	r1, r4, #1
    4180: ebfffe85     	bl	0x3b9c <.plt+0x4a0>     @ imm = #-0x5ec  // CALL bcm2835_gpio_write
    4184: e1a010a4     	lsr	r1, r4, #1
    4188: e3a0000d     	mov	r0, #13
    418c: e28dd008     	add	sp, sp, #8
    4190: e8bd4010     	pop	{r4, lr}
    4194: eafffe80     	b	0x3b9c <.plt+0x4a0>     @ imm = #-0x600  // CALL bcm2835_gpio_write

