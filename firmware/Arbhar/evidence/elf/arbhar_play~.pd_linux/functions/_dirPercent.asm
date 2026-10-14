00002db8 <_dirPercent>:
    2db8: eeb50ac0     	vcmpe.f32	s0, #0
    2dbc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2dc0: b3a03000     	movlt	r3, #0
    2dc4: ba000005     	blt	0x2de0 <_dirPercent+0x28> @ imm = #0x14
    2dc8: eddf7a07     	vldr	s15, [pc, #28]          @ 0x2dec <_dirPercent+0x34>
    2dcc: eeb40ae7     	vcmpe.f32	s0, s15
    2dd0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2dd4: 9efd7ac0     	vcvtls.s32.f32	s15, s0
    2dd8: 83a03064     	movhi	r3, #100
    2ddc: 9e173a90     	vmovls	r3, s15
    2de0: e2800a02     	add	r0, r0, #8192
    2de4: e58036b4     	str	r3, [r0, #0x6b4]
    2de8: e12fff1e     	bx	lr
    2dec: 00 00 c8 42  	.word	0x42c80000

