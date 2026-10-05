; lubadh::Channel::InputBuffer::interpolate(float) const
; VA 0x38ab4 size 140

   38ab4: eefd7ac0     	vcvt.s32.f32	s15, s0
   38ab8: eeb07a40     	vmov.f32	s14, s0
   38abc: e9900006     	ldmib	r0, {r1, r2}
   38ac0: eef04a08     	vmov.f32	s9, #3.000000e+00
   38ac4: eef03a00     	vmov.f32	s7, #2.000000e+00
   38ac8: eeb75a00     	vmov.f32	s10, #1.000000e+00
   38acc: e0422001     	sub	r2, r2, r1
   38ad0: eddf5a19     	vldr	s11, [pc, #100]         @ 0x38b3c ; float 0.166666701436
   38ad4: ee173a90     	vmov	r3, s15
   38ad8: e1a02142     	asr	r2, r2, #2
   38adc: e2422004     	sub	r2, r2, #4
   38ae0: e1520003     	cmp	r2, r3
   38ae4: b1a03002     	movlt	r3, r2
   38ae8: e1c33fc3     	bic	r3, r3, r3, asr #31
   38aec: ee073a90     	vmov	s15, r3
   38af0: e0812103     	add	r2, r1, r3, lsl #2
   38af4: eef87ae7     	vcvt.f32.s32	s15, s15
   38af8: ed920a01     	vldr	s0, [r2, #4]
   38afc: edd26a02     	vldr	s13, [r2, #8]
   38b00: ee777a67     	vsub.f32	s15, s14, s15
   38b04: ed924a00     	vldr	s8, [r2]
   38b08: ed926a03     	vldr	s12, [r2, #12]
   38b0c: ee367ac0     	vsub.f32	s14, s13, s0
   38b10: ee355a67     	vsub.f32	s10, s10, s15
   38b14: eef06a46     	vmov.f32	s13, s12
   38b18: ee366a44     	vsub.f32	s12, s12, s8
   38b1c: eee46a23     	vfma.f32	s13, s8, s7
   38b20: eea76a64     	vfms.f32	s12, s14, s9
   38b24: ee655ac5     	vnmul.f32	s11, s11, s10
   38b28: eee06a64     	vfms.f32	s13, s0, s9
   38b2c: eee76a86     	vfma.f32	s13, s15, s12
   38b30: eea57aa6     	vfma.f32	s14, s11, s13
   38b34: eea70a87     	vfma.f32	s0, s15, s14
   38b38: e12fff1e     	bx	lr
   38b3c: ad aa 2a 3e  	.word	0x3e2aaaad
