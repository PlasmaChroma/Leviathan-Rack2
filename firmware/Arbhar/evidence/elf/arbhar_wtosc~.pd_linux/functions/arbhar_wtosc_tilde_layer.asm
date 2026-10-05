0000275c <arbhar_wtosc_tilde_layer>:
    275c: eef67a00     	vmov.f32	s15, #5.000000e-01
    2760: ee300a27     	vadd.f32	s0, s0, s15
    2764: eefd0ac0     	vcvt.s32.f32	s1, s0
    2768: ee101a90     	vmov	r1, s1
    276c: e3510005     	cmp	r1, #5
    2770: 8a000001     	bhi	0x277c <arbhar_wtosc_tilde_layer+0x20> @ imm = #0x4
    2774: edc00a14     	vstr	s1, [r0, #80]
    2778: e12fff1e     	bx	lr
    277c: e59f0004     	ldr	r0, [pc, #0x4]          @ 0x2788 <arbhar_wtosc_tilde_layer+0x2c>
    2780: e08f0000     	add	r0, pc, r0
    2784: eaffff5f     	b	0x2508 <.plt+0x224>     @ imm = #-0x284
    2788: cc 5c 00 00  	.word	0x00005ccc

