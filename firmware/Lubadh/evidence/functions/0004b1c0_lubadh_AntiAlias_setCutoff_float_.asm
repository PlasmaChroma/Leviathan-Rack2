; lubadh::AntiAlias::setCutoff(float)
; VA 0x4b1c0 size 48

   4b1c0: eeb70ac0     	vcvt.f64.f32	d0, s0
   4b1c4: eddf0b07     	vldr	d16, [pc, #28]          @ 0x4b1e8 ; float 3.14159265359
   4b1c8: eef71b00     	vmov.f64	d17, #1.000000e+00
   4b1cc: ee200b20     	vmul.f64	d0, d0, d16
   4b1d0: eec10b80     	vdiv.f64	d16, d17, d0
   4b1d4: eef77be0     	vcvt.f32.f64	s15, d16
   4b1d8: edc07a00     	vstr	s15, [r0]
   4b1dc: edc07a03     	vstr	s15, [r0, #12]
   4b1e0: e12fff1e     	bx	lr
   4b1e4: e320f000     	nop
   4b1e8: 18 2d 44 54  	.word	0x54442d18
   4b1ec: fb 21 09 40  	.word	0x400921fb
