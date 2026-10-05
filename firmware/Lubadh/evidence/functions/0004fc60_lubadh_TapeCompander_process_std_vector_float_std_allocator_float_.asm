; lubadh::TapeCompander::process(std::vector<float, std::allocator<float> >&)
; VA 0x4fc60 size 68

   4fc60: e5913000     	ldr	r3, [r1]
   4fc64: e5912004     	ldr	r2, [r1, #0x4]
   4fc68: e1530002     	cmp	r3, r2
   4fc6c: 012fff1e     	bxeq	lr
   4fc70: ecf37a01     	vldmia	r3!, {s15}
   4fc74: eef57ac0     	vcmpe.f32	s15, #0
   4fc78: ee277aa7     	vmul.f32	s14, s15, s15
   4fc7c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4fc80: 4eb17a47     	vnegmi.f32	s14, s14
   4fc84: e1530002     	cmp	r3, r2
   4fc88: ee776a67     	vsub.f32	s13, s14, s15
   4fc8c: ed037a01     	vstr	s14, [r3, #-4]
   4fc90: ed907a00     	vldr	s14, [r0]
   4fc94: eee77a26     	vfma.f32	s15, s14, s13
   4fc98: ed437a01     	vstr	s15, [r3, #-4]
   4fc9c: 1afffff3     	bne	0x4fc70
   4fca0: e12fff1e     	bx	lr
