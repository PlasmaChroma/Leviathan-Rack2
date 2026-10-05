000029b8 <arbhar_rec_tilde_layer>:
    29b8: eef67a00     	vmov.f32	s15, #5.000000e-01
    29bc: ee300a27     	vadd.f32	s0, s0, s15
    29c0: eefd0ac0     	vcvt.s32.f32	s1, s0
    29c4: ee103a90     	vmov	r3, s1
    29c8: e3530005     	cmp	r3, #5
    29cc: 9580327c     	strls	r3, [r0, #0x27c]
    29d0: e12fff1e     	bx	lr

