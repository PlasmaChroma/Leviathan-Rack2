00002854 <_observeRecording>:
    2854: eefd7ac0     	vcvt.s32.f32	s15, s0
    2858: e2800a02     	add	r0, r0, #8192
    285c: ee173a90     	vmov	r3, s15
    2860: eeb50ac0     	vcmpe.f32	s0, #0
    2864: e3530000     	cmp	r3, #0
    2868: c3a03001     	movgt	r3, #1
    286c: d3a03000     	movle	r3, #0
    2870: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2874: e58036cc     	str	r3, [r0, #0x6cc]
    2878: c3a03000     	movgt	r3, #0
    287c: d3e03102     	mvnle	r3, #-2147483648
    2880: e58036d0     	str	r3, [r0, #0x6d0]
    2884: e12fff1e     	bx	lr

