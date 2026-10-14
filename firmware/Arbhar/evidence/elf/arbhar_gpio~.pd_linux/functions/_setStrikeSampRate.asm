00004340 <_setStrikeSampRate>:
    4340: eef70ac0     	vcvt.f64.f32	d16, s0
    4344: e2803d77     	add	r3, r0, #7616
    4348: e59f000c     	ldr	r0, [pc, #0xc]          @ 0x435c <_setStrikeSampRate+0x1c>
    434c: ed830a04     	vstr	s0, [r3, #16]
    4350: e08f0000     	add	r0, pc, r0
    4354: ec532b30     	vmov	r2, r3, d16
    4358: eafffe06     	b	0x3b78 <.plt+0x47c>     @ imm = #-0x7e8
    435c: e0 06 01 00  	.word	0x000106e0

