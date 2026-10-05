; lubadh::Channel::AudioData::interpolate(lubadh::Tap const&) const
; VA 0x38748 size 180

   38748: e5903000     	ldr	r3, [r0]
   3874c: e302ca2d     	movw	r12, #0x2a2d
   38750: e340c1c2     	movt	r12, #0x1c2
   38754: e5d12014     	ldrb	r2, [r1, #0x14]
   38758: e52de004     	str	lr, [sp, #-0x4]!
   3875c: eef06a00     	vmov.f32	s13, #2.000000e+00
   38760: e59330e8     	ldr	r3, [r3, #0xe8]
   38764: e3520000     	cmp	r2, #0
   38768: e591e004     	ldr	lr, [r1, #0x4]
   3876c: e5902048     	ldr	r2, [r0, #0x48]
   38770: eeb05a08     	vmov.f32	s10, #3.000000e+00
   38774: ed916a02     	vldr	s12, [r1, #8]
   38778: edd37a00     	vldr	s15, [r3]
   3877c: 1dd17a06     	vldrne	s15, [r1, #24]
   38780: eddf5a1c     	vldr	s11, [pc, #112]         @ 0x387f8 ; float 0.166666701436
   38784: eef57ac0     	vcmpe.f32	s15, #0
   38788: eef77a00     	vmov.f32	s15, #1.000000e+00
   3878c: ee777ac6     	vsub.f32	s15, s15, s12
   38790: eef1fa10     	vmrs	APSR_nzcv, fpscr
   38794: ee655ae7     	vnmul.f32	s11, s11, s15
   38798: a3a03002     	movge	r3, #2
   3879c: b3e03002     	mvnlt	r3, #2
   387a0: e083300e     	add	r3, r3, lr
   387a4: e153000c     	cmp	r3, r12
   387a8: a1a0300c     	movge	r3, r12
   387ac: e3530001     	cmp	r3, #1
   387b0: b3a03001     	movlt	r3, #1
   387b4: e2433001     	sub	r3, r3, #1
   387b8: e0823103     	add	r3, r2, r3, lsl #2
   387bc: ed937a03     	vldr	s14, [r3, #12]
   387c0: edd34a00     	vldr	s9, [r3]
   387c4: ed930a01     	vldr	s0, [r3, #4]
   387c8: edd37a02     	vldr	s15, [r3, #8]
   387cc: eeb04a47     	vmov.f32	s8, s14
   387d0: eea44aa6     	vfma.f32	s8, s9, s13
   387d4: ee377a64     	vsub.f32	s14, s14, s9
   387d8: ee777ac0     	vsub.f32	s15, s15, s0
   387dc: eea77ac5     	vfms.f32	s14, s15, s10
   387e0: eef06a44     	vmov.f32	s13, s8
   387e4: eee06a45     	vfms.f32	s13, s0, s10
   387e8: eee76a06     	vfma.f32	s13, s14, s12
   387ec: eee57aa6     	vfma.f32	s15, s11, s13
   387f0: eea70a86     	vfma.f32	s0, s15, s12
   387f4: e49df004     	ldr	pc, [sp], #4
   387f8: ad aa 2a 3e  	.word	0x3e2aaaad
