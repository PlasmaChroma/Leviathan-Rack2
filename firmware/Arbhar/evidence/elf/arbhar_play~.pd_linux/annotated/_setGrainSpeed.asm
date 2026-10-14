00002eac <_setGrainSpeed>:
    2eac: eeb50ac0     	vcmpe.f32	s0, #0
    2eb0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2eb4: ba000001     	blt	0x2ec0 <_setGrainSpeed+0x14> @ imm = #0x4
    2eb8: ed800a30     	vstr	s0, [r0, #192]
    2ebc: e12fff1e     	bx	lr
    2ec0: e59f0004     	ldr	r0, [pc, #0x4]          @ 0x2ecc <_setGrainSpeed+0x20>  // u32=0x9c24; f32?=5.60127022e-41
    2ec4: e08f0000     	add	r0, pc, r0
    2ec8: eafffdd3     	b	0x261c <.plt+0x23c>     @ imm = #-0x8b4  // CALL post
    2ecc: 24 9c 00 00  	.word	0x00009c24

