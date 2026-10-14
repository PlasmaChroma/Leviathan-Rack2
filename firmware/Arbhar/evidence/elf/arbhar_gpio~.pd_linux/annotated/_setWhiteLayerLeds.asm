000040fc <_setWhiteLayerLeds>:
    40fc: eefc7ae0     	vcvt.u32.f32	s15, s1
    4100: e2800a01     	add	r0, r0, #4096
    4104: eebc0ac0     	vcvt.u32.f32	s0, s0
    4108: ee172a90     	vmov	r2, s15
    410c: ee103a10     	vmov	r3, s0
    4110: e5c02dd9     	strb	r2, [r0, #0xdd9]
    4114: e5c03dd8     	strb	r3, [r0, #0xdd8]
    4118: e12fff1e     	bx	lr

