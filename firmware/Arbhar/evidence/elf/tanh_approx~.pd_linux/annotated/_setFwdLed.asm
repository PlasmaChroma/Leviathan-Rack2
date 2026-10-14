00002704 <_setFwdLed>:
    2704: eefc7ac0     	vcvt.u32.f32	s15, s0
    2708: e3a00025     	mov	r0, #37
    270c: e92d4010     	push	{r4, lr}
    2710: e24dd008     	sub	sp, sp, #8
    2714: edcd7a01     	vstr	s15, [sp, #4]
    2718: e5dd4004     	ldrb	r4, [sp, #0x4]
    271c: e2041001     	and	r1, r4, #1
    2720: ebfffe5e     	bl	0x20a0 <.plt+0x158>     @ imm = #-0x688  // CALL bcm2835_gpio_write
    2724: e1a010a4     	lsr	r1, r4, #1
    2728: e3a00016     	mov	r0, #22
    272c: e28dd008     	add	sp, sp, #8
    2730: e8bd4010     	pop	{r4, lr}
    2734: eafffe59     	b	0x20a0 <.plt+0x158>     @ imm = #-0x69c  // CALL bcm2835_gpio_write

