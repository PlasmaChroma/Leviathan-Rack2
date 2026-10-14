00003e20 <_memReadIntpRev>:
    3e20: eefd5ac0     	vcvt.s32.f32	s11, s0
    3e24: e2811032     	add	r1, r1, #50
    3e28: eddf3a19     	vldr	s7, [pc, #100]          @ 0x3e94 <_memReadIntpRev+0x74>  // f32=0.166666701
    3e2c: e7903101     	ldr	r3, [r0, r1, lsl #2]
    3e30: ee152a90     	vmov	r2, s11
    3e34: eeb04a08     	vmov.f32	s8, #3.000000e+00
    3e38: eeb77a00     	vmov.f32	s14, #1.000000e+00
    3e3c: e242010b     	sub	r0, r2, #-1073741822
    3e40: eef80ae5     	vcvt.f32.s32	s1, s11
    3e44: e083c100     	add	r12, r3, r0, lsl #2
    3e48: ed9c5a01     	vldr	s10, [r12, #4]
    3e4c: eddc6a00     	vldr	s13, [r12]
    3e50: eddc7a03     	vldr	s15, [r12, #12]
    3e54: ed9c6a02     	vldr	s12, [r12, #8]
    3e58: ee764ae7     	vsub.f32	s9, s13, s15
    3e5c: ee361a45     	vsub.f32	s2, s12, s10
    3e60: ee557a04     	vnmls.f32	s15, s10, s8
    3e64: ee414a04     	vmla.f32	s9, s2, s8
    3e68: ee761aa6     	vadd.f32	s3, s13, s13
    3e6c: ee372a40     	vsub.f32	s4, s14, s0
    3e70: ee772ae1     	vsub.f32	s5, s15, s3
    3e74: ee300a60     	vsub.f32	s0, s0, s1
    3e78: ee323a20     	vadd.f32	s6, s4, s1
    3e7c: ee442a80     	vmla.f32	s5, s9, s0
    3e80: ee234a23     	vmul.f32	s8, s6, s7
    3e84: ee021a84     	vmla.f32	s2, s5, s8
    3e88: ee015a00     	vmla.f32	s10, s2, s0
    3e8c: eeb00a45     	vmov.f32	s0, s10
    3e90: e12fff1e     	bx	lr
    3e94: ad aa 2a 3e  	.word	0x3e2aaaad

