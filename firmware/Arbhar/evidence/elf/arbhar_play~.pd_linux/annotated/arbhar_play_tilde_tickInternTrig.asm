00007928 <arbhar_play_tilde_tickInternTrig>:
    7928: e2803a02     	add	r3, r0, #8192
    792c: ed9f7a43     	vldr	s14, [pc, #268]         @ 0x7a40 <arbhar_play_tilde_tickInternTrig+0x118>  // f32=32
    7930: e5931894     	ldr	r1, [r3, #0x894]
    7934: ed9f6a42     	vldr	s12, [pc, #264]         @ 0x7a44 <arbhar_play_tilde_tickInternTrig+0x11c>  // f32=0
    7938: edd16ad7     	vldr	s13, [r1, #860]
    793c: edd17a0c     	vldr	s15, [r1, #48]
    7940: ee567a87     	vnmls.f32	s15, s13, s14
    7944: eef47ac6     	vcmpe.f32	s15, s12
    7948: eef1fa10     	vmrs	APSR_nzcv, fpscr
    794c: 5a000034     	bpl	0x7a24 <arbhar_play_tilde_tickInternTrig+0xfc> @ imm = #0xd0
    7950: ed9f0a3c     	vldr	s0, [pc, #240]          @ 0x7a48 <arbhar_play_tilde_tickInternTrig+0x120>  // f32=4095
    7954: eddf0a3c     	vldr	s1, [pc, #240]          @ 0x7a4c <arbhar_play_tilde_tickInternTrig+0x124>  // f32=0.000244200259
    7958: ee371a80     	vadd.f32	s2, s15, s0
    795c: eddf1b33     	vldr	d17, [pc, #204]         @ 0x7a30 <arbhar_play_tilde_tickInternTrig+0x108>  // f64=0.050000000000000003
    7960: eeb41ac6     	vcmpe.f32	s2, s12
    7964: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7968: beb01a46     	vmovlt.f32	s2, s12
    796c: eeb41ac0     	vcmpe.f32	s2, s0
    7970: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7974: 8eb01a40     	vmovhi.f32	s2, s0
    7978: ee611a20     	vmul.f32	s3, s2, s1
    797c: eeb62a00     	vmov.f32	s4, #5.000000e-01
    7980: eef41ac2     	vcmpe.f32	s3, s4
    7984: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7988: 8eb72a00     	vmovhi.f32	s4, #1.000000e+00
    798c: 8d9f6a2f     	vldrhi	s12, [pc, #188]         @ 0x7a50 <arbhar_play_tilde_tickInternTrig+0x128>
    7990: 83a01001     	movhi	r1, #1
    7994: 93a01000     	movls	r1, #0
    7998: 9d9f6a2c     	vldrls	s12, [pc, #176]         @ 0x7a50 <arbhar_play_tilde_tickInternTrig+0x128>
    799c: 8e322a61     	vsubhi.f32	s4, s4, s3
    79a0: 9e312aa1     	vaddls.f32	s4, s3, s3
    79a4: 8e720a02     	vaddhi.f32	s1, s4, s4
    79a8: 9ef00a42     	vmovls.f32	s1, s4
    79ac: ee602aa0     	vmul.f32	s5, s1, s1
    79b0: ee223a86     	vmul.f32	s6, s5, s12
    79b4: ee633a20     	vmul.f32	s7, s6, s1
    79b8: eef70ae3     	vcvt.f64.f32	d16, s7
    79bc: eef40be1     	vcmpe.f64	d16, d17
    79c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    79c4: ba000017     	blt	0x7a28 <arbhar_play_tilde_tickInternTrig+0x100> @ imm = #0x5c
    79c8: eef62b00     	vmov.f64	d18, #5.000000e-01
    79cc: ed9f4b19     	vldr	d4, [pc, #100]          @ 0x7a38 <arbhar_play_tilde_tickInternTrig+0x110>  // f64=0.40000000000000002
    79d0: ee305ba2     	vadd.f64	d5, d16, d18
    79d4: eef75bc5     	vcvt.f32.f64	s11, d5
    79d8: eef73ae5     	vcvt.f64.f32	d19, s11
    79dc: ee734b84     	vadd.f64	d20, d19, d4
    79e0: eebd7be4     	vcvt.s32.f64	s14, d20
    79e4: ee17ca10     	vmov	r12, s14
    79e8: eef64a00     	vmov.f32	s9, #5.000000e-01
    79ec: eef41ae4     	vcmpe.f32	s3, s9
    79f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    79f4: c3a02001     	movgt	r2, #1
    79f8: d3a02000     	movle	r2, #0
    79fc: e58020ac     	str	r2, [r0, #0xac]
    7a00: e35c0000     	cmp	r12, #0
    7a04: d12fff1e     	bxle	lr
    7a08: e92d4010     	push	{r4, lr}
    7a0c: e1a04000     	mov	r4, r0
    7a10: ebfffc87     	bl	0x6c34 <play_next>      @ imm = #-0xde4
    7a14: e5940100     	ldr	r0, [r4, #0x100]
    7a18: eeb70a00     	vmov.f32	s0, #1.000000e+00
    7a1c: e8bd4010     	pop	{r4, lr}
    7a20: eaffeb21     	b	0x26ac <.plt+0x2cc>     @ imm = #-0x537c  // CALL outlet_float
    7a24: e3a01001     	mov	r1, #1
    7a28: e58010ac     	str	r1, [r0, #0xac]
    7a2c: e12fff1e     	bx	lr
    7a30: 9a 99 99 99  	.word	0x9999999a
    7a34: 99 99 a9 3f  	.word	0x3fa99999
    7a38: 9a 99 99 99  	.word	0x9999999a
    7a3c: 99 99 d9 3f  	.word	0x3fd99999
    7a40: 00 00 00 42  	.word	0x42000000
    7a44: 00 00 00 00  	.word	0x00000000
    7a48: 00 f0 7f 45  	.word	0x457ff000
    7a4c: 01 08 80 39  	.word	0x39800801
    7a50: 00 00 28 42  	.word	0x42280000

