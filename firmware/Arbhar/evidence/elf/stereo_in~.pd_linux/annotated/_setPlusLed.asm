000026ec <_setPlusLed>:
    26ec: eefc7ac0     	vcvt.u32.f32	s15, s0
    26f0: e3a00028     	mov	r0, #40
    26f4: e92d4010     	push	{r4, lr}
    26f8: e24dd008     	sub	sp, sp, #8
    26fc: edcd7a01     	vstr	s15, [sp, #4]
    2700: e5dd4004     	ldrb	r4, [sp, #0x4]
    2704: e2041001     	and	r1, r4, #1
    2708: ebfffeb1     	bl	0x21d4 <.plt+0x170>     @ imm = #-0x53c  // CALL bcm2835_gpio_write
    270c: e1a010a4     	lsr	r1, r4, #1
    2710: e3a00029     	mov	r0, #41
    2714: e28dd008     	add	sp, sp, #8
    2718: e8bd4010     	pop	{r4, lr}
    271c: eafffeac     	b	0x21d4 <.plt+0x170>     @ imm = #-0x550  // CALL bcm2835_gpio_write

