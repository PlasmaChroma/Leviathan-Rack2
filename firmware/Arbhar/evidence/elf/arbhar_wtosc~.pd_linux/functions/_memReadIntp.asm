00003da8 <_memReadIntp>:
    3da8: eefd5ac0     	vcvt.s32.f32	s11, s0
    3dac: e2811032     	add	r1, r1, #50
    3db0: eddf3a19     	vldr	s7, [pc, #100]          @ 0x3e1c <_memReadIntp+0x74>
    3db4: e7903101     	ldr	r3, [r0, r1, lsl #2]
    3db8: ee152a90     	vmov	r2, s11
    3dbc: eeb04a08     	vmov.f32	s8, #3.000000e+00
    3dc0: eeb77a00     	vmov.f32	s14, #1.000000e+00
    3dc4: e2420107     	sub	r0, r2, #-1073741823
    3dc8: eef80ae5     	vcvt.f32.s32	s1, s11
    3dcc: e083c100     	add	r12, r3, r0, lsl #2
    3dd0: ed9c5a01     	vldr	s10, [r12, #4]
    3dd4: eddc6a00     	vldr	s13, [r12]
    3dd8: eddc7a03     	vldr	s15, [r12, #12]
    3ddc: ed9c6a02     	vldr	s12, [r12, #8]
    3de0: ee764ae7     	vsub.f32	s9, s13, s15
    3de4: ee361a45     	vsub.f32	s2, s12, s10
    3de8: ee557a04     	vnmls.f32	s15, s10, s8
    3dec: ee414a04     	vmla.f32	s9, s2, s8
    3df0: ee761aa6     	vadd.f32	s3, s13, s13
    3df4: ee372a40     	vsub.f32	s4, s14, s0
    3df8: ee772ae1     	vsub.f32	s5, s15, s3
    3dfc: ee300a60     	vsub.f32	s0, s0, s1
    3e00: ee323a20     	vadd.f32	s6, s4, s1
    3e04: ee442a80     	vmla.f32	s5, s9, s0
    3e08: ee234a23     	vmul.f32	s8, s6, s7
    3e0c: ee021a84     	vmla.f32	s2, s5, s8
    3e10: ee015a00     	vmla.f32	s10, s2, s0
    3e14: eeb00a45     	vmov.f32	s0, s10
    3e18: e12fff1e     	bx	lr
    3e1c: ad aa 2a 3e  	.word	0x3e2aaaad

