; lubadh::OnePole::processAP(std::vector<float, std::allocator<float> >&)
; VA 0x4b160 size 96

   4b160: e5913000     	ldr	r3, [r1]
   4b164: e5911004     	ldr	r1, [r1, #0x4]
   4b168: e1530001     	cmp	r3, r1
   4b16c: 012fff1e     	bxeq	lr
   4b170: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4b174: edd07a00     	vldr	s15, [r0]
   4b178: ed937a00     	vldr	s14, [r3]
   4b17c: ed905a01     	vldr	s10, [r0, #4]
   4b180: ee766a67     	vsub.f32	s13, s12, s15
   4b184: edd05a02     	vldr	s11, [r0, #8]
   4b188: ee777a86     	vadd.f32	s15, s15, s12
   4b18c: ee377a05     	vadd.f32	s14, s14, s10
   4b190: eea57ae6     	vfms.f32	s14, s11, s13
   4b194: eec76a27     	vdiv.f32	s13, s14, s15
   4b198: edc06a02     	vstr	s13, [r0, #8]
   4b19c: e5932000     	ldr	r2, [r3]
   4b1a0: e5802004     	str	r2, [r0, #0x4]
   4b1a4: edd37a00     	vldr	s15, [r3]
   4b1a8: ee777ae6     	vsub.f32	s15, s15, s13
   4b1ac: ee777aa6     	vadd.f32	s15, s15, s13
   4b1b0: ece37a01     	vstmia	r3!, {s15}
   4b1b4: e1510003     	cmp	r1, r3
   4b1b8: 1affffed     	bne	0x4b174
   4b1bc: e12fff1e     	bx	lr
