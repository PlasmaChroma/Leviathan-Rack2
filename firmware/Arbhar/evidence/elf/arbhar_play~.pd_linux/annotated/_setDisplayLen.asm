00003d68 <_setDisplayLen>:
    3d68: eefd7ac0     	vcvt.s32.f32	s15, s0
    3d6c: ee171a90     	vmov	r1, s15
    3d70: e3510005     	cmp	r1, #5
    3d74: 8a000003     	bhi	0x3d88 <_setDisplayLen+0x20> @ imm = #0xc
    3d78: eefd0ae0     	vcvt.s32.f32	s1, s1
    3d7c: e0801101     	add	r1, r0, r1, lsl #2
    3d80: edc10a0f     	vstr	s1, [r1, #60]
    3d84: e12fff1e     	bx	lr
    3d88: e3710001     	cmn	r1, #1
    3d8c: 1a000007     	bne	0x3db0 <_setDisplayLen+0x48> @ imm = #0x1c
    3d90: eebd0ae0     	vcvt.s32.f32	s0, s1
    3d94: ed800a0f     	vstr	s0, [r0, #60]
    3d98: ed800a10     	vstr	s0, [r0, #64]
    3d9c: ed800a11     	vstr	s0, [r0, #68]
    3da0: ed800a12     	vstr	s0, [r0, #72]
    3da4: ed800a13     	vstr	s0, [r0, #76]
    3da8: ed800a14     	vstr	s0, [r0, #80]
    3dac: e12fff1e     	bx	lr
    3db0: e59f0004     	ldr	r0, [pc, #0x4]          @ 0x3dbc <_setDisplayLen+0x54>  // u32=0x8df4; f32?=5.09231862e-41
    3db4: e08f0000     	add	r0, pc, r0
    3db8: eafffa17     	b	0x261c <.plt+0x23c>     @ imm = #-0x17a4  // CALL post
    3dbc: f4 8d 00 00  	.word	0x00008df4

