00003dc0 <arbhar_play_tilde_midiNoteShare>:
    3dc0: eef50ac0     	vcmpe.f32	s1, #0
    3dc4: e92d4010     	push	{r4, lr}
    3dc8: e2804a02     	add	r4, r0, #8192
    3dcc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3dd0: da000029     	ble	0x3e7c <arbhar_play_tilde_midiNoteShare+0xbc> @ imm = #0xa4
    3dd4: e2843e6f     	add	r3, r4, #1776
    3dd8: edd37a00     	vldr	s15, [r3]
    3ddc: eef57a40     	vcmp.f32	s15, #0
    3de0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3de4: 0a000090     	beq	0x402c <arbhar_play_tilde_midiNoteShare+0x26c> @ imm = #0x240
    3de8: edd31a03     	vldr	s3, [r3, #12]
    3dec: eef51a40     	vcmp.f32	s3, #0
    3df0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3df4: 0a00008e     	beq	0x4034 <arbhar_play_tilde_midiNoteShare+0x274> @ imm = #0x238
    3df8: e2841c07     	add	r1, r4, #1792
    3dfc: ed912a02     	vldr	s4, [r1, #8]
    3e00: eeb52a40     	vcmp.f32	s4, #0
    3e04: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3e08: 0a00008b     	beq	0x403c <arbhar_play_tilde_midiNoteShare+0x27c> @ imm = #0x22c
    3e0c: e2842e71     	add	r2, r4, #1808
    3e10: edd22a01     	vldr	s5, [r2, #4]
    3e14: eef52a40     	vcmp.f32	s5, #0
    3e18: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3e1c: 0a000088     	beq	0x4044 <arbhar_play_tilde_midiNoteShare+0x284> @ imm = #0x220
    3e20: e284ce72     	add	r12, r4, #1824
    3e24: ed9c3a00     	vldr	s6, [r12]
    3e28: eeb53a40     	vcmp.f32	s6, #0
    3e2c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3e30: 0a000079     	beq	0x401c <arbhar_play_tilde_midiNoteShare+0x25c> @ imm = #0x1e4
    3e34: eddc3a03     	vldr	s7, [r12, #12]
    3e38: eef53a40     	vcmp.f32	s7, #0
    3e3c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3e40: 0a000077     	beq	0x4024 <arbhar_play_tilde_midiNoteShare+0x264> @ imm = #0x1dc
    3e44: e284ee73     	add	lr, r4, #1840
    3e48: ed9e4a02     	vldr	s8, [lr, #8]
    3e4c: eeb54a40     	vcmp.f32	s8, #0
    3e50: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3e54: 0a000065     	beq	0x3ff0 <arbhar_play_tilde_midiNoteShare+0x230> @ imm = #0x194
    3e58: e2843d1d     	add	r3, r4, #1856
    3e5c: edd34a01     	vldr	s9, [r3, #4]
    3e60: eef54a40     	vcmp.f32	s9, #0
    3e64: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3e68: 03a03007     	moveq	r3, #7
    3e6c: 0a000060     	beq	0x3ff4 <arbhar_play_tilde_midiNoteShare+0x234> @ imm = #0x180
    3e70: ebfffc32     	bl	0x2f40 <_getChord>      @ imm = #-0xf38
    3e74: e58406ec     	str	r0, [r4, #0x6ec]
    3e78: e8bd8010     	pop	{r4, pc}
    3e7c: e284ee6f     	add	lr, r4, #1776
    3e80: edde0a00     	vldr	s1, [lr]
    3e84: eeb40a60     	vcmp.f32	s0, s1
    3e88: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3e8c: 1a000004     	bne	0x3ea4 <arbhar_play_tilde_midiNoteShare+0xe4> @ imm = #0x10
    3e90: e3a0c000     	mov	r12, #0
    3e94: e3a02000     	mov	r2, #0
    3e98: e58ec000     	str	r12, [lr]
    3e9c: e58ec004     	str	r12, [lr, #0x4]
    3ea0: e58426f8     	str	r2, [r4, #0x6f8]
    3ea4: e2841e6f     	add	r1, r4, #1776
    3ea8: e281e00c     	add	lr, r1, #12
    3eac: ed915a03     	vldr	s10, [r1, #12]
    3eb0: eeb40a45     	vcmp.f32	s0, s10
    3eb4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3eb8: 1a000005     	bne	0x3ed4 <arbhar_play_tilde_midiNoteShare+0x114> @ imm = #0x14
    3ebc: e2803c27     	add	r3, r0, #9984
    3ec0: e3a01000     	mov	r1, #0
    3ec4: e3a0c000     	mov	r12, #0
    3ec8: e58e1000     	str	r1, [lr]
    3ecc: e5831000     	str	r1, [r3]
    3ed0: e584c704     	str	r12, [r4, #0x704]
    3ed4: e284ec07     	add	lr, r4, #1792
    3ed8: e28e3008     	add	r3, lr, #8
    3edc: edde5a02     	vldr	s11, [lr, #8]
    3ee0: eeb40a65     	vcmp.f32	s0, s11
    3ee4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3ee8: 1a000005     	bne	0x3f04 <arbhar_play_tilde_midiNoteShare+0x144> @ imm = #0x14
    3eec: e2842c07     	add	r2, r4, #1792
    3ef0: e3a01000     	mov	r1, #0
    3ef4: e3a0c000     	mov	r12, #0
    3ef8: e5831000     	str	r1, [r3]
    3efc: e582100c     	str	r1, [r2, #0xc]
    3f00: e584c710     	str	r12, [r4, #0x710]
    3f04: e284ee71     	add	lr, r4, #1808
    3f08: e28e3004     	add	r3, lr, #4
    3f0c: ed9e6a01     	vldr	s12, [lr, #4]
    3f10: eeb40a46     	vcmp.f32	s0, s12
    3f14: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3f18: 1a000005     	bne	0x3f34 <arbhar_play_tilde_midiNoteShare+0x174> @ imm = #0x14
    3f1c: e2842e71     	add	r2, r4, #1808
    3f20: e3a01000     	mov	r1, #0
    3f24: e3a0c000     	mov	r12, #0
    3f28: e5831000     	str	r1, [r3]
    3f2c: e5821008     	str	r1, [r2, #0x8]
    3f30: e584c71c     	str	r12, [r4, #0x71c]
    3f34: e284ee72     	add	lr, r4, #1824
    3f38: edde6a00     	vldr	s13, [lr]
    3f3c: eeb40a66     	vcmp.f32	s0, s13
    3f40: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3f44: 1a000004     	bne	0x3f5c <arbhar_play_tilde_midiNoteShare+0x19c> @ imm = #0x10
    3f48: e3a03000     	mov	r3, #0
    3f4c: e3a02000     	mov	r2, #0
    3f50: e58e3000     	str	r3, [lr]
    3f54: e58e3004     	str	r3, [lr, #0x4]
    3f58: e5842728     	str	r2, [r4, #0x728]
    3f5c: e284ce72     	add	r12, r4, #1824
    3f60: e28ce00c     	add	lr, r12, #12
    3f64: ed9c7a03     	vldr	s14, [r12, #12]
    3f68: eeb40a47     	vcmp.f32	s0, s14
    3f6c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3f70: 1a000005     	bne	0x3f8c <arbhar_play_tilde_midiNoteShare+0x1cc> @ imm = #0x14
    3f74: e2843e73     	add	r3, r4, #1840
    3f78: e3a01000     	mov	r1, #0
    3f7c: e3a02000     	mov	r2, #0
    3f80: e58e1000     	str	r1, [lr]
    3f84: e5831000     	str	r1, [r3]
    3f88: e5842734     	str	r2, [r4, #0x734]
    3f8c: e284ee73     	add	lr, r4, #1840
    3f90: e28e3008     	add	r3, lr, #8
    3f94: edde7a02     	vldr	s15, [lr, #8]
    3f98: eeb40a67     	vcmp.f32	s0, s15
    3f9c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3fa0: 1a000005     	bne	0x3fbc <arbhar_play_tilde_midiNoteShare+0x1fc> @ imm = #0x14
    3fa4: e2842e73     	add	r2, r4, #1840
    3fa8: e3a01000     	mov	r1, #0
    3fac: e3a0c000     	mov	r12, #0
    3fb0: e5831000     	str	r1, [r3]
    3fb4: e582100c     	str	r1, [r2, #0xc]
    3fb8: e584c740     	str	r12, [r4, #0x740]
    3fbc: e284ed1d     	add	lr, r4, #1856
    3fc0: e28e3004     	add	r3, lr, #4
    3fc4: edde1a01     	vldr	s3, [lr, #4]
    3fc8: eeb40a61     	vcmp.f32	s0, s3
    3fcc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3fd0: 1affffa6     	bne	0x3e70 <arbhar_play_tilde_midiNoteShare+0xb0> @ imm = #-0x168
    3fd4: e2842d1d     	add	r2, r4, #1856
    3fd8: e3a01000     	mov	r1, #0
    3fdc: e3a0c000     	mov	r12, #0
    3fe0: e5831000     	str	r1, [r3]
    3fe4: e5821008     	str	r1, [r2, #0x8]
    3fe8: e584c74c     	str	r12, [r4, #0x74c]
    3fec: eaffff9f     	b	0x3e70 <arbhar_play_tilde_midiNoteShare+0xb0> @ imm = #-0x184
    3ff0: e3a03006     	mov	r3, #6
    3ff4: eebd1ac1     	vcvt.s32.f32	s2, s2
    3ff8: e3a0100c     	mov	r1, #12
    3ffc: e02e0391     	mla	lr, r1, r3, r0
    4000: e302c6f8     	movw	r12, #0x26f8
    4004: e28e3d9b     	add	r3, lr, #9920
    4008: ee111a10     	vmov	r1, s2
    400c: ed830a0c     	vstr	s0, [r3, #48]
    4010: edc30a0d     	vstr	s1, [r3, #52]
    4014: e78e100c     	str	r1, [lr, r12]
    4018: eaffff94     	b	0x3e70 <arbhar_play_tilde_midiNoteShare+0xb0> @ imm = #-0x1b0
    401c: e3a03004     	mov	r3, #4
    4020: eafffff3     	b	0x3ff4 <arbhar_play_tilde_midiNoteShare+0x234> @ imm = #-0x34
    4024: e3a03005     	mov	r3, #5
    4028: eafffff1     	b	0x3ff4 <arbhar_play_tilde_midiNoteShare+0x234> @ imm = #-0x3c
    402c: e3a03000     	mov	r3, #0
    4030: eaffffef     	b	0x3ff4 <arbhar_play_tilde_midiNoteShare+0x234> @ imm = #-0x44
    4034: e3a03001     	mov	r3, #1
    4038: eaffffed     	b	0x3ff4 <arbhar_play_tilde_midiNoteShare+0x234> @ imm = #-0x4c
    403c: e3a03002     	mov	r3, #2
    4040: eaffffeb     	b	0x3ff4 <arbhar_play_tilde_midiNoteShare+0x234> @ imm = #-0x54
    4044: e3a03003     	mov	r3, #3
    4048: eaffffe9     	b	0x3ff4 <arbhar_play_tilde_midiNoteShare+0x234> @ imm = #-0x5c

