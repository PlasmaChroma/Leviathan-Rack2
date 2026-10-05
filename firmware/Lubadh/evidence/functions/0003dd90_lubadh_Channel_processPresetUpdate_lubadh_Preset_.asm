; lubadh::Channel::processPresetUpdate(lubadh::Preset&)
; VA 0x3dd90 size 2932

   3dd90: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   3dd94: e1a06000     	mov	r6, r0
   3dd98: e3a03001     	mov	r3, #1
   3dd9c: ed2d8b02     	vpush	{d8}
   3dda0: e2800fa7     	add	r0, r0, #668
   3dda4: e5962020     	ldr	r2, [r6, #0x20]
   3dda8: e24dd054     	sub	sp, sp, #84
   3ddac: e5c63285     	strb	r3, [r6, #0x285]
   3ddb0: e1a07001     	mov	r7, r1
   3ddb4: e2822915     	add	r2, r2, #344064
   3ddb8: e5c632a4     	strb	r3, [r6, #0x2a4]
   3ddbc: eddf0bfd     	vldr	d16, [pc, #1012]        @ 0x3e1b8 ; float 6.36598738204e-313
   3ddc0: e58610d4     	str	r1, [r6, #0xd4]
   3ddc4: e5d22abc     	ldrb	r2, [r2, #0xabc]
   3ddc8: e58632a8     	str	r3, [r6, #0x2a8]
   3ddcc: e3520000     	cmp	r2, #0
   3ddd0: f440078f     	vst1.32	{d16}, [r0]
   3ddd4: 0a000005     	beq	0x3ddf0
   3ddd8: e596201c     	ldr	r2, [r6, #0x1c]
   3dddc: e2821fa7     	add	r1, r2, #668
   3dde0: e5c23285     	strb	r3, [r2, #0x285]
   3dde4: e5c232a4     	strb	r3, [r2, #0x2a4]
   3dde8: e58232a8     	str	r3, [r2, #0x2a8]
   3ddec: f441078f     	vst1.32	{d16}, [r1]
   3ddf0: e59630e8     	ldr	r3, [r6, #0xe8]
   3ddf4: e1a00006     	mov	r0, r6
   3ddf8: e59330b0     	ldr	r3, [r3, #0xb0]
   3ddfc: e5931004     	ldr	r1, [r3, #0x4]
   3de00: ebffef7d     	bl	0x39bfc
   3de04: e59630e8     	ldr	r3, [r6, #0xe8]
   3de08: e1a00006     	mov	r0, r6
   3de0c: e59330b0     	ldr	r3, [r3, #0xb0]
   3de10: e5931008     	ldr	r1, [r3, #0x8]
   3de14: ebffee7b     	bl	0x39808
   3de18: e59630e8     	ldr	r3, [r6, #0xe8]
   3de1c: e593208c     	ldr	r2, [r3, #0x8c]
   3de20: e3520000     	cmp	r2, #0
   3de24: 0a000155     	beq	0x3e380
   3de28: e3520001     	cmp	r2, #1
   3de2c: 1a000004     	bne	0x3de44
   3de30: e59330b0     	ldr	r3, [r3, #0xb0]
   3de34: e1a00006     	mov	r0, r6
   3de38: e5931010     	ldr	r1, [r3, #0x10]
   3de3c: ebffeeb7     	bl	0x39920
   3de40: e59630e8     	ldr	r3, [r6, #0xe8]
   3de44: e59310b0     	ldr	r1, [r3, #0xb0]
   3de48: e5912024     	ldr	r2, [r1, #0x24]
   3de4c: e591002c     	ldr	r0, [r1, #0x2c]
   3de50: e2522000     	subs	r2, r2, #0
   3de54: e591c034     	ldr	r12, [r1, #0x34]
   3de58: 13a02001     	movne	r2, #1
   3de5c: e586c08c     	str	r12, [r6, #0x8c]
   3de60: e3500000     	cmp	r0, #0
   3de64: e5c621d8     	strb	r2, [r6, #0x1d8]
   3de68: 0a00011d     	beq	0x3e2e4
   3de6c: edd37a1a     	vldr	s15, [r3, #104]
   3de70: eeb17a00     	vmov.f32	s14, #4.000000e+00
   3de74: eddf6ad1     	vldr	s13, [pc, #836]         @ 0x3e1c0 ; float 4095
   3de78: e286ca2a     	add	r12, r6, #172032
   3de7c: e3500001     	cmp	r0, #1
   3de80: ee677a87     	vmul.f32	s15, s15, s14
   3de84: ee677aa6     	vmul.f32	s15, s15, s13
   3de88: eefd7ae7     	vcvt.s32.f32	s15, s15
   3de8c: ee172a90     	vmov	r2, s15
   3de90: e58c24b4     	str	r2, [r12, #0x4b4]
   3de94: 1a00012c     	bne	0x3e34c
   3de98: e59c24c0     	ldr	r2, [r12, #0x4c0]
   3de9c: eeb77a00     	vmov.f32	s14, #1.000000e+00
   3dea0: edd17a05     	vldr	s15, [r1, #20]
   3dea4: ee062a10     	vmov	s12, r2
   3dea8: eeb86ac6     	vcvt.f32.s32	s12, s12
   3deac: eef87ae7     	vcvt.f32.s32	s15, s15
   3deb0: ee677a86     	vmul.f32	s15, s15, s12
   3deb4: ee876aa6     	vdiv.f32	s12, s15, s13
   3deb8: ed836a2b     	vstr	s12, [r3, #172]
   3debc: e59c0498     	ldr	r0, [r12, #0x498]
   3dec0: e2812901     	add	r2, r1, #16384
   3dec4: ed837a2a     	vstr	s14, [r3, #168]
   3dec8: eef75a00     	vmov.f32	s11, #1.000000e+00
   3decc: ed966ab6     	vldr	s12, [r6, #728]
   3ded0: e0811100     	add	r1, r1, r0, lsl #2
   3ded4: eddf4aba     	vldr	s9, [pc, #744]          @ 0x3e1c4 ; float 2.70000004768
   3ded8: ed9f4ab8     	vldr	s8, [pc, #736]          @ 0x3e1c0 ; float 4095
   3dedc: edd17a89     	vldr	s15, [r1, #548]
   3dee0: e28c1e4b     	add	r1, r12, #1200
   3dee4: e2811008     	add	r1, r1, #8
   3dee8: edc67a0b     	vstr	s15, [r6, #44]
   3deec: ee376ac6     	vsub.f32	s12, s15, s12
   3def0: edd36a2b     	vldr	s13, [r3, #172]
   3def4: ed927a9c     	vldr	s14, [r2, #624]
   3def8: edd37a2a     	vldr	s15, [r3, #168]
   3defc: eefd6ae6     	vcvt.s32.f32	s13, s13
   3df00: ed925a9b     	vldr	s10, [r2, #620]
   3df04: ee277a27     	vmul.f32	s14, s14, s15
   3df08: eef86ae6     	vcvt.f32.s32	s13, s13
   3df0c: eeb47ae5     	vcmpe.f32	s14, s11
   3df10: eec67aa4     	vdiv.f32	s15, s13, s9
   3df14: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3df18: 5eb07a65     	vmovpl.f32	s14, s11
   3df1c: ee777aa5     	vadd.f32	s15, s15, s11
   3df20: eefd7ae7     	vcvt.s32.f32	s15, s15
   3df24: edc67ab8     	vstr	s15, [r6, #736]
   3df28: eef87ae7     	vcvt.f32.s32	s15, s15
   3df2c: edd16a00     	vldr	s13, [r1]
   3df30: eec64a27     	vdiv.f32	s9, s12, s15
   3df34: eef86ae6     	vcvt.f32.s32	s13, s13
   3df38: eec67a84     	vdiv.f32	s15, s13, s8
   3df3c: edc64ab7     	vstr	s9, [r6, #732]
   3df40: ee677a85     	vmul.f32	s15, s15, s10
   3df44: edc37a29     	vstr	s15, [r3, #164]
   3df48: 5a000003     	bpl	0x3df5c
   3df4c: eeb57ac0     	vcmpe.f32	s14, #0
   3df50: eddf7a9c     	vldr	s15, [pc, #624]         @ 0x3e1c8 ; float 0
   3df54: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3df58: deb07a67     	vmovle.f32	s14, s15
   3df5c: e2860c4a     	add	r0, r6, #18944
   3df60: eef76a00     	vmov.f32	s13, #1.000000e+00
   3df64: e2861901     	add	r1, r6, #16384
   3df68: ed807a0a     	vstr	s14, [r0, #40]
   3df6c: edd27a9d     	vldr	s15, [r2, #628]
   3df70: ed937a2a     	vldr	s14, [r3, #168]
   3df74: ee677a87     	vmul.f32	s15, s15, s14
   3df78: eef47ae6     	vcmpe.f32	s15, s13
   3df7c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3df80: 5ef07a66     	vmovpl.f32	s15, s13
   3df84: 5a000003     	bpl	0x3df98
   3df88: eef57ac0     	vcmpe.f32	s15, #0
   3df8c: ed9f7a8d     	vldr	s14, [pc, #564]         @ 0x3e1c8 ; float 0
   3df90: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3df94: def07a47     	vmovle.f32	s15, s14
   3df98: e2811ea2     	add	r1, r1, #2592
   3df9c: eeb77a00     	vmov.f32	s14, #1.000000e+00
   3dfa0: edc17a03     	vstr	s15, [r1, #12]
   3dfa4: edd27a9e     	vldr	s15, [r2, #632]
   3dfa8: edd36a2a     	vldr	s13, [r3, #168]
   3dfac: ee677aa6     	vmul.f32	s15, s15, s13
   3dfb0: eef47ac7     	vcmpe.f32	s15, s14
   3dfb4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3dfb8: 5ef07a47     	vmovpl.f32	s15, s14
   3dfbc: 5a000003     	bpl	0x3dfd0
   3dfc0: eef57ac0     	vcmpe.f32	s15, #0
   3dfc4: ed9f7a7f     	vldr	s14, [pc, #508]         @ 0x3e1c8 ; float 0
   3dfc8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3dfcc: def07a47     	vmovle.f32	s15, s14
   3dfd0: e2861d69     	add	r1, r6, #6720
   3dfd4: eeb77a00     	vmov.f32	s14, #1.000000e+00
   3dfd8: e2864a01     	add	r4, r6, #4096
   3dfdc: edc17a07     	vstr	s15, [r1, #28]
   3dfe0: edd27a9f     	vldr	s15, [r2, #636]
   3dfe4: edd36a2a     	vldr	s13, [r3, #168]
   3dfe8: ee677aa6     	vmul.f32	s15, s15, s13
   3dfec: eef47ac7     	vcmpe.f32	s15, s14
   3dff0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3dff4: 5ef07a47     	vmovpl.f32	s15, s14
   3dff8: 5a000003     	bpl	0x3e00c
   3dffc: eef57ac0     	vcmpe.f32	s15, #0
   3e000: ed9f7a70     	vldr	s14, [pc, #448]         @ 0x3e1c8 ; float 0
   3e004: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e008: def07a47     	vmovle.f32	s15, s14
   3e00c: e2841e9f     	add	r1, r4, #2544
   3e010: eeb77a00     	vmov.f32	s14, #1.000000e+00
   3e014: edc17a02     	vstr	s15, [r1, #8]
   3e018: edd27aa0     	vldr	s15, [r2, #640]
   3e01c: edd36a2a     	vldr	s13, [r3, #168]
   3e020: ee677aa6     	vmul.f32	s15, s15, s13
   3e024: eef47ac7     	vcmpe.f32	s15, s14
   3e028: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e02c: 5ef07a47     	vmovpl.f32	s15, s14
   3e030: 5a000003     	bpl	0x3e044
   3e034: eef57ac0     	vcmpe.f32	s15, #0
   3e038: ed9f7a62     	vldr	s14, [pc, #392]         @ 0x3e1c8 ; float 0
   3e03c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e040: def07a47     	vmovle.f32	s15, s14
   3e044: e2841ea6     	add	r1, r4, #2656
   3e048: eeb77a00     	vmov.f32	s14, #1.000000e+00
   3e04c: edc17a00     	vstr	s15, [r1]
   3e050: edd27a9e     	vldr	s15, [r2, #632]
   3e054: edd36a2a     	vldr	s13, [r3, #168]
   3e058: ee677aa6     	vmul.f32	s15, s15, s13
   3e05c: eef47ac7     	vcmpe.f32	s15, s14
   3e060: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e064: 5ef07a47     	vmovpl.f32	s15, s14
   3e068: 5a000003     	bpl	0x3e07c
   3e06c: eef57ac0     	vcmpe.f32	s15, #0
   3e070: ed9f7a54     	vldr	s14, [pc, #336]         @ 0x3e1c8 ; float 0
   3e074: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e078: def07a47     	vmovle.f32	s15, s14
   3e07c: e2861a03     	add	r1, r6, #12288
   3e080: eeb77a00     	vmov.f32	s14, #1.000000e+00
   3e084: edc17a9a     	vstr	s15, [r1, #616]
   3e088: edd27a9f     	vldr	s15, [r2, #636]
   3e08c: edd36a2a     	vldr	s13, [r3, #168]
   3e090: ee677aa6     	vmul.f32	s15, s15, s13
   3e094: eef47ac7     	vcmpe.f32	s15, s14
   3e098: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e09c: 5ef07a47     	vmovpl.f32	s15, s14
   3e0a0: 5a000003     	bpl	0x3e0b4
   3e0a4: eef57ac0     	vcmpe.f32	s15, #0
   3e0a8: ed9f7a46     	vldr	s14, [pc, #280]         @ 0x3e1c8 ; float 0
   3e0ac: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e0b0: def07a47     	vmovle.f32	s15, s14
   3e0b4: edc17a81     	vstr	s15, [r1, #516]
   3e0b8: eeb77a00     	vmov.f32	s14, #1.000000e+00
   3e0bc: edd27aa0     	vldr	s15, [r2, #640]
   3e0c0: edd36a2a     	vldr	s13, [r3, #168]
   3e0c4: ee677aa6     	vmul.f32	s15, s15, s13
   3e0c8: eef47ac7     	vcmpe.f32	s15, s14
   3e0cc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e0d0: 5ef07a47     	vmovpl.f32	s15, s14
   3e0d4: 5a000003     	bpl	0x3e0e8
   3e0d8: eef57ac0     	vcmpe.f32	s15, #0
   3e0dc: ed9f7a39     	vldr	s14, [pc, #228]         @ 0x3e1c8 ; float 0
   3e0e0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e0e4: def07a47     	vmovle.f32	s15, s14
   3e0e8: edc17a9b     	vstr	s15, [r1, #620]
   3e0ec: eeb77a00     	vmov.f32	s14, #1.000000e+00
   3e0f0: edd30a2a     	vldr	s1, [r3, #168]
   3e0f4: edd27aa6     	vldr	s15, [r2, #664]
   3e0f8: ee607aa7     	vmul.f32	s15, s1, s15
   3e0fc: eef47ac7     	vcmpe.f32	s15, s14
   3e100: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e104: 5a000081     	bpl	0x3e310
   3e108: eef57ac0     	vcmpe.f32	s15, #0
   3e10c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e110: ca0001ca     	bgt	0x3e840
   3e114: e2861ba7     	add	r1, r6, #171008
   3e118: f2c00010     	vmov.i32	d16, #0x0
   3e11c: eeb06a47     	vmov.f32	s12, s14
   3e120: e1a03001     	mov	r3, r1
   3e124: eddf6a27     	vldr	s13, [pc, #156]         @ 0x3e1c8 ; float 0
   3e128: e2811fcf     	add	r1, r1, #828
   3e12c: e2865a29     	add	r5, r6, #167936
   3e130: f441078f     	vst1.32	{d16}, [r1]
   3e134: ed837ad1     	vstr	s14, [r3, #836]
   3e138: ed920aa1     	vldr	s0, [r2, #644]
   3e13c: eef77a00     	vmov.f32	s15, #1.000000e+00
   3e140: e2853d3d     	add	r3, r5, #3904
   3e144: e2860c62     	add	r0, r6, #25088
   3e148: e2800048     	add	r0, r0, #72
   3e14c: eeb40ae7     	vcmpe.f32	s0, s15
   3e150: edc36a03     	vstr	s13, [r3, #12]
   3e154: ed836a02     	vstr	s12, [r3, #8]
   3e158: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e15c: 5eb00a67     	vmovpl.f32	s0, s15
   3e160: 5a000003     	bpl	0x3e174
   3e164: eeb50ac0     	vcmpe.f32	s0, #0
   3e168: eddf7a16     	vldr	s15, [pc, #88]          @ 0x3e1c8 ; float 0
   3e16c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e170: deb00a67     	vmovle.f32	s0, s15
   3e174: ed927aa2     	vldr	s14, [r2, #648]
   3e178: eef07a00     	vmov.f32	s15, #2.000000e+00
   3e17c: ee600a87     	vmul.f32	s1, s1, s14
   3e180: eef40ae7     	vcmpe.f32	s1, s15
   3e184: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e188: 5ef00a67     	vmovpl.f32	s1, s15
   3e18c: 5a000003     	bpl	0x3e1a0
   3e190: eef50ac0     	vcmpe.f32	s1, #0
   3e194: eddf7a0b     	vldr	s15, [pc, #44]          @ 0x3e1c8 ; float 0
   3e198: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e19c: def00a67     	vmovle.f32	s1, s15
   3e1a0: eb00478e     	bl	0x4ffe0
   3e1a4: e59630e8     	ldr	r3, [r6, #0xe8]
   3e1a8: ed9f7a0b     	vldr	s14, [pc, #44]          @ 0x3e1dc ; float 20000
   3e1ac: e59330b0     	ldr	r3, [r3, #0xb0]
   3e1b0: ea00000b     	b	0x3e1e4
   3e1b4: e320f000     	nop
   3e1b8: b9 00 00 00  	.word	0x000000b9
   3e1bc: 1e 00 00 00  	.word	0x0000001e
   3e1c0: 00 f0 7f 45  	.word	0x457ff000
   3e1c4: cd cc 2c 40  	.word	0x402ccccd
   3e1c8: 00 00 00 00  	.word	0x00000000
   3e1cc: 89 41 d0 3e  	.word	0x3ed04189
   3e1d0: 0f 41 d5 39  	.word	0x39d5410f
   3e1d4: cd cc 4c 3f  	.word	0x3f4ccccd
   3e1d8: cd cc 4c 3e  	.word	0x3e4ccccd
   3e1dc: 00 40 9c 46  	.word	0x469c4000
   3e1e0: 41 12 40 47  	.word	0x47401241
   3e1e4: e2833901     	add	r3, r3, #16384
   3e1e8: edd37aa3     	vldr	s15, [r3, #652]
   3e1ec: eef47ac7     	vcmpe.f32	s15, s14
   3e1f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e1f4: 5d1f0a0c     	vldrpl	s0, [pc, #-48]          @ 0x3e1cc ; float 0.406749993563
   3e1f8: 5a000005     	bpl	0x3e214
   3e1fc: eeb37a04     	vmov.f32	s14, #2.000000e+01
   3e200: eef47ac7     	vcmpe.f32	s15, s14
   3e204: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e208: cd1f7a0c     	vldrgt	s14, [pc, #-48]         @ 0x3e1e0 ; float 49170.2539062
   3e20c: dd1f0a11     	vldrle	s0, [pc, #-68]          @ 0x3e1d0 ; float 0.000406749983085
   3e210: ce870a87     	vdivgt.f32	s0, s15, s14
   3e214: edd30aa4     	vldr	s1, [r3, #656]
   3e218: ed5f7a13     	vldr	s15, [pc, #-76]         @ 0x3e1d4 ; float 0.800000011921
   3e21c: eef40ae7     	vcmpe.f32	s1, s15
   3e220: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e224: 5ef00a67     	vmovpl.f32	s1, s15
   3e228: 5a000003     	bpl	0x3e23c
   3e22c: ed5f7a17     	vldr	s15, [pc, #-92]         @ 0x3e1d8 ; float 0.20000000298
   3e230: eef40ae7     	vcmpe.f32	s1, s15
   3e234: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e238: def00a67     	vmovle.f32	s1, s15
   3e23c: ed938aa5     	vldr	s16, [r3, #660]
   3e240: ed5f7a1b     	vldr	s15, [pc, #-108]        @ 0x3e1dc ; float 20000
   3e244: eeb48ae7     	vcmpe.f32	s16, s15
   3e248: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e24c: 5eb08a67     	vmovpl.f32	s16, s15
   3e250: 5a000003     	bpl	0x3e264
   3e254: eef37a04     	vmov.f32	s15, #2.000000e+01
   3e258: eeb48ae7     	vcmpe.f32	s16, s15
   3e25c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e260: deb08a67     	vmovle.f32	s16, s15
   3e264: e3a03003     	mov	r3, #3
   3e268: e2860d67     	add	r0, r6, #6592
   3e26c: e58439e8     	str	r3, [r4, #0x9e8]
   3e270: e2844e9e     	add	r4, r4, #2528
   3e274: eb0031bd     	bl	0x4a970
   3e278: ed1f7a28     	vldr	s14, [pc, #-160]        @ 0x3e1e0 ; float 49170.2539062
   3e27c: eddf1be7     	vldr	d17, [pc, #924]         @ 0x3e620 ; float 3.14159265359
   3e280: eef72b00     	vmov.f64	d18, #1.000000e+00
   3e284: e284400c     	add	r4, r4, #12
   3e288: e5960020     	ldr	r0, [r6, #0x20]
   3e28c: eec87a07     	vdiv.f32	s15, s16, s14
   3e290: eef70ae7     	vcvt.f64.f32	d16, s15
   3e294: ee600ba1     	vmul.f64	d16, d16, d17
   3e298: eec21ba0     	vdiv.f64	d17, d18, d16
   3e29c: eef77be1     	vcvt.f32.f64	s15, d17
   3e2a0: edc47a00     	vstr	s15, [r4]
   3e2a4: ebffa672     	bl	0x27c74
   3e2a8: e59630e8     	ldr	r3, [r6, #0xe8]
   3e2ac: e1a00006     	mov	r0, r6
   3e2b0: e59330b0     	ldr	r3, [r3, #0xb0]
   3e2b4: e2833901     	add	r3, r3, #16384
   3e2b8: e5931224     	ldr	r1, [r3, #0x224]
   3e2bc: ebfff417     	bl	0x3b320
   3e2c0: e2873901     	add	r3, r7, #16384
   3e2c4: e5933268     	ldr	r3, [r3, #0x268]
   3e2c8: e3530003     	cmp	r3, #3
   3e2cc: 979ff103     	ldrls	pc, [pc, r3, lsl #2]
   3e2d0: ea00017d     	b	0x3e8cc
   3e2d4: e8 e6 03 00  	.word	0x0003e6e8
   3e2d8: 44 e6 03 00  	.word	0x0003e644
   3e2dc: a0 e5 03 00  	.word	0x0003e5a0
   3e2e0: 98 e3 03 00  	.word	0x0003e398
   3e2e4: e5912014     	ldr	r2, [r1, #0x14]
   3e2e8: e3010388     	movw	r0, #0x1388
   3e2ec: e286ca2a     	add	r12, r6, #172032
   3e2f0: eeb77a00     	vmov.f32	s14, #1.000000e+00
   3e2f4: e1520000     	cmp	r2, r0
   3e2f8: a1a02000     	movge	r2, r0
   3e2fc: e1c22fc2     	bic	r2, r2, r2, asr #31
   3e300: ee072a90     	vmov	s15, r2
   3e304: eef87ae7     	vcvt.f32.s32	s15, s15
   3e308: edc37a2b     	vstr	s15, [r3, #172]
   3e30c: eafffeea     	b	0x3debc
   3e310: e2865a29     	add	r5, r6, #167936
   3e314: e2861ba7     	add	r1, r6, #171008
   3e318: e2853d3d     	add	r3, r5, #3904
   3e31c: e3a00000     	mov	r0, #0
   3e320: eeb08a47     	vmov.f32	s16, s14
   3e324: eef07a47     	vmov.f32	s15, s14
   3e328: e306e666     	movw	lr, #0x6666
   3e32c: e343ef66     	movt	lr, #0x3f66
   3e330: e581e33c     	str	lr, [r1, #0x33c]
   3e334: ed837a00     	vstr	s14, [r3]
   3e338: ee070a10     	vmov	s14, r0
   3e33c: e5830004     	str	r0, [r3, #0x4]
   3e340: eec76a88     	vdiv.f32	s13, s15, s16
   3e344: ee876a08     	vdiv.f32	s12, s14, s16
   3e348: eaffff7a     	b	0x3e138
   3e34c: e5912014     	ldr	r2, [r1, #0x14]
   3e350: e301e388     	movw	lr, #0x1388
   3e354: ed9f7ab3     	vldr	s14, [pc, #716]         @ 0x3e628 ; float 0
   3e358: eef77a00     	vmov.f32	s15, #1.000000e+00
   3e35c: e152000e     	cmp	r2, lr
   3e360: a1a0200e     	movge	r2, lr
   3e364: e3500002     	cmp	r0, #2
   3e368: e1c22fc2     	bic	r2, r2, r2, asr #31
   3e36c: 1eb07a67     	vmovne.f32	s14, s15
   3e370: ee072a90     	vmov	s15, r2
   3e374: eef87ae7     	vcvt.f32.s32	s15, s15
   3e378: edc37a2b     	vstr	s15, [r3, #172]
   3e37c: eafffece     	b	0x3debc
   3e380: e59330b0     	ldr	r3, [r3, #0xb0]
   3e384: e1a00006     	mov	r0, r6
   3e388: e593100c     	ldr	r1, [r3, #0xc]
   3e38c: ebffed63     	bl	0x39920
   3e390: e59630e8     	ldr	r3, [r6, #0xe8]
   3e394: eafffeaa     	b	0x3de44
   3e398: e59fe290     	ldr	lr, [pc, #0x290]        @ 0x3e630
   3e39c: e3a03000     	mov	r3, #0
   3e3a0: e58d3008     	str	r3, [sp, #0x8]
   3e3a4: e28d8010     	add	r8, sp, #16
   3e3a8: e1a0c008     	mov	r12, r8
   3e3ac: f2c00010     	vmov.i32	d16, #0x0
   3e3b0: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3e3b4: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3e3b8: edcd0b00     	vstr	d16, [sp]
   3e3bc: e89e0007     	ldm	lr, {r0, r1, r2}
   3e3c0: e88c0007     	stm	r12, {r0, r1, r2}
   3e3c4: e3a0001c     	mov	r0, #28
   3e3c8: ebff5d4f     	bl	0x1590c     @ imm = #-0x28ac4 ; _Znwj
   3e3cc: e1a0e008     	mov	lr, r8
   3e3d0: e1a0c000     	mov	r12, r0
   3e3d4: e58d0000     	str	r0, [sp]
   3e3d8: e28c401c     	add	r4, r12, #28
   3e3dc: e59650e8     	ldr	r5, [r6, #0xe8]
   3e3e0: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3e3e4: e58c0000     	str	r0, [r12]
   3e3e8: e58c1004     	str	r1, [r12, #0x4]
   3e3ec: e58c2008     	str	r2, [r12, #0x8]
   3e3f0: e58c300c     	str	r3, [r12, #0xc]
   3e3f4: e58d4008     	str	r4, [sp, #0x8]
   3e3f8: e8be0007     	ldm	lr!, {r0, r1, r2}
   3e3fc: e58c0010     	str	r0, [r12, #0x10]
   3e400: e58c1014     	str	r1, [r12, #0x14]
   3e404: e2850070     	add	r0, r5, #112
   3e408: e1a0100d     	mov	r1, sp
   3e40c: e58c2018     	str	r2, [r12, #0x18]
   3e410: e58d4004     	str	r4, [sp, #0x4]
   3e414: eb00067d     	bl	0x3fe10
   3e418: e59d0000     	ldr	r0, [sp]
   3e41c: e3500000     	cmp	r0, #0
   3e420: 0a000000     	beq	0x3e428
   3e424: ebff5e85     	bl	0x15e40    @ imm = #-0x285ec ; _ZdlPv
   3e428: e59630e8     	ldr	r3, [r6, #0xe8]
   3e42c: e2860f83     	add	r0, r6, #524
   3e430: e3a04001     	mov	r4, #1
   3e434: ed9f8a7c     	vldr	s16, [pc, #496]         @ 0x3e62c ; float 0.0010000000475
   3e438: e59330b0     	ldr	r3, [r3, #0xb0]
   3e43c: e5932020     	ldr	r2, [r3, #0x20]
   3e440: e3520000     	cmp	r2, #0
   3e444: d3a01001     	movle	r1, #1
   3e448: c593101c     	ldrgt	r1, [r3, #0x1c]
   3e44c: eb002473     	bl	0x47620
   3e450: e59650e8     	ldr	r5, [r6, #0xe8]
   3e454: e28590b4     	add	r9, r5, #180
   3e458: e59510b4     	ldr	r1, [r5, #0xb4]
   3e45c: e59530b8     	ldr	r3, [r5, #0xb8]
   3e460: e59520b0     	ldr	r2, [r5, #0xb0]
   3e464: e1510003     	cmp	r1, r3
   3e468: 158510b8     	strne	r1, [r5, #0xb8]
   3e46c: ea000009     	b	0x3e498
   3e470: e1a03104     	lsl	r3, r4, #2
   3e474: e0820003     	add	r0, r2, r3
   3e478: edd07a11     	vldr	s15, [r0, #68]
   3e47c: eef07ae7     	vabs.f32	s15, s15
   3e480: eef47ac8     	vcmpe.f32	s15, s16
   3e484: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e488: ca0000bd     	bgt	0x3e784
   3e48c: e2844001     	add	r4, r4, #1
   3e490: e3540078     	cmp	r4, #120
   3e494: 0a00000d     	beq	0x3e4d0
   3e498: e0823104     	add	r3, r2, r4, lsl #2
   3e49c: ed937a10     	vldr	s14, [r3, #64]
   3e4a0: eef07ac7     	vabs.f32	s15, s14
   3e4a4: eef47ac8     	vcmpe.f32	s15, s16
   3e4a8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e4ac: daffffef     	ble	0x3e470
   3e4b0: e59530bc     	ldr	r3, [r5, #0xbc]
   3e4b4: e1510003     	cmp	r1, r3
   3e4b8: 0a0000bc     	beq	0x3e7b0
   3e4bc: e2844001     	add	r4, r4, #1
   3e4c0: eca17a01     	vstmia	r1!, {s14}
   3e4c4: e3540078     	cmp	r4, #120
   3e4c8: e58510b8     	str	r1, [r5, #0xb8]
   3e4cc: 1afffff1     	bne	0x3e498
   3e4d0: e59530b4     	ldr	r3, [r5, #0xb4]
   3e4d4: e0410003     	sub	r0, r1, r3
   3e4d8: e1a02240     	asr	r2, r0, #4
   3e4dc: e1a00140     	asr	r0, r0, #2
   3e4e0: e3520000     	cmp	r2, #0
   3e4e4: da0000b5     	ble	0x3e7c0
   3e4e8: e0832202     	add	r2, r3, r2, lsl #4
   3e4ec: eef17a00     	vmov.f32	s15, #4.000000e+00
   3e4f0: ea00000e     	b	0x3e530
   3e4f4: ed937a01     	vldr	s14, [r3, #4]
   3e4f8: eeb47a67     	vcmp.f32	s14, s15
   3e4fc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e500: 0a0000bd     	beq	0x3e7fc
   3e504: ed937a02     	vldr	s14, [r3, #8]
   3e508: eeb47a67     	vcmp.f32	s14, s15
   3e50c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e510: 0a0000b7     	beq	0x3e7f4
   3e514: ed937a03     	vldr	s14, [r3, #12]
   3e518: eeb47a67     	vcmp.f32	s14, s15
   3e51c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e520: 0a0000b7     	beq	0x3e804
   3e524: e2833010     	add	r3, r3, #16
   3e528: e1530002     	cmp	r3, r2
   3e52c: 0a0000a1     	beq	0x3e7b8
   3e530: ed937a00     	vldr	s14, [r3]
   3e534: eeb47a67     	vcmp.f32	s14, s15
   3e538: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e53c: 1affffec     	bne	0x3e4f4
   3e540: e5972040     	ldr	r2, [r7, #0x40]
   3e544: e3520003     	cmp	r2, #3
   3e548: 13a03000     	movne	r3, #0
   3e54c: 1a000001     	bne	0x3e558
   3e550: e0533001     	subs	r3, r3, r1
   3e554: 13a03001     	movne	r3, #1
   3e558: e1a00008     	mov	r0, r8
   3e55c: e5c630e4     	strb	r3, [r6, #0xe4]
   3e560: e30217c0     	movw	r1, #0x27c0
   3e564: e3401007     	movt	r1, #0x7
   3e568: ebffe236     	bl	0x36e48
   3e56c: e3090fec     	movw	r0, #0x9fec
   3e570: e3400009     	movt	r0, #0x9
   3e574: e1a01008     	mov	r1, r8
   3e578: e3a02000     	mov	r2, #0
   3e57c: eb00c667     	bl	0x6ff20
   3e580: e59d0010     	ldr	r0, [sp, #0x10]
   3e584: e28d3018     	add	r3, sp, #24
   3e588: e1500003     	cmp	r0, r3
   3e58c: 0a000000     	beq	0x3e594
   3e590: ebff5e2a     	bl	0x15e40    @ imm = #-0x28758 ; _ZdlPv
   3e594: e28dd054     	add	sp, sp, #84
   3e598: ecbd8b02     	vpop	{d8}
   3e59c: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   3e5a0: e59fe08c     	ldr	lr, [pc, #0x8c]         @ 0x3e634
   3e5a4: e3a03000     	mov	r3, #0
   3e5a8: e58d3008     	str	r3, [sp, #0x8]
   3e5ac: e28d8010     	add	r8, sp, #16
   3e5b0: e1a0c008     	mov	r12, r8
   3e5b4: f2c00010     	vmov.i32	d16, #0x0
   3e5b8: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3e5bc: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3e5c0: edcd0b00     	vstr	d16, [sp]
   3e5c4: e89e0003     	ldm	lr, {r0, r1}
   3e5c8: e88c0003     	stm	r12, {r0, r1}
   3e5cc: e3a00018     	mov	r0, #24
   3e5d0: ebff5ccd     	bl	0x1590c     @ imm = #-0x28ccc ; _Znwj
   3e5d4: e1a0e008     	mov	lr, r8
   3e5d8: e1a0c000     	mov	r12, r0
   3e5dc: e58d0000     	str	r0, [sp]
   3e5e0: e28c4018     	add	r4, r12, #24
   3e5e4: e59650e8     	ldr	r5, [r6, #0xe8]
   3e5e8: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3e5ec: e58c0000     	str	r0, [r12]
   3e5f0: e58c1004     	str	r1, [r12, #0x4]
   3e5f4: e58c2008     	str	r2, [r12, #0x8]
   3e5f8: e58c300c     	str	r3, [r12, #0xc]
   3e5fc: e58d4008     	str	r4, [sp, #0x8]
   3e600: e8be0003     	ldm	lr!, {r0, r1}
   3e604: e58c0010     	str	r0, [r12, #0x10]
   3e608: e58c1014     	str	r1, [r12, #0x14]
   3e60c: e2850070     	add	r0, r5, #112
   3e610: e1a0100d     	mov	r1, sp
   3e614: e58d4004     	str	r4, [sp, #0x4]
   3e618: eb0005fc     	bl	0x3fe10
   3e61c: eaffff7d     	b	0x3e418
   3e620: 18 2d 44 54  	.word	0x54442d18
   3e624: fb 21 09 40  	.word	0x400921fb
   3e628: 00 00 00 00  	.word	0x00000000
   3e62c: 6f 12 83 3a  	.word	0x3a83126f
   3e630: f8 2f 07 00  	.word	0x00072ff8
   3e634: 9c 30 07 00  	.word	0x0007309c
   3e638: 90 2f 07 00  	.word	0x00072f90
   3e63c: 5c 30 07 00  	.word	0x0007305c
   3e640: 66 66 66 3f  	.word	0x3f666666
   3e644: e51fe014     	ldr	lr, [pc, #-0x14]        @ 0x3e638
   3e648: e3a03000     	mov	r3, #0
   3e64c: e58d3008     	str	r3, [sp, #0x8]
   3e650: e28d8010     	add	r8, sp, #16
   3e654: e1a0c008     	mov	r12, r8
   3e658: f2c00010     	vmov.i32	d16, #0x0
   3e65c: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3e660: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3e664: edcd0b00     	vstr	d16, [sp]
   3e668: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3e66c: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3e670: e89e0007     	ldm	lr, {r0, r1, r2}
   3e674: e88c0007     	stm	r12, {r0, r1, r2}
   3e678: e3a0002c     	mov	r0, #44
   3e67c: ebff5ca2     	bl	0x1590c     @ imm = #-0x28d78 ; _Znwj
   3e680: e1a04008     	mov	r4, r8
   3e684: e28d9030     	add	r9, sp, #48
   3e688: e1a0e000     	mov	lr, r0
   3e68c: e280502c     	add	r5, r0, #44
   3e690: e58d0000     	str	r0, [sp]
   3e694: e58d5008     	str	r5, [sp, #0x8]
   3e698: e1a0c004     	mov	r12, r4
   3e69c: e28ee010     	add	lr, lr, #16
   3e6a0: e2844010     	add	r4, r4, #16
   3e6a4: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   3e6a8: e50e0010     	str	r0, [lr, #-0x10]
   3e6ac: e50e100c     	str	r1, [lr, #-0xc]
   3e6b0: e50e2008     	str	r2, [lr, #-0x8]
   3e6b4: e50e3004     	str	r3, [lr, #-0x4]
   3e6b8: e15c0009     	cmp	r12, r9
   3e6bc: 1afffff5     	bne	0x3e698
   3e6c0: e59630e8     	ldr	r3, [r6, #0xe8]
   3e6c4: e8b40007     	ldm	r4!, {r0, r1, r2}
   3e6c8: e58e0000     	str	r0, [lr]
   3e6cc: e58e1004     	str	r1, [lr, #0x4]
   3e6d0: e2830070     	add	r0, r3, #112
   3e6d4: e58e2008     	str	r2, [lr, #0x8]
   3e6d8: e1a0100d     	mov	r1, sp
   3e6dc: e58d5004     	str	r5, [sp, #0x4]
   3e6e0: eb0005ca     	bl	0x3fe10
   3e6e4: eaffff4b     	b	0x3e418
   3e6e8: e51fe0b4     	ldr	lr, [pc, #-0xb4]        @ 0x3e63c
   3e6ec: e28d8010     	add	r8, sp, #16
   3e6f0: e1a0c008     	mov	r12, r8
   3e6f4: f2c00010     	vmov.i32	d16, #0x0
   3e6f8: e3a04000     	mov	r4, #0
   3e6fc: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3e700: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3e704: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3e708: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3e70c: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3e710: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3e714: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   3e718: e88c000f     	stm	r12, {r0, r1, r2, r3}
   3e71c: e3a00040     	mov	r0, #64
   3e720: edcd0b00     	vstr	d16, [sp]
   3e724: e58d4008     	str	r4, [sp, #0x8]
   3e728: ebff5c77     	bl	0x1590c     @ imm = #-0x28e24 ; _Znwj
   3e72c: e1a04008     	mov	r4, r8
   3e730: e28d9050     	add	r9, sp, #80
   3e734: e1a0e000     	mov	lr, r0
   3e738: e2805040     	add	r5, r0, #64
   3e73c: e58d0000     	str	r0, [sp]
   3e740: e58d5008     	str	r5, [sp, #0x8]
   3e744: e1a0c004     	mov	r12, r4
   3e748: e28ee010     	add	lr, lr, #16
   3e74c: e2844010     	add	r4, r4, #16
   3e750: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   3e754: e50e0010     	str	r0, [lr, #-0x10]
   3e758: e50e100c     	str	r1, [lr, #-0xc]
   3e75c: e50e2008     	str	r2, [lr, #-0x8]
   3e760: e50e3004     	str	r3, [lr, #-0x4]
   3e764: e15c0009     	cmp	r12, r9
   3e768: 1afffff5     	bne	0x3e744
   3e76c: e59600e8     	ldr	r0, [r6, #0xe8]
   3e770: e1a0100d     	mov	r1, sp
   3e774: e58d5004     	str	r5, [sp, #0x4]
   3e778: e2800070     	add	r0, r0, #112
   3e77c: eb0005a3     	bl	0x3fe10
   3e780: eaffff24     	b	0x3e418
   3e784: e59500bc     	ldr	r0, [r5, #0xbc]
   3e788: e1510000     	cmp	r1, r0
   3e78c: 1affff4a     	bne	0x3e4bc
   3e790: e0822003     	add	r2, r2, r3
   3e794: e2822040     	add	r2, r2, #64
   3e798: e1a00009     	mov	r0, r9
   3e79c: eb000620     	bl	0x40024
   3e7a0: e59630e8     	ldr	r3, [r6, #0xe8]
   3e7a4: e59510b8     	ldr	r1, [r5, #0xb8]
   3e7a8: e59320b0     	ldr	r2, [r3, #0xb0]
   3e7ac: eaffff36     	b	0x3e48c
   3e7b0: e0822104     	add	r2, r2, r4, lsl #2
   3e7b4: eafffff6     	b	0x3e794
   3e7b8: e0410003     	sub	r0, r1, r3
   3e7bc: e1a00140     	asr	r0, r0, #2
   3e7c0: e3500002     	cmp	r0, #2
   3e7c4: 0a000016     	beq	0x3e824
   3e7c8: e3500003     	cmp	r0, #3
   3e7cc: 0a00000e     	beq	0x3e80c
   3e7d0: e3500001     	cmp	r0, #1
   3e7d4: 11a03001     	movne	r3, r1
   3e7d8: 1affff58     	bne	0x3e540
   3e7dc: ed937a00     	vldr	s14, [r3]
   3e7e0: eef17a00     	vmov.f32	s15, #4.000000e+00
   3e7e4: eeb47a67     	vcmp.f32	s14, s15
   3e7e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e7ec: 11a03001     	movne	r3, r1
   3e7f0: eaffff52     	b	0x3e540
   3e7f4: e2833008     	add	r3, r3, #8
   3e7f8: eaffff50     	b	0x3e540
   3e7fc: e2833004     	add	r3, r3, #4
   3e800: eaffff4e     	b	0x3e540
   3e804: e283300c     	add	r3, r3, #12
   3e808: eaffff4c     	b	0x3e540
   3e80c: ed937a00     	vldr	s14, [r3]
   3e810: eef17a00     	vmov.f32	s15, #4.000000e+00
   3e814: eeb47a67     	vcmp.f32	s14, s15
   3e818: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e81c: 0affff47     	beq	0x3e540
   3e820: e2833004     	add	r3, r3, #4
   3e824: ed937a00     	vldr	s14, [r3]
   3e828: eef17a00     	vmov.f32	s15, #4.000000e+00
   3e82c: eeb47a67     	vcmp.f32	s14, s15
   3e830: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e834: 0affff41     	beq	0x3e540
   3e838: e2833004     	add	r3, r3, #4
   3e83c: eaffffe6     	b	0x3e7dc
   3e840: ee377a67     	vsub.f32	s14, s14, s15
   3e844: ee776aa7     	vadd.f32	s13, s15, s15
   3e848: ed1f6a84     	vldr	s12, [pc, #-528]        @ 0x3e640 ; float 0.899999976158
   3e84c: e2865a29     	add	r5, r6, #167936
   3e850: e2858d3d     	add	r8, r5, #3904
   3e854: e2863ba7     	add	r3, r6, #171008
   3e858: e2888004     	add	r8, r8, #4
   3e85c: e2859d3d     	add	r9, r5, #3904
   3e860: ee270a07     	vmul.f32	s0, s14, s14
   3e864: eef46ac6     	vcmpe.f32	s13, s12
   3e868: eea70aa7     	vfma.f32	s0, s15, s15
   3e86c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e870: eeb50a40     	vcmp.f32	s0, #0
   3e874: 5ef06a46     	vmovpl.f32	s13, s12
   3e878: eeb18ac0     	vsqrt.f32	s16, s0
   3e87c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3e880: edc36acf     	vstr	s13, [r3, #828]
   3e884: edc97a00     	vstr	s15, [r9]
   3e888: ed887a00     	vstr	s14, [r8]
   3e88c: 5afffeab     	bpl	0x3e340
   3e890: ebff5cb6     	bl	0x15b70    @ imm = #-0x28d28 ; sqrtf
   3e894: e59630e8     	ldr	r3, [r6, #0xe8]
   3e898: ed997a00     	vldr	s14, [r9]
   3e89c: edd87a00     	vldr	s15, [r8]
   3e8a0: e59310b0     	ldr	r1, [r3, #0xb0]
   3e8a4: eec76a08     	vdiv.f32	s13, s14, s16
   3e8a8: edd30a2a     	vldr	s1, [r3, #168]
   3e8ac: ee876a88     	vdiv.f32	s12, s15, s16
   3e8b0: e2812901     	add	r2, r1, #16384
   3e8b4: eafffe1f     	b	0x3e138
   3e8b8: e59d0000     	ldr	r0, [sp]
   3e8bc: e3500000     	cmp	r0, #0
   3e8c0: 0a000000     	beq	0x3e8c8
   3e8c4: ebff5d5d     	bl	0x15e40    @ imm = #-0x28a8c ; _ZdlPv
   3e8c8: ebff5da4     	bl	0x15f60    @ imm = #-0x28970 ; __cxa_end_cleanup
   3e8cc: e28d8010     	add	r8, sp, #16
   3e8d0: eafffed4     	b	0x3e428
   3e8d4: e59d0010     	ldr	r0, [sp, #0x10]
   3e8d8: e28d3018     	add	r3, sp, #24
   3e8dc: e1500003     	cmp	r0, r3
   3e8e0: 1afffff7     	bne	0x3e8c4
   3e8e4: eafffff7     	b	0x3e8c8
   3e8e8: eafffff2     	b	0x3e8b8
   3e8ec: eafffff1     	b	0x3e8b8
   3e8f0: eafffff0     	b	0x3e8b8
   3e8f4: eaffffef     	b	0x3e8b8
   3e8f8: eaffffee     	b	0x3e8b8
   3e8fc: eaffffed     	b	0x3e8b8
   3e900: eaffffec     	b	0x3e8b8
