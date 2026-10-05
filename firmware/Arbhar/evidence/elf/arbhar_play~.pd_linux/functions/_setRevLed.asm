00005970 <_setRevLed>:
    5970: eefc7ac0     	vcvt.u32.f32	s15, s0
    5974: e3a00023     	mov	r0, #35
    5978: e92d4010     	push	{r4, lr}
    597c: e24dd008     	sub	sp, sp, #8
    5980: edcd7a01     	vstr	s15, [sp, #4]
    5984: e5dd4004     	ldrb	r4, [sp, #0x4]
    5988: e2041001     	and	r1, r4, #1
    598c: ebfff328     	bl	0x2634 <.plt+0x254>     @ imm = #-0x3360
    5990: e1a010a4     	lsr	r1, r4, #1
    5994: e3a0000d     	mov	r0, #13
    5998: e28dd008     	add	sp, sp, #8
    599c: e8bd4010     	pop	{r4, lr}
    59a0: eafff323     	b	0x2634 <.plt+0x254>     @ imm = #-0x3374

