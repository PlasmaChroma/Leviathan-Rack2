; lubadh::AudioEngine::processChannel(lubadh::Channel&, int, std::vector<float, std::allocator<float> >&, std::vector<float, std::allocator<float> >&)
; VA 0x47c08 size 5692

   47c08: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   47c0c: ee072a90     	vmov	s15, r2
   47c10: e1a0a003     	mov	r10, r3
   47c14: ed2d8b0c     	vpush	{d8, d9, d10, d11, d12, d13}
   47c18: e24dd094     	sub	sp, sp, #148
   47c1c: e5d13284     	ldrb	r3, [r1, #0x284]
   47c20: eeb88ae7     	vcvt.f32.s32	s16, s15
   47c24: e1a04001     	mov	r4, r1
   47c28: e59d60e8     	ldr	r6, [sp, #0xe8]
   47c2c: e3530000     	cmp	r3, #0
   47c30: e58d202c     	str	r2, [sp, #0x2c]
   47c34: e58d003c     	str	r0, [sp, #0x3c]
   47c38: 1a00045e     	bne	0x48db8
   47c3c: e59430e8     	ldr	r3, [r4, #0xe8]
   47c40: e5932034     	ldr	r2, [r3, #0x34]
   47c44: e5931018     	ldr	r1, [r3, #0x18]
   47c48: e593002c     	ldr	r0, [r3, #0x2c]
   47c4c: e58d1028     	str	r1, [sp, #0x28]
   47c50: e1510000     	cmp	r1, r0
   47c54: e5927000     	ldr	r7, [r2]
   47c58: e58d0048     	str	r0, [sp, #0x48]
   47c5c: e1a02001     	mov	r2, r1
   47c60: e5930028     	ldr	r0, [r3, #0x28]
   47c64: e5931024     	ldr	r1, [r3, #0x24]
   47c68: e58d1040     	str	r1, [sp, #0x40]
   47c6c: e5931014     	ldr	r1, [r3, #0x14]
   47c70: e58d100c     	str	r1, [sp, #0xc]
   47c74: e593101c     	ldr	r1, [r3, #0x1c]
   47c78: e58d101c     	str	r1, [sp, #0x1c]
   47c7c: e5931020     	ldr	r1, [r3, #0x20]
   47c80: e58d1014     	str	r1, [sp, #0x14]
   47c84: a1a01002     	movge	r1, r2
   47c88: a712f012     	sdivge	r2, r2, r0
   47c8c: edd37a04     	vldr	s15, [r3, #16]
   47c90: ed939a03     	vldr	s18, [r3, #12]
   47c94: b59d2028     	ldrlt	r2, [sp, #0x28]
   47c98: e58d0038     	str	r0, [sp, #0x38]
   47c9c: a0621290     	mlsge	r2, r0, r2, r1
   47ca0: e59d102c     	ldr	r1, [sp, #0x2c]
   47ca4: e58d2024     	str	r2, [sp, #0x24]
   47ca8: e1a00004     	mov	r0, r4
   47cac: e5932064     	ldr	r2, [r3, #0x64]
   47cb0: ee299a27     	vmul.f32	s18, s18, s15
   47cb4: ed93ca17     	vldr	s24, [r3, #92]
   47cb8: e58d2018     	str	r2, [sp, #0x18]
   47cbc: ebffbdd1     	bl	0x37408
   47cc0: e59430e8     	ldr	r3, [r4, #0xe8]
   47cc4: eef77a00     	vmov.f32	s15, #1.000000e+00
   47cc8: edd39a00     	vldr	s19, [r3]
   47ccc: ed93aa01     	vldr	s20, [r3, #4]
   47cd0: edd38a03     	vldr	s17, [r3, #12]
   47cd4: eef0aae9     	vabs.f32	s21, s19
   47cd8: ed937a04     	vldr	s14, [r3, #16]
   47cdc: eeb0baca     	vabs.f32	s22, s20
   47ce0: ee688a87     	vmul.f32	s17, s17, s14
   47ce4: eef4aae7     	vcmpe.f32	s21, s15
   47ce8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   47cec: 5ddfbac8     	vldrpl	s23, [pc, #800]         @ 0x48014>&, std::vector<float, std::allocator<float>>&)+0x40c> ; float 20000
   47cf0: 5a000005     	bpl	0x47d0c
   47cf4: eddf7ac7     	vldr	s15, [pc, #796]         @ 0x48018>&, std::vector<float, std::allocator<float>>&)+0x410> ; float 0.0010000000475
   47cf8: eef4aae7     	vcmpe.f32	s21, s15
   47cfc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   47d00: cddfbac3     	vldrgt	s23, [pc, #780]         @ 0x48014>&, std::vector<float, std::allocator<float>>&)+0x40c> ; float 20000
   47d04: ce6abaab     	vmulgt.f32	s23, s21, s23
   47d08: def3ba04     	vmovle.f32	s23, #2.000000e+01
   47d0c: e59330a0     	ldr	r3, [r3, #0xa0]
   47d10: e284bd66     	add	r11, r4, #6528
   47d14: e28bb010     	add	r11, r11, #16
   47d18: e3530000     	cmp	r3, #0
   47d1c: e1a0000b     	mov	r0, r11
   47d20: 0eb00a6b     	vmoveq.f32	s0, s23
   47d24: 1d9f0aba     	vldrne	s0, [pc, #744]          @ 0x48014>&, std::vector<float, std::allocator<float>>&)+0x40c> ; float 20000
   47d28: eb000d24     	bl	0x4b1c0
   47d2c: e2843d66     	add	r3, r4, #6528
   47d30: e2833028     	add	r3, r3, #40
   47d34: eeb00a6b     	vmov.f32	s0, s23
   47d38: e1a00003     	mov	r0, r3
   47d3c: e58d3054     	str	r3, [sp, #0x54]
   47d40: eb000d1e     	bl	0x4b1c0
   47d44: e1a01006     	mov	r1, r6
   47d48: e2840d67     	add	r0, r4, #6592
   47d4c: eb001dc8     	bl	0x4f474
   47d50: e2840d67     	add	r0, r4, #6592
   47d54: e1a01006     	mov	r1, r6
   47d58: e2800038     	add	r0, r0, #56
   47d5c: eb001df0     	bl	0x4f524
   47d60: e2840d69     	add	r0, r4, #6720
   47d64: e1a01006     	mov	r1, r6
   47d68: e280001c     	add	r0, r0, #28
   47d6c: eb001fbb     	bl	0x4fc60
   47d70: e2840d69     	add	r0, r4, #6720
   47d74: e1a01006     	mov	r1, r6
   47d78: e2800020     	add	r0, r0, #32
   47d7c: eb001fd9     	bl	0x4fce8
   47d80: e5d4327c     	ldrb	r3, [r4, #0x27c]
   47d84: e3530000     	cmp	r3, #0
   47d88: 1a0001f4     	bne	0x48560
   47d8c: e5d4327d     	ldrb	r3, [r4, #0x27d]
   47d90: e3530000     	cmp	r3, #0
   47d94: 1a000401     	bne	0x48da0
   47d98: e5d432e4     	ldrb	r3, [r4, #0x2e4]
   47d9c: e3530000     	cmp	r3, #0
   47da0: e2843ba7     	add	r3, r4, #171008
   47da4: e58d3020     	str	r3, [sp, #0x20]
   47da8: 1a000390     	bne	0x48bf0
   47dac: e28d9060     	add	r9, sp, #96
   47db0: e28d7078     	add	r7, sp, #120
   47db4: e28d8080     	add	r8, sp, #128
   47db8: e28d3068     	add	r3, sp, #104
   47dbc: e58d3008     	str	r3, [sp, #0x8]
   47dc0: e59d3020     	ldr	r3, [sp, #0x20]
   47dc4: e2845a2a     	add	r5, r4, #172032
   47dc8: e1a0000b     	mov	r0, r11
   47dcc: e1a01006     	mov	r1, r6
   47dd0: e2833ff9     	add	r3, r3, #996
   47dd4: e58d3058     	str	r3, [sp, #0x58]
   47dd8: e1a0b003     	mov	r11, r3
   47ddc: eb000d03     	bl	0x4b1f0
   47de0: e1a0000b     	mov	r0, r11
   47de4: e1a01006     	mov	r1, r6
   47de8: e285300c     	add	r3, r5, #12
   47dec: e58d305c     	str	r3, [sp, #0x5c]
   47df0: ebffc320     	bl	0x38a78
   47df4: e595000c     	ldr	r0, [r5, #0xc]
   47df8: e5952010     	ldr	r2, [r5, #0x10]
   47dfc: e1500002     	cmp	r0, r2
   47e00: 0a000002     	beq	0x47e10
   47e04: e0422000     	sub	r2, r2, r0
   47e08: e3a01000     	mov	r1, #0
   47e0c: ebff37d8     	bl	0x15d74    @ imm = #-0x320a0 ; memset
   47e10: e2843ee6     	add	r3, r4, #3680
   47e14: e2842fda     	add	r2, r4, #872
   47e18: e2833008     	add	r3, r3, #8
   47e1c: e58d2020     	str	r2, [sp, #0x20]
   47e20: e58d3030     	str	r3, [sp, #0x30]
   47e24: e1cd27f8     	strd	r2, r3, [sp, #120]
   47e28: e4970004     	ldr	r0, [r7], #4
   47e2c: eb0016d4     	bl	0x4d984
   47e30: e2406004     	sub	r6, r0, #4
   47e34: e280b04c     	add	r11, r0, #76
   47e38: e5b60004     	ldr	r0, [r6, #0x4]!
   47e3c: e3500000     	cmp	r0, #0
   47e40: 0a000003     	beq	0x47e54
   47e44: eeb01a48     	vmov.f32	s2, s16
   47e48: eef00a68     	vmov.f32	s1, s17
   47e4c: eeb00a69     	vmov.f32	s0, s19
   47e50: eb000fe2     	bl	0x4bde0
   47e54: e15b0006     	cmp	r11, r6
   47e58: 1afffff6     	bne	0x47e38
   47e5c: e1580007     	cmp	r8, r7
   47e60: 1afffff0     	bne	0x47e28
   47e64: e59d0020     	ldr	r0, [sp, #0x20]
   47e68: eb001725     	bl	0x4db04
   47e6c: e3500000     	cmp	r0, #0
   47e70: 1a000382     	bne	0x48c80
   47e74: e59d303c     	ldr	r3, [sp, #0x3c]
   47e78: e5930084     	ldr	r0, [r3, #0x84]
   47e7c: e1a01004     	mov	r1, r4
   47e80: ebff882f     	bl	0x29f44
   47e84: e5d4307c     	ldrb	r3, [r4, #0x7c]
   47e88: e3530000     	cmp	r3, #0
   47e8c: 1a0003aa     	bne	0x48d3c
   47e90: e59d3020     	ldr	r3, [sp, #0x20]
   47e94: e58d3060     	str	r3, [sp, #0x60]
   47e98: e59d3030     	ldr	r3, [sp, #0x30]
   47e9c: e58d904c     	str	r9, [sp, #0x4c]
   47ea0: e58d3064     	str	r3, [sp, #0x64]
   47ea4: e59d304c     	ldr	r3, [sp, #0x4c]
   47ea8: e493b004     	ldr	r11, [r3], #4
   47eac: e58d304c     	str	r3, [sp, #0x4c]
   47eb0: e59d3020     	ldr	r3, [sp, #0x20]
   47eb4: e1a0000b     	mov	r0, r11
   47eb8: e153000b     	cmp	r3, r11
   47ebc: 13a03001     	movne	r3, #1
   47ec0: 059430e8     	ldreq	r3, [r4, #0xe8]
   47ec4: 0593308c     	ldreq	r3, [r3, #0x8c]
   47ec8: 016f3f13     	clzeq	r3, r3
   47ecc: 01a032a3     	lsreq	r3, r3, #5
   47ed0: e58d3034     	str	r3, [sp, #0x34]
   47ed4: eb0016aa     	bl	0x4d984
   47ed8: e2402004     	sub	r2, r0, #4
   47edc: e280304c     	add	r3, r0, #76
   47ee0: e58d2010     	str	r2, [sp, #0x10]
   47ee4: e58d3044     	str	r3, [sp, #0x44]
   47ee8: ea00000c     	b	0x47f20
   47eec: e1530002     	cmp	r3, r2
   47ef0: aa0001ce     	bge	0x48630
   47ef4: e1590003     	cmp	r9, r3
   47ef8: a3a03000     	movge	r3, #0
   47efc: b3a03001     	movlt	r3, #1
   47f00: e1590002     	cmp	r9, r2
   47f04: c3833001     	orrgt	r3, r3, #1
   47f08: e3530000     	cmp	r3, #0
   47f0c: 1a000258     	bne	0x48874
   47f10: e59d3044     	ldr	r3, [sp, #0x44]
   47f14: e59d2010     	ldr	r2, [sp, #0x10]
   47f18: e1530002     	cmp	r3, r2
   47f1c: 0a00002c     	beq	0x47fd4
   47f20: e59d3010     	ldr	r3, [sp, #0x10]
   47f24: e5b36004     	ldr	r6, [r3, #0x4]!
   47f28: e58d3010     	str	r3, [sp, #0x10]
   47f2c: e3560000     	cmp	r6, #0
   47f30: 0afffff6     	beq	0x47f10
   47f34: e3a03000     	mov	r3, #0
   47f38: e5cd3080     	strb	r3, [sp, #0x80]
   47f3c: e3a03000     	mov	r3, #0
   47f40: e58d307c     	str	r3, [sp, #0x7c]
   47f44: e5d63014     	ldrb	r3, [r6, #0x14]
   47f48: e5969004     	ldr	r9, [r6, #0x4]
   47f4c: e3530000     	cmp	r3, #0
   47f50: e596700c     	ldr	r7, [r6, #0xc]
   47f54: e59d300c     	ldr	r3, [sp, #0xc]
   47f58: e59d201c     	ldr	r2, [sp, #0x1c]
   47f5c: 0ef07a69     	vmoveq.f32	s15, s19
   47f60: 1dd67a06     	vldrne	s15, [r6, #24]
   47f64: e58d8078     	str	r8, [sp, #0x78]
   47f68: eef57ac0     	vcmpe.f32	s15, #0
   47f6c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   47f70: aaffffdd     	bge	0x47eec
   47f74: e1530002     	cmp	r3, r2
   47f78: aa0001e6     	bge	0x48718
   47f7c: e59d3014     	ldr	r3, [sp, #0x14]
   47f80: e59d2028     	ldr	r2, [sp, #0x28]
   47f84: e1590003     	cmp	r9, r3
   47f88: b3a03000     	movlt	r3, #0
   47f8c: a3a03001     	movge	r3, #1
   47f90: e1590002     	cmp	r9, r2
   47f94: b3833001     	orrlt	r3, r3, #1
   47f98: e3530000     	cmp	r3, #0
   47f9c: 0affffdb     	beq	0x47f10
   47fa0: e3a01001     	mov	r1, #1
   47fa4: e1a00006     	mov	r0, r6
   47fa8: eb000fef     	bl	0x4bf6c
   47fac: e2502000     	subs	r2, r0, #0
   47fb0: 0a0002d6     	beq	0x48b10
   47fb4: e59d0078     	ldr	r0, [sp, #0x78]
   47fb8: e1500008     	cmp	r0, r8
   47fbc: 0affffd3     	beq	0x47f10
   47fc0: ebff379e     	bl	0x15e40    @ imm = #-0x32188 ; _ZdlPv
   47fc4: e59d3044     	ldr	r3, [sp, #0x44]
   47fc8: e59d2010     	ldr	r2, [sp, #0x10]
   47fcc: e1530002     	cmp	r3, r2
   47fd0: 1affffd2     	bne	0x47f20
   47fd4: e59d3008     	ldr	r3, [sp, #0x8]
   47fd8: e59d204c     	ldr	r2, [sp, #0x4c]
   47fdc: e1530002     	cmp	r3, r2
   47fe0: 1affffaf     	bne	0x47ea4
   47fe4: e59d0020     	ldr	r0, [sp, #0x20]
   47fe8: e3027a2d     	movw	r7, #0x2a2d
   47fec: e34071c2     	movt	r7, #0x1c2
   47ff0: eb001663     	bl	0x4d984
   47ff4: eddfca08     	vldr	s25, [pc, #32]          @ 0x4801c>&, std::vector<float, std::allocator<float>>&)+0x414> ; float 0
   47ff8: e2853018     	add	r3, r5, #24
   47ffc: e240b004     	sub	r11, r0, #4
   48000: e58d300c     	str	r3, [sp, #0xc]
   48004: ed9fca05     	vldr	s24, [pc, #20]          @ 0x48020>&, std::vector<float, std::allocator<float>>&)+0x418> ; float 4096
   48008: e280304c     	add	r3, r0, #76
   4800c: e58d3008     	str	r3, [sp, #0x8]
   48010: ea000009     	b	0x4803c
   48014: 00 40 9c 46  	.word	0x469c4000
   48018: 6f 12 83 3a  	.word	0x3a83126f
   4801c: 00 00 00 00  	.word	0x00000000
   48020: 00 00 80 45  	.word	0x45800000
   48024: bd 37 86 35  	.word	0x358637bd
   48028: ad aa 2a 3e  	.word	0x3e2aaaad
   4802c: 85 eb 51 3f  	.word	0x3f51eb85
   48030: e59d3008     	ldr	r3, [sp, #0x8]
   48034: e153000b     	cmp	r3, r11
   48038: 0a0000d9     	beq	0x483a4
   4803c: e5bb6004     	ldr	r6, [r11, #0x4]!
   48040: e3560000     	cmp	r6, #0
   48044: 0afffff9     	beq	0x48030
   48048: e59d100c     	ldr	r1, [sp, #0xc]
   4804c: e1a00006     	mov	r0, r6
   48050: eb000fcc     	bl	0x4bf88
   48054: e596800c     	ldr	r8, [r6, #0xc]
   48058: ee078a90     	vmov	s15, r8
   4805c: edd6ba04     	vldr	s23, [r6, #16]
   48060: edd66a02     	vldr	s13, [r6, #8]
   48064: eeb87ae7     	vcvt.f32.s32	s14, s15
   48068: edd67a01     	vldr	s15, [r6, #4]
   4806c: e5d63014     	ldrb	r3, [r6, #0x14]
   48070: eef87ae7     	vcvt.f32.s32	s15, s15
   48074: e3530000     	cmp	r3, #0
   48078: ee377a2b     	vadd.f32	s14, s14, s23
   4807c: ee777aa6     	vadd.f32	s15, s15, s13
   48080: ee777ac7     	vsub.f32	s15, s15, s14
   48084: ee87da88     	vdiv.f32	s26, s15, s16
   48088: 0a000231     	beq	0x48954
   4808c: edd67a06     	vldr	s15, [r6, #24]
   48090: eef57ac0     	vcmpe.f32	s15, #0
   48094: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48098: a2888002     	addge	r8, r8, #2
   4809c: b2488003     	sublt	r8, r8, #3
   480a0: eef07ae7     	vabs.f32	s15, s15
   480a4: eeb17a00     	vmov.f32	s14, #4.000000e+00
   480a8: eef76a00     	vmov.f32	s13, #1.000000e+00
   480ac: ee277a87     	vmul.f32	s14, s15, s14
   480b0: eeb47ae6     	vcmpe.f32	s14, s13
   480b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   480b8: 5eb07a66     	vmovpl.f32	s14, s13
   480bc: 5a000002     	bpl	0x480cc
   480c0: eeb57ac0     	vcmpe.f32	s14, #0
   480c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   480c8: deb07a6c     	vmovle.f32	s14, s25
   480cc: e5953018     	ldr	r3, [r5, #0x18]
   480d0: e595201c     	ldr	r2, [r5, #0x1c]
   480d4: e1520003     	cmp	r2, r3
   480d8: 0a000004     	beq	0x480f0
   480dc: edd37a00     	vldr	s15, [r3]
   480e0: ee677a87     	vmul.f32	s15, s15, s14
   480e4: ece37a01     	vstmia	r3!, {s15}
   480e8: e1520003     	cmp	r2, r3
   480ec: 1afffffa     	bne	0x480dc
   480f0: e59d0030     	ldr	r0, [sp, #0x30]
   480f4: eb001622     	bl	0x4d984
   480f8: ed5f2a37     	vldr	s5, [pc, #-220]         @ 0x48024>&, std::vector<float, std::allocator<float>>&)+0x41c> ; float 9.99999997475e-07
   480fc: e240e004     	sub	lr, r0, #4
   48100: e280904c     	add	r9, r0, #76
   48104: eef76a00     	vmov.f32	s13, #1.000000e+00
   48108: e5be3004     	ldr	r3, [lr, #0x4]!
   4810c: e3530000     	cmp	r3, #0
   48110: 0a00005f     	beq	0x48294
   48114: e5d62014     	ldrb	r2, [r6, #0x14]
   48118: edd67a04     	vldr	s15, [r6, #16]
   4811c: e3520000     	cmp	r2, #0
   48120: e596200c     	ldr	r2, [r6, #0xc]
   48124: 0a000205     	beq	0x48940
   48128: ed967a06     	vldr	s14, [r6, #24]
   4812c: ee295a07     	vmul.f32	s10, s18, s14
   48130: eeb06a45     	vmov.f32	s12, s10
   48134: e5d31014     	ldrb	r1, [r3, #0x14]
   48138: ee986a87     	vfnms.f32	s12, s17, s14
   4813c: ed937a04     	vldr	s14, [r3, #16]
   48140: e3510000     	cmp	r1, #0
   48144: e593100c     	ldr	r1, [r3, #0xc]
   48148: eec63a08     	vdiv.f32	s7, s12, s16
   4814c: 0a0001f8     	beq	0x48934
   48150: ed936a06     	vldr	s12, [r3, #24]
   48154: ee694a06     	vmul.f32	s9, s18, s12
   48158: eef05a64     	vmov.f32	s11, s9
   4815c: e5950018     	ldr	r0, [r5, #0x18]
   48160: eed85a86     	vfnms.f32	s11, s17, s12
   48164: e595c01c     	ldr	r12, [r5, #0x1c]
   48168: e15c0000     	cmp	r12, r0
   4816c: ee853a88     	vdiv.f32	s6, s11, s16
   48170: 0a000047     	beq	0x48294
   48174: ee356a64     	vsub.f32	s12, s10, s9
   48178: eeb06ac6     	vabs.f32	s12, s12
   4817c: eeb46ae2     	vcmpe.f32	s12, s5
   48180: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48184: da000013     	ble	0x481d8
   48188: ee052a90     	vmov	s11, r2
   4818c: ee041a10     	vmov	s8, r1
   48190: ee266a0c     	vmul.f32	s12, s12, s24
   48194: eef85ae5     	vcvt.f32.s32	s11, s11
   48198: eeb84ac4     	vcvt.f32.s32	s8, s8
   4819c: ee755aa7     	vadd.f32	s11, s11, s15
   481a0: ee344a07     	vadd.f32	s8, s8, s14
   481a4: ee755ac4     	vsub.f32	s11, s11, s8
   481a8: eef05ae5     	vabs.f32	s11, s11
   481ac: ee854a86     	vdiv.f32	s8, s11, s12
   481b0: eeb44ae6     	vcmpe.f32	s8, s13
   481b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   481b8: 5eb74a00     	vmovpl.f32	s8, #1.000000e+00
   481bc: 5a000002     	bpl	0x481cc
   481c0: eeb54ac0     	vcmpe.f32	s8, #0
   481c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   481c8: deb04a6c     	vmovle.f32	s8, s25
   481cc: ed906a00     	vldr	s12, [r0]
   481d0: ee264a04     	vmul.f32	s8, s12, s8
   481d4: ed804a00     	vstr	s8, [r0]
   481d8: ee777a85     	vadd.f32	s15, s15, s10
   481dc: eef47ae6     	vcmpe.f32	s15, s13
   481e0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   481e4: a2823001     	addge	r3, r2, #1
   481e8: ba000005     	blt	0x48204
   481ec: ee777ae6     	vsub.f32	s15, s15, s13
   481f0: e1a02003     	mov	r2, r3
   481f4: e2833001     	add	r3, r3, #1
   481f8: eef47ae6     	vcmpe.f32	s15, s13
   481fc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48200: aafffff9     	bge	0x481ec
   48204: eef57ac0     	vcmpe.f32	s15, #0
   48208: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4820c: 42423001     	submi	r3, r2, #1
   48210: 5a000005     	bpl	0x4822c
   48214: ee777aa6     	vadd.f32	s15, s15, s13
   48218: e1a02003     	mov	r2, r3
   4821c: e2433001     	sub	r3, r3, #1
   48220: eef57ac0     	vcmpe.f32	s15, #0
   48224: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48228: 4afffff9     	bmi	0x48214
   4822c: ee377a24     	vadd.f32	s14, s14, s9
   48230: eeb47ae6     	vcmpe.f32	s14, s13
   48234: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48238: a2813001     	addge	r3, r1, #1
   4823c: ba000005     	blt	0x48258
   48240: ee377a66     	vsub.f32	s14, s14, s13
   48244: e1a01003     	mov	r1, r3
   48248: e2833001     	add	r3, r3, #1
   4824c: eeb47ae6     	vcmpe.f32	s14, s13
   48250: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48254: aafffff9     	bge	0x48240
   48258: eeb57ac0     	vcmpe.f32	s14, #0
   4825c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48260: 42413001     	submi	r3, r1, #1
   48264: 5a000005     	bpl	0x48280
   48268: ee377a26     	vadd.f32	s14, s14, s13
   4826c: e1a01003     	mov	r1, r3
   48270: e2433001     	sub	r3, r3, #1
   48274: eeb57ac0     	vcmpe.f32	s14, #0
   48278: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4827c: 4afffff9     	bmi	0x48268
   48280: e2800004     	add	r0, r0, #4
   48284: ee355a23     	vadd.f32	s10, s10, s7
   48288: ee744a83     	vadd.f32	s9, s9, s6
   4828c: e15c0000     	cmp	r12, r0
   48290: 1affffb7     	bne	0x48174
   48294: e159000e     	cmp	r9, lr
   48298: 1affff9a     	bne	0x48108
   4829c: e595200c     	ldr	r2, [r5, #0xc]
   482a0: e5950010     	ldr	r0, [r5, #0x10]
   482a4: e0400002     	sub	r0, r0, r2
   482a8: e1b03120     	lsrs	r3, r0, #2
   482ac: 0affff5f     	beq	0x48030
   482b0: e2843a29     	add	r3, r4, #167936
   482b4: e5951018     	ldr	r1, [r5, #0x18]
   482b8: ed1f2aa6     	vldr	s4, [pc, #-664]         @ 0x48028>&, std::vector<float, std::allocator<float>>&)+0x420> ; float 0.166666701436
   482bc: eef03a08     	vmov.f32	s7, #3.000000e+00
   482c0: e0800001     	add	r0, r0, r1
   482c4: eef02a00     	vmov.f32	s5, #2.000000e+00
   482c8: e593cfc4     	ldr	r12, [r3, #0xfc4]
   482cc: eef77a00     	vmov.f32	s15, #1.000000e+00
   482d0: e1580007     	cmp	r8, r7
   482d4: ee774aeb     	vsub.f32	s9, s15, s23
   482d8: b1a03008     	movlt	r3, r8
   482dc: a1a03007     	movge	r3, r7
   482e0: e3530001     	cmp	r3, #1
   482e4: ed925a00     	vldr	s10, [r2]
   482e8: b3a03001     	movlt	r3, #1
   482ec: ecb13a01     	vldmia	r1!, {s6}
   482f0: e2433001     	sub	r3, r3, #1
   482f4: ee624a64     	vnmul.f32	s9, s4, s9
   482f8: e08c3103     	add	r3, r12, r3, lsl #2
   482fc: ed936a01     	vldr	s12, [r3, #4]
   48300: ed937a02     	vldr	s14, [r3, #8]
   48304: ed934a00     	vldr	s8, [r3]
   48308: edd36a03     	vldr	s13, [r3, #12]
   4830c: ee377a46     	vsub.f32	s14, s14, s12
   48310: eef05a66     	vmov.f32	s11, s13
   48314: ee766ac4     	vsub.f32	s13, s13, s8
   48318: eee45a22     	vfma.f32	s11, s8, s5
   4831c: eee76a63     	vfms.f32	s13, s14, s7
   48320: eee65a63     	vfms.f32	s11, s12, s7
   48324: eee65aab     	vfma.f32	s11, s13, s23
   48328: eea47aa5     	vfma.f32	s14, s9, s11
   4832c: eea76a2b     	vfma.f32	s12, s14, s23
   48330: ee7bba8d     	vadd.f32	s23, s23, s26
   48334: eeb07a45     	vmov.f32	s14, s10
   48338: eef4bae7     	vcmpe.f32	s23, s15
   4833c: eea37a06     	vfma.f32	s14, s6, s12
   48340: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48344: a2883001     	addge	r3, r8, #1
   48348: eca27a01     	vstmia	r2!, {s14}
   4834c: ba000005     	blt	0x48368
   48350: ee7bbae7     	vsub.f32	s23, s23, s15
   48354: e1a08003     	mov	r8, r3
   48358: e2833001     	add	r3, r3, #1
   4835c: eef4bae7     	vcmpe.f32	s23, s15
   48360: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48364: aafffff9     	bge	0x48350
   48368: eef5bac0     	vcmpe.f32	s23, #0
   4836c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48370: 42483001     	submi	r3, r8, #1
   48374: 5a000005     	bpl	0x48390
   48378: ee7bbaa7     	vadd.f32	s23, s23, s15
   4837c: e1a08003     	mov	r8, r3
   48380: e2433001     	sub	r3, r3, #1
   48384: eef5bac0     	vcmpe.f32	s23, #0
   48388: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4838c: 4afffff9     	bmi	0x48378
   48390: e1500001     	cmp	r0, r1
   48394: 1affffcd     	bne	0x482d0
   48398: e59d3008     	ldr	r3, [sp, #0x8]
   4839c: e153000b     	cmp	r3, r11
   483a0: 1affff25     	bne	0x4803c
   483a4: e59d105c     	ldr	r1, [sp, #0x5c]
   483a8: e59d0054     	ldr	r0, [sp, #0x54]
   483ac: eb000b8f     	bl	0x4b1f0
   483b0: e59d0020     	ldr	r0, [sp, #0x20]
   483b4: eb001685     	bl	0x4ddd0
   483b8: e1a03000     	mov	r3, r0
   483bc: e3500001     	cmp	r0, #1
   483c0: 9a000323     	bls	0x49054
   483c4: e1a02000     	mov	r2, r0
   483c8: ed1f7ae9     	vldr	s14, [pc, #-932]        @ 0x4802c>&, std::vector<float, std::allocator<float>>&)+0x424> ; float 0.819999992847
   483cc: eef77a00     	vmov.f32	s15, #1.000000e+00
   483d0: e2422001     	sub	r2, r2, #1
   483d4: ee677a87     	vmul.f32	s15, s15, s14
   483d8: e3520001     	cmp	r2, #1
   483dc: 1afffffb     	bne	0x483d0
   483e0: e2846a01     	add	r6, r4, #4096
   483e4: e2841d66     	add	r1, r4, #6528
   483e8: e596298c     	ldr	r2, [r6, #0x98c]
   483ec: ed917a00     	vldr	s14, [r1]
   483f0: e1520003     	cmp	r2, r3
   483f4: 0a00024b     	beq	0x48d28
   483f8: ee777ac7     	vsub.f32	s15, s15, s14
   483fc: eef46a00     	vmov.f32	s13, #1.250000e-01
   48400: e3a02008     	mov	r2, #8
   48404: ee677aa6     	vmul.f32	s15, s15, s13
   48408: edc17a01     	vstr	s15, [r1, #4]
   4840c: e586398c     	str	r3, [r6, #0x98c]
   48410: ee377a27     	vadd.f32	s14, s14, s15
   48414: e2422001     	sub	r2, r2, #1
   48418: ed817a00     	vstr	s14, [r1]
   4841c: e5862988     	str	r2, [r6, #0x988]
   48420: e59d302c     	ldr	r3, [sp, #0x2c]
   48424: e3530000     	cmp	r3, #0
   48428: da000009     	ble	0x48454
   4842c: e1a01003     	mov	r1, r3
   48430: e595300c     	ldr	r3, [r5, #0xc]
   48434: e59a2000     	ldr	r2, [r10]
   48438: e0831101     	add	r1, r3, r1, lsl #2
   4843c: ecf36a01     	vldmia	r3!, {s13}
   48440: edd27a00     	vldr	s15, [r2]
   48444: eee67a87     	vfma.f32	s15, s13, s14
   48448: e1510003     	cmp	r1, r3
   4844c: ece27a01     	vstmia	r2!, {s15}
   48450: 1afffff9     	bne	0x4843c
   48454: e2840c32     	add	r0, r4, #12800
   48458: e1a0100a     	mov	r1, r10
   4845c: e2800004     	add	r0, r0, #4
   48460: e2847c62     	add	r7, r4, #25088
   48464: eb001c2e     	bl	0x4f524
   48468: e2840dc9     	add	r0, r4, #12864
   4846c: e1a0100a     	mov	r1, r10
   48470: e2800028     	add	r0, r0, #40
   48474: eb001df9     	bl	0x4fc60
   48478: e2840dc9     	add	r0, r4, #12864
   4847c: e1a0100a     	mov	r1, r10
   48480: e280002c     	add	r0, r0, #44
   48484: eb001e17     	bl	0x4fce8
   48488: e2877060     	add	r7, r7, #96
   4848c: e89a000a     	ldm	r10, {r1, r3}
   48490: e1530001     	cmp	r3, r1
   48494: 13a08000     	movne	r8, #0
   48498: 0a000007     	beq	0x484bc
   4849c: e0811108     	add	r1, r1, r8, lsl #2
   484a0: e1a00007     	mov	r0, r7
   484a4: eb000485     	bl	0x496c0
   484a8: e2888001     	add	r8, r8, #1
   484ac: e89a000a     	ldm	r10, {r1, r3}
   484b0: e0433001     	sub	r3, r3, r1
   484b4: e1580143     	cmp	r8, r3, asr #2
   484b8: 3afffff7     	blo	0x4849c
   484bc: e2850fbe     	add	r0, r5, #760
   484c0: e1a0100a     	mov	r1, r10
   484c4: ebffc463     	bl	0x39658
   484c8: e2840c62     	add	r0, r4, #25088
   484cc: e1a0100a     	mov	r1, r10
   484d0: e2800054     	add	r0, r0, #84
   484d4: eb001e73     	bl	0x4fea8
   484d8: e5d63968     	ldrb	r3, [r6, #0x968]
   484dc: e3530000     	cmp	r3, #0
   484e0: 1a0001dc     	bne	0x48c58
   484e4: e59d0030     	ldr	r0, [sp, #0x30]
   484e8: eb001525     	bl	0x4d984
   484ec: e2407004     	sub	r7, r0, #4
   484f0: e280504c     	add	r5, r0, #76
   484f4: e5b72004     	ldr	r2, [r7, #0x4]!
   484f8: e3520000     	cmp	r2, #0
   484fc: 0a000006     	beq	0x4851c
   48500: e594c0e8     	ldr	r12, [r4, #0xe8]
   48504: eeb00a68     	vmov.f32	s0, s17
   48508: e59d3058     	ldr	r3, [sp, #0x58]
   4850c: e1a01004     	mov	r1, r4
   48510: e59d003c     	ldr	r0, [sp, #0x3c]
   48514: eddc0a29     	vldr	s1, [r12, #164]
   48518: ebfffcbe     	bl	0x47818
   4851c: e1570005     	cmp	r7, r5
   48520: 1afffff3     	bne	0x484f4
   48524: e5d63968     	ldrb	r3, [r6, #0x968]
   48528: e3530000     	cmp	r3, #0
   4852c: 0a000004     	beq	0x48544
   48530: e596396c     	ldr	r3, [r6, #0x96c]
   48534: e3530000     	cmp	r3, #0
   48538: ba00024f     	blt	0x48e7c
   4853c: e35300fe     	cmp	r3, #254
   48540: ca0002ce     	bgt	0x49080
   48544: e59d0020     	ldr	r0, [sp, #0x20]
   48548: eb0012fa     	bl	0x4d138
   4854c: e59d0030     	ldr	r0, [sp, #0x30]
   48550: eb0012f8     	bl	0x4d138
   48554: e28dd094     	add	sp, sp, #148
   48558: ecbd8b0c     	vpop	{d8, d9, d10, d11, d12, d13}
   4855c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   48560: e8960006     	ldm	r6, {r1, r2}
   48564: e59a0000     	ldr	r0, [r10]
   48568: e1510002     	cmp	r1, r2
   4856c: 0a000001     	beq	0x48578
   48570: e0422001     	sub	r2, r2, r1
   48574: ebff351a     	bl	0x159e4    @ imm = #-0x32b98 ; memmove
   48578: e5d4327d     	ldrb	r3, [r4, #0x27d]
   4857c: e3530000     	cmp	r3, #0
   48580: 0afffe04     	beq	0x47d98
   48584: e5d4327c     	ldrb	r3, [r4, #0x27c]
   48588: eddf6aef     	vldr	s13, [pc, #956]         @ 0x4894c>&, std::vector<float, std::allocator<float>>&)+0xd44> ; float 0.00406750012189
   4858c: e3530000     	cmp	r3, #0
   48590: eddf7aee     	vldr	s15, [pc, #952]         @ 0x48950>&, std::vector<float, std::allocator<float>>&)+0xd48> ; float -0.00406750012189
   48594: 0ef06a67     	vmoveq.f32	s13, s15
   48598: e59d302c     	ldr	r3, [sp, #0x2c]
   4859c: e3530000     	cmp	r3, #0
   485a0: dafffdfc     	ble	0x47d98
   485a4: e59a0000     	ldr	r0, [r10]
   485a8: e3a03000     	mov	r3, #0
   485ac: edd47aa0     	vldr	s15, [r4, #640]
   485b0: e3a01000     	mov	r1, #0
   485b4: e1a02000     	mov	r2, r0
   485b8: eeb76a00     	vmov.f32	s12, #1.000000e+00
   485bc: ea000006     	b	0x485dc
   485c0: eef47ac6     	vcmpe.f32	s15, s12
   485c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   485c8: ca00029d     	bgt	0x49044
   485cc: e59dc02c     	ldr	r12, [sp, #0x2c]
   485d0: e2833001     	add	r3, r3, #1
   485d4: e15c0003     	cmp	r12, r3
   485d8: 0afffdee     	beq	0x47d98
   485dc: ed927a00     	vldr	s14, [r2]
   485e0: ee677a27     	vmul.f32	s15, s14, s15
   485e4: ece27a01     	vstmia	r2!, {s15}
   485e8: edd47aa0     	vldr	s15, [r4, #640]
   485ec: ee767aa7     	vadd.f32	s15, s13, s15
   485f0: eef57ac0     	vcmpe.f32	s15, #0
   485f4: edc47aa0     	vstr	s15, [r4, #640]
   485f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   485fc: 5affffef     	bpl	0x485c0
   48600: e59dc02c     	ldr	r12, [sp, #0x2c]
   48604: e0800103     	add	r0, r0, r3, lsl #2
   48608: e5841280     	str	r1, [r4, #0x280]
   4860c: e3a01000     	mov	r1, #0
   48610: e04c2003     	sub	r2, r12, r3
   48614: e15c0003     	cmp	r12, r3
   48618: d3a02004     	movle	r2, #4
   4861c: c1a02102     	lslgt	r2, r2, #2
   48620: ebff35d3     	bl	0x15d74    @ imm = #-0x328b4 ; memset
   48624: e3a03000     	mov	r3, #0
   48628: e5c4327d     	strb	r3, [r4, #0x27d]
   4862c: eafffdd9     	b	0x47d98
   48630: e59d3038     	ldr	r3, [sp, #0x38]
   48634: e1590003     	cmp	r9, r3
   48638: aa00007e     	bge	0x48838
   4863c: e59d300c     	ldr	r3, [sp, #0xc]
   48640: e1590003     	cmp	r9, r3
   48644: b3a03001     	movlt	r3, #1
   48648: a3a03000     	movge	r3, #0
   4864c: e1590002     	cmp	r9, r2
   48650: b3a03000     	movlt	r3, #0
   48654: e3530000     	cmp	r3, #0
   48658: 0afffe2c     	beq	0x47f10
   4865c: e3a01001     	mov	r1, #1
   48660: e1a00006     	mov	r0, r6
   48664: eb000e40     	bl	0x4bf6c
   48668: e3500000     	cmp	r0, #0
   4866c: 1afffe50     	bne	0x47fb4
   48670: e59d901c     	ldr	r9, [sp, #0x1c]
   48674: e3a03001     	mov	r3, #1
   48678: e58d0000     	str	r0, [sp]
   4867c: e3a01001     	mov	r1, #1
   48680: e58d3004     	str	r3, [sp, #0x4]
   48684: e1a02009     	mov	r2, r9
   48688: e59d3018     	ldr	r3, [sp, #0x18]
   4868c: e1a00006     	mov	r0, r6
   48690: eb001775     	bl	0x4e46c
   48694: e59d3034     	ldr	r3, [sp, #0x34]
   48698: e1570009     	cmp	r7, r9
   4869c: d0491007     	suble	r1, r9, r7
   486a0: c3a01000     	movgt	r1, #0
   486a4: e3530000     	cmp	r3, #0
   486a8: 0afffe41     	beq	0x47fb4
   486ac: e59d300c     	ldr	r3, [sp, #0xc]
   486b0: e1a0000b     	mov	r0, r11
   486b4: e596207c     	ldr	r2, [r6, #0x7c]
   486b8: e0431001     	sub	r1, r3, r1
   486bc: eb001767     	bl	0x4e460
   486c0: e2509000     	subs	r9, r0, #0
   486c4: 0afffe3a     	beq	0x47fb4
   486c8: e1a01006     	mov	r1, r6
   486cc: e3a02001     	mov	r2, #1
   486d0: eb000d88     	bl	0x4bcf8
   486d4: e59d301c     	ldr	r3, [sp, #0x1c]
   486d8: e1570003     	cmp	r7, r3
   486dc: ca000004     	bgt	0x486f4
   486e0: eeb01a48     	vmov.f32	s2, s16
   486e4: eef00a68     	vmov.f32	s1, s17
   486e8: eeb00a69     	vmov.f32	s0, s19
   486ec: e1a00009     	mov	r0, r9
   486f0: eb000dba     	bl	0x4bde0
   486f4: e1a00009     	mov	r0, r9
   486f8: e3a03001     	mov	r3, #1
   486fc: e58d3004     	str	r3, [sp, #0x4]
   48700: e58d3000     	str	r3, [sp]
   48704: e3a01001     	mov	r1, #1
   48708: e59d3018     	ldr	r3, [sp, #0x18]
   4870c: e59d200c     	ldr	r2, [sp, #0xc]
   48710: eb001755     	bl	0x4e46c
   48714: eafffe26     	b	0x47fb4
   48718: e59d3040     	ldr	r3, [sp, #0x40]
   4871c: e1590003     	cmp	r9, r3
   48720: aa000004     	bge	0x48738
   48724: e3a01000     	mov	r1, #0
   48728: e1a00006     	mov	r0, r6
   4872c: eb000e0e     	bl	0x4bf6c
   48730: e2501000     	subs	r1, r0, #0
   48734: 0a0000ce     	beq	0x48a74
   48738: e3a01001     	mov	r1, #1
   4873c: e1a00006     	mov	r0, r6
   48740: eb000e09     	bl	0x4bf6c
   48744: e2502000     	subs	r2, r0, #0
   48748: 1afffe19     	bne	0x47fb4
   4874c: e59d3024     	ldr	r3, [sp, #0x24]
   48750: e59d1028     	ldr	r1, [sp, #0x28]
   48754: e1530001     	cmp	r3, r1
   48758: 0a000099     	beq	0x489c4
   4875c: e1530009     	cmp	r3, r9
   48760: ca000008     	bgt	0x48788
   48764: e59d300c     	ldr	r3, [sp, #0xc]
   48768: e59d2014     	ldr	r2, [sp, #0x14]
   4876c: e1590003     	cmp	r9, r3
   48770: b3a03001     	movlt	r3, #1
   48774: a3a03000     	movge	r3, #0
   48778: e1590002     	cmp	r9, r2
   4877c: b3a03000     	movlt	r3, #0
   48780: e3530000     	cmp	r3, #0
   48784: 0afffe0a     	beq	0x47fb4
   48788: e3a03000     	mov	r3, #0
   4878c: e3a01001     	mov	r1, #1
   48790: e1a02003     	mov	r2, r3
   48794: e58d3004     	str	r3, [sp, #0x4]
   48798: e58d2000     	str	r2, [sp]
   4879c: e1a00006     	mov	r0, r6
   487a0: e59d3018     	ldr	r3, [sp, #0x18]
   487a4: e59d2024     	ldr	r2, [sp, #0x24]
   487a8: eb00172f     	bl	0x4e46c
   487ac: e59d3024     	ldr	r3, [sp, #0x24]
   487b0: e1530009     	cmp	r3, r9
   487b4: e1a02003     	mov	r2, r3
   487b8: b3a02000     	movlt	r2, #0
   487bc: a3a02001     	movge	r2, #1
   487c0: e1530007     	cmp	r3, r7
   487c4: c3a02000     	movgt	r2, #0
   487c8: e1a09002     	mov	r9, r2
   487cc: e3520000     	cmp	r2, #0
   487d0: 0a0000df     	beq	0x48b54
   487d4: e0431007     	sub	r1, r3, r7
   487d8: e59d3034     	ldr	r3, [sp, #0x34]
   487dc: e3530000     	cmp	r3, #0
   487e0: 0afffdf3     	beq	0x47fb4
   487e4: e59d3014     	ldr	r3, [sp, #0x14]
   487e8: e1a0000b     	mov	r0, r11
   487ec: e596207c     	ldr	r2, [r6, #0x7c]
   487f0: e0431001     	sub	r1, r3, r1
   487f4: eb001719     	bl	0x4e460
   487f8: e2507000     	subs	r7, r0, #0
   487fc: 0afffdec     	beq	0x47fb4
   48800: e1a01006     	mov	r1, r6
   48804: e3a02001     	mov	r2, #1
   48808: eb000d3a     	bl	0x4bcf8
   4880c: e3590000     	cmp	r9, #0
   48810: 1a0001ac     	bne	0x48ec8
   48814: e1a00007     	mov	r0, r7
   48818: e3a03000     	mov	r3, #0
   4881c: e58d3004     	str	r3, [sp, #0x4]
   48820: e3a03001     	mov	r3, #1
   48824: e3a01001     	mov	r1, #1
   48828: e58d3000     	str	r3, [sp]
   4882c: e1cd21d4     	ldrd	r2, r3, [sp, #20]
   48830: eb00170d     	bl	0x4e46c
   48834: eafffdde     	b	0x47fb4
   48838: e3a01000     	mov	r1, #0
   4883c: e1a00006     	mov	r0, r6
   48840: eb000dc9     	bl	0x4bf6c
   48844: e2501000     	subs	r1, r0, #0
   48848: 0a0000c3     	beq	0x48b5c
   4884c: e59d301c     	ldr	r3, [sp, #0x1c]
   48850: e59d200c     	ldr	r2, [sp, #0xc]
   48854: e1590003     	cmp	r9, r3
   48858: a3a03001     	movge	r3, #1
   4885c: b3a03000     	movlt	r3, #0
   48860: e1590002     	cmp	r9, r2
   48864: a3a03000     	movge	r3, #0
   48868: e3530000     	cmp	r3, #0
   4886c: 0afffdd0     	beq	0x47fb4
   48870: eaffff79     	b	0x4865c
   48874: e3a01001     	mov	r1, #1
   48878: e1a00006     	mov	r0, r6
   4887c: eb000dba     	bl	0x4bf6c
   48880: e3500000     	cmp	r0, #0
   48884: 1afffdca     	bne	0x47fb4
   48888: e3a03001     	mov	r3, #1
   4888c: e58d0000     	str	r0, [sp]
   48890: e58d3004     	str	r3, [sp, #0x4]
   48894: e3a01001     	mov	r1, #1
   48898: e59d3018     	ldr	r3, [sp, #0x18]
   4889c: e1a00006     	mov	r0, r6
   488a0: e59d201c     	ldr	r2, [sp, #0x1c]
   488a4: eb0016f0     	bl	0x4e46c
   488a8: e59d301c     	ldr	r3, [sp, #0x1c]
   488ac: e1590003     	cmp	r9, r3
   488b0: e1a02003     	mov	r2, r3
   488b4: b3a02000     	movlt	r2, #0
   488b8: a3a02001     	movge	r2, #1
   488bc: e1570003     	cmp	r7, r3
   488c0: c3a02000     	movgt	r2, #0
   488c4: e3520000     	cmp	r2, #0
   488c8: e1a09002     	mov	r9, r2
   488cc: 10431007     	subne	r1, r3, r7
   488d0: e59d3034     	ldr	r3, [sp, #0x34]
   488d4: 01a01002     	moveq	r1, r2
   488d8: e3530000     	cmp	r3, #0
   488dc: 0afffdb4     	beq	0x47fb4
   488e0: e59d300c     	ldr	r3, [sp, #0xc]
   488e4: e1a0000b     	mov	r0, r11
   488e8: e596207c     	ldr	r2, [r6, #0x7c]
   488ec: e0431001     	sub	r1, r3, r1
   488f0: eb0016da     	bl	0x4e460
   488f4: e2507000     	subs	r7, r0, #0
   488f8: 0afffdad     	beq	0x47fb4
   488fc: e1a01006     	mov	r1, r6
   48900: e3a02001     	mov	r2, #1
   48904: eb000cfb     	bl	0x4bcf8
   48908: e3590000     	cmp	r9, #0
   4890c: 0a000004     	beq	0x48924
   48910: eeb01a48     	vmov.f32	s2, s16
   48914: eef00a68     	vmov.f32	s1, s17
   48918: eeb00a69     	vmov.f32	s0, s19
   4891c: e1a00007     	mov	r0, r7
   48920: eb000d2e     	bl	0x4bde0
   48924: e3a03001     	mov	r3, #1
   48928: e1a00007     	mov	r0, r7
   4892c: e58d3004     	str	r3, [sp, #0x4]
   48930: eaffff72     	b	0x48700
   48934: ee694a0a     	vmul.f32	s9, s18, s20
   48938: eeb06a69     	vmov.f32	s12, s19
   4893c: eafffe05     	b	0x48158
   48940: eeb07a69     	vmov.f32	s14, s19
   48944: ee295a0a     	vmul.f32	s10, s18, s20
   48948: eafffdf8     	b	0x48130
   4894c: aa 48 85 3b  	.word	0x3b8548aa
   48950: aa 48 85 bb  	.word	0xbb8548aa
   48954: eef59ac0     	vcmpe.f32	s19, #0
   48958: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4895c: a2888002     	addge	r8, r8, #2
   48960: b2488003     	sublt	r8, r8, #3
   48964: ee7a7acb     	vsub.f32	s15, s21, s22
   48968: e5953018     	ldr	r3, [r5, #0x18]
   4896c: e595201c     	ldr	r2, [r5, #0x1c]
   48970: e1520003     	cmp	r2, r3
   48974: ee875a88     	vdiv.f32	s10, s15, s16
   48978: 0afffddc     	beq	0x480f0
   4897c: eef06a4b     	vmov.f32	s13, s22
   48980: eef15a00     	vmov.f32	s11, #4.000000e+00
   48984: eeb76a00     	vmov.f32	s12, #1.000000e+00
   48988: ee667aa5     	vmul.f32	s15, s13, s11
   4898c: eef47ac6     	vcmpe.f32	s15, s12
   48990: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48994: 5ef77a00     	vmovpl.f32	s15, #1.000000e+00
   48998: 5a000002     	bpl	0x489a8
   4899c: eef57ac0     	vcmpe.f32	s15, #0
   489a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   489a4: def07a6c     	vmovle.f32	s15, s25
   489a8: ed937a00     	vldr	s14, [r3]
   489ac: ee766a85     	vadd.f32	s13, s13, s10
   489b0: ee677a27     	vmul.f32	s15, s14, s15
   489b4: ece37a01     	vstmia	r3!, {s15}
   489b8: e1520003     	cmp	r2, r3
   489bc: 1afffff1     	bne	0x48988
   489c0: eafffdca     	b	0x480f0
   489c4: e59d1014     	ldr	r1, [sp, #0x14]
   489c8: e1590003     	cmp	r9, r3
   489cc: b3a03001     	movlt	r3, #1
   489d0: a3a03000     	movge	r3, #0
   489d4: e1590001     	cmp	r9, r1
   489d8: b3a03000     	movlt	r3, #0
   489dc: e3530000     	cmp	r3, #0
   489e0: 0afffd73     	beq	0x47fb4
   489e4: e58d2004     	str	r2, [sp, #0x4]
   489e8: e3a01001     	mov	r1, #1
   489ec: e58d2000     	str	r2, [sp]
   489f0: e1a00006     	mov	r0, r6
   489f4: e59d3018     	ldr	r3, [sp, #0x18]
   489f8: e59d2024     	ldr	r2, [sp, #0x24]
   489fc: eb00169a     	bl	0x4e46c
   48a00: e59d3028     	ldr	r3, [sp, #0x28]
   48a04: e1570003     	cmp	r7, r3
   48a08: a0431007     	subge	r1, r3, r7
   48a0c: e59d3034     	ldr	r3, [sp, #0x34]
   48a10: b3a01000     	movlt	r1, #0
   48a14: e3530000     	cmp	r3, #0
   48a18: 0afffd65     	beq	0x47fb4
   48a1c: e59d3014     	ldr	r3, [sp, #0x14]
   48a20: e1a0000b     	mov	r0, r11
   48a24: e596207c     	ldr	r2, [r6, #0x7c]
   48a28: e0431001     	sub	r1, r3, r1
   48a2c: eb00168b     	bl	0x4e460
   48a30: e2509000     	subs	r9, r0, #0
   48a34: 0afffd5e     	beq	0x47fb4
   48a38: e1a01006     	mov	r1, r6
   48a3c: e3a02001     	mov	r2, #1
   48a40: eb000cac     	bl	0x4bcf8
   48a44: e59d3028     	ldr	r3, [sp, #0x28]
   48a48: e1570003     	cmp	r7, r3
   48a4c: ba000004     	blt	0x48a64
   48a50: eeb01a48     	vmov.f32	s2, s16
   48a54: eef00a68     	vmov.f32	s1, s17
   48a58: eeb00a69     	vmov.f32	s0, s19
   48a5c: e1a00009     	mov	r0, r9
   48a60: eb000cde     	bl	0x4bde0
   48a64: e3a03000     	mov	r3, #0
   48a68: e1a00009     	mov	r0, r9
   48a6c: e58d3004     	str	r3, [sp, #0x4]
   48a70: eaffff6a     	b	0x48820
   48a74: e59d2040     	ldr	r2, [sp, #0x40]
   48a78: e1a00006     	mov	r0, r6
   48a7c: e58d1004     	str	r1, [sp, #0x4]
   48a80: e300399a     	movw	r3, #0x99a
   48a84: e58d1000     	str	r1, [sp]
   48a88: eb001677     	bl	0x4e46c
   48a8c: e59d3040     	ldr	r3, [sp, #0x40]
   48a90: e1a0000b     	mov	r0, r11
   48a94: e596207c     	ldr	r2, [r6, #0x7c]
   48a98: e1570003     	cmp	r7, r3
   48a9c: a0431007     	subge	r1, r3, r7
   48aa0: a59d3048     	ldrge	r3, [sp, #0x48]
   48aa4: b59d1048     	ldrlt	r1, [sp, #0x48]
   48aa8: a0431001     	subge	r1, r3, r1
   48aac: eb00166b     	bl	0x4e460
   48ab0: e2503000     	subs	r3, r0, #0
   48ab4: e58d3050     	str	r3, [sp, #0x50]
   48ab8: 0affff1e     	beq	0x48738
   48abc: e3a02000     	mov	r2, #0
   48ac0: e1a01006     	mov	r1, r6
   48ac4: eb000c8b     	bl	0x4bcf8
   48ac8: e59d2040     	ldr	r2, [sp, #0x40]
   48acc: e1570002     	cmp	r7, r2
   48ad0: ba000004     	blt	0x48ae8
   48ad4: eeb01a48     	vmov.f32	s2, s16
   48ad8: eef00a68     	vmov.f32	s1, s17
   48adc: eeb00a69     	vmov.f32	s0, s19
   48ae0: e59d0050     	ldr	r0, [sp, #0x50]
   48ae4: eb000cbd     	bl	0x4bde0
   48ae8: e3a03000     	mov	r3, #0
   48aec: e59d0050     	ldr	r0, [sp, #0x50]
   48af0: e58d3004     	str	r3, [sp, #0x4]
   48af4: e3a01000     	mov	r1, #0
   48af8: e3a03001     	mov	r3, #1
   48afc: e59d2048     	ldr	r2, [sp, #0x48]
   48b00: e58d3000     	str	r3, [sp]
   48b04: e300399a     	movw	r3, #0x99a
   48b08: eb001657     	bl	0x4e46c
   48b0c: eaffff09     	b	0x48738
   48b10: e58d2004     	str	r2, [sp, #0x4]
   48b14: e3a01001     	mov	r1, #1
   48b18: e58d2000     	str	r2, [sp]
   48b1c: e1a00006     	mov	r0, r6
   48b20: e59d3018     	ldr	r3, [sp, #0x18]
   48b24: e59d2028     	ldr	r2, [sp, #0x28]
   48b28: eb00164f     	bl	0x4e46c
   48b2c: e59d3028     	ldr	r3, [sp, #0x28]
   48b30: e1590003     	cmp	r9, r3
   48b34: e1a02003     	mov	r2, r3
   48b38: c3a02000     	movgt	r2, #0
   48b3c: d3a02001     	movle	r2, #1
   48b40: e1570003     	cmp	r7, r3
   48b44: b3a02000     	movlt	r2, #0
   48b48: e1a09002     	mov	r9, r2
   48b4c: e3520000     	cmp	r2, #0
   48b50: 1affff1f     	bne	0x487d4
   48b54: e1a01009     	mov	r1, r9
   48b58: eaffff1e     	b	0x487d8
   48b5c: e3a03001     	mov	r3, #1
   48b60: e59d2038     	ldr	r2, [sp, #0x38]
   48b64: e88d000a     	stm	sp, {r1, r3}
   48b68: e1a00006     	mov	r0, r6
   48b6c: e300399a     	movw	r3, #0x99a
   48b70: eb00163d     	bl	0x4e46c
   48b74: e59d3038     	ldr	r3, [sp, #0x38]
   48b78: e1a0000b     	mov	r0, r11
   48b7c: e596207c     	ldr	r2, [r6, #0x7c]
   48b80: e1570003     	cmp	r7, r3
   48b84: d0431007     	suble	r1, r3, r7
   48b88: c3a01001     	movgt	r1, #1
   48b8c: d2611001     	rsble	r1, r1, #1
   48b90: eb001632     	bl	0x4e460
   48b94: e2503000     	subs	r3, r0, #0
   48b98: e58d3050     	str	r3, [sp, #0x50]
   48b9c: 0affff2a     	beq	0x4884c
   48ba0: e3a02000     	mov	r2, #0
   48ba4: e1a01006     	mov	r1, r6
   48ba8: eb000c52     	bl	0x4bcf8
   48bac: e59d2038     	ldr	r2, [sp, #0x38]
   48bb0: e1570002     	cmp	r7, r2
   48bb4: ca000004     	bgt	0x48bcc
   48bb8: eeb01a48     	vmov.f32	s2, s16
   48bbc: eef00a68     	vmov.f32	s1, s17
   48bc0: eeb00a69     	vmov.f32	s0, s19
   48bc4: e59d0050     	ldr	r0, [sp, #0x50]
   48bc8: eb000c84     	bl	0x4bde0
   48bcc: e3a03001     	mov	r3, #1
   48bd0: e59d0050     	ldr	r0, [sp, #0x50]
   48bd4: e58d3004     	str	r3, [sp, #0x4]
   48bd8: e3a02001     	mov	r2, #1
   48bdc: e58d3000     	str	r3, [sp]
   48be0: e3a01000     	mov	r1, #0
   48be4: e300399a     	movw	r3, #0x99a
   48be8: eb00161f     	bl	0x4e46c
   48bec: eaffff16     	b	0x4884c
   48bf0: e2833ffe     	add	r3, r3, #1016
   48bf4: e1a01006     	mov	r1, r6
   48bf8: e1a00003     	mov	r0, r3
   48bfc: e58d3010     	str	r3, [sp, #0x10]
   48c00: ebffbf9c     	bl	0x38a78
   48c04: eef70a00     	vmov.f32	s1, #1.000000e+00
   48c08: e2845fb9     	add	r5, r4, #740
   48c0c: eeb01a48     	vmov.f32	s2, s16
   48c10: eeb00a60     	vmov.f32	s0, s1
   48c14: e1a00005     	mov	r0, r5
   48c18: eb000c70     	bl	0x4bde0
   48c1c: e5943278     	ldr	r3, [r4, #0x278]
   48c20: e5933004     	ldr	r3, [r3, #0x4]
   48c24: e3530001     	cmp	r3, #1
   48c28: 0a0000ac     	beq	0x48ee0
   48c2c: e59432e8     	ldr	r3, [r4, #0x2e8]
   48c30: e1530007     	cmp	r3, r7
   48c34: da0000f5     	ble	0x49010
   48c38: e1a00005     	mov	r0, r5
   48c3c: e28d9060     	add	r9, sp, #96
   48c40: eb000c02     	bl	0x4bc50
   48c44: e28d7078     	add	r7, sp, #120
   48c48: e28d3068     	add	r3, sp, #104
   48c4c: e28d8080     	add	r8, sp, #128
   48c50: e58d3008     	str	r3, [sp, #0x8]
   48c54: eafffc59     	b	0x47dc0
   48c58: e59430e8     	ldr	r3, [r4, #0xe8]
   48c5c: e2840d65     	add	r0, r4, #6464
   48c60: e2800028     	add	r0, r0, #40
   48c64: e59330b0     	ldr	r3, [r3, #0xb0]
   48c68: e5933004     	ldr	r3, [r3, #0x4]
   48c6c: e3530001     	cmp	r3, #1
   48c70: 1e288a2a     	vmulne.f32	s16, s16, s21
   48c74: eeb00a48     	vmov.f32	s0, s16
   48c78: eb000b9f     	bl	0x4bafc
   48c7c: eafffe18     	b	0x484e4
   48c80: e59534d8     	ldr	r3, [r5, #0x4d8]
   48c84: eeb8cacc     	vcvt.f32.s32	s24, s24
   48c88: e1a00004     	mov	r0, r4
   48c8c: e3530000     	cmp	r3, #0
   48c90: 1e073a90     	vmovne	s15, r3
   48c94: 1eb87ae7     	vcvtne.f32.s32	s14, s15
   48c98: 0ef07a4c     	vmoveq.f32	s15, s24
   48c9c: 1ecc7a07     	vdivne.f32	s15, s24, s14
   48ca0: eefd7ae7     	vcvt.s32.f32	s15, s15
   48ca4: ee173a90     	vmov	r3, s15
   48ca8: e58534f4     	str	r3, [r5, #0x4f4]
   48cac: ebffb966     	bl	0x3724c
   48cb0: e59d0020     	ldr	r0, [sp, #0x20]
   48cb4: eb0012d8     	bl	0x4d81c
   48cb8: e5d03014     	ldrb	r3, [r0, #0x14]
   48cbc: e59524d8     	ldr	r2, [r5, #0x4d8]
   48cc0: e3530000     	cmp	r3, #0
   48cc4: e590600c     	ldr	r6, [r0, #0xc]
   48cc8: e5903004     	ldr	r3, [r0, #0x4]
   48ccc: 0ef07a69     	vmoveq.f32	s15, s19
   48cd0: 1dd07a06     	vldrne	s15, [r0, #24]
   48cd4: eef57ac0     	vcmpe.f32	s15, #0
   48cd8: ee072a90     	vmov	s15, r2
   48cdc: eef8cae7     	vcvt.f32.s32	s25, s15
   48ce0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48ce4: ba000041     	blt	0x48df0
   48ce8: e59d700c     	ldr	r7, [sp, #0xc]
   48cec: e1570003     	cmp	r7, r3
   48cf0: da0000d9     	ble	0x4905c
   48cf4: e59db038     	ldr	r11, [sp, #0x38]
   48cf8: e08b3003     	add	r3, r11, r3
   48cfc: e08b6006     	add	r6, r11, r6
   48d00: e0433007     	sub	r3, r3, r7
   48d04: ee073a90     	vmov	s15, r3
   48d08: e0466007     	sub	r6, r6, r7
   48d0c: eef87ae7     	vcvt.f32.s32	s15, s15
   48d10: eec7ba8c     	vdiv.f32	s23, s15, s24
   48d14: ee6bbaac     	vmul.f32	s23, s23, s25
   48d18: eeb00a6b     	vmov.f32	s0, s23
   48d1c: ebff35a0     	bl	0x163a4    @ imm = #-0x32980 ; floorf
   48d20: e2853e4e     	add	r3, r5, #1248
   48d24: ea00003d     	b	0x48e20
   48d28: e5962988     	ldr	r2, [r6, #0x988]
   48d2c: e3520000     	cmp	r2, #0
   48d30: cdd17a01     	vldrgt	s15, [r1, #4]
   48d34: dafffdb9     	ble	0x48420
   48d38: eafffdb4     	b	0x48410
   48d3c: e59d0030     	ldr	r0, [sp, #0x30]
   48d40: e5946078     	ldr	r6, [r4, #0x78]
   48d44: eb00130e     	bl	0x4d984
   48d48: e2402004     	sub	r2, r0, #4
   48d4c: e280304c     	add	r3, r0, #76
   48d50: ea000001     	b	0x48d5c
   48d54: e1530002     	cmp	r3, r2
   48d58: 0afffc4c     	beq	0x47e90
   48d5c: e5b21004     	ldr	r1, [r2, #0x4]!
   48d60: e3510000     	cmp	r1, #0
   48d64: 0afffffa     	beq	0x48d54
   48d68: e5910004     	ldr	r0, [r1, #0x4]
   48d6c: e591100c     	ldr	r1, [r1, #0xc]
   48d70: e1500006     	cmp	r0, r6
   48d74: d3a00000     	movle	r0, #0
   48d78: c3a00001     	movgt	r0, #1
   48d7c: e1510006     	cmp	r1, r6
   48d80: d3a01000     	movle	r1, #0
   48d84: c3a01001     	movgt	r1, #1
   48d88: e1500001     	cmp	r0, r1
   48d8c: 0afffff0     	beq	0x48d54
   48d90: e3a01000     	mov	r1, #0
   48d94: e1a00004     	mov	r0, r4
   48d98: ebffd909     	bl	0x3f1c4
   48d9c: eafffc3b     	b	0x47e90
   48da0: e8960006     	ldm	r6, {r1, r2}
   48da4: e59a0000     	ldr	r0, [r10]
   48da8: e1520001     	cmp	r2, r1
   48dac: 1afffdef     	bne	0x48570
   48db0: eddf6acc     	vldr	s13, [pc, #816]         @ 0x490e8>&, std::vector<float, std::allocator<float>>&)+0x14e0> ; float -0.00406750012189
   48db4: eafffdf7     	b	0x48598
   48db8: e2810fda     	add	r0, r1, #872
   48dbc: eb001350     	bl	0x4db04
   48dc0: e3500000     	cmp	r0, #0
   48dc4: 1afffb9c     	bne	0x47c3c
   48dc8: e2840ee6     	add	r0, r4, #3680
   48dcc: e2800008     	add	r0, r0, #8
   48dd0: eb00134b     	bl	0x4db04
   48dd4: e2501000     	subs	r1, r0, #0
   48dd8: 1afffb97     	bne	0x47c3c
   48ddc: e1a00004     	mov	r0, r4
   48de0: ebffc48c     	bl	0x3a018
   48de4: e3a03001     	mov	r3, #1
   48de8: e5c43286     	strb	r3, [r4, #0x286]
   48dec: eafffb92     	b	0x47c3c
   48df0: e59d7014     	ldr	r7, [sp, #0x14]
   48df4: e1570003     	cmp	r7, r3
   48df8: ba000025     	blt	0x48e94
   48dfc: e0473003     	sub	r3, r7, r3
   48e00: ee073a90     	vmov	s15, r3
   48e04: e0476006     	sub	r6, r7, r6
   48e08: eef87ae7     	vcvt.f32.s32	s15, s15
   48e0c: eec7ba8c     	vdiv.f32	s23, s15, s24
   48e10: ee6bbaac     	vmul.f32	s23, s23, s25
   48e14: eeb00a6b     	vmov.f32	s0, s23
   48e18: ebff3561     	bl	0x163a4    @ imm = #-0x32a7c ; floorf
   48e1c: e2853e4e     	add	r3, r5, #1248
   48e20: ee076a90     	vmov	s15, r6
   48e24: ee7bbac0     	vsub.f32	s23, s23, s0
   48e28: eeb87ae7     	vcvt.f32.s32	s14, s15
   48e2c: edc3ba00     	vstr	s23, [r3]
   48e30: eec77a0c     	vdiv.f32	s15, s14, s24
   48e34: ee67caac     	vmul.f32	s25, s15, s25
   48e38: eeb00a6c     	vmov.f32	s0, s25
   48e3c: ebff3558     	bl	0x163a4    @ imm = #-0x32aa0 ; floorf
   48e40: ee7c7ac0     	vsub.f32	s15, s25, s0
   48e44: e2853e4e     	add	r3, r5, #1248
   48e48: eeb67a00     	vmov.f32	s14, #5.000000e-01
   48e4c: edc37a01     	vstr	s15, [r3, #4]
   48e50: ee7b7ae7     	vsub.f32	s15, s23, s15
   48e54: e59d303c     	ldr	r3, [sp, #0x3c]
   48e58: eef07ae7     	vabs.f32	s15, s15
   48e5c: e5930084     	ldr	r0, [r3, #0x84]
   48e60: eef47ac7     	vcmpe.f32	s15, s14
   48e64: eef1fa10     	vmrs	APSR_nzcv, fpscr
   48e68: dafffc03     	ble	0x47e7c
   48e6c: e59524d4     	ldr	r2, [r5, #0x4d4]
   48e70: e1a01004     	mov	r1, r4
   48e74: ebff8427     	bl	0x29f18
   48e78: eafffbfd     	b	0x47e74
   48e7c: e2840d65     	add	r0, r4, #6464
   48e80: e2800028     	add	r0, r0, #40
   48e84: eb000b0d     	bl	0x4bac0
   48e88: e59d0030     	ldr	r0, [sp, #0x30]
   48e8c: eb00119c     	bl	0x4d504
   48e90: eafffdab     	b	0x48544
   48e94: e59db038     	ldr	r11, [sp, #0x38]
   48e98: e043300b     	sub	r3, r3, r11
   48e9c: e046600b     	sub	r6, r6, r11
   48ea0: e0473003     	sub	r3, r7, r3
   48ea4: ee073a90     	vmov	s15, r3
   48ea8: e0476006     	sub	r6, r7, r6
   48eac: eef87ae7     	vcvt.f32.s32	s15, s15
   48eb0: eec7ba8c     	vdiv.f32	s23, s15, s24
   48eb4: ee6bbaac     	vmul.f32	s23, s23, s25
   48eb8: eeb00a6b     	vmov.f32	s0, s23
   48ebc: ebff3538     	bl	0x163a4    @ imm = #-0x32b20 ; floorf
   48ec0: e2853e4e     	add	r3, r5, #1248
   48ec4: eaffffd5     	b	0x48e20
   48ec8: eeb01a48     	vmov.f32	s2, s16
   48ecc: eef00a68     	vmov.f32	s1, s17
   48ed0: eeb00a69     	vmov.f32	s0, s19
   48ed4: e1a00007     	mov	r0, r7
   48ed8: eb000bc0     	bl	0x4bde0
   48edc: eafffe4c     	b	0x48814
   48ee0: e59d102c     	ldr	r1, [sp, #0x2c]
   48ee4: e30f3a2c     	movw	r3, #0xfa2c
   48ee8: e34031c1     	movt	r3, #0x1c1
   48eec: e59422e8     	ldr	r2, [r4, #0x2e8]
   48ef0: e0433081     	sub	r3, r3, r1, lsl #1
   48ef4: e1520003     	cmp	r2, r3
   48ef8: 2a000014     	bhs	0x48f50
   48efc: e5d4325c     	ldrb	r3, [r4, #0x25c]
   48f00: e3530000     	cmp	r3, #0
   48f04: 0a000004     	beq	0x48f1c
   48f08: e59430e8     	ldr	r3, [r4, #0xe8]
   48f0c: e59330b0     	ldr	r3, [r3, #0xb0]
   48f10: e5933030     	ldr	r3, [r3, #0x30]
   48f14: e1520103     	cmp	r2, r3, lsl #2
   48f18: ca000074     	bgt	0x490f0
   48f1c: e28d9060     	add	r9, sp, #96
   48f20: e28d7078     	add	r7, sp, #120
   48f24: e28d8080     	add	r8, sp, #128
   48f28: e28d3068     	add	r3, sp, #104
   48f2c: e58d3008     	str	r3, [sp, #0x8]
   48f30: e59d3010     	ldr	r3, [sp, #0x10]
   48f34: e1a02005     	mov	r2, r5
   48f38: e59d003c     	ldr	r0, [sp, #0x3c]
   48f3c: e1a01004     	mov	r1, r4
   48f40: eddf0a69     	vldr	s1, [pc, #420]          @ 0x490ec>&, std::vector<float, std::allocator<float>>&)+0x14e4> ; float 0
   48f44: eeb70a00     	vmov.f32	s0, #1.000000e+00
   48f48: ebfffa32     	bl	0x47818
   48f4c: eafffb9b     	b	0x47dc0
   48f50: e5947008     	ldr	r7, [r4, #0x8]
   48f54: e28d3068     	add	r3, sp, #104
   48f58: e5948004     	ldr	r8, [r4, #0x4]
   48f5c: e28d9060     	add	r9, sp, #96
   48f60: e2572000     	subs	r2, r7, #0
   48f64: e58d3008     	str	r3, [sp, #0x8]
   48f68: 13a02001     	movne	r2, #1
   48f6c: e3580000     	cmp	r8, #0
   48f70: 13a02000     	movne	r2, #0
   48f74: e58d3060     	str	r3, [sp, #0x60]
   48f78: e3520000     	cmp	r2, #0
   48f7c: 1a000056     	bne	0x490dc
   48f80: e357000f     	cmp	r7, #15
   48f84: e58d7078     	str	r7, [sp, #0x78]
   48f88: 8a00004c     	bhi	0x490c0
   48f8c: e3570001     	cmp	r7, #1
   48f90: 1a000041     	bne	0x4909c
   48f94: e5d82000     	ldrb	r2, [r8]
   48f98: e5cd2068     	strb	r2, [sp, #0x68]
   48f9c: e3a02000     	mov	r2, #0
   48fa0: e58d7064     	str	r7, [sp, #0x64]
   48fa4: e7c32007     	strb	r2, [r3, r7]
   48fa8: e3e03103     	mvn	r3, #-1073741824
   48fac: e59d2064     	ldr	r2, [sp, #0x64]
   48fb0: e0433002     	sub	r3, r3, r2
   48fb4: e353002a     	cmp	r3, #42
   48fb8: 9a000034     	bls	0x49090
   48fbc: e303119c     	movw	r1, #0x319c
   48fc0: e3401007     	movt	r1, #0x7
   48fc4: e3a0202b     	mov	r2, #43
   48fc8: e1a00009     	mov	r0, r9
   48fcc: ebff33ad     	bl	0x15e88    @ imm = #-0x3314c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   48fd0: e3090fec     	movw	r0, #0x9fec
   48fd4: e3400009     	movt	r0, #0x9
   48fd8: e3a02000     	mov	r2, #0
   48fdc: e1a01009     	mov	r1, r9
   48fe0: eb009bce     	bl	0x6ff20
   48fe4: e59d0060     	ldr	r0, [sp, #0x60]
   48fe8: e59d3008     	ldr	r3, [sp, #0x8]
   48fec: e1500003     	cmp	r0, r3
   48ff0: 0a000000     	beq	0x48ff8
   48ff4: ebff3391     	bl	0x15e40    @ imm = #-0x331bc ; _ZdlPv
   48ff8: e3a01000     	mov	r1, #0
   48ffc: e1a00004     	mov	r0, r4
   49000: e28d7078     	add	r7, sp, #120
   49004: e28d8080     	add	r8, sp, #128
   49008: ebffd86d     	bl	0x3f1c4
   4900c: eaffffc7     	b	0x48f30
   49010: e28d3068     	add	r3, sp, #104
   49014: e59d003c     	ldr	r0, [sp, #0x3c]
   49018: e58d3008     	str	r3, [sp, #0x8]
   4901c: e1a02005     	mov	r2, r5
   49020: e59d3010     	ldr	r3, [sp, #0x10]
   49024: e1a01004     	mov	r1, r4
   49028: eddf0a2f     	vldr	s1, [pc, #188]          @ 0x490ec>&, std::vector<float, std::allocator<float>>&)+0x14e4> ; float 0
   4902c: eeb70a00     	vmov.f32	s0, #1.000000e+00
   49030: e28d9060     	add	r9, sp, #96
   49034: e28d7078     	add	r7, sp, #120
   49038: e28d8080     	add	r8, sp, #128
   4903c: ebfff9f5     	bl	0x47818
   49040: eafffb5e     	b	0x47dc0
   49044: e3a03000     	mov	r3, #0
   49048: ed846aa0     	vstr	s12, [r4, #640]
   4904c: e5c4327d     	strb	r3, [r4, #0x27d]
   49050: eafffb50     	b	0x47d98
   49054: eef77a00     	vmov.f32	s15, #1.000000e+00
   49058: eafffce0     	b	0x483e0
   4905c: e0433007     	sub	r3, r3, r7
   49060: ee073a90     	vmov	s15, r3
   49064: e0466007     	sub	r6, r6, r7
   49068: eef87ae7     	vcvt.f32.s32	s15, s15
   4906c: eec7ba8c     	vdiv.f32	s23, s15, s24
   49070: ee6bbaac     	vmul.f32	s23, s23, s25
   49074: eeb00a6b     	vmov.f32	s0, s23
   49078: ebff34c9     	bl	0x163a4    @ imm = #-0x32cdc ; floorf
   4907c: eaffff66     	b	0x48e1c
   49080: e2840d65     	add	r0, r4, #6464
   49084: e2800028     	add	r0, r0, #40
   49088: eb000a8c     	bl	0x4bac0
   4908c: eafffd2c     	b	0x48544
   49090: e3010b08     	movw	r0, #0x1b08
   49094: e3400007     	movt	r0, #0x7
   49098: ebff32c6     	bl	0x15bb8    @ imm = #-0x334e8 ; _ZSt20__throw_length_errorPKc
   4909c: e3570000     	cmp	r7, #0
   490a0: 0affffbd     	beq	0x48f9c
   490a4: e59d0008     	ldr	r0, [sp, #0x8]
   490a8: e1a02007     	mov	r2, r7
   490ac: e1a01008     	mov	r1, r8
   490b0: ebff33e3     	bl	0x16044    @ imm = #-0x33074 ; memcpy
   490b4: e59d7078     	ldr	r7, [sp, #0x78]
   490b8: e59d3060     	ldr	r3, [sp, #0x60]
   490bc: eaffffb6     	b	0x48f9c
   490c0: e28d1078     	add	r1, sp, #120
   490c4: e1a00009     	mov	r0, r9
   490c8: ebff349a     	bl	0x16338    @ imm = #-0x32d98 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   490cc: e59d3078     	ldr	r3, [sp, #0x78]
   490d0: e58d3068     	str	r3, [sp, #0x68]
   490d4: e58d0060     	str	r0, [sp, #0x60]
   490d8: eafffff2     	b	0x490a8
   490dc: e3000ca8     	movw	r0, #0xca8
   490e0: e3400007     	movt	r0, #0x7
   490e4: ebff32fe     	bl	0x15ce4    @ imm = #-0x33408 ; _ZSt19__throw_logic_errorPKc
   490e8: aa 48 85 bb  	.word	0xbb8548aa
   490ec: 00 00 00 00  	.word	0x00000000
   490f0: e5949008     	ldr	r9, [r4, #0x8]
   490f4: e28d8080     	add	r8, sp, #128
   490f8: e5943004     	ldr	r3, [r4, #0x4]
   490fc: e28d7078     	add	r7, sp, #120
   49100: e2592000     	subs	r2, r9, #0
   49104: e58d8078     	str	r8, [sp, #0x78]
   49108: 13a02001     	movne	r2, #1
   4910c: e3530000     	cmp	r3, #0
   49110: 13a02000     	movne	r2, #0
   49114: e3520000     	cmp	r2, #0
   49118: 1affffef     	bne	0x490dc
   4911c: e359000f     	cmp	r9, #15
   49120: e58d9060     	str	r9, [sp, #0x60]
   49124: 8a000030     	bhi	0x491ec
   49128: e3590001     	cmp	r9, #1
   4912c: 1a000024     	bne	0x491c4
   49130: e5d32000     	ldrb	r2, [r3]
   49134: e1a03008     	mov	r3, r8
   49138: e5cd2080     	strb	r2, [sp, #0x80]
   4913c: e3a02000     	mov	r2, #0
   49140: e58d907c     	str	r9, [sp, #0x7c]
   49144: e7c32009     	strb	r2, [r3, r9]
   49148: e3e03103     	mvn	r3, #-1073741824
   4914c: e59d207c     	ldr	r2, [sp, #0x7c]
   49150: e0433002     	sub	r3, r3, r2
   49154: e3530021     	cmp	r3, #33
   49158: 9a000016     	bls	0x491b8
   4915c: e30311c8     	movw	r1, #0x31c8
   49160: e3401007     	movt	r1, #0x7
   49164: e3a02022     	mov	r2, #34
   49168: e1a00007     	mov	r0, r7
   4916c: ebff3345     	bl	0x15e88    @ imm = #-0x332ec ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   49170: e3090fec     	movw	r0, #0x9fec
   49174: e3400009     	movt	r0, #0x9
   49178: e3a02000     	mov	r2, #0
   4917c: e1a01007     	mov	r1, r7
   49180: eb009b66     	bl	0x6ff20
   49184: e59d0078     	ldr	r0, [sp, #0x78]
   49188: e1500008     	cmp	r0, r8
   4918c: 0a000000     	beq	0x49194
   49190: ebff332a     	bl	0x15e40    @ imm = #-0x33358 ; _ZdlPv
   49194: e3a01000     	mov	r1, #0
   49198: e1a00004     	mov	r0, r4
   4919c: ebffd808     	bl	0x3f1c4
   491a0: e28d9060     	add	r9, sp, #96
   491a4: e28d3068     	add	r3, sp, #104
   491a8: e58d3008     	str	r3, [sp, #0x8]
   491ac: e3a03000     	mov	r3, #0
   491b0: e5c4325c     	strb	r3, [r4, #0x25c]
   491b4: eaffff5d     	b	0x48f30
   491b8: e3010b08     	movw	r0, #0x1b08
   491bc: e3400007     	movt	r0, #0x7
   491c0: ebff327c     	bl	0x15bb8    @ imm = #-0x33610 ; _ZSt20__throw_length_errorPKc
   491c4: e3590000     	cmp	r9, #0
   491c8: 01a03008     	moveq	r3, r8
   491cc: 0affffda     	beq	0x4913c
   491d0: e1a00008     	mov	r0, r8
   491d4: e1a02009     	mov	r2, r9
   491d8: e1a01003     	mov	r1, r3
   491dc: ebff3398     	bl	0x16044    @ imm = #-0x331a0 ; memcpy
   491e0: e59d9060     	ldr	r9, [sp, #0x60]
   491e4: e59d3078     	ldr	r3, [sp, #0x78]
   491e8: eaffffd3     	b	0x4913c
   491ec: e28d1060     	add	r1, sp, #96
   491f0: e1a00007     	mov	r0, r7
   491f4: e58d3008     	str	r3, [sp, #0x8]
   491f8: ebff344e     	bl	0x16338    @ imm = #-0x32ec8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_createERjj
   491fc: e59d2060     	ldr	r2, [sp, #0x60]
   49200: e59d3008     	ldr	r3, [sp, #0x8]
   49204: e58d2080     	str	r2, [sp, #0x80]
   49208: e58d0078     	str	r0, [sp, #0x78]
   4920c: eafffff0     	b	0x491d4
   49210: e59d0078     	ldr	r0, [sp, #0x78]
   49214: e1500008     	cmp	r0, r8
   49218: 0a000000     	beq	0x49220
   4921c: ebff3307     	bl	0x15e40    @ imm = #-0x333e4 ; _ZdlPv
   49220: ebff334e     	bl	0x15f60    @ imm = #-0x332c8 ; __cxa_end_cleanup
   49224: eafffff9     	b	0x49210
   49228: e59d0060     	ldr	r0, [sp, #0x60]
   4922c: e59d3008     	ldr	r3, [sp, #0x8]
   49230: e1500003     	cmp	r0, r3
   49234: 1afffff8     	bne	0x4921c
   49238: eafffff8     	b	0x49220
   4923c: eafffff9     	b	0x49228
   49240: eafffff2     	b	0x49210
