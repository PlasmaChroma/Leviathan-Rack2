0000276c <_setPlusLed>:
    276c: eefc7ac0     	vcvt.u32.f32	s15, s0
    2770: e3a00028     	mov	r0, #40
    2774: e92d4010     	push	{r4, lr}
    2778: e24dd008     	sub	sp, sp, #8
    277c: edcd7a01     	vstr	s15, [sp, #4]
    2780: e5dd4004     	ldrb	r4, [sp, #0x4]
    2784: e2041001     	and	r1, r4, #1
    2788: ebfffe44     	bl	0x20a0 <.plt+0x158>     @ imm = #-0x6f0
    278c: e1a010a4     	lsr	r1, r4, #1
    2790: e3a00029     	mov	r0, #41
    2794: e28dd008     	add	sp, sp, #8
    2798: e8bd4010     	pop	{r4, lr}
    279c: eafffe3f     	b	0x20a0 <.plt+0x158>     @ imm = #-0x704

