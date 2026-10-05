00002d38 <arbhar_play_tilde_killOffset>:
    2d38: eefd7ac0     	vcvt.s32.f32	s15, s0
    2d3c: ee173a90     	vmov	r3, s15
    2d40: e3530052     	cmp	r3, #82
    2d44: d2800a02     	addle	r0, r0, #8192
    2d48: d58036c8     	strle	r3, [r0, #0x6c8]
    2d4c: e12fff1e     	bx	lr

