0000334c <_setPlusLed>:
    334c: eefc7ac0     	vcvt.u32.f32	s15, s0
    3350: e3a00028     	mov	r0, #40
    3354: e92d4010     	push	{r4, lr}
    3358: e24dd008     	sub	sp, sp, #8
    335c: edcd7a01     	vstr	s15, [sp, #4]
    3360: e5dd4004     	ldrb	r4, [sp, #0x4]
    3364: e2041001     	and	r1, r4, #1
    3368: ebfffcfc     	bl	0x2760 <.plt+0x260>     @ imm = #-0xc10  // CALL bcm2835_gpio_write
    336c: e1a010a4     	lsr	r1, r4, #1
    3370: e3a00029     	mov	r0, #41
    3374: e28dd008     	add	sp, sp, #8
    3378: e8bd4010     	pop	{r4, lr}
    337c: eafffcf7     	b	0x2760 <.plt+0x260>     @ imm = #-0xc24  // CALL bcm2835_gpio_write

