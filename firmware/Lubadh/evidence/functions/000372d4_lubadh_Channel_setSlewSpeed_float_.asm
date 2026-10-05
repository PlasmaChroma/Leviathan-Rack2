; lubadh::Channel::setSlewSpeed(float)
; VA 0x372d4 size 72

   372d4: e59030e8     	ldr	r3, [r0, #0xe8]
   372d8: eef76a00     	vmov.f32	s13, #1.000000e+00
   372dc: ed800a0b     	vstr	s0, [r0, #44]
   372e0: edd07ab6     	vldr	s15, [r0, #728]
   372e4: ed9f6a0b     	vldr	s12, [pc, #44]          @ 0x37318 ; float 2.70000004768
   372e8: ed937a2b     	vldr	s14, [r3, #172]
   372ec: ee300a67     	vsub.f32	s0, s0, s15
   372f0: eebd7ac7     	vcvt.s32.f32	s14, s14
   372f4: eeb87ac7     	vcvt.f32.s32	s14, s14
   372f8: eec77a06     	vdiv.f32	s15, s14, s12
   372fc: ee777aa6     	vadd.f32	s15, s15, s13
   37300: eefd7ae7     	vcvt.s32.f32	s15, s15
   37304: eeb87ae7     	vcvt.f32.s32	s14, s15
   37308: edc07ab8     	vstr	s15, [r0, #736]
   3730c: eec07a07     	vdiv.f32	s15, s0, s14
   37310: edc07ab7     	vstr	s15, [r0, #732]
   37314: e12fff1e     	bx	lr
   37318: cd cc 2c 40  	.word	0x402ccccd
