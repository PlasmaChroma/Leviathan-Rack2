00002d7c <_setSpray>:
    2d7c: eef77a00     	vmov.f32	s15, #1.000000e+00
    2d80: eeb40ae7     	vcmpe.f32	s0, s15
    2d84: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2d88: aefd7ac0     	vcvtge.s32.f32	s15, s0
    2d8c: b3a03001     	movlt	r3, #1
    2d90: ae173a90     	vmovge	r3, s15
    2d94: e58030bc     	str	r3, [r0, #0xbc]
    2d98: e12fff1e     	bx	lr

