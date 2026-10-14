00004130 <_setFwdLed>:
    4130: eefc7ac0     	vcvt.u32.f32	s15, s0
    4134: e3a00025     	mov	r0, #37
    4138: e92d4010     	push	{r4, lr}
    413c: e24dd008     	sub	sp, sp, #8
    4140: edcd7a01     	vstr	s15, [sp, #4]
    4144: e5dd4004     	ldrb	r4, [sp, #0x4]
    4148: e2041001     	and	r1, r4, #1
    414c: ebfffe92     	bl	0x3b9c <.plt+0x4a0>     @ imm = #-0x5b8
    4150: e1a010a4     	lsr	r1, r4, #1
    4154: e3a00016     	mov	r0, #22
    4158: e28dd008     	add	sp, sp, #8
    415c: e8bd4010     	pop	{r4, lr}
    4160: eafffe8d     	b	0x3b9c <.plt+0x4a0>     @ imm = #-0x5cc

