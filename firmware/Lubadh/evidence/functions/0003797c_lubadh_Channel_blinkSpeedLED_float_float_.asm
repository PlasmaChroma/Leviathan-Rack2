; lubadh::Channel::blinkSpeedLED(float, float)
; VA 0x3797c size 56

   3797c: ed9f7a09     	vldr	s14, [pc, #36]          @ 0x379a8 ; float 1000
   37980: eddf7a09     	vldr	s15, [pc, #36]          @ 0x379ac ; float 2.70000004768
   37984: eddf6a09     	vldr	s13, [pc, #36]          @ 0x379b0 ; float 5.40000009537
   37988: ee876a20     	vdiv.f32	s12, s14, s1
   3798c: ee807a27     	vdiv.f32	s14, s0, s15
   37990: eec67a26     	vdiv.f32	s15, s12, s13
   37994: eebd7ac7     	vcvt.s32.f32	s14, s14
   37998: ed807aa4     	vstr	s14, [r0, #656]
   3799c: eefd7ae7     	vcvt.s32.f32	s15, s15
   379a0: edc07aa5     	vstr	s15, [r0, #660]
   379a4: e12fff1e     	bx	lr
   379a8: 00 00 7a 44  	.word	0x447a0000
   379ac: cd cc 2c 40  	.word	0x402ccccd
   379b0: cd cc ac 40  	.word	0x40accccd
