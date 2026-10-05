000026ac <_setFwdLed>:
    26ac: eefc7ac0     	vcvt.u32.f32	s15, s0
    26b0: e3a00025     	mov	r0, #37
    26b4: e92d4010     	push	{r4, lr}
    26b8: e24dd008     	sub	sp, sp, #8
    26bc: edcd7a01     	vstr	s15, [sp, #4]
    26c0: e5dd4004     	ldrb	r4, [sp, #0x4]
    26c4: e2041001     	and	r1, r4, #1
    26c8: ebfffe7b     	bl	0x20bc <.plt+0x164>     @ imm = #-0x614
    26cc: e1a010a4     	lsr	r1, r4, #1
    26d0: e3a00016     	mov	r0, #22
    26d4: e28dd008     	add	sp, sp, #8
    26d8: e8bd4010     	pop	{r4, lr}
    26dc: eafffe76     	b	0x20bc <.plt+0x164>     @ imm = #-0x628

