000032e4 <_setFwdLed>:
    32e4: eefc7ac0     	vcvt.u32.f32	s15, s0
    32e8: e3a00025     	mov	r0, #37
    32ec: e92d4010     	push	{r4, lr}
    32f0: e24dd008     	sub	sp, sp, #8
    32f4: edcd7a01     	vstr	s15, [sp, #4]
    32f8: e5dd4004     	ldrb	r4, [sp, #0x4]
    32fc: e2041001     	and	r1, r4, #1
    3300: ebfffd16     	bl	0x2760 <.plt+0x260>     @ imm = #-0xba8  // CALL bcm2835_gpio_write
    3304: e1a010a4     	lsr	r1, r4, #1
    3308: e3a00016     	mov	r0, #22
    330c: e28dd008     	add	sp, sp, #8
    3310: e8bd4010     	pop	{r4, lr}
    3314: eafffd11     	b	0x2760 <.plt+0x260>     @ imm = #-0xbbc  // CALL bcm2835_gpio_write

