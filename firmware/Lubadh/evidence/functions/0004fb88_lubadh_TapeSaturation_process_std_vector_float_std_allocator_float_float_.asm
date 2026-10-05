; lubadh::TapeSaturation::process(std::vector<float, std::allocator<float> >&, float)
; VA 0x4fb88 size 216

   4fb88: eddf6a30     	vldr	s13, [pc, #192]         @ 0x4fc50>&, float)+0xc8> ; float 1.39999997616
   4fb8c: eef77a00     	vmov.f32	s15, #1.000000e+00
   4fb90: ed907a00     	vldr	s14, [r0]
   4fb94: ee864a87     	vdiv.f32	s8, s13, s14
   4fb98: eeb44ae7     	vcmpe.f32	s8, s15
   4fb9c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4fba0: 5eb04a67     	vmovpl.f32	s8, s15
   4fba4: 5a000003     	bpl	0x4fbb8
   4fba8: eeb54ac0     	vcmpe.f32	s8, #0
   4fbac: eddf7a28     	vldr	s15, [pc, #160]         @ 0x4fc54>&, float)+0xcc> ; float 0
   4fbb0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4fbb4: deb04a67     	vmovle.f32	s8, s15
   4fbb8: e5913000     	ldr	r3, [r1]
   4fbbc: e5912004     	ldr	r2, [r1, #0x4]
   4fbc0: e0422003     	sub	r2, r2, r3
   4fbc4: e1b01122     	lsrs	r1, r2, #2
   4fbc8: 012fff1e     	bxeq	lr
   4fbcc: eeb12a44     	vneg.f32	s4, s8
   4fbd0: e0832002     	add	r2, r3, r2
   4fbd4: ed9f5a1f     	vldr	s10, [pc, #124]         @ 0x4fc58>&, float)+0xd0> ; float 28.2743339539
   4fbd8: eef73a00     	vmov.f32	s7, #1.000000e+00
   4fbdc: ed9f3a1e     	vldr	s6, [pc, #120]          @ 0x4fc5c>&, float)+0xd4> ; float 9.42477798462
   4fbe0: eeff2a00     	vmov.f32	s5, #-1.000000e+00
   4fbe4: ecf37a01     	vldmia	r3!, {s15}
   4fbe8: eef01a45     	vmov.f32	s3, s10
   4fbec: eef06a44     	vmov.f32	s13, s8
   4fbf0: edd04a01     	vldr	s9, [r0, #4]
   4fbf4: ee277a87     	vmul.f32	s14, s15, s14
   4fbf8: ee675a07     	vmul.f32	s11, s14, s14
   4fbfc: ee356a85     	vadd.f32	s12, s11, s10
   4fc00: eee51a83     	vfma.f32	s3, s11, s6
   4fc04: ee267a07     	vmul.f32	s14, s12, s14
   4fc08: ee876a21     	vdiv.f32	s12, s14, s3
   4fc0c: eeb46ae3     	vcmpe.f32	s12, s7
   4fc10: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4fc14: 5a000003     	bpl	0x4fc28
   4fc18: eeb46ae2     	vcmpe.f32	s12, s5
   4fc1c: eef06a42     	vmov.f32	s13, s4
   4fc20: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4fc24: ce646a06     	vmulgt.f32	s13, s8, s12
   4fc28: ee766ae7     	vsub.f32	s13, s13, s15
   4fc2c: eeb07a67     	vmov.f32	s14, s15
   4fc30: e1530002     	cmp	r3, r2
   4fc34: eea47aa6     	vfma.f32	s14, s9, s13
   4fc38: ee377a67     	vsub.f32	s14, s14, s15
   4fc3c: eee07a07     	vfma.f32	s15, s0, s14
   4fc40: ed437a01     	vstr	s15, [r3, #-4]
   4fc44: 012fff1e     	bxeq	lr
   4fc48: ed907a00     	vldr	s14, [r0]
   4fc4c: eaffffe4     	b	0x4fbe4
   4fc50: 33 33 b3 3f  	.word	0x3fb33333
   4fc54: 00 00 00 00  	.word	0x00000000
   4fc58: d6 31 e2 41  	.word	0x41e231d6
   4fc5c: e4 cb 16 41  	.word	0x4116cbe4
