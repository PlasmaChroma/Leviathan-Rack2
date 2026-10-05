; lubadh::OnePole::processHP(std::vector<float, std::allocator<float> >&)
; VA 0x4b104 size 92

   4b104: e5913000     	ldr	r3, [r1]
   4b108: e5911004     	ldr	r1, [r1, #0x4]
   4b10c: e1530001     	cmp	r3, r1
   4b110: 012fff1e     	bxeq	lr
   4b114: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4b118: ed907a00     	vldr	s14, [r0]
   4b11c: edd37a00     	vldr	s15, [r3]
   4b120: ed905a01     	vldr	s10, [r0, #4]
   4b124: ee766a47     	vsub.f32	s13, s12, s14
   4b128: edd05a02     	vldr	s11, [r0, #8]
   4b12c: ee377a06     	vadd.f32	s14, s14, s12
   4b130: ee777a85     	vadd.f32	s15, s15, s10
   4b134: eee57ae6     	vfms.f32	s15, s11, s13
   4b138: eec76a87     	vdiv.f32	s13, s15, s14
   4b13c: edc06a02     	vstr	s13, [r0, #8]
   4b140: e5932000     	ldr	r2, [r3]
   4b144: e5802004     	str	r2, [r0, #0x4]
   4b148: edd37a00     	vldr	s15, [r3]
   4b14c: ee777ae6     	vsub.f32	s15, s15, s13
   4b150: ece37a01     	vstmia	r3!, {s15}
   4b154: e1510003     	cmp	r1, r3
   4b158: 1affffee     	bne	0x4b118
   4b15c: e12fff1e     	bx	lr
