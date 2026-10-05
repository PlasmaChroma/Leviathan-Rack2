; lubadh::TapeAge::setTapeAge(float)
; VA 0x4f998 size 164

   4f998: eddf4a20     	vldr	s9, [pc, #128]          @ 0x4fa20 ; float 180
   4f99c: eef37a04     	vmov.f32	s15, #2.000000e+01
   4f9a0: ed9f5a1f     	vldr	s10, [pc, #124]         @ 0x4fa24 ; float -17000
   4f9a4: ed9f7a1f     	vldr	s14, [pc, #124]         @ 0x4fa28 ; float 20000
   4f9a8: eee07a24     	vfma.f32	s15, s0, s9
   4f9ac: eddf0a1e     	vldr	s1, [pc, #120]          @ 0x4fa2c ; float 0.20000000298
   4f9b0: eea07a05     	vfma.f32	s14, s0, s10
   4f9b4: eddf6a1d     	vldr	s13, [pc, #116]         @ 0x4fa30 ; float 49170.2539062
   4f9b8: eddf5a1d     	vldr	s11, [pc, #116]         @ 0x4fa34 ; float 0
   4f9bc: ed9f6a1d     	vldr	s12, [pc, #116]         @ 0x4fa38 ; float 0.600000023842
   4f9c0: eeb05a60     	vmov.f32	s10, s1
   4f9c4: e92d4010     	push	{r4, lr}
   4f9c8: e1a04000     	mov	r4, r0
   4f9cc: eea05a25     	vfma.f32	s10, s0, s11
   4f9d0: eee00a06     	vfma.f32	s1, s0, s12
   4f9d4: e2800010     	add	r0, r0, #16
   4f9d8: ee870aa6     	vdiv.f32	s0, s15, s13
   4f9dc: ee876a26     	vdiv.f32	s12, s14, s13
   4f9e0: ed845a03     	vstr	s10, [r4, #12]
   4f9e4: edc40a01     	vstr	s1, [r4, #4]
   4f9e8: ed840a00     	vstr	s0, [r4]
   4f9ec: ed846a02     	vstr	s12, [r4, #8]
   4f9f0: ebffebde     	bl	0x4a970
   4f9f4: edd47a02     	vldr	s15, [r4, #8]
   4f9f8: eddf1b06     	vldr	d17, [pc, #24]          @ 0x4fa18 ; float 3.14159265359
   4f9fc: eef72b00     	vmov.f64	d18, #1.000000e+00
   4fa00: eef70ae7     	vcvt.f64.f32	d16, s15
   4fa04: ee600ba1     	vmul.f64	d16, d16, d17
   4fa08: eec21ba0     	vdiv.f64	d17, d18, d16
   4fa0c: eef77be1     	vcvt.f32.f64	s15, d17
   4fa10: edc47a0f     	vstr	s15, [r4, #60]
   4fa14: e8bd8010     	pop	{r4, pc}
   4fa18: 18 2d 44 54  	.word	0x54442d18
   4fa1c: fb 21 09 40  	.word	0x400921fb
   4fa20: 00 00 34 43  	.word	0x43340000
   4fa24: 00 d0 84 c6  	.word	0xc684d000
   4fa28: 00 40 9c 46  	.word	0x469c4000
   4fa2c: cd cc 4c 3e  	.word	0x3e4ccccd
   4fa30: 41 12 40 47  	.word	0x47401241
   4fa34: 00 00 00 00  	.word	0x00000000
   4fa38: 9a 99 19 3f  	.word	0x3f19999a
