00002700 <arbhar_wtosc_tilde_start>:
    2700: eddf7a05     	vldr	s15, [pc, #20]          @ 0x271c <arbhar_wtosc_tilde_start+0x1c>  // f32=0
    2704: eeb40ae7     	vcmpe.f32	s0, s15
    2708: eef1fa10     	vmrs	APSR_nzcv, fpscr
    270c: beb00a67     	vmovlt.f32	s0, s15
    2710: eebd0ac0     	vcvt.s32.f32	s0, s0
    2714: ed800a0f     	vstr	s0, [r0, #60]
    2718: e12fff1e     	bx	lr
    271c: 00 00 00 00  	.word	0x00000000

