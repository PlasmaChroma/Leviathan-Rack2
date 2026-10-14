00002738 <_setRevLed>:
    2738: eefc7ac0     	vcvt.u32.f32	s15, s0
    273c: e3a00023     	mov	r0, #35
    2740: e92d4010     	push	{r4, lr}
    2744: e24dd008     	sub	sp, sp, #8
    2748: edcd7a01     	vstr	s15, [sp, #4]
    274c: e5dd4004     	ldrb	r4, [sp, #0x4]
    2750: e2041001     	and	r1, r4, #1
    2754: ebfffe51     	bl	0x20a0 <.plt+0x158>     @ imm = #-0x6bc  // CALL bcm2835_gpio_write
    2758: e1a010a4     	lsr	r1, r4, #1
    275c: e3a0000d     	mov	r0, #13
    2760: e28dd008     	add	sp, sp, #8
    2764: e8bd4010     	pop	{r4, lr}
    2768: eafffe4c     	b	0x20a0 <.plt+0x158>     @ imm = #-0x6d0  // CALL bcm2835_gpio_write

