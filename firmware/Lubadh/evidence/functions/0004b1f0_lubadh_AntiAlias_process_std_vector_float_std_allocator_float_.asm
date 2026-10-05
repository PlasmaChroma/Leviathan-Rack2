; lubadh::AntiAlias::process(std::vector<float, std::allocator<float> >&)
; VA 0x4b1f0 size 112

   4b1f0: e280300c     	add	r3, r0, #12
   4b1f4: e280c024     	add	r12, r0, #36
   4b1f8: e52de004     	str	lr, [sp, #-0x4]!
   4b1fc: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4b200: e591e000     	ldr	lr, [r1]
   4b204: e5910004     	ldr	r0, [r1, #0x4]
   4b208: e15e0000     	cmp	lr, r0
   4b20c: 0a00000f     	beq	0x4b250
   4b210: e1a0200e     	mov	r2, lr
   4b214: ed137a03     	vldr	s14, [r3, #-12]
   4b218: edd27a00     	vldr	s15, [r2]
   4b21c: ed135a02     	vldr	s10, [r3, #-8]
   4b220: ee766a47     	vsub.f32	s13, s12, s14
   4b224: ed535a01     	vldr	s11, [r3, #-4]
   4b228: ee377a06     	vadd.f32	s14, s14, s12
   4b22c: ee777a85     	vadd.f32	s15, s15, s10
   4b230: eee57ae6     	vfms.f32	s15, s11, s13
   4b234: eec76a87     	vdiv.f32	s13, s15, s14
   4b238: ed436a01     	vstr	s13, [r3, #-4]
   4b23c: e5921000     	ldr	r1, [r2]
   4b240: e5031008     	str	r1, [r3, #-0x8]
   4b244: ece26a01     	vstmia	r2!, {s13}
   4b248: e1500002     	cmp	r0, r2
   4b24c: 1afffff0     	bne	0x4b214
   4b250: e283300c     	add	r3, r3, #12
   4b254: e15c0003     	cmp	r12, r3
   4b258: 1affffea     	bne	0x4b208
   4b25c: e49df004     	ldr	pc, [sp], #4
