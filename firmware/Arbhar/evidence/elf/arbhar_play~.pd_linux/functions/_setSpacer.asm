00002df0 <_setSpacer>:
    2df0: eef77a00     	vmov.f32	s15, #1.000000e+00
    2df4: e2803d9a     	add	r3, r0, #9856
    2df8: e59f001c     	ldr	r0, [pc, #0x1c]         @ 0x2e1c <_setSpacer+0x2c>
    2dfc: e08f0000     	add	r0, pc, r0
    2e00: eeb40ae7     	vcmpe.f32	s0, s15
    2e04: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2e08: beb00a67     	vmovlt.f32	s0, s15
    2e0c: eef70ac0     	vcvt.f64.f32	d16, s0
    2e10: ed830a0e     	vstr	s0, [r3, #56]
    2e14: ec532b30     	vmov	r2, r3, d16
    2e18: eafffdff     	b	0x261c <.plt+0x23c>     @ imm = #-0x804
    2e1c: 7c 9c 00 00  	.word	0x00009c7c

