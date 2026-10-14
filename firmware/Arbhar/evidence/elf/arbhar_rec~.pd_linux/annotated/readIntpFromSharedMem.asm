00004968 <readIntpFromSharedMem>:
    4968: e0801101     	add	r1, r0, r1, lsl #2
    496c: e59131e8     	ldr	r3, [r1, #0x1e8]
    4970: e3530000     	cmp	r3, #0
    4974: 0a000020     	beq	0x49fc <readIntpFromSharedMem+0x94> @ imm = #0x80
    4978: eebd7ac0     	vcvt.s32.f32	s14, s0
    497c: e591c21c     	ldr	r12, [r1, #0x21c]
    4980: eddf3a1f     	vldr	s7, [pc, #124]          @ 0x4a04 <readIntpFromSharedMem+0x9c>  // f32=0.166666701
    4984: e24c0002     	sub	r0, r12, #2
    4988: ee172a10     	vmov	r2, s14
    498c: eef04a08     	vmov.f32	s9, #3.000000e+00
    4990: eef76a00     	vmov.f32	s13, #1.000000e+00
    4994: e3520001     	cmp	r2, #1
    4998: eef81ac7     	vcvt.f32.s32	s3, s14
    499c: b3a02001     	movlt	r2, #1
    49a0: e1500002     	cmp	r0, r2
    49a4: d24c2003     	suble	r2, r12, #3
    49a8: e2421107     	sub	r1, r2, #-1073741823
    49ac: e0833101     	add	r3, r3, r1, lsl #2
    49b0: ed935a01     	vldr	s10, [r3, #4]
    49b4: edd35a00     	vldr	s11, [r3]
    49b8: edd37a03     	vldr	s15, [r3, #12]
    49bc: ed936a02     	vldr	s12, [r3, #8]
    49c0: ee354ae7     	vsub.f32	s8, s11, s15
    49c4: ee760a45     	vsub.f32	s1, s12, s10
    49c8: ee557a24     	vnmls.f32	s15, s10, s9
    49cc: ee004aa4     	vmla.f32	s8, s1, s9
    49d0: ee351aa5     	vadd.f32	s2, s11, s11
    49d4: ee362ac0     	vsub.f32	s4, s13, s0
    49d8: ee772ac1     	vsub.f32	s5, s15, s2
    49dc: ee300a61     	vsub.f32	s0, s0, s3
    49e0: ee323a21     	vadd.f32	s6, s4, s3
    49e4: ee442a00     	vmla.f32	s5, s8, s0
    49e8: ee237a23     	vmul.f32	s14, s6, s7
    49ec: ee420a87     	vmla.f32	s1, s5, s14
    49f0: ee005a80     	vmla.f32	s10, s1, s0
    49f4: eeb00a45     	vmov.f32	s0, s10
    49f8: e12fff1e     	bx	lr
    49fc: ed9f0a01     	vldr	s0, [pc, #4]            @ 0x4a08 <readIntpFromSharedMem+0xa0>  // f32=0
    4a00: e12fff1e     	bx	lr
    4a04: ad aa 2a 3e  	.word	0x3e2aaaad
    4a08: 00 00 00 00  	.word	0x00000000

