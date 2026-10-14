000029d8 <_setRevLed>:
    29d8: eefc7ac0     	vcvt.u32.f32	s15, s0
    29dc: e3a00023     	mov	r0, #35
    29e0: e92d4010     	push	{r4, lr}
    29e4: e24dd008     	sub	sp, sp, #8
    29e8: edcd7a01     	vstr	s15, [sp, #4]
    29ec: e5dd4004     	ldrb	r4, [sp, #0x4]
    29f0: e2041001     	and	r1, r4, #1
    29f4: ebfffdc4     	bl	0x210c <.plt+0x17c>     @ imm = #-0x8f0
    29f8: e1a010a4     	lsr	r1, r4, #1
    29fc: e3a0000d     	mov	r0, #13
    2a00: e28dd008     	add	sp, sp, #8
    2a04: e8bd4010     	pop	{r4, lr}
    2a08: eafffdbf     	b	0x210c <.plt+0x17c>     @ imm = #-0x904

