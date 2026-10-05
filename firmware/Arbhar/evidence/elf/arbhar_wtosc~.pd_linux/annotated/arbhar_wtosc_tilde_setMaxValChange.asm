00002738 <arbhar_wtosc_tilde_setMaxValChange>:
    2738: eddf7a04     	vldr	s15, [pc, #16]          @ 0x2750 <arbhar_wtosc_tilde_setMaxValChange+0x18>  // f32=0
    273c: eeb40ae7     	vcmpe.f32	s0, s15
    2740: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2744: beb00a67     	vmovlt.f32	s0, s15
    2748: ed800a17     	vstr	s0, [r0, #92]
    274c: e12fff1e     	bx	lr
    2750: 00 00 00 00  	.word	0x00000000

