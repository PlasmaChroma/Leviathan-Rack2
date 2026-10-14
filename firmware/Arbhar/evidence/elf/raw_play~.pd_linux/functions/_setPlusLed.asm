00002a0c <_setPlusLed>:
    2a0c: eefc7ac0     	vcvt.u32.f32	s15, s0
    2a10: e3a00028     	mov	r0, #40
    2a14: e92d4010     	push	{r4, lr}
    2a18: e24dd008     	sub	sp, sp, #8
    2a1c: edcd7a01     	vstr	s15, [sp, #4]
    2a20: e5dd4004     	ldrb	r4, [sp, #0x4]
    2a24: e2041001     	and	r1, r4, #1
    2a28: ebfffdb7     	bl	0x210c <.plt+0x17c>     @ imm = #-0x924
    2a2c: e1a010a4     	lsr	r1, r4, #1
    2a30: e3a00029     	mov	r0, #41
    2a34: e28dd008     	add	sp, sp, #8
    2a38: e8bd4010     	pop	{r4, lr}
    2a3c: eafffdb2     	b	0x210c <.plt+0x17c>     @ imm = #-0x938

