0000087c <fftwObject_tilde_convLevel>:
     87c: eef77a00     	vmov.f32	s15, #1.000000e+00
     880: e2800866     	add	r0, r0, #6684672
     884: e2801ece     	add	r1, r0, #3296
     888: eeb40ae7     	vcmpe.f32	s0, s15
     88c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     890: beb00a67     	vmovlt.f32	s0, s15
     894: ed810a00     	vstr	s0, [r1]
     898: e12fff1e     	bx	lr

