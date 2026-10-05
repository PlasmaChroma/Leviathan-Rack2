00000d90 <comport_pollintervall>:
     d90: eddf7a08     	vldr	s15, [pc, #32]          @ 0xdb8 <comport_pollintervall+0x28>
     d94: e2800d43     	add	r0, r0, #4288
     d98: eeb40ae7     	vcmpe.f32	s0, s15
     d9c: eef1fa10     	vmrs	APSR_nzcv, fpscr
     da0: 5eb70ac0     	vcvtpl.f64.f32	d0, s0
     da4: 4d9f0b01     	vldrmi	d0, [pc, #4]            @ 0xdb0 <comport_pollintervall+0x20>
     da8: ed800b06     	vstr	d0, [r0, #24]
     dac: e12fff1e     	bx	lr
     db0: 00 00 00 00  	.word	0x00000000
     db4: 00 00 f0 3f  	.word	0x3ff00000
     db8: 00 00 80 3f  	.word	0x3f800000

