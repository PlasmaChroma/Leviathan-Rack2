00002684 <_setFwdLed>:
    2684: eefc7ac0     	vcvt.u32.f32	s15, s0
    2688: e3a00025     	mov	r0, #37
    268c: e92d4010     	push	{r4, lr}
    2690: e24dd008     	sub	sp, sp, #8
    2694: edcd7a01     	vstr	s15, [sp, #4]
    2698: e5dd4004     	ldrb	r4, [sp, #0x4]
    269c: e2041001     	and	r1, r4, #1
    26a0: ebfffecb     	bl	0x21d4 <.plt+0x170>     @ imm = #-0x4d4  // CALL bcm2835_gpio_write
    26a4: e1a010a4     	lsr	r1, r4, #1
    26a8: e3a00016     	mov	r0, #22
    26ac: e28dd008     	add	sp, sp, #8
    26b0: e8bd4010     	pop	{r4, lr}
    26b4: eafffec6     	b	0x21d4 <.plt+0x170>     @ imm = #-0x4e8  // CALL bcm2835_gpio_write

