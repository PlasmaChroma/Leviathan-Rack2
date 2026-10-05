00004198 <_setPlusLed>:
    4198: eefc7ac0     	vcvt.u32.f32	s15, s0
    419c: e3a00028     	mov	r0, #40
    41a0: e92d4010     	push	{r4, lr}
    41a4: e24dd008     	sub	sp, sp, #8
    41a8: edcd7a01     	vstr	s15, [sp, #4]
    41ac: e5dd4004     	ldrb	r4, [sp, #0x4]
    41b0: e2041001     	and	r1, r4, #1
    41b4: ebfffe78     	bl	0x3b9c <.plt+0x4a0>     @ imm = #-0x620
    41b8: e1a010a4     	lsr	r1, r4, #1
    41bc: e3a00029     	mov	r0, #41
    41c0: e28dd008     	add	sp, sp, #8
    41c4: e8bd4010     	pop	{r4, lr}
    41c8: eafffe73     	b	0x3b9c <.plt+0x4a0>     @ imm = #-0x634

