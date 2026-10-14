00004c1c <_tickInternTrig>:
    4c1c: e92d4010     	push	{r4, lr}
    4c20: e3a01000     	mov	r1, #0
    4c24: ed9f0a29     	vldr	s0, [pc, #164]          @ 0x4cd0 <_tickInternTrig+0xb4>
    4c28: e1a04000     	mov	r4, r0
    4c2c: ebfff650     	bl	0x2574 <.plt+0x74>      @ imm = #-0x26c0
    4c30: e5942274     	ldr	r2, [r4, #0x274]
    4c34: eddf7a26     	vldr	s15, [pc, #152]         @ 0x4cd4 <_tickInternTrig+0xb8>
    4c38: ee200a27     	vmul.f32	s0, s0, s15
    4c3c: eefd0ac0     	vcvt.s32.f32	s1, s0
    4c40: ee103a90     	vmov	r3, s1
    4c44: e1530002     	cmp	r3, r2
    4c48: a1a03002     	movge	r3, r2
    4c4c: e2430040     	sub	r0, r3, #64
    4c50: e3530040     	cmp	r3, #64
    4c54: e1c01fc0     	bic	r1, r0, r0, asr #31
    4c58: e5841274     	str	r1, [r4, #0x274]
    4c5c: da000013     	ble	0x4cb0 <_tickInternTrig+0x94> @ imm = #0x4c
    4c60: e1a00004     	mov	r0, r4
    4c64: ebfff6d2     	bl	0x27b4 <.plt+0x2b4>     @ imm = #-0x24b8
    4c68: eeb20a06     	vmov.f32	s0, #1.100000e+01
    4c6c: e3a01000     	mov	r1, #0
    4c70: e1a00004     	mov	r0, r4
    4c74: ebfff63e     	bl	0x2574 <.plt+0x74>      @ imm = #-0x2708
    4c78: ed9f1a16     	vldr	s2, [pc, #88]           @ 0x4cd8 <_tickInternTrig+0xbc>
    4c7c: eddf0b11     	vldr	d16, [pc, #68]          @ 0x4cc8 <_tickInternTrig+0xac>
    4c80: ed9f7a15     	vldr	s14, [pc, #84]          @ 0x4cdc <_tickInternTrig+0xc0>
    4c84: ee711a40     	vsub.f32	s3, s2, s0
    4c88: eeb72ae1     	vcvt.f64.f32	d2, s3
    4c8c: ee220b20     	vmul.f64	d0, d2, d16
    4c90: eef72bc0     	vcvt.f32.f64	s5, d0
    4c94: eef42ac7     	vcmpe.f32	s5, s14
    4c98: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4c9c: bef02a47     	vmovlt.f32	s5, s14
    4ca0: edc42a97     	vstr	s5, [r4, #604]
    4ca4: edc42a0e     	vstr	s5, [r4, #56]
    4ca8: edc42a1a     	vstr	s5, [r4, #104]
    4cac: e8bd8010     	pop	{r4, pc}
    4cb0: ed9f0a09     	vldr	s0, [pc, #36]           @ 0x4cdc <_tickInternTrig+0xc0>
    4cb4: e3a0206d     	mov	r2, #109
    4cb8: e3a01000     	mov	r1, #0
    4cbc: e1a00004     	mov	r0, r4
    4cc0: ebfff68b     	bl	0x26f4 <.plt+0x1f4>     @ imm = #-0x25d4
    4cc4: eaffffe5     	b	0x4c60 <_tickInternTrig+0x44> @ imm = #-0x6c
    4cc8: 01 be 80 03  	.word	0x0380be01
    4ccc: ff 00 30 3f  	.word	0x3f3000ff
    4cd0: 00 00 18 42  	.word	0x42180000
    4cd4: 00 00 40 42  	.word	0x42400000
    4cd8: 00 f0 7f 45  	.word	0x457ff000
    4cdc: 00 00 00 00  	.word	0x00000000

