0000593c <_setFwdLed>:
    593c: eefc7ac0     	vcvt.u32.f32	s15, s0
    5940: e3a00025     	mov	r0, #37
    5944: e92d4010     	push	{r4, lr}
    5948: e24dd008     	sub	sp, sp, #8
    594c: edcd7a01     	vstr	s15, [sp, #4]
    5950: e5dd4004     	ldrb	r4, [sp, #0x4]
    5954: e2041001     	and	r1, r4, #1
    5958: ebfff335     	bl	0x2634 <.plt+0x254>     @ imm = #-0x332c  // CALL bcm2835_gpio_write
    595c: e1a010a4     	lsr	r1, r4, #1
    5960: e3a00016     	mov	r0, #22
    5964: e28dd008     	add	sp, sp, #8
    5968: e8bd4010     	pop	{r4, lr}
    596c: eafff330     	b	0x2634 <.plt+0x254>     @ imm = #-0x3340  // CALL bcm2835_gpio_write

