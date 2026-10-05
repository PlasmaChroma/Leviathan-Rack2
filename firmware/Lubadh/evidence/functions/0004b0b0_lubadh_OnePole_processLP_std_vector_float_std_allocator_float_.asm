; lubadh::OnePole::processLP(std::vector<float, std::allocator<float> >&)
; VA 0x4b0b0 size 84

   4b0b0: e5913000     	ldr	r3, [r1]
   4b0b4: e5911004     	ldr	r1, [r1, #0x4]
   4b0b8: e1530001     	cmp	r3, r1
   4b0bc: 012fff1e     	bxeq	lr
   4b0c0: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4b0c4: ed907a00     	vldr	s14, [r0]
   4b0c8: edd37a00     	vldr	s15, [r3]
   4b0cc: ed905a01     	vldr	s10, [r0, #4]
   4b0d0: ee766a47     	vsub.f32	s13, s12, s14
   4b0d4: edd05a02     	vldr	s11, [r0, #8]
   4b0d8: ee377a06     	vadd.f32	s14, s14, s12
   4b0dc: ee777a85     	vadd.f32	s15, s15, s10
   4b0e0: eee57ae6     	vfms.f32	s15, s11, s13
   4b0e4: eec76a87     	vdiv.f32	s13, s15, s14
   4b0e8: edc06a02     	vstr	s13, [r0, #8]
   4b0ec: e5932000     	ldr	r2, [r3]
   4b0f0: e5802004     	str	r2, [r0, #0x4]
   4b0f4: ece36a01     	vstmia	r3!, {s13}
   4b0f8: e1510003     	cmp	r1, r3
   4b0fc: 1afffff0     	bne	0x4b0c4
   4b100: e12fff1e     	bx	lr
