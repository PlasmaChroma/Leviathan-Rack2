0000289c <arbhar_wtosc_tilde_setFilterCutoff>:
    289c: eeb77a00     	vmov.f32	s14, #1.000000e+00
    28a0: e1a01000     	mov	r1, r0
    28a4: eddf2b1d     	vldr	d18, [pc, #116]         @ 0x2920 <arbhar_wtosc_tilde_setFilterCutoff+0x84>  // f64=6.2831853071795862
    28a8: e52de004     	str	lr, [sp, #-0x4]!
    28ac: e24dd00c     	sub	sp, sp, #12
    28b0: edd17a07     	vldr	s15, [r1, #28]
    28b4: e59f0074     	ldr	r0, [pc, #0x74]         @ 0x2930 <arbhar_wtosc_tilde_setFilterCutoff+0x94>  // u32=0x5c0c; f32?=3.3020197e-41
    28b8: eddf5b1a     	vldr	d21, [pc, #104]         @ 0x2928 <arbhar_wtosc_tilde_setFilterCutoff+0x8c>  // f64=0
    28bc: e08f0000     	add	r0, pc, r0
    28c0: eeb40ac7     	vcmpe.f32	s0, s14
    28c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    28c8: beb00a47     	vmovlt.f32	s0, s14
    28cc: eef74ac0     	vcvt.f64.f32	d20, s0
    28d0: ed810a19     	vstr	s0, [r1, #100]
    28d4: ee240ba2     	vmul.f64	d0, d20, d18
    28d8: ec532b34     	vmov	r2, r3, d20
    28dc: eef71ae7     	vcvt.f64.f32	d17, s15
    28e0: eec00b21     	vdiv.f64	d16, d0, d17
    28e4: eef73b00     	vmov.f64	d19, #1.000000e+00
    28e8: eef40be5     	vcmpe.f64	d16, d21
    28ec: eef1fa10     	vmrs	APSR_nzcv, fpscr
    28f0: bef00b65     	vmovlt.f64	d16, d21
    28f4: eef40be3     	vcmpe.f64	d16, d19
    28f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    28fc: 8ef00b63     	vmovhi.f64	d16, d19
    2900: eef70be0     	vcvt.f32.f64	s1, d16
    2904: eeb71ae0     	vcvt.f64.f32	d1, s1
    2908: edc10a1b     	vstr	s1, [r1, #108]
    290c: ed8d1b00     	vstr	d1, [sp]
    2910: ebfffefc     	bl	0x2508 <.plt+0x224>     @ imm = #-0x410  // CALL post
    2914: e28dd00c     	add	sp, sp, #12
    2918: e49df004     	ldr	pc, [sp], #4
    291c: e320f000     	nop
    2920: 18 2d 44 54  	.word	0x54442d18
    2924: fb 21 19 40  	.word	0x401921fb
    2928: 00 00 00 00  	.word	0x00000000
    292c: 00 00 00 00  	.word	0x00000000
    2930: 0c 5c 00 00  	.word	0x00005c0c

