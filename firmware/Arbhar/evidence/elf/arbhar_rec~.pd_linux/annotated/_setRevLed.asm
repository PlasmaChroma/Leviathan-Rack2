00003318 <_setRevLed>:
    3318: eefc7ac0     	vcvt.u32.f32	s15, s0
    331c: e3a00023     	mov	r0, #35
    3320: e92d4010     	push	{r4, lr}
    3324: e24dd008     	sub	sp, sp, #8
    3328: edcd7a01     	vstr	s15, [sp, #4]
    332c: e5dd4004     	ldrb	r4, [sp, #0x4]
    3330: e2041001     	and	r1, r4, #1
    3334: ebfffd09     	bl	0x2760 <.plt+0x260>     @ imm = #-0xbdc  // CALL bcm2835_gpio_write
    3338: e1a010a4     	lsr	r1, r4, #1
    333c: e3a0000d     	mov	r0, #13
    3340: e28dd008     	add	sp, sp, #8
    3344: e8bd4010     	pop	{r4, lr}
    3348: eafffd04     	b	0x2760 <.plt+0x260>     @ imm = #-0xbf0  // CALL bcm2835_gpio_write

