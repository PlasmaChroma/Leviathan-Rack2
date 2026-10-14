00002720 <arbhar_wtosc_tilde_setActive>:
    2720: eeb50ac0     	vcmpe.f32	s0, #0
    2724: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2728: c3a03001     	movgt	r3, #1
    272c: d3a03000     	movle	r3, #0
    2730: e5803054     	str	r3, [r0, #0x54]
    2734: e12fff1e     	bx	lr

