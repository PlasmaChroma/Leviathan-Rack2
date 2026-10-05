000040cc <_setFreezeCount>:
    40cc: eefc0ae0     	vcvt.u32.f32	s1, s1
    40d0: e24dd008     	sub	sp, sp, #8
    40d4: e3a02f71     	mov	r2, #452
    40d8: eefc7ac0     	vcvt.u32.f32	s15, s0
    40dc: ee10ca90     	vmov	r12, s1
    40e0: edcd7a01     	vstr	s15, [sp, #4]
    40e4: e5dd3004     	ldrb	r3, [sp, #0x4]
    40e8: e0200392     	mla	r0, r2, r3, r0
    40ec: e2801fb5     	add	r1, r0, #724
    40f0: e1c1c0b2     	strh	r12, [r1, #2]
    40f4: e28dd008     	add	sp, sp, #8
    40f8: e12fff1e     	bx	lr

