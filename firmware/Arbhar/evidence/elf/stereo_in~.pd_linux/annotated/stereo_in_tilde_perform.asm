00002d28 <stereo_in_tilde_perform>:
    2d28: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    2d2c: e1a0b000     	mov	r11, r0
    2d30: ed2d8b0e     	vpush	{d8, d9, d10, d11, d12, d13, d14}
    2d34: e5907018     	ldr	r7, [r0, #0x18]
    2d38: e5906004     	ldr	r6, [r0, #0x4]
    2d3c: e3570000     	cmp	r7, #0
    2d40: e5905008     	ldr	r5, [r0, #0x8]
    2d44: e24dd00c     	sub	sp, sp, #12
    2d48: e590a00c     	ldr	r10, [r0, #0xc]
    2d4c: edd68a0d     	vldr	s17, [r6, #52]
    2d50: e5909010     	ldr	r9, [r0, #0x10]
    2d54: e5908014     	ldr	r8, [r0, #0x14]
    2d58: ed968a0c     	vldr	s16, [r6, #48]
    2d5c: ed969a0f     	vldr	s18, [r6, #60]
    2d60: ed96da0e     	vldr	s26, [r6, #56]
    2d64: edd69a12     	vldr	s19, [r6, #72]
    2d68: ed96aa13     	vldr	s20, [r6, #76]
    2d6c: edd6aa14     	vldr	s21, [r6, #80]
    2d70: ed96ba15     	vldr	s22, [r6, #84]
    2d74: edd6ba10     	vldr	s23, [r6, #64]
    2d78: ed96ca11     	vldr	s24, [r6, #68]
    2d7c: da00006f     	ble	0x2f40 <stereo_in_tilde_perform+0x218> @ imm = #0x1bc
    2d80: e3170001     	tst	r7, #1
    2d84: e04aa005     	sub	r10, r10, r5
    2d88: e0499005     	sub	r9, r9, r5
    2d8c: e0488005     	sub	r8, r8, r5
    2d90: e0857107     	add	r7, r5, r7, lsl #2
    2d94: e1a04005     	mov	r4, r5
    2d98: eef9da08     	vmov.f32	s27, #-6.000000e+00
    2d9c: eeb1ea08     	vmov.f32	s28, #6.000000e+00
    2da0: eef3ea0b     	vmov.f32	s29, #2.700000e+01
    2da4: eef2ca02     	vmov.f32	s25, #9.000000e+00
    2da8: 0a00000c     	beq	0x2de0 <stereo_in_tilde_perform+0xb8> @ imm = #0x30
    2dac: e5962020     	ldr	r2, [r6, #0x20]
    2db0: e08a3005     	add	r3, r10, r5
    2db4: e5950000     	ldr	r0, [r5]
    2db8: e3520000     	cmp	r2, #0
    2dbc: ed931a00     	vldr	s2, [r3]
    2dc0: ca00003a     	bgt	0x2eb0 <stereo_in_tilde_perform+0x188> @ imm = #0xe8
    2dc4: e2854004     	add	r4, r5, #4
    2dc8: e0891005     	add	r1, r9, r5
    2dcc: e088c005     	add	r12, r8, r5
    2dd0: e1570004     	cmp	r7, r4
    2dd4: e5810000     	str	r0, [r1]
    2dd8: ed8c1a00     	vstr	s2, [r12]
    2ddc: 0a000057     	beq	0x2f40 <stereo_in_tilde_perform+0x218> @ imm = #0x15c
    2de0: e5962020     	ldr	r2, [r6, #0x20]
    2de4: e08a3004     	add	r3, r10, r4
    2de8: e2845004     	add	r5, r4, #4
    2dec: e3520000     	cmp	r2, #0
    2df0: eeb00a69     	vmov.f32	s0, s19
    2df4: e5942000     	ldr	r2, [r4]
    2df8: ed934a00     	vldr	s8, [r3]
    2dfc: eef00a68     	vmov.f32	s1, s17
    2e00: da000020     	ble	0x2e88 <stereo_in_tilde_perform+0x160> @ imm = #0x80
    2e04: e58d2004     	str	r2, [sp, #0x4]
    2e08: ee2b1a84     	vmul.f32	s2, s23, s8
    2e0c: ebfffd11     	bl	0x2258 <.plt+0x1f4>     @ imm = #-0xbbc  // CALL z_pole_filter
    2e10: eef00a48     	vmov.f32	s1, s16
    2e14: eeb01a40     	vmov.f32	s2, s0
    2e18: eef08a40     	vmov.f32	s17, s0
    2e1c: eeb00a4a     	vmov.f32	s0, s20
    2e20: ebfffcd9     	bl	0x218c <.plt+0x128>     @ imm = #-0xc9c  // CALL r_pole_filter
    2e24: eef00a49     	vmov.f32	s1, s18
    2e28: eeb01a40     	vmov.f32	s2, s0
    2e2c: eeb08a40     	vmov.f32	s16, s0
    2e30: eeb00a6a     	vmov.f32	s0, s21
    2e34: ebfffd07     	bl	0x2258 <.plt+0x1f4>     @ imm = #-0xbe4  // CALL z_pole_filter
    2e38: eef00a4d     	vmov.f32	s1, s26
    2e3c: eeb01a40     	vmov.f32	s2, s0
    2e40: eeb09a40     	vmov.f32	s18, s0
    2e44: eeb00a4b     	vmov.f32	s0, s22
    2e48: ebfffccf     	bl	0x218c <.plt+0x128>     @ imm = #-0xcc4  // CALL r_pole_filter
    2e4c: e59d2004     	ldr	r2, [sp, #0x4]
    2e50: eef06a6e     	vmov.f32	s13, s29
    2e54: ee6c1a00     	vmul.f32	s3, s24, s0
    2e58: eef41aed     	vcmpe.f32	s3, s27
    2e5c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2e60: bef01a6d     	vmovlt.f32	s3, s27
    2e64: eef41ace     	vcmpe.f32	s3, s28
    2e68: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2e6c: 8ef01a4e     	vmovhi.f32	s3, s28
    2e70: ee212aa1     	vmul.f32	s4, s3, s3
    2e74: ee722a2e     	vadd.f32	s5, s4, s29
    2e78: ee426a2c     	vmla.f32	s13, s4, s25
    2e7c: ee623aa1     	vmul.f32	s7, s5, s3
    2e80: ee834aa6     	vdiv.f32	s8, s7, s13
    2e84: eeb0da40     	vmov.f32	s26, s0
    2e88: e089c004     	add	r12, r9, r4
    2e8c: e0881004     	add	r1, r8, r4
    2e90: e5960020     	ldr	r0, [r6, #0x20]
    2e94: e08a3005     	add	r3, r10, r5
    2e98: e58c2000     	str	r2, [r12]
    2e9c: ed814a00     	vstr	s8, [r1]
    2ea0: e3500000     	cmp	r0, #0
    2ea4: ed931a00     	vldr	s2, [r3]
    2ea8: e5940004     	ldr	r0, [r4, #0x4]
    2eac: daffffc4     	ble	0x2dc4 <stereo_in_tilde_perform+0x9c> @ imm = #-0xf0
    2eb0: e58d0004     	str	r0, [sp, #0x4]
    2eb4: eef00a68     	vmov.f32	s1, s17
    2eb8: ee2b1a81     	vmul.f32	s2, s23, s2
    2ebc: eeb00a69     	vmov.f32	s0, s19
    2ec0: ebfffce4     	bl	0x2258 <.plt+0x1f4>     @ imm = #-0xc70  // CALL z_pole_filter
    2ec4: eef00a48     	vmov.f32	s1, s16
    2ec8: eeb01a40     	vmov.f32	s2, s0
    2ecc: eef08a40     	vmov.f32	s17, s0
    2ed0: eeb00a4a     	vmov.f32	s0, s20
    2ed4: ebfffcac     	bl	0x218c <.plt+0x128>     @ imm = #-0xd50  // CALL r_pole_filter
    2ed8: eef00a49     	vmov.f32	s1, s18
    2edc: eeb01a40     	vmov.f32	s2, s0
    2ee0: eeb08a40     	vmov.f32	s16, s0
    2ee4: eeb00a6a     	vmov.f32	s0, s21
    2ee8: ebfffcda     	bl	0x2258 <.plt+0x1f4>     @ imm = #-0xc98  // CALL z_pole_filter
    2eec: eef00a4d     	vmov.f32	s1, s26
    2ef0: eeb01a40     	vmov.f32	s2, s0
    2ef4: eeb09a40     	vmov.f32	s18, s0
    2ef8: eeb00a4b     	vmov.f32	s0, s22
    2efc: ebfffca2     	bl	0x218c <.plt+0x128>     @ imm = #-0xd78  // CALL r_pole_filter
    2f00: e59d0004     	ldr	r0, [sp, #0x4]
    2f04: eeb06a6e     	vmov.f32	s12, s29
    2f08: ee6c7a00     	vmul.f32	s15, s24, s0
    2f0c: eef47aed     	vcmpe.f32	s15, s27
    2f10: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2f14: bef07a6d     	vmovlt.f32	s15, s27
    2f18: eef47ace     	vcmpe.f32	s15, s28
    2f1c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    2f20: 8ef07a4e     	vmovhi.f32	s15, s28
    2f24: ee676aa7     	vmul.f32	s13, s15, s15
    2f28: ee367aae     	vadd.f32	s14, s13, s29
    2f2c: ee066aac     	vmla.f32	s12, s13, s25
    2f30: eeb0da40     	vmov.f32	s26, s0
    2f34: ee270a27     	vmul.f32	s0, s14, s15
    2f38: ee801a06     	vdiv.f32	s2, s0, s12
    2f3c: eaffffa0     	b	0x2dc4 <stereo_in_tilde_perform+0x9c> @ imm = #-0x180
    2f40: ee185a10     	vmov	r5, s16
    2f44: e28b001c     	add	r0, r11, #28
    2f48: ee1d9a10     	vmov	r9, s26
    2f4c: eddf0a0c     	vldr	s1, [pc, #48]           @ 0x2f84 <stereo_in_tilde_perform+0x25c>  // f32=0
    2f50: edc68a0d     	vstr	s17, [r6, #52]
    2f54: ed869a0f     	vstr	s18, [r6, #60]
    2f58: e025a0a5     	eor	r10, r5, r5, lsr #1
    2f5c: e31a0202     	tst	r10, #536870912
    2f60: e02980a9     	eor	r8, r9, r9, lsr #1
    2f64: 0eb08a60     	vmoveq.f32	s16, s1
    2f68: e3180202     	tst	r8, #536870912
    2f6c: 0eb0da60     	vmoveq.f32	s26, s1
    2f70: ed868a0c     	vstr	s16, [r6, #48]
    2f74: ed86da0e     	vstr	s26, [r6, #56]
    2f78: e28dd00c     	add	sp, sp, #12
    2f7c: ecbd8b0e     	vpop	{d8, d9, d10, d11, d12, d13, d14}
    2f80: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2f84: 00 00 00 00  	.word	0x00000000

