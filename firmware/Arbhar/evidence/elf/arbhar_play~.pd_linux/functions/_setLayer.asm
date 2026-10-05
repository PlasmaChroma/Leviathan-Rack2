00002e78 <_setLayer>:
    2e78: eef67a00     	vmov.f32	s15, #5.000000e-01
    2e7c: ee300a27     	vadd.f32	s0, s0, s15
    2e80: eefd0ac0     	vcvt.s32.f32	s1, s0
    2e84: ee101a90     	vmov	r1, s1
    2e88: e3510005     	cmp	r1, #5
    2e8c: 8a000002     	bhi	0x2e9c <_setLayer+0x24> @ imm = #0x8
    2e90: e2802a02     	add	r2, r0, #8192
    2e94: e5821630     	str	r1, [r2, #0x630]
    2e98: e12fff1e     	bx	lr
    2e9c: e59f0004     	ldr	r0, [pc, #0x4]          @ 0x2ea8 <_setLayer+0x30>
    2ea0: e08f0000     	add	r0, pc, r0
    2ea4: eafffddc     	b	0x261c <.plt+0x23c>     @ imm = #-0x890
    2ea8: 30 9c 00 00  	.word	0x00009c30

