0000ad64 <_getTimeFromStrikeCV>:
    ad64: e59f30a4     	ldr	r3, [pc, #0xa4]         @ 0xae10 <_getTimeFromStrikeCV+0xac>  // u32=0x1c63c; f32?=1.62948591e-40
    ad68: e2802a01     	add	r2, r0, #4096
    ad6c: ed9f6a24     	vldr	s12, [pc, #144]         @ 0xae04 <_getTimeFromStrikeCV+0xa0>  // f32=1.33333337
    ad70: e282cede     	add	r12, r2, #3552
    ad74: e08f3003     	add	r3, pc, r3
    ad78: e5921ddc     	ldr	r1, [r2, #0xddc]
    ad7c: e5933088     	ldr	r3, [r3, #0x88]
    ad80: eef76a00     	vmov.f32	s13, #1.000000e+00
    ad84: ed9f7a1f     	vldr	s14, [pc, #124]         @ 0xae08 <_getTimeFromStrikeCV+0xa4>  // f32=2500
    ad88: e0413003     	sub	r3, r1, r3
    ad8c: e5821fa8     	str	r1, [r2, #0xfa8]
    ad90: ee073a90     	vmov	s15, r3
    ad94: eeb80ae7     	vcvt.f32.s32	s0, s15
    ad98: ee600a06     	vmul.f32	s1, s0, s12
    ad9c: eef40ae6     	vcmpe.f32	s1, s13
    ada0: edcc0a01     	vstr	s1, [r12, #4]
    ada4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    ada8: eef40ac7     	vcmpe.f32	s1, s14
    adac: a3a03001     	movge	r3, #1
    adb0: b3a03000     	movlt	r3, #0
    adb4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    adb8: 42033001     	andmi	r3, r3, #1
    adbc: 53a03000     	movpl	r3, #0
    adc0: e3530000     	cmp	r3, #0
    adc4: 1a000003     	bne	0xadd8 <_getTimeFromStrikeCV+0x74> @ imm = #0xc
    adc8: e59f3044     	ldr	r3, [pc, #0x44]         @ 0xae14 <_getTimeFromStrikeCV+0xb0>  // u32=0x1c5e4; f32?=1.62825276e-40
    adcc: e08fc003     	add	r12, pc, r3
    add0: e58c1088     	str	r1, [r12, #0x88]
    add4: e12fff1e     	bx	lr
    add8: e592cfa4     	ldr	r12, [r2, #0xfa4]
    addc: e2800c1e     	add	r0, r0, #7680
    ade0: eddf1a09     	vldr	s3, [pc, #36]           @ 0xae0c <_getTimeFromStrikeCV+0xa8>  // f32=60000
    ade4: e2822ede     	add	r2, r2, #3552
    ade8: ee01ca10     	vmov	s2, r12
    adec: eeb82ac1     	vcvt.f32.s32	s4, s2
    adf0: ee622a20     	vmul.f32	s5, s4, s1
    adf4: ee813aa2     	vdiv.f32	s6, s3, s5
    adf8: ed823a02     	vstr	s6, [r2, #8]
    adfc: ed803a00     	vstr	s6, [r0]
    ae00: eafffff0     	b	0xadc8 <_getTimeFromStrikeCV+0x64> @ imm = #-0x40
    ae04: ab aa aa 3f  	.word	0x3faaaaab
    ae08: 00 40 1c 45  	.word	0x451c4000
    ae0c: 00 60 6a 47  	.word	0x476a6000
    ae10: 3c c6 01 00  	.word	0x0001c63c
    ae14: e4 c5 01 00  	.word	0x0001c5e4

