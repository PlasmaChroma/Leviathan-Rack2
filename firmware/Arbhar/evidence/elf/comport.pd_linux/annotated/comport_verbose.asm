000010d4 <comport_verbose>:
    10d4: eefd7ac0     	vcvt.s32.f32	s15, s0
    10d8: eeb50ac0     	vcmpe.f32	s0, #0
    10dc: e2800a01     	add	r0, r0, #4096
    10e0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    10e4: ee171a90     	vmov	r1, s15
    10e8: edc07a38     	vstr	s15, [r0, #224]
    10ec: d12fff1e     	bxle	lr
    10f0: e59f2004     	ldr	r2, [pc, #0x4]          @ 0x10fc <comport_verbose+0x28>  // u32=0x3158; f32?=1.77012022e-41
    10f4: e08f0002     	add	r0, pc, r2
    10f8: eafffe8f     	b	0xb3c <.plt+0x134>      @ imm = #-0x5c4  // CALL post
    10fc: 58 31 00 00  	.word	0x00003158

