000041a8 <arbhar_play_tilde_test>:
    41a8: eefd5ac0     	vcvt.s32.f32	s11, s0
    41ac: e2803a02     	add	r3, r0, #8192
    41b0: e52de004     	str	lr, [sp, #-0x4]!
    41b4: e24dd00c     	sub	sp, sp, #12
    41b8: e5931898     	ldr	r1, [r3, #0x898]
    41bc: eddf3a1f     	vldr	s7, [pc, #124]          @ 0x4240 <arbhar_play_tilde_test+0x98>
    41c0: e59f007c     	ldr	r0, [pc, #0x7c]         @ 0x4244 <arbhar_play_tilde_test+0x9c>
    41c4: ee152a90     	vmov	r2, s11
    41c8: e08f0000     	add	r0, pc, r0
    41cc: eeb04a08     	vmov.f32	s8, #3.000000e+00
    41d0: eeb77a00     	vmov.f32	s14, #1.000000e+00
    41d4: e242c107     	sub	r12, r2, #-1073741823
    41d8: eef80ae5     	vcvt.f32.s32	s1, s11
    41dc: e081e10c     	add	lr, r1, r12, lsl #2
    41e0: ed9e6a01     	vldr	s12, [lr, #4]
    41e4: ed9e5a00     	vldr	s10, [lr]
    41e8: edde7a03     	vldr	s15, [lr, #12]
    41ec: edde6a02     	vldr	s13, [lr, #8]
    41f0: ee754a67     	vsub.f32	s9, s10, s15
    41f4: ee361ac6     	vsub.f32	s2, s13, s12
    41f8: ee567a04     	vnmls.f32	s15, s12, s8
    41fc: ee414a04     	vmla.f32	s9, s2, s8
    4200: ee751a05     	vadd.f32	s3, s10, s10
    4204: ee372a40     	vsub.f32	s4, s14, s0
    4208: ee303a60     	vsub.f32	s6, s0, s1
    420c: ee772ae1     	vsub.f32	s5, s15, s3
    4210: ee324a20     	vadd.f32	s8, s4, s1
    4214: ee442a83     	vmla.f32	s5, s9, s6
    4218: ee643a23     	vmul.f32	s7, s8, s7
    421c: ee021aa3     	vmla.f32	s2, s5, s7
    4220: eeb70ac0     	vcvt.f64.f32	d0, s0
    4224: ee016a03     	vmla.f32	s12, s2, s6
    4228: ed8d0b00     	vstr	d0, [sp]
    422c: eef70ac6     	vcvt.f64.f32	d16, s12
    4230: ec532b30     	vmov	r2, r3, d16
    4234: ebfff8f8     	bl	0x261c <.plt+0x23c>     @ imm = #-0x1c20
    4238: e28dd00c     	add	sp, sp, #12
    423c: e49df004     	ldr	pc, [sp], #4
    4240: ad aa 2a 3e  	.word	0x3e2aaaad
    4244: 08 8a 00 00  	.word	0x00008a08

