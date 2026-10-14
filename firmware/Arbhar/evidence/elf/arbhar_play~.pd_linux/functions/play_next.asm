00006c34 <play_next>:
    6c34: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    6c38: eef30a07     	vmov.f32	s1, #2.300000e+01
    6c3c: ed9f1afb     	vldr	s2, [pc, #1004]         @ 0x7030 <play_next+0x3fc>
    6c40: e2806a02     	add	r6, r0, #8192
    6c44: e1a04000     	mov	r4, r0
    6c48: ed2d8b0a     	vpush	{d8, d9, d10, d11, d12}
    6c4c: e24dd014     	sub	sp, sp, #20
    6c50: eeb00a41     	vmov.f32	s0, s2
    6c54: ebffee1c     	bl	0x24cc <.plt+0xec>      @ imm = #-0x4790
    6c58: ed9f1af4     	vldr	s2, [pc, #976]          @ 0x7030 <play_next+0x3fc>
    6c5c: e1a00004     	mov	r0, r4
    6c60: eef30a0c     	vmov.f32	s1, #2.800000e+01
    6c64: eeb00a41     	vmov.f32	s0, s2
    6c68: ebffee17     	bl	0x24cc <.plt+0xec>      @ imm = #-0x47a4
    6c6c: e5962894     	ldr	r2, [r6, #0x894]
    6c70: e2863e85     	add	r3, r6, #2128
    6c74: e59616e8     	ldr	r1, [r6, #0x6e8]
    6c78: edd27a25     	vldr	s15, [r2, #148]
    6c7c: e3510000     	cmp	r1, #0
    6c80: d3a0a000     	movle	r10, #0
    6c84: eeb77a00     	vmov.f32	s14, #1.000000e+00
    6c88: ee370aa7     	vadd.f32	s0, s15, s15
    6c8c: ee700a47     	vsub.f32	s1, s0, s14
    6c90: eebd1ae0     	vcvt.s32.f32	s2, s1
    6c94: eef81ac1     	vcvt.f32.s32	s3, s2
    6c98: edc31a03     	vstr	s3, [r3, #12]
    6c9c: ed922a28     	vldr	s4, [r2, #160]
    6ca0: eefd2ac2     	vcvt.s32.f32	s5, s4
    6ca4: ee120a90     	vmov	r0, s5
    6ca8: e6bf1070     	sxth	r1, r0
    6cac: e1c41ab0     	strh	r1, [r4, #160]
    6cb0: da000004     	ble	0x6cc8 <play_next+0x94> @ imm = #0x10
    6cb4: e1a00004     	mov	r0, r4
    6cb8: ebfff0a0     	bl	0x2f40 <_getChord>      @ imm = #-0x3d80
    6cbc: e1d41af0     	ldrsh	r1, [r4, #160]
    6cc0: e240a001     	sub	r10, r0, #1
    6cc4: e1caafca     	bic	r10, r10, r10, asr #31
    6cc8: eef78a00     	vmov.f32	s17, #1.000000e+00
    6ccc: e2848c25     	add	r8, r4, #9472
    6cd0: e59656e0     	ldr	r5, [r6, #0x6e0]
    6cd4: e2888028     	add	r8, r8, #40
    6cd8: e28a9001     	add	r9, r10, #1
    6cdc: e58d9004     	str	r9, [sp, #0x4]
    6ce0: e3a09000     	mov	r9, #0
    6ce4: e308c51f     	movw	r12, #0x851f
    6ce8: e30be589     	movw	lr, #0xb589
    6cec: e345c1eb     	movt	r12, #0x51eb
    6cf0: e341e4f8     	movt	lr, #0x14f8
    6cf4: e58dc008     	str	r12, [sp, #0x8]
    6cf8: e58de00c     	str	lr, [sp, #0xc]
    6cfc: eeb09a08     	vmov.f32	s18, #3.000000e+00
    6d00: eeb1aa04     	vmov.f32	s20, #5.000000e+00
    6d04: eeff9a00     	vmov.f32	s19, #-1.000000e+00
    6d08: e3510000     	cmp	r1, #0
    6d0c: e2855001     	add	r5, r5, #1
    6d10: ed9f1bc2     	vldr	d1, [pc, #776]          @ 0x7020 <play_next+0x3ec>
    6d14: c3a0a00f     	movgt	r10, #15
    6d18: d3a0a000     	movle	r10, #0
    6d1c: e3550051     	cmp	r5, #81
    6d20: d3a07074     	movle	r7, #116
    6d24: c3a05000     	movgt	r5, #0
    6d28: d0070597     	mulle	r7, r7, r5
    6d2c: c3a0bf41     	movgt	r11, #260
    6d30: d287bf41     	addle	r11, r7, #260
    6d34: e59676c8     	ldr	r7, [r6, #0x6c8]
    6d38: e0872005     	add	r2, r7, r5
    6d3c: e082300a     	add	r3, r2, r10
    6d40: ee033a10     	vmov	s6, r3
    6d44: eeb80bc3     	vcvt.f64.s32	d0, s6
    6d48: ebffee06     	bl	0x2568 <.plt+0x188>     @ imm = #-0x47e8
    6d4c: e596162c     	ldr	r1, [r6, #0x62c]
    6d50: e351002a     	cmp	r1, #42
    6d54: eebd8bc0     	vcvt.s32.f64	s16, d0
    6d58: ca0001c6     	bgt	0x7478 <play_next+0x844> @ imm = #0x718
    6d5c: e3a00074     	mov	r0, #116
    6d60: e3a0c001     	mov	r12, #1
    6d64: e02e4590     	mla	lr, r0, r5, r4
    6d68: e58ec12c     	str	r12, [lr, #0x12c]
    6d6c: e3a02074     	mov	r2, #116
    6d70: e084700b     	add	r7, r4, r11
    6d74: e02b4592     	mla	r11, r2, r5, r4
    6d78: e3a03000     	mov	r3, #0
    6d7c: e1a01007     	mov	r1, r7
    6d80: e1a00004     	mov	r0, r4
    6d84: e58b3124     	str	r3, [r11, #0x124]
    6d88: ebffee26     	bl	0x2628 <.plt+0x248>     @ imm = #-0x4768
    6d8c: e5960894     	ldr	r0, [r6, #0x894]
    6d90: e048c004     	sub	r12, r8, r4
    6d94: e30c1235     	movw	r1, #0xc235
    6d98: e24c2074     	sub	r2, r12, #116
    6d9c: e3401f72     	movt	r1, #0xf72
    6da0: ed905a3c     	vldr	s10, [r0, #240]
    6da4: e1a03004     	mov	r3, r4
    6da8: e1a0b122     	lsr	r11, r2, #2
    6dac: e3a02000     	mov	r2, #0
    6db0: e00c0b91     	mul	r12, r1, r11
    6db4: ed845a32     	vstr	s10, [r4, #200]
    6db8: e28c1001     	add	r1, r12, #1
    6dbc: e590b0f4     	ldr	r11, [r0, #0xf4]
    6dc0: e2111007     	ands	r1, r1, #7
    6dc4: e584b0cc     	str	r11, [r4, #0xcc]
    6dc8: e590c0f8     	ldr	r12, [r0, #0xf8]
    6dcc: e584c0d0     	str	r12, [r4, #0xd0]
    6dd0: 0a000029     	beq	0x6e7c <play_next+0x248> @ imm = #0xa4
    6dd4: e3510001     	cmp	r1, #1
    6dd8: 0a000021     	beq	0x6e64 <play_next+0x230> @ imm = #0x84
    6ddc: e3510002     	cmp	r1, #2
    6de0: 0a00001b     	beq	0x6e54 <play_next+0x220> @ imm = #0x6c
    6de4: e3510003     	cmp	r1, #3
    6de8: 0a000015     	beq	0x6e44 <play_next+0x210> @ imm = #0x54
    6dec: e3510004     	cmp	r1, #4
    6df0: 0a00000f     	beq	0x6e34 <play_next+0x200> @ imm = #0x3c
    6df4: e3510005     	cmp	r1, #5
    6df8: 0a000009     	beq	0x6e24 <play_next+0x1f0> @ imm = #0x24
    6dfc: e3510006     	cmp	r1, #6
    6e00: 0a000003     	beq	0x6e14 <play_next+0x1e0> @ imm = #0xc
    6e04: e594e12c     	ldr	lr, [r4, #0x12c]
    6e08: e2843074     	add	r3, r4, #116
    6e0c: e35e0000     	cmp	lr, #0
    6e10: c3a02001     	movgt	r2, #1
    6e14: e593b12c     	ldr	r11, [r3, #0x12c]
    6e18: e2833074     	add	r3, r3, #116
    6e1c: e35b0000     	cmp	r11, #0
    6e20: c2822001     	addgt	r2, r2, #1
    6e24: e593112c     	ldr	r1, [r3, #0x12c]
    6e28: e2833074     	add	r3, r3, #116
    6e2c: e3510000     	cmp	r1, #0
    6e30: c2822001     	addgt	r2, r2, #1
    6e34: e593c12c     	ldr	r12, [r3, #0x12c]
    6e38: e2833074     	add	r3, r3, #116
    6e3c: e35c0000     	cmp	r12, #0
    6e40: c2822001     	addgt	r2, r2, #1
    6e44: e593e12c     	ldr	lr, [r3, #0x12c]
    6e48: e2833074     	add	r3, r3, #116
    6e4c: e35e0000     	cmp	lr, #0
    6e50: c2822001     	addgt	r2, r2, #1
    6e54: e593b12c     	ldr	r11, [r3, #0x12c]
    6e58: e2833074     	add	r3, r3, #116
    6e5c: e35b0000     	cmp	r11, #0
    6e60: c2822001     	addgt	r2, r2, #1
    6e64: e593112c     	ldr	r1, [r3, #0x12c]
    6e68: e2833074     	add	r3, r3, #116
    6e6c: e3510000     	cmp	r1, #0
    6e70: c2822001     	addgt	r2, r2, #1
    6e74: e1530008     	cmp	r3, r8
    6e78: 0a00001b     	beq	0x6eec <play_next+0x2b8> @ imm = #0x6c
    6e7c: e593c12c     	ldr	r12, [r3, #0x12c]
    6e80: e2833074     	add	r3, r3, #116
    6e84: e35c0000     	cmp	r12, #0
    6e88: e593c288     	ldr	r12, [r3, #0x288]
    6e8c: e593e12c     	ldr	lr, [r3, #0x12c]
    6e90: c2822001     	addgt	r2, r2, #1
    6e94: e593b1a0     	ldr	r11, [r3, #0x1a0]
    6e98: e35e0000     	cmp	lr, #0
    6e9c: e5931214     	ldr	r1, [r3, #0x214]
    6ea0: c2822001     	addgt	r2, r2, #1
    6ea4: e35b0000     	cmp	r11, #0
    6ea8: c2822001     	addgt	r2, r2, #1
    6eac: e593e2fc     	ldr	lr, [r3, #0x2fc]
    6eb0: e3510000     	cmp	r1, #0
    6eb4: e593b370     	ldr	r11, [r3, #0x370]
    6eb8: c2822001     	addgt	r2, r2, #1
    6ebc: e35c0000     	cmp	r12, #0
    6ec0: c2822001     	addgt	r2, r2, #1
    6ec4: e59313e4     	ldr	r1, [r3, #0x3e4]
    6ec8: e35e0000     	cmp	lr, #0
    6ecc: e2833fcb     	add	r3, r3, #812
    6ed0: c2822001     	addgt	r2, r2, #1
    6ed4: e35b0000     	cmp	r11, #0
    6ed8: c2822001     	addgt	r2, r2, #1
    6edc: e3510000     	cmp	r1, #0
    6ee0: c2822001     	addgt	r2, r2, #1
    6ee4: e1530008     	cmp	r3, r8
    6ee8: 1affffe3     	bne	0x6e7c <play_next+0x248> @ imm = #-0x74
    6eec: e3a0c074     	mov	r12, #116
    6ef0: e586262c     	str	r2, [r6, #0x62c]
    6ef4: e02b459c     	mla	r11, r12, r5, r4
    6ef8: eeb55ac0     	vcmpe.f32	s10, #0
    6efc: edcb8a47     	vstr	s17, [r11, #284]
    6f00: e59420ac     	ldr	r2, [r4, #0xac]
    6f04: e3520000     	cmp	r2, #0
    6f08: c3a02001     	movgt	r2, #1
    6f0c: d3a02000     	movle	r2, #0
    6f10: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6f14: c2022001     	andgt	r2, r2, #1
    6f18: d3a02000     	movle	r2, #0
    6f1c: e3520000     	cmp	r2, #0
    6f20: 1a000138     	bne	0x7408 <play_next+0x7d4> @ imm = #0x4e0
    6f24: e3a0e074     	mov	lr, #116
    6f28: e596b630     	ldr	r11, [r6, #0x630]
    6f2c: e023459e     	mla	r3, lr, r5, r4
    6f30: e1d4caf0     	ldrsh	r12, [r4, #160]
    6f34: e583b13c     	str	r11, [r3, #0x13c]
    6f38: ed907acb     	vldr	s14, [r0, #812]
    6f3c: edd07aca     	vldr	s15, [r0, #808]
    6f40: eebd0ac7     	vcvt.s32.f32	s0, s14
    6f44: edd00a32     	vldr	s1, [r0, #200]
    6f48: eebd1ae7     	vcvt.s32.f32	s2, s15
    6f4c: ee101a10     	vmov	r1, s0
    6f50: ee110a10     	vmov	r0, s2
    6f54: e1a02201     	lsl	r2, r1, #4
    6f58: e062e200     	rsb	lr, r2, r0, lsl #4
    6f5c: ee01ea90     	vmov	s3, lr
    6f60: eeb82ae1     	vcvt.f32.s32	s4, s3
    6f64: ee702ac2     	vsub.f32	s5, s1, s4
    6f68: eef52ac0     	vcmpe.f32	s5, #0
    6f6c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6f70: ba0001b0     	blt	0x7638 <play_next+0xa04> @ imm = #0x6c0
    6f74: ed9f3a32     	vldr	s6, [pc, #200]          @ 0x7044 <play_next+0x410>
    6f78: e2863e85     	add	r3, r6, #2128
    6f7c: eddf3a2d     	vldr	s7, [pc, #180]          @ 0x7038 <play_next+0x404>
    6f80: eef42ac3     	vcmpe.f32	s5, s6
    6f84: ed9f4a2a     	vldr	s8, [pc, #168]          @ 0x7034 <play_next+0x400>
    6f88: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6f8c: 8ef02a43     	vmovhi.f32	s5, s6
    6f90: e35c0000     	cmp	r12, #0
    6f94: ee734ae2     	vsub.f32	s9, s7, s5
    6f98: ee24ba84     	vmul.f32	s22, s9, s8
    6f9c: ed83ba02     	vstr	s22, [r3, #8]
    6fa0: 0a0001e9     	beq	0x774c <play_next+0xb18> @ imm = #0x7a4
    6fa4: eeb5bac0     	vcmpe.f32	s22, #0
    6fa8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6fac: aa0001a5     	bge	0x7648 <play_next+0xa14> @ imm = #0x694
    6fb0: eebe5a00     	vmov.f32	s10, #-5.000000e-01
    6fb4: eeb4bac5     	vcmpe.f32	s22, s10
    6fb8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6fbc: ba000201     	blt	0x77c8 <play_next+0xb94> @ imm = #0x804
    6fc0: ebffeda1     	bl	0x264c <.plt+0x26c>     @ imm = #-0x497c
    6fc4: eef8ca00     	vmov.f32	s25, #-2.000000e+00
    6fc8: e1a0b000     	mov	r11, r0
    6fcc: ebffed9e     	bl	0x264c <.plt+0x26c>     @ imm = #-0x4988
    6fd0: ee2b7a2c     	vmul.f32	s14, s22, s25
    6fd4: e59dc008     	ldr	r12, [sp, #0x8]
    6fd8: eddf9b12     	vldr	d25, [pc, #72]          @ 0x7028 <play_next+0x3f4>
    6fdc: e35b0000     	cmp	r11, #0
    6fe0: eef7aac7     	vcvt.f64.f32	d26, s14
    6fe4: ee6abba9     	vmul.f64	d27, d26, d25
    6fe8: e0c1309c     	smull	r3, r1, r12, r0
    6fec: e1a02fc0     	asr	r2, r0, #31
    6ff0: e20b3001     	and	r3, r11, #1
    6ff4: e3a0b064     	mov	r11, #100
    6ff8: b2633000     	rsblt	r3, r3, #0
    6ffc: ee003a10     	vmov	s0, r3
    7000: e06222c1     	rsb	r2, r2, r1, asr #5
    7004: eef8dbc0     	vcvt.f64.s32	d29, s0
    7008: e060029b     	mls	r0, r11, r2, r0
    700c: ee070a90     	vmov	s15, r0
    7010: eef8cbe7     	vcvt.f64.s32	d28, s15
    7014: ee4cdbeb     	vmls.f64	d29, d28, d27
    7018: eef7bbed     	vcvt.f32.f64	s23, d29
    701c: ea00000c     	b	0x7054 <play_next+0x420> @ imm = #0x30
    7020: 00 00 00 00  	.word	0x00000000
    7024: 00 80 54 40  	.word	0x40548000
    7028: 7b 14 ae 47  	.word	0x47ae147b
    702c: e1 7a 84 3f  	.word	0x3f847ae1
    7030: 00 00 00 00  	.word	0x00000000
    7034: 00 00 00 3a  	.word	0x3a000000
    7038: 00 00 00 45  	.word	0x45000000
    703c: 00 00 80 3a  	.word	0x3a800000
    7040: 00 00 00 42  	.word	0x42000000
    7044: 00 f0 7f 45  	.word	0x457ff000
    7048: 00 00 00 00  	.word	0x00000000
    704c: 02 10 00 3a  	.word	0x3a001002
    7050: 00 80 7c 45  	.word	0x457c8000
    7054: e3a0e074     	mov	lr, #116
    7058: e023459e     	mla	r3, lr, r5, r4
    705c: eeb02aeb     	vabs.f32	s4, s23
    7060: ed832a4d     	vstr	s4, [r3, #308]
    7064: ed832a4e     	vstr	s4, [r3, #312]
    7068: e3a01074     	mov	r1, #116
    706c: e596b894     	ldr	r11, [r6, #0x894]
    7070: e0234591     	mla	r3, r1, r5, r4
    7074: ed5fca11     	vldr	s25, [pc, #-68]         @ 0x7038 <play_next+0x404>
    7078: edc38a55     	vstr	s17, [r3, #340]
    707c: ed9bca02     	vldr	s24, [r11, #8]
    7080: e59628c8     	ldr	r2, [r6, #0x8c8]
    7084: eddbba68     	vldr	s23, [r11, #416]
    7088: e352009e     	cmp	r2, #158
    708c: d2422001     	suble	r2, r2, #1
    7090: c3a02f9e     	movgt	r2, #632
    7094: ee7c7acc     	vsub.f32	s15, s25, s24
    7098: d1a02102     	lslle	r2, r2, #2
    709c: e08b0002     	add	r0, r11, r2
    70a0: ed907a00     	vldr	s14, [r0]
    70a4: ee270a89     	vmul.f32	s0, s15, s18
    70a8: ee7c0aeb     	vsub.f32	s1, s25, s23
    70ac: eeb57a40     	vcmp.f32	s14, #0
    70b0: eec0aa20     	vdiv.f32	s21, s0, s1
    70b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    70b8: 1a000101     	bne	0x74c4 <play_next+0x890> @ imm = #0x404
    70bc: eddb3a03     	vldr	s7, [r11, #12]
    70c0: ed1f4a23     	vldr	s8, [pc, #-140]         @ 0x703c <play_next+0x408>
    70c4: ee7c4ae3     	vsub.f32	s9, s25, s7
    70c8: ee44aa84     	vmla.f32	s21, s9, s8
    70cc: eeb70aea     	vcvt.f64.f32	d0, s21
    70d0: ebffed84     	bl	0x26e8 <.plt+0x308>     @ imm = #-0x49f0
    70d4: eef7abc0     	vcvt.f32.f64	s21, d0
    70d8: e2862e63     	add	r2, r6, #1584
    70dc: ed925a01     	vldr	s10, [r2, #4]
    70e0: eeb55ac0     	vcmpe.f32	s10, #0
    70e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    70e8: ca00010b     	bgt	0x751c <play_next+0x8e8> @ imm = #0x42c
    70ec: e286ce6b     	add	r12, r6, #1712
    70f0: ed9cca00     	vldr	s24, [r12]
    70f4: eeb5cac0     	vcmpe.f32	s24, #0
    70f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    70fc: ca000125     	bgt	0x7598 <play_next+0x964> @ imm = #0x494
    7100: e596e6e8     	ldr	lr, [r6, #0x6e8]
    7104: e596b6ec     	ldr	r11, [r6, #0x6ec]
    7108: e35e0000     	cmp	lr, #0
    710c: c3a0e001     	movgt	lr, #1
    7110: d3a0e000     	movle	lr, #0
    7114: e35b0000     	cmp	r11, #0
    7118: c20ee001     	andgt	lr, lr, #1
    711c: d3a0e000     	movle	lr, #0
    7120: e35e0000     	cmp	lr, #0
    7124: 0eb07a68     	vmoveq.f32	s14, s17
    7128: 1a000155     	bne	0x7684 <play_next+0xa50> @ imm = #0x554
    712c: edc4aa30     	vstr	s21, [r4, #192]
    7130: e3a0c074     	mov	r12, #116
    7134: e5960894     	ldr	r0, [r6, #0x894]
    7138: e02e459c     	mla	lr, r12, r5, r4
    713c: ed877a07     	vstr	s14, [r7, #28]
    7140: edc7aa05     	vstr	s21, [r7, #20]
    7144: ed1f6a43     	vldr	s12, [pc, #-268]        @ 0x7040 <play_next+0x40c>
    7148: edd0bad8     	vldr	s23, [r0, #864]
    714c: edd0ca0e     	vldr	s25, [r0, #56]
    7150: ed5f6a45     	vldr	s13, [pc, #-276]        @ 0x7044 <play_next+0x410>
    7154: ee5bca86     	vnmls.f32	s25, s23, s12
    7158: ed1fca46     	vldr	s24, [pc, #-280]        @ 0x7048 <play_next+0x414>
    715c: ed5f7a46     	vldr	s15, [pc, #-280]        @ 0x704c <play_next+0x418>
    7160: eeb00a68     	vmov.f32	s0, s17
    7164: ee3c7aa6     	vadd.f32	s14, s25, s13
    7168: eeb47acc     	vcmpe.f32	s14, s24
    716c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7170: beb07a4c     	vmovlt.f32	s14, s24
    7174: eeb47ae6     	vcmpe.f32	s14, s13
    7178: eef1fa10     	vmrs	APSR_nzcv, fpscr
    717c: 8eb07a66     	vmovhi.f32	s14, s13
    7180: ee170a27     	vnmls.f32	s0, s14, s15
    7184: ed8e0a51     	vstr	s0, [lr, #324]
    7188: ed901ad5     	vldr	s2, [r0, #852]
    718c: edd01a0a     	vldr	s3, [r0, #40]
    7190: ee511a06     	vnmls.f32	s3, s2, s12
    7194: eef41acc     	vcmpe.f32	s3, s24
    7198: eef1fa10     	vmrs	APSR_nzcv, fpscr
    719c: 5a0000c6     	bpl	0x74bc <play_next+0x888> @ imm = #0x318
    71a0: ee312aa6     	vadd.f32	s4, s3, s13
    71a4: ed1fba57     	vldr	s22, [pc, #-348]        @ 0x7050 <play_next+0x41c>
    71a8: eeb42acc     	vcmpe.f32	s4, s24
    71ac: eef1fa10     	vmrs	APSR_nzcv, fpscr
    71b0: beb02a4c     	vmovlt.f32	s4, s24
    71b4: eeb42acb     	vcmpe.f32	s4, s22
    71b8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    71bc: 8a0000be     	bhi	0x74bc <play_next+0x888> @ imm = #0x2f8
    71c0: eef7cac2     	vcvt.f64.f32	d28, s4
    71c4: eddfdbe3     	vldr	d29, [pc, #908]         @ 0x7558 <play_next+0x924>
    71c8: ee6cebad     	vmul.f64	d30, d28, d29
    71cc: eef72bee     	vcvt.f32.f64	s5, d30
    71d0: eebd3ae2     	vcvt.s32.f32	s6, s5
    71d4: eef8aac3     	vcvt.f32.s32	s21, s6
    71d8: ebffed1b     	bl	0x264c <.plt+0x26c>     @ imm = #-0x4b94
    71dc: e59d1008     	ldr	r1, [sp, #0x8]
    71e0: e3a0c064     	mov	r12, #100
    71e4: e3a02074     	mov	r2, #116
    71e8: e0cb3091     	smull	r3, r11, r1, r0
    71ec: e1a03fc0     	asr	r3, r0, #31
    71f0: e0214592     	mla	r1, r2, r5, r4
    71f4: e063b2cb     	rsb	r11, r3, r11, asr #5
    71f8: e0600b9c     	mls	r0, r12, r11, r0
    71fc: ee030a90     	vmov	s7, r0
    7200: eeb84ae3     	vcvt.f32.s32	s8, s7
    7204: eeb44aea     	vcmpe.f32	s8, s21
    7208: eef1fa10     	vmrs	APSR_nzcv, fpscr
    720c: 43e0c000     	mvnmi	r12, #0
    7210: 53a0c001     	movpl	r12, #1
    7214: e581c140     	str	r12, [r1, #0x140]
    7218: e59636bc     	ldr	r3, [r6, #0x6bc]
    721c: e3530000     	cmp	r3, #0
    7220: da000005     	ble	0x723c <play_next+0x608> @ imm = #0x14
    7224: ed915a42     	vldr	s10, [r1, #264]
    7228: edd14a46     	vldr	s9, [r1, #280]
    722c: eef85ac5     	vcvt.f32.s32	s11, s10
    7230: ee256aa4     	vmul.f32	s12, s11, s9
    7234: eefdbac6     	vcvt.s32.f32	s23, s12
    7238: edc1ba42     	vstr	s23, [r1, #264]
    723c: e3a0e074     	mov	lr, #116
    7240: e3a02000     	mov	r2, #0
    7244: e021459e     	mla	r1, lr, r5, r4
    7248: eddffbc4     	vldr	d31, [pc, #784]         @ 0x7560 <play_next+0x92c>
    724c: e3a00000     	mov	r0, #0
    7250: edd1ca46     	vldr	s25, [r1, #280]
    7254: e5812164     	str	r2, [r1, #0x164]
    7258: e5812168     	str	r2, [r1, #0x168]
    725c: eef71aec     	vcvt.f64.f32	d17, s25
    7260: ee710baf     	vadd.f64	d16, d17, d31
    7264: eef76be0     	vcvt.f32.f64	s13, d16
    7268: eef46ae8     	vcmpe.f32	s13, s17
    726c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7270: 8ef06a68     	vmovhi.f32	s13, s17
    7274: e37c0001     	cmn	r12, #1
    7278: e3a0c074     	mov	r12, #116
    727c: e02b459c     	mla	r11, r12, r5, r4
    7280: 03a02903     	moveq	r2, #49152
    7284: edc16a5b     	vstr	s13, [r1, #364]
    7288: 03442400     	movteq	r2, #0x4400
    728c: edc16a5c     	vstr	s13, [r1, #368]
    7290: 0dd16a42     	vldreq	s13, [r1, #264]
    7294: ee3ccaa8     	vadd.f32	s24, s25, s17
    7298: 1e062a90     	vmovne	s13, r2
    729c: 0ef86ae6     	vcvteq.f32.s32	s13, s13
    72a0: ed81ca54     	vstr	s24, [r1, #336]
    72a4: e58b2128     	str	r2, [r11, #0x128]
    72a8: edcb6a49     	vstr	s13, [r11, #292]
    72ac: edd47a34     	vldr	s15, [r4, #208]
    72b0: eef57a40     	vcmp.f32	s15, #0
    72b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    72b8: 058b0174     	streq	r0, [r11, #0x174]
    72bc: 1a00010f     	bne	0x7700 <play_next+0xacc> @ imm = #0x43c
    72c0: e3a0e074     	mov	lr, #116
    72c4: e5963894     	ldr	r3, [r6, #0x894]
    72c8: e02b459e     	mla	r11, lr, r5, r4
    72cc: e3a0c001     	mov	r12, #1
    72d0: e3a00008     	mov	r0, #8
    72d4: e2861e6b     	add	r1, r6, #1712
    72d8: e0812000     	add	r2, r1, r0
    72dc: e58bc15c     	str	r12, [r11, #0x15c]
    72e0: e58b0160     	str	r0, [r11, #0x160]
    72e4: ed9b3a42     	vldr	s6, [r11, #264]
    72e8: edd4aa35     	vldr	s21, [r4, #212]
    72ec: eef83ac3     	vcvt.f32.s32	s7, s6
    72f0: ed92ba00     	vldr	s22, [r2]
    72f4: eef82aea     	vcvt.f32.s32	s5, s21
    72f8: ee824aa3     	vdiv.f32	s8, s5, s7
    72fc: ee644a0b     	vmul.f32	s9, s8, s22
    7300: edcb4a52     	vstr	s9, [r11, #328]
    7304: ed935a1e     	vldr	s10, [r3, #120]
    7308: eefd5ac5     	vcvt.s32.f32	s11, s10
    730c: ee15ea90     	vmov	lr, s11
    7310: e35e0004     	cmp	lr, #4
    7314: d1a0ec1e     	lslle	lr, lr, r12
    7318: d08ee00c     	addle	lr, lr, r12
    731c: e3a0c074     	mov	r12, #116
    7320: e02b459c     	mla	r11, r12, r5, r4
    7324: c3a0e00b     	movgt	lr, #11
    7328: e58be13c     	str	lr, [r11, #0x13c]
    732c: e59400ac     	ldr	r0, [r4, #0xac]
    7330: e3500000     	cmp	r0, #0
    7334: da000006     	ble	0x7354 <play_next+0x720> @ imm = #0x18
    7338: ed946a33     	vldr	s12, [r4, #204]
    733c: eeb56ac0     	vcmpe.f32	s12, #0
    7340: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7344: da000002     	ble	0x7354 <play_next+0x720> @ imm = #0x8
    7348: e5941030     	ldr	r1, [r4, #0x30]
    734c: e3510000     	cmp	r1, #0
    7350: 1a000135     	bne	0x782c <play_next+0xbf8> @ imm = #0x4d4
    7354: e3a03074     	mov	r3, #116
    7358: e3a02000     	mov	r2, #0
    735c: e02e4593     	mla	lr, r3, r5, r4
    7360: eddf0a84     	vldr	s1, [pc, #528]          @ 0x7578 <play_next+0x944>
    7364: e1a00004     	mov	r0, r4
    7368: ed9f0a83     	vldr	s0, [pc, #524]          @ 0x757c <play_next+0x948>
    736c: e58e2114     	str	r2, [lr, #0x114]
    7370: edd7ba02     	vldr	s23, [r7, #8]
    7374: eeb81aeb     	vcvt.f32.s32	s2, s23
    7378: ebffec53     	bl	0x24cc <.plt+0xec>      @ imm = #-0x4eb4
    737c: e594c030     	ldr	r12, [r4, #0x30]
    7380: e35c0000     	cmp	r12, #0
    7384: da00000d     	ble	0x73c0 <play_next+0x78c> @ imm = #0x34
    7388: e5961894     	ldr	r1, [r6, #0x894]
    738c: edd1ca75     	vldr	s25, [r1, #468]
    7390: eefd6aec     	vcvt.s32.f32	s13, s25
    7394: ee163a90     	vmov	r3, s13
    7398: e3530000     	cmp	r3, #0
    739c: 1d9f1a76     	vldrne	s2, [pc, #472]          @ 0x757c <play_next+0x948>
    73a0: 01a00004     	moveq	r0, r4
    73a4: 11a00004     	movne	r0, r4
    73a8: 0eb01a68     	vmoveq.f32	s2, s17
    73ac: 0ddf0a73     	vldreq	s1, [pc, #460]          @ 0x7580 <play_next+0x94c>
    73b0: 0d9f0a71     	vldreq	s0, [pc, #452]          @ 0x757c <play_next+0x948>
    73b4: 1ddf0a71     	vldrne	s1, [pc, #452]          @ 0x7580 <play_next+0x94c>
    73b8: 1eb00a41     	vmovne.f32	s0, s2
    73bc: ebffec42     	bl	0x24cc <.plt+0xec>      @ imm = #-0x4ef8
    73c0: eeb88ac8     	vcvt.f32.s32	s16, s16
    73c4: eefdaac8     	vcvt.s32.f32	s21, s16
    73c8: ee1aca90     	vmov	r12, s21
    73cc: e35c0051     	cmp	r12, #81
    73d0: 8a000006     	bhi	0x73f0 <play_next+0x7bc> @ imm = #0x18
    73d4: e3a01074     	mov	r1, #116
    73d8: e0234c91     	mla	r3, r1, r12, r4
    73dc: e593212c     	ldr	r2, [r3, #0x12c]
    73e0: e3520000     	cmp	r2, #0
    73e4: cdc38a55     	vstrgt	s17, [r3, #340]
    73e8: c3a02001     	movgt	r2, #1
    73ec: c5832158     	strgt	r2, [r3, #0x158]
    73f0: e59d0004     	ldr	r0, [sp, #0x4]
    73f4: e2899001     	add	r9, r9, #1
    73f8: e1590000     	cmp	r9, r0
    73fc: 0a00009c     	beq	0x7674 <play_next+0xa40> @ imm = #0x270
    7400: e1d41af0     	ldrsh	r1, [r4, #160]
    7404: eafffe3f     	b	0x6d08 <play_next+0xd4> @ imm = #-0x704
    7408: ebffec8f     	bl	0x264c <.plt+0x26c>     @ imm = #-0x4dc4
    740c: e3043dd3     	movw	r3, #0x4dd3
    7410: e3413062     	movt	r3, #0x1062
    7414: edd45a32     	vldr	s11, [r4, #200]
    7418: e3a0effa     	mov	lr, #1000
    741c: eddf1b51     	vldr	d17, [pc, #324]         @ 0x7568 <play_next+0x934>
    7420: eef70ae5     	vcvt.f64.f32	d16, s11
    7424: edd46a31     	vldr	s13, [r4, #196]
    7428: ed9f6a55     	vldr	s12, [pc, #340]         @ 0x7584 <play_next+0x950>
    742c: ee20bba1     	vmul.f64	d11, d16, d17
    7430: e0cc3093     	smull	r3, r12, r3, r0
    7434: e1a01fc0     	asr	r1, r0, #31
    7438: e061234c     	rsb	r2, r1, r12, asr #6
    743c: e060029e     	mls	r0, lr, r2, r0
    7440: ee0a0a90     	vmov	s21, r0
    7444: eeb8caea     	vcvt.f32.s32	s24, s21
    7448: eef72acc     	vcvt.f64.f32	d18, s24
    744c: ee623b8b     	vmul.f64	d19, d18, d11
    7450: eef7bbe3     	vcvt.f32.f64	s23, d19
    7454: ee76caeb     	vsub.f32	s25, s13, s23
    7458: eef4cac6     	vcmpe.f32	s25, s12
    745c: edcbca47     	vstr	s25, [r11, #284]
    7460: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7464: d3a00000     	movle	r0, #0
    7468: d58b012c     	strle	r0, [r11, #0x12c]
    746c: daffffd3     	ble	0x73c0 <play_next+0x78c> @ imm = #-0xb4
    7470: e5960894     	ldr	r0, [r6, #0x894]
    7474: eafffeaa     	b	0x6f24 <play_next+0x2f0> @ imm = #-0x558
    7478: e087000a     	add	r0, r7, r10
    747c: e080cfa0     	add	r12, r0, r0, lsr #31
    7480: e085e0cc     	add	lr, r5, r12, asr #1
    7484: ee03ea90     	vmov	s7, lr
    7488: eeb84ae3     	vcvt.f32.s32	s8, s7
    748c: eefd4ac4     	vcvt.s32.f32	s9, s8
    7490: ee147a90     	vmov	r7, s9
    7494: e3570051     	cmp	r7, #81
    7498: 8afffe33     	bhi	0x6d6c <play_next+0x138> @ imm = #-0x734
    749c: e3a02074     	mov	r2, #116
    74a0: e0234792     	mla	r3, r2, r7, r4
    74a4: e593112c     	ldr	r1, [r3, #0x12c]
    74a8: e3510000     	cmp	r1, #0
    74ac: c3a01001     	movgt	r1, #1
    74b0: cdc38a55     	vstrgt	s17, [r3, #340]
    74b4: c5831158     	strgt	r1, [r3, #0x158]
    74b8: eafffe2b     	b	0x6d6c <play_next+0x138> @ imm = #-0x754
    74bc: eddfaa31     	vldr	s21, [pc, #196]         @ 0x7588 <play_next+0x954>
    74c0: eaffff44     	b	0x71d8 <play_next+0x5a4> @ imm = #-0x2f0
    74c4: ed9b1a66     	vldr	s2, [r11, #408]
    74c8: e59fe0c0     	ldr	lr, [pc, #0xc0]         @ 0x7590 <play_next+0x95c>
    74cc: eddf1a2e     	vldr	s3, [pc, #184]          @ 0x758c <play_next+0x958>
    74d0: e08fc00e     	add	r12, pc, lr
    74d4: ed9c2a09     	vldr	s4, [r12, #36]
    74d8: ee21ba21     	vmul.f32	s22, s2, s3
    74dc: eeb4ba42     	vcmp.f32	s22, s4
    74e0: eeb7cacb     	vcvt.f64.f32	d12, s22
    74e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    74e8: 1a0000ca     	bne	0x7818 <play_next+0xbe4> @ imm = #0x328
    74ec: e59fb0a0     	ldr	r11, [pc, #0xa0]        @ 0x7594 <play_next+0x960>
    74f0: e08f300b     	add	r3, pc, r11
    74f4: ed83ba09     	vstr	s22, [r3, #36]
    74f8: eeb70aea     	vcvt.f64.f32	d0, s21
    74fc: ebffec79     	bl	0x26e8 <.plt+0x308>     @ imm = #-0x4e1c
    7500: eeb0bb40     	vmov.f64	d11, d0
    7504: eeb00b4c     	vmov.f64	d0, d12
    7508: ebffec76     	bl	0x26e8 <.plt+0x308>     @ imm = #-0x4e28
    750c: eef72bcb     	vcvt.f32.f64	s5, d11
    7510: eeb73bc0     	vcvt.f32.f64	s6, d0
    7514: ee62aa83     	vmul.f32	s21, s5, s6
    7518: eafffeee     	b	0x70d8 <play_next+0x4a4> @ imm = #-0x448
    751c: ebffec4a     	bl	0x264c <.plt+0x26c>     @ imm = #-0x4ed8
    7520: ed9f1b12     	vldr	d1, [pc, #72]           @ 0x7570 <play_next+0x93c>
    7524: ee050a90     	vmov	s11, r0
    7528: eeb80be5     	vcvt.f64.s32	d0, s11
    752c: ebffec0d     	bl	0x2568 <.plt+0x188>     @ imm = #-0x4fcc
    7530: eef26a0e     	vmov.f32	s13, #1.500000e+01
    7534: eeb76bc0     	vcvt.f32.f64	s12, d0
    7538: eeb46ae6     	vcmpe.f32	s12, s13
    753c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7540: 8a000098     	bhi	0x77a8 <play_next+0xb74> @ imm = #0x260
    7544: e286ee6a     	add	lr, r6, #1696
    7548: eddeca02     	vldr	s25, [lr, #8]
    754c: ee6aaaac     	vmul.f32	s21, s21, s25
    7550: eafffee5     	b	0x70ec <play_next+0x4b8> @ imm = #-0x46c
    7554: e320f000     	nop
    7558: 19 90 01 19  	.word	0x19019019
    755c: 90 01 99 3f  	.word	0x3f990190
    7560: 9a 99 99 99  	.word	0x9999999a
    7564: 99 99 b9 3f  	.word	0x3fb99999
    7568: fc a9 f1 d2  	.word	0xd2f1a9fc
    756c: 4d 62 50 3f  	.word	0x3f50624d
    7570: 00 00 00 00  	.word	0x00000000
    7574: 00 00 59 40  	.word	0x40590000
    7578: 00 00 c6 42  	.word	0x42c60000
    757c: 00 00 00 00  	.word	0x00000000
    7580: 00 00 ea 42  	.word	0x42ea0000
    7584: cd cc cc 3d  	.word	0x3dcccccd
    7588: 00 00 c8 42  	.word	0x42c80000
    758c: ab aa aa 3d  	.word	0x3daaaaab
    7590: c4 6c 01 00  	.word	0x00016cc4
    7594: a4 6c 01 00  	.word	0x00016ca4
    7598: ebffec2b     	bl	0x264c <.plt+0x26c>     @ imm = #-0x4f54
    759c: e59d100c     	ldr	r1, [sp, #0xc]
    75a0: e3a02d35     	mov	r2, #3392
    75a4: e3402003     	movt	r2, #0x3
    75a8: eddfebca     	vldr	d30, [pc, #808]         @ 0x78d8 <play_next+0xca4>
    75ac: ee387acc     	vsub.f32	s14, s17, s24
    75b0: eddffbca     	vldr	d31, [pc, #808]         @ 0x78e0 <play_next+0xcac>
    75b4: eef71acc     	vcvt.f64.f32	d17, s24
    75b8: e0cb3091     	smull	r3, r11, r1, r0
    75bc: e1a03fc0     	asr	r3, r0, #31
    75c0: e063c74b     	rsb	r12, r3, r11, asr #14
    75c4: e0600c92     	mls	r0, r2, r12, r0
    75c8: e2401b61     	sub	r1, r0, #99328
    75cc: e241be2a     	sub	r11, r1, #672
    75d0: ee07ba90     	vmov	s15, r11
    75d4: eeb80ae7     	vcvt.f32.s32	s0, s15
    75d8: eef70ac0     	vcvt.f64.f32	d16, s0
    75dc: ee602bae     	vmul.f64	d18, d16, d30
    75e0: eeb50ac0     	vcmpe.f32	s0, #0
    75e4: eef70be2     	vcvt.f32.f64	s1, d18
    75e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    75ec: ee2c1a20     	vmul.f32	s2, s24, s1
    75f0: ee671a01     	vmul.f32	s3, s14, s2
    75f4: beb02a69     	vmovlt.f32	s4, s19
    75f8: aeb02a68     	vmovge.f32	s4, s17
    75fc: eef41bef     	vcmpe.f64	d17, d31
    7600: ee612a82     	vmul.f32	s5, s3, s4
    7604: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7608: da000006     	ble	0x7628 <play_next+0x9f4> @ imm = #0x18
    760c: ee713bef     	vsub.f64	d19, d17, d31
    7610: eef74b08     	vmov.f64	d20, #1.500000e+00
    7614: ee635ba4     	vmul.f64	d21, d19, d20
    7618: eeb7bbe5     	vcvt.f32.f64	s22, d21
    761c: eeb5bac0     	vcmpe.f32	s22, #0
    7620: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7624: aa0000a6     	bge	0x78c4 <play_next+0xc90> @ imm = #0x298
    7628: eeb07a68     	vmov.f32	s14, s17
    762c: ee017a22     	vmla.f32	s14, s2, s5
    7630: ee6aaa87     	vmul.f32	s21, s21, s14
    7634: eafffeb1     	b	0x7100 <play_next+0x4cc> @ imm = #-0x53c
    7638: e286be85     	add	r11, r6, #2128
    763c: e35c0000     	cmp	r12, #0
    7640: edcb8a02     	vstr	s17, [r11, #8]
    7644: 0a000040     	beq	0x774c <play_next+0xb18> @ imm = #0x100
    7648: ebffebff     	bl	0x264c <.plt+0x26c>     @ imm = #-0x5004
    764c: e286ce85     	add	r12, r6, #2128
    7650: eef60a00     	vmov.f32	s1, #5.000000e-01
    7654: ed9c1a02     	vldr	s2, [r12, #8]
    7658: e3500000     	cmp	r0, #0
    765c: e2001001     	and	r1, r0, #1
    7660: b2611000     	rsblt	r1, r1, #0
    7664: ee011a90     	vmov	s3, r1
    7668: eef8bae1     	vcvt.f32.s32	s23, s3
    766c: ee41ba60     	vmls.f32	s23, s2, s1
    7670: eafffe77     	b	0x7054 <play_next+0x420> @ imm = #-0x624
    7674: e58656e0     	str	r5, [r6, #0x6e0]
    7678: e28dd014     	add	sp, sp, #20
    767c: ecbd8b0a     	vpop	{d8, d9, d10, d11, d12}
    7680: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    7684: e5963850     	ldr	r3, [r6, #0x850]
    7688: e3530000     	cmp	r3, #0
    768c: da000077     	ble	0x7870 <play_next+0xc3c> @ imm = #0x1dc
    7690: e59f0284     	ldr	r0, [pc, #0x284]        @ 0x791c <play_next+0xce8>
    7694: e08fe000     	add	lr, pc, r0
    7698: e59e1028     	ldr	r1, [lr, #0x28]
    769c: e2811001     	add	r1, r1, #1
    76a0: e15b0001     	cmp	r11, r1
    76a4: d3a01000     	movle	r1, #0
    76a8: e58e1028     	str	r1, [lr, #0x28]
    76ac: e3a0b00c     	mov	r11, #12
    76b0: eddf6b8c     	vldr	d22, [pc, #560]         @ 0x78e8 <play_next+0xcb4>
    76b4: e02b419b     	mla	r11, r11, r1, r4
    76b8: e28b3d9d     	add	r3, r11, #10048
    76bc: edd33a04     	vldr	s7, [r3, #16]
    76c0: eef77ae3     	vcvt.f64.f32	d23, s7
    76c4: ee270ba6     	vmul.f64	d0, d23, d22
    76c8: ebffebe8     	bl	0x2670 <.plt+0x290>     @ imm = #-0x5060
    76cc: eddf8b87     	vldr	d24, [pc, #540]         @ 0x78f0 <play_next+0xcbc>
    76d0: e28b2d9d     	add	r2, r11, #10048
    76d4: eddf9b87     	vldr	d25, [pc, #540]         @ 0x78f8 <play_next+0xcc4>
    76d8: ed924a05     	vldr	s8, [r2, #20]
    76dc: eddf4a8b     	vldr	s9, [pc, #556]          @ 0x7910 <play_next+0xcdc>
    76e0: ee247a24     	vmul.f32	s14, s8, s9
    76e4: ee200b28     	vmul.f64	d0, d0, d24
    76e8: eeb75bc0     	vcvt.f32.f64	s10, d0
    76ec: eef7aac5     	vcvt.f64.f32	d26, s10
    76f0: ee6abba9     	vmul.f64	d27, d26, d25
    76f4: eef75beb     	vcvt.f32.f64	s11, d27
    76f8: ee6aaaa5     	vmul.f32	s21, s21, s11
    76fc: eafffe8a     	b	0x712c <play_next+0x4f8> @ imm = #-0x5d8
    7700: ebffebd1     	bl	0x264c <.plt+0x26c>     @ imm = #-0x50bc
    7704: ee070a10     	vmov	s14, r0
    7708: eeba7ae0     	vcvt.f32.s32	s14, s14, #31
    770c: ed8b7a5d     	vstr	s14, [r11, #372]
    7710: e59430a8     	ldr	r3, [r4, #0xa8]
    7714: e3530000     	cmp	r3, #0
    7718: 0afffee8     	beq	0x72c0 <play_next+0x68c> @ imm = #-0x460
    771c: ed9b0a48     	vldr	s0, [r11, #288]
    7720: eef72ac7     	vcvt.f64.f32	d18, s14
    7724: eddf3b75     	vldr	d19, [pc, #468]         @ 0x7900 <play_next+0xccc>
    7728: eddb0a5b     	vldr	s1, [r11, #364]
    772c: eef74ac0     	vcvt.f64.f32	d20, s0
    7730: ee424ba3     	vmla.f64	d20, d18, d19
    7734: ee381ac7     	vsub.f32	s2, s17, s14
    7738: ee601a81     	vmul.f32	s3, s1, s2
    773c: eeb72be4     	vcvt.f32.f64	s4, d20
    7740: edcb1a5b     	vstr	s3, [r11, #364]
    7744: ed8b2a48     	vstr	s4, [r11, #288]
    7748: eafffedc     	b	0x72c0 <play_next+0x68c> @ imm = #-0x490
    774c: ebffebbe     	bl	0x264c <.plt+0x26c>     @ imm = #-0x5108
    7750: e286be85     	add	r11, r6, #2128
    7754: e28bb008     	add	r11, r11, #8
    7758: eddb2a00     	vldr	s5, [r11]
    775c: eef52ac0     	vcmpe.f32	s5, #0
    7760: e3500000     	cmp	r0, #0
    7764: e2002001     	and	r2, r0, #1
    7768: b2622000     	rsblt	r2, r2, #0
    776c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7770: ee032a10     	vmov	s6, r2
    7774: eeb8bac3     	vcvt.f32.s32	s22, s6
    7778: 4a000043     	bmi	0x788c <play_next+0xc58> @ imm = #0x10c
    777c: eef63a00     	vmov.f32	s7, #5.000000e-01
    7780: ee224aa3     	vmul.f32	s8, s5, s7
    7784: e3a0e074     	mov	lr, #116
    7788: e02c459e     	mla	r12, lr, r5, r4
    778c: eef6aa00     	vmov.f32	s21, #5.000000e-01
    7790: eeb4baea     	vcmpe.f32	s22, s21
    7794: eef1fa10     	vmrs	APSR_nzcv, fpscr
    7798: ce3bba44     	vsubgt.f32	s22, s22, s8
    779c: de3bba04     	vaddle.f32	s22, s22, s8
    77a0: ed8cba4c     	vstr	s22, [r12, #304]
    77a4: eafffe2f     	b	0x7068 <play_next+0x434> @ imm = #-0x744
    77a8: eddfba59     	vldr	s23, [pc, #356]         @ 0x7914 <play_next+0xce0>
    77ac: e2860e6a     	add	r0, r6, #1696
    77b0: eeb46aeb     	vcmpe.f32	s12, s23
    77b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    77b8: 9dd0ba03     	vldrls	s23, [r0, #12]
    77bc: 8dd0ba01     	vldrhi	s23, [r0, #4]
    77c0: ee6aaaab     	vmul.f32	s21, s21, s23
    77c4: eafffe48     	b	0x70ec <play_next+0x4b8> @ imm = #-0x6e0
    77c8: ebffeb9f     	bl	0x264c <.plt+0x26c>     @ imm = #-0x5184
    77cc: eef65a00     	vmov.f32	s11, #5.000000e-01
    77d0: e59d1008     	ldr	r1, [sp, #0x8]
    77d4: eddf4b4b     	vldr	d20, [pc, #300]         @ 0x7908 <play_next+0xcd4>
    77d8: ee7b6a25     	vadd.f32	s13, s22, s11
    77dc: ee366aa6     	vadd.f32	s12, s13, s13
    77e0: ee76aa28     	vadd.f32	s21, s12, s17
    77e4: e0cb2091     	smull	r2, r11, r1, r0
    77e8: e1a0cfc0     	asr	r12, r0, #31
    77ec: e3a02064     	mov	r2, #100
    77f0: e06c32cb     	rsb	r3, r12, r11, asr #5
    77f4: eef75aea     	vcvt.f64.f32	d21, s21
    77f8: e0600392     	mls	r0, r2, r3, r0
    77fc: ee656ba4     	vmul.f64	d22, d21, d20
    7800: ee0c0a10     	vmov	s24, r0
    7804: eef77b00     	vmov.f64	d23, #1.000000e+00
    7808: eef88bcc     	vcvt.f64.s32	d24, s24
    780c: ee467be8     	vmls.f64	d23, d22, d24
    7810: eef7bbe7     	vcvt.f32.f64	s23, d23
    7814: eafffe0e     	b	0x7054 <play_next+0x420> @ imm = #-0x7c8
    7818: e59f1100     	ldr	r1, [pc, #0x100]        @ 0x7920 <play_next+0xcec>
    781c: ec532b1c     	vmov	r2, r3, d12
    7820: e08f0001     	add	r0, pc, r1
    7824: ebffeb7c     	bl	0x261c <.plt+0x23c>     @ imm = #-0x5210
    7828: eaffff2f     	b	0x74ec <play_next+0x8b8> @ imm = #-0x344
    782c: ebffeb86     	bl	0x264c <.plt+0x26c>     @ imm = #-0x51e8
    7830: e596262c     	ldr	r2, [r6, #0x62c]
    7834: ed94ca2c     	vldr	s24, [r4, #176]
    7838: e59b1108     	ldr	r1, [r11, #0x108]
    783c: ee072a90     	vmov	s15, r2
    7840: eeb87ae7     	vcvt.f32.s32	s14, s15
    7844: ee8a0a07     	vdiv.f32	s0, s20, s14
    7848: ee20ba0c     	vmul.f32	s22, s0, s24
    784c: eb000d4a     	bl	0xad7c <__aeabi_idivmod> @ imm = #0x3528
    7850: e3a00001     	mov	r0, #1
    7854: e58b0114     	str	r0, [r11, #0x114]
    7858: ee001a90     	vmov	s1, r1
    785c: eeb81ae0     	vcvt.f32.s32	s2, s1
    7860: ee6b1a01     	vmul.f32	s3, s22, s2
    7864: eebd2ae1     	vcvt.s32.f32	s4, s3
    7868: ed8b2a44     	vstr	s4, [r11, #272]
    786c: eafffed3     	b	0x73c0 <play_next+0x78c> @ imm = #-0x4b4
    7870: ebffeb75     	bl	0x264c <.plt+0x26c>     @ imm = #-0x522c
    7874: e1a0100b     	mov	r1, r11
    7878: eb000d3f     	bl	0xad7c <__aeabi_idivmod> @ imm = #0x34fc
    787c: e59f20a0     	ldr	r2, [pc, #0xa0]         @ 0x7924 <play_next+0xcf0>
    7880: e08fc002     	add	r12, pc, r2
    7884: e58c1028     	str	r1, [r12, #0x28]
    7888: eaffff87     	b	0x76ac <play_next+0xa78> @ imm = #-0x1e4
    788c: ebffeb6e     	bl	0x264c <.plt+0x26c>     @ imm = #-0x5248
    7890: e59dc008     	ldr	r12, [sp, #0x8]
    7894: eddb4a00     	vldr	s9, [r11]
    7898: e3a0b064     	mov	r11, #100
    789c: ed9f5a1d     	vldr	s10, [pc, #116]         @ 0x7918 <play_next+0xce4>
    78a0: ee645a85     	vmul.f32	s11, s9, s10
    78a4: e0c1309c     	smull	r3, r1, r12, r0
    78a8: e1a03fc0     	asr	r3, r0, #31
    78ac: e06322c1     	rsb	r2, r3, r1, asr #5
    78b0: e060029b     	mls	r0, r11, r2, r0
    78b4: ee060a90     	vmov	s13, r0
    78b8: eeb86ae6     	vcvt.f32.s32	s12, s13
    78bc: ee264a25     	vmul.f32	s8, s12, s11
    78c0: eaffffaf     	b	0x7784 <play_next+0xb50> @ imm = #-0x144
    78c4: eeb4bae8     	vcmpe.f32	s22, s17
    78c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    78cc: 8eb0ba68     	vmovhi.f32	s22, s17
    78d0: ee722a8b     	vadd.f32	s5, s5, s22
    78d4: eaffff53     	b	0x7628 <play_next+0x9f4> @ imm = #-0x2b4
    78d8: f1 68 e3 88  	.word	0x88e368f1
    78dc: b5 f8 e4 3e  	.word	0x3ee4f8b5
    78e0: 9a 99 99 99  	.word	0x9999999a
    78e4: 99 99 c9 3f  	.word	0x3fc99999
    78e8: 7b 5b 3c fe  	.word	0xfe3c5b7b
    78ec: 03 93 ad 3f  	.word	0x3fad9303
    78f0: 2f b1 c2 50  	.word	0x50c2b12f
    78f4: 02 5a 20 40  	.word	0x40205a02
    78f8: 5b 9d c7 6c  	.word	0x6cc79d5b
    78fc: da 4f 6f 3f  	.word	0x3f6f4fda
    7900: cd cc cc cc  	.word	0xcccccccd
    7904: cc cc fc 3f  	.word	0x3ffccccc
    7908: 7b 14 ae 47  	.word	0x47ae147b
    790c: e1 7a 84 3f  	.word	0x3f847ae1
    7910: 04 02 01 3c  	.word	0x3c010204
    7914: 00 00 70 42  	.word	0x42700000
    7918: 0a d7 a3 bb  	.word	0xbba3d70a
    791c: 00 6b 01 00  	.word	0x00016b00
    7920: 04 55 00 00  	.word	0x00005504
    7924: 14 69 01 00  	.word	0x00016914

