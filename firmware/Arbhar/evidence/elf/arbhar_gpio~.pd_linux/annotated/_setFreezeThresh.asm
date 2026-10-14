0000409c <_setFreezeThresh>:
    409c: eefc0ae0     	vcvt.u32.f32	s1, s1
    40a0: e24dd008     	sub	sp, sp, #8
    40a4: e3a02f71     	mov	r2, #452
    40a8: eefc7ac0     	vcvt.u32.f32	s15, s0
    40ac: ee10ca90     	vmov	r12, s1
    40b0: edcd7a01     	vstr	s15, [sp, #4]
    40b4: e5dd3004     	ldrb	r3, [sp, #0x4]
    40b8: e0200392     	mla	r0, r2, r3, r0
    40bc: e2801fb5     	add	r1, r0, #724
    40c0: e1c1c0b0     	strh	r12, [r1]
    40c4: e28dd008     	add	sp, sp, #8
    40c8: e12fff1e     	bx	lr

