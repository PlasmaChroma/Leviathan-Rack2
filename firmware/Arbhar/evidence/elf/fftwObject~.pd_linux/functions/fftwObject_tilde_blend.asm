000008c0 <fftwObject_tilde_blend>:
     8c0: ed9f7a0b     	vldr	s14, [pc, #44]          @ 0x8f4 <fftwObject_tilde_blend+0x34>
     8c4: e3003cdc     	movw	r3, #0xcdc
     8c8: e3403066     	movt	r3, #0x66
     8cc: e0800003     	add	r0, r0, r3
     8d0: eef77a00     	vmov.f32	s15, #1.000000e+00
     8d4: eeb40ac7     	vcmpe.f32	s0, s14
     8d8: eef1fa10     	vmrs	APSR_nzcv, fpscr
     8dc: beb00a47     	vmovlt.f32	s0, s14
     8e0: eeb40ae7     	vcmpe.f32	s0, s15
     8e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
     8e8: 8eb00a67     	vmovhi.f32	s0, s15
     8ec: ed800a00     	vstr	s0, [r0]
     8f0: e12fff1e     	bx	lr
     8f4: 00 00 00 00  	.word	0x00000000

