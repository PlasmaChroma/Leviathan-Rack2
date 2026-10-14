000059a4 <_setPlusLed>:
    59a4: eefc7ac0     	vcvt.u32.f32	s15, s0
    59a8: e3a00028     	mov	r0, #40
    59ac: e92d4010     	push	{r4, lr}
    59b0: e24dd008     	sub	sp, sp, #8
    59b4: edcd7a01     	vstr	s15, [sp, #4]
    59b8: e5dd4004     	ldrb	r4, [sp, #0x4]
    59bc: e2041001     	and	r1, r4, #1
    59c0: ebfff31b     	bl	0x2634 <.plt+0x254>     @ imm = #-0x3394  // CALL bcm2835_gpio_write
    59c4: e1a010a4     	lsr	r1, r4, #1
    59c8: e3a00029     	mov	r0, #41
    59cc: e28dd008     	add	sp, sp, #8
    59d0: e8bd4010     	pop	{r4, lr}
    59d4: eafff316     	b	0x2634 <.plt+0x254>     @ imm = #-0x33a8  // CALL bcm2835_gpio_write

