000029a4 <_setFwdLed>:
    29a4: eefc7ac0     	vcvt.u32.f32	s15, s0
    29a8: e3a00025     	mov	r0, #37
    29ac: e92d4010     	push	{r4, lr}
    29b0: e24dd008     	sub	sp, sp, #8
    29b4: edcd7a01     	vstr	s15, [sp, #4]
    29b8: e5dd4004     	ldrb	r4, [sp, #0x4]
    29bc: e2041001     	and	r1, r4, #1
    29c0: ebfffdd1     	bl	0x210c <.plt+0x17c>     @ imm = #-0x8bc  // CALL bcm2835_gpio_write
    29c4: e1a010a4     	lsr	r1, r4, #1
    29c8: e3a00016     	mov	r0, #22
    29cc: e28dd008     	add	sp, sp, #8
    29d0: e8bd4010     	pop	{r4, lr}
    29d4: eafffdcc     	b	0x210c <.plt+0x17c>     @ imm = #-0x8d0  // CALL bcm2835_gpio_write

