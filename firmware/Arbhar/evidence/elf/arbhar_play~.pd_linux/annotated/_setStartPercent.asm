000038b0 <_setStartPercent>:
    38b0: eeb50ac0     	vcmpe.f32	s0, #0
    38b4: eddf7a1f     	vldr	s15, [pc, #124]         @ 0x3938 <_setStartPercent+0x88>  // f32=100
    38b8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    38bc: a3a03001     	movge	r3, #1
    38c0: eeb40ae7     	vcmpe.f32	s0, s15
    38c4: b3a03000     	movlt	r3, #0
    38c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    38cc: 92033001     	andls	r3, r3, #1
    38d0: 83a03000     	movhi	r3, #0
    38d4: e3530000     	cmp	r3, #0
    38d8: 0a00000c     	beq	0x3910 <_setStartPercent+0x60> @ imm = #0x30
    38dc: eeb70ac0     	vcvt.f64.f32	d0, s0
    38e0: e2803a02     	add	r3, r0, #8192
    38e4: eddf0b11     	vldr	d16, [pc, #68]          @ 0x3930 <_setStartPercent+0x80>  // f64=0.01
    38e8: e5931630     	ldr	r1, [r3, #0x630]
    38ec: e0802101     	add	r2, r0, r1, lsl #2
    38f0: ee201b20     	vmul.f64	d1, d0, d16
    38f4: edd20a0f     	vldr	s1, [r2, #60]
    38f8: eeb82ae0     	vcvt.f32.s32	s4, s1
    38fc: eeb73ac2     	vcvt.f64.f32	d3, s4
    3900: ee230b01     	vmul.f64	d0, d3, d1
    3904: eefd1bc0     	vcvt.s32.f64	s3, d0
    3908: edc01a2d     	vstr	s3, [r0, #180]
    390c: e12fff1e     	bx	lr
    3910: eeb50ac0     	vcmpe.f32	s0, #0
    3914: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3918: a2803a02     	addge	r3, r0, #8192
    391c: a5933630     	ldrge	r3, [r3, #0x630]
    3920: a0803103     	addge	r3, r0, r3, lsl #2
    3924: a593303c     	ldrge	r3, [r3, #0x3c]
    3928: e58030b4     	str	r3, [r0, #0xb4]
    392c: e12fff1e     	bx	lr
    3930: 7b 14 ae 47  	.word	0x47ae147b
    3934: e1 7a 84 3f  	.word	0x3f847ae1
    3938: 00 00 c8 42  	.word	0x42c80000

