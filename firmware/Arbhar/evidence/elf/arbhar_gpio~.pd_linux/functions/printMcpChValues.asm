000042f4 <printMcpChValues>:
    42f4: eef27a0e     	vmov.f32	s15, #1.500000e+01
    42f8: e2801d77     	add	r1, r0, #7616
    42fc: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x433c <printMcpChValues+0x48>
    4300: e08f0000     	add	r0, pc, r0
    4304: eeb50ac0     	vcmpe.f32	s0, #0
    4308: eef1fa10     	vmrs	APSR_nzcv, fpscr
    430c: eeb40ae7     	vcmpe.f32	s0, s15
    4310: 43a03001     	movmi	r3, #1
    4314: 53a03000     	movpl	r3, #0
    4318: eef1fa10     	vmrs	APSR_nzcv, fpscr
    431c: c3833001     	orrgt	r3, r3, #1
    4320: e3530000     	cmp	r3, #0
    4324: 1eff0b00     	vmovne.f64	d16, #-1.000000e+00
    4328: 1ebf0a00     	vmovne.f32	s0, #-1.000000e+00
    432c: 0ef70ac0     	vcvteq.f64.f32	d16, s0
    4330: ed810a05     	vstr	s0, [r1, #20]
    4334: ec532b30     	vmov	r2, r3, d16
    4338: eafffe0e     	b	0x3b78 <.plt+0x47c>     @ imm = #-0x7c8
    433c: 1c 07 01 00  	.word	0x0001071c

