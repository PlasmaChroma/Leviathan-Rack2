00002a28 <arbhar_rec_tilde_overdub>:
    2a28: eddf7a06     	vldr	s15, [pc, #24]          @ 0x2a48 <arbhar_rec_tilde_overdub+0x20>
    2a2c: eeb40ae7     	vcmpe.f32	s0, s15
    2a30: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2a34: beb00a67     	vmovlt.f32	s0, s15
    2a38: ed800a97     	vstr	s0, [r0, #604]
    2a3c: ed800a0e     	vstr	s0, [r0, #56]
    2a40: ed800a1a     	vstr	s0, [r0, #104]
    2a44: e12fff1e     	bx	lr
    2a48: 00 00 00 00  	.word	0x00000000

