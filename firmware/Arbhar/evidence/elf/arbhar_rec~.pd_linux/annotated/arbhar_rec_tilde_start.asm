000029e8 <arbhar_rec_tilde_start>:
    29e8: eebd0ac0     	vcvt.s32.f32	s0, s0
    29ec: e3a02001     	mov	r2, #1
    29f0: e5802284     	str	r2, [r0, #0x284]
    29f4: ee103a10     	vmov	r3, s0
    29f8: e1c31fc3     	bic	r1, r3, r3, asr #31
    29fc: e5801080     	str	r1, [r0, #0x80]
    2a00: e12fff1e     	bx	lr

