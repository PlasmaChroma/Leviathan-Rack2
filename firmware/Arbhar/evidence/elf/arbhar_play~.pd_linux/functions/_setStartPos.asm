00002d60 <_setStartPos>:
    2d60: eeb50ac0     	vcmpe.f32	s0, #0
    2d64: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2d68: aefd7ac0     	vcvtge.s32.f32	s15, s0
    2d6c: b3a03000     	movlt	r3, #0
    2d70: ae173a90     	vmovge	r3, s15
    2d74: e58030b4     	str	r3, [r0, #0xb4]
    2d78: e12fff1e     	bx	lr

