; lubadh::Channel::setTime(int, bool)
; VA 0x37a44 size 1516

   37a44: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   37a48: e2806a2a     	add	r6, r0, #172032
   37a4c: e6ec5011     	usat	r5, #0xc, r1
   37a50: e1a04000     	mov	r4, r0
   37a54: ed2d8b02     	vpush	{d8}
   37a58: e59634ac     	ldr	r3, [r6, #0x4ac]
   37a5c: e0453003     	sub	r3, r5, r3
   37a60: e3530000     	cmp	r3, #0
   37a64: b2633000     	rsblt	r3, r3, #0
   37a68: e353001d     	cmp	r3, #29
   37a6c: da0000bb     	ble	0x37d60
   37a70: e5903104     	ldr	r3, [r0, #0x104]
   37a74: e3530000     	cmp	r3, #0
   37a78: 03822001     	orreq	r2, r2, #1
   37a7c: e3520000     	cmp	r2, #0
   37a80: 0a0000b9     	beq	0x37d6c
   37a84: e59634d0     	ldr	r3, [r6, #0x4d0]
   37a88: e3530005     	cmp	r3, #5
   37a8c: 979ff103     	ldrls	pc, [pc, r3, lsl #2]
   37a90: ea0000af     	b	0x37d54
   37a94: 3c 7e 03 00  	.word	0x00037e3c
   37a98: a4 7e 03 00  	.word	0x00037ea4
   37a9c: e0 7e 03 00  	.word	0x00037ee0
   37aa0: 14 7f 03 00  	.word	0x00037f14
   37aa4: 28 7f 03 00  	.word	0x00037f28
   37aa8: 3c 7f 03 00  	.word	0x00037f3c
   37aac: ee075a90     	vmov	s15, r5
   37ab0: e2833901     	add	r3, r3, #16384
   37ab4: eef76a00     	vmov.f32	s13, #1.000000e+00
   37ab8: e58654bc     	str	r5, [r6, #0x4bc]
   37abc: eeb86ae7     	vcvt.f32.s32	s12, s15
   37ac0: eddf7af5     	vldr	s15, [pc, #980]         @ 0x37e9c ; float 4095
   37ac4: ee867a27     	vdiv.f32	s14, s12, s15
   37ac8: edd37a9c     	vldr	s15, [r3, #624]
   37acc: ee677a27     	vmul.f32	s15, s14, s15
   37ad0: ed827a2a     	vstr	s14, [r2, #168]
   37ad4: eef47ae6     	vcmpe.f32	s15, s13
   37ad8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37adc: 5ef07a66     	vmovpl.f32	s15, s13
   37ae0: 5a000003     	bpl	0x37af4
   37ae4: eef57ac0     	vcmpe.f32	s15, #0
   37ae8: ed9f7aea     	vldr	s14, [pc, #936]         @ 0x37e98 ; float 0
   37aec: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37af0: def07a47     	vmovle.f32	s15, s14
   37af4: e2840c4a     	add	r0, r4, #18944
   37af8: eeb77a00     	vmov.f32	s14, #1.000000e+00
   37afc: e2841901     	add	r1, r4, #16384
   37b00: edc07a0a     	vstr	s15, [r0, #40]
   37b04: edd37a9d     	vldr	s15, [r3, #628]
   37b08: edd26a2a     	vldr	s13, [r2, #168]
   37b0c: ee677aa6     	vmul.f32	s15, s15, s13
   37b10: eef47ac7     	vcmpe.f32	s15, s14
   37b14: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37b18: 5ef07a47     	vmovpl.f32	s15, s14
   37b1c: 5a000003     	bpl	0x37b30
   37b20: eef57ac0     	vcmpe.f32	s15, #0
   37b24: ed9f7adb     	vldr	s14, [pc, #876]         @ 0x37e98 ; float 0
   37b28: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37b2c: def07a47     	vmovle.f32	s15, s14
   37b30: e2811ea2     	add	r1, r1, #2592
   37b34: eeb77a00     	vmov.f32	s14, #1.000000e+00
   37b38: edc17a03     	vstr	s15, [r1, #12]
   37b3c: edd37a9f     	vldr	s15, [r3, #636]
   37b40: edd26a2a     	vldr	s13, [r2, #168]
   37b44: ee677aa6     	vmul.f32	s15, s15, s13
   37b48: eef47ac7     	vcmpe.f32	s15, s14
   37b4c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37b50: 5ef07a47     	vmovpl.f32	s15, s14
   37b54: 5a000003     	bpl	0x37b68
   37b58: eef57ac0     	vcmpe.f32	s15, #0
   37b5c: ed9f7acd     	vldr	s14, [pc, #820]         @ 0x37e98 ; float 0
   37b60: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37b64: def07a47     	vmovle.f32	s15, s14
   37b68: e2840d67     	add	r0, r4, #6592
   37b6c: eeb77a00     	vmov.f32	s14, #1.000000e+00
   37b70: e2841a01     	add	r1, r4, #4096
   37b74: edc07a0e     	vstr	s15, [r0, #56]
   37b78: edd37a9e     	vldr	s15, [r3, #632]
   37b7c: edd26a2a     	vldr	s13, [r2, #168]
   37b80: ee677aa6     	vmul.f32	s15, s15, s13
   37b84: eef47ac7     	vcmpe.f32	s15, s14
   37b88: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37b8c: 5ef07a47     	vmovpl.f32	s15, s14
   37b90: 5a000003     	bpl	0x37ba4
   37b94: eef57ac0     	vcmpe.f32	s15, #0
   37b98: ed9f7abe     	vldr	s14, [pc, #760]         @ 0x37e98 ; float 0
   37b9c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37ba0: def07a47     	vmovle.f32	s15, s14
   37ba4: e2810ea5     	add	r0, r1, #2640
   37ba8: eeb77a00     	vmov.f32	s14, #1.000000e+00
   37bac: edc07a03     	vstr	s15, [r0, #12]
   37bb0: edd37aa0     	vldr	s15, [r3, #640]
   37bb4: edd26a2a     	vldr	s13, [r2, #168]
   37bb8: ee677aa6     	vmul.f32	s15, s15, s13
   37bbc: eef47ac7     	vcmpe.f32	s15, s14
   37bc0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37bc4: 5ef07a47     	vmovpl.f32	s15, s14
   37bc8: 5a000003     	bpl	0x37bdc
   37bcc: eef57ac0     	vcmpe.f32	s15, #0
   37bd0: ed9f7ab0     	vldr	s14, [pc, #704]         @ 0x37e98 ; float 0
   37bd4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37bd8: def07a47     	vmovle.f32	s15, s14
   37bdc: e2811ea6     	add	r1, r1, #2656
   37be0: eeb77a00     	vmov.f32	s14, #1.000000e+00
   37be4: edc17a00     	vstr	s15, [r1]
   37be8: edd37a9f     	vldr	s15, [r3, #636]
   37bec: edd26a2a     	vldr	s13, [r2, #168]
   37bf0: ee677aa6     	vmul.f32	s15, s15, s13
   37bf4: eef47ac7     	vcmpe.f32	s15, s14
   37bf8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37bfc: 5ef07a47     	vmovpl.f32	s15, s14
   37c00: 5a000003     	bpl	0x37c14
   37c04: eef57ac0     	vcmpe.f32	s15, #0
   37c08: ed9f7aa2     	vldr	s14, [pc, #648]         @ 0x37e98 ; float 0
   37c0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37c10: def07a47     	vmovle.f32	s15, s14
   37c14: e2841a03     	add	r1, r4, #12288
   37c18: eeb77a00     	vmov.f32	s14, #1.000000e+00
   37c1c: edc17a81     	vstr	s15, [r1, #516]
   37c20: edd37a9e     	vldr	s15, [r3, #632]
   37c24: edd26a2a     	vldr	s13, [r2, #168]
   37c28: ee677aa6     	vmul.f32	s15, s15, s13
   37c2c: eef47ac7     	vcmpe.f32	s15, s14
   37c30: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37c34: 5ef07a47     	vmovpl.f32	s15, s14
   37c38: 5a000003     	bpl	0x37c4c
   37c3c: eef57ac0     	vcmpe.f32	s15, #0
   37c40: ed9f7a94     	vldr	s14, [pc, #592]         @ 0x37e98 ; float 0
   37c44: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37c48: def07a47     	vmovle.f32	s15, s14
   37c4c: edc17a9a     	vstr	s15, [r1, #616]
   37c50: eeb77a00     	vmov.f32	s14, #1.000000e+00
   37c54: edd37aa0     	vldr	s15, [r3, #640]
   37c58: edd26a2a     	vldr	s13, [r2, #168]
   37c5c: ee677aa6     	vmul.f32	s15, s15, s13
   37c60: eef47ac7     	vcmpe.f32	s15, s14
   37c64: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37c68: 5ef07a47     	vmovpl.f32	s15, s14
   37c6c: 5a000003     	bpl	0x37c80
   37c70: eef57ac0     	vcmpe.f32	s15, #0
   37c74: ed9f7a87     	vldr	s14, [pc, #540]         @ 0x37e98 ; float 0
   37c78: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37c7c: def07a47     	vmovle.f32	s15, s14
   37c80: edc17a9b     	vstr	s15, [r1, #620]
   37c84: eef77a00     	vmov.f32	s15, #1.000000e+00
   37c88: ed930aa1     	vldr	s0, [r3, #644]
   37c8c: e2840c62     	add	r0, r4, #25088
   37c90: e2800048     	add	r0, r0, #72
   37c94: eeb40ae7     	vcmpe.f32	s0, s15
   37c98: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37c9c: 5eb00a67     	vmovpl.f32	s0, s15
   37ca0: 5a000003     	bpl	0x37cb4
   37ca4: eeb50ac0     	vcmpe.f32	s0, #0
   37ca8: eddf7a7a     	vldr	s15, [pc, #488]         @ 0x37e98 ; float 0
   37cac: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37cb0: deb00a67     	vmovle.f32	s0, s15
   37cb4: edd30aa2     	vldr	s1, [r3, #648]
   37cb8: eef77a00     	vmov.f32	s15, #1.000000e+00
   37cbc: ed927a2a     	vldr	s14, [r2, #168]
   37cc0: ee600a87     	vmul.f32	s1, s1, s14
   37cc4: eef40ae7     	vcmpe.f32	s1, s15
   37cc8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37ccc: 5ef00a67     	vmovpl.f32	s1, s15
   37cd0: 5a000003     	bpl	0x37ce4
   37cd4: eef50ac0     	vcmpe.f32	s1, #0
   37cd8: eddf7a6e     	vldr	s15, [pc, #440]         @ 0x37e98 ; float 0
   37cdc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37ce0: def00a67     	vmovle.f32	s1, s15
   37ce4: eb0060bd     	bl	0x4ffe0
   37ce8: e59420e8     	ldr	r2, [r4, #0xe8]
   37cec: eeb77a00     	vmov.f32	s14, #1.000000e+00
   37cf0: e59230b0     	ldr	r3, [r2, #0xb0]
   37cf4: edd26a2a     	vldr	s13, [r2, #168]
   37cf8: e2833901     	add	r3, r3, #16384
   37cfc: edd37aa6     	vldr	s15, [r3, #664]
   37d00: ee667aa7     	vmul.f32	s15, s13, s15
   37d04: eef47ac7     	vcmpe.f32	s15, s14
   37d08: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37d0c: 5a00009c     	bpl	0x37f84
   37d10: eef57ac0     	vcmpe.f32	s15, #0
   37d14: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37d18: ca0000a8     	bgt	0x37fc0
   37d1c: e2842ba7     	add	r2, r4, #171008
   37d20: f2c00010     	vmov.i32	d16, #0x0
   37d24: eef07a47     	vmov.f32	s15, s14
   37d28: e1a03002     	mov	r3, r2
   37d2c: e2847a29     	add	r7, r4, #167936
   37d30: e2822fcf     	add	r2, r2, #828
   37d34: ed9f6a57     	vldr	s12, [pc, #348]         @ 0x37e98 ; float 0
   37d38: f442078f     	vst1.32	{d16}, [r2]
   37d3c: ed837ad1     	vstr	s14, [r3, #836]
   37d40: e5971f5c     	ldr	r1, [r7, #0xf5c]
   37d44: e2872d3d     	add	r2, r7, #3904
   37d48: ed826a03     	vstr	s12, [r2, #12]
   37d4c: edc27a02     	vstr	s15, [r2, #8]
   37d50: edc16a0a     	vstr	s13, [r1, #40]
   37d54: e58654ac     	str	r5, [r6, #0x4ac]
   37d58: ecbd8b02     	vpop	{d8}
   37d5c: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   37d60: e3520000     	cmp	r2, #0
   37d64: 1affff46     	bne	0x37a84
   37d68: eafffffa     	b	0x37d58
   37d6c: e2433001     	sub	r3, r3, #1
   37d70: e3530001     	cmp	r3, #1
   37d74: 8afffff7     	bhi	0x37d58
   37d78: e59631f8     	ldr	r3, [r6, #0x1f8]
   37d7c: e5933000     	ldr	r3, [r3]
   37d80: e3530003     	cmp	r3, #3
   37d84: 0afffff3     	beq	0x37d58
   37d88: e590101c     	ldr	r1, [r0, #0x1c]
   37d8c: e2813a2a     	add	r3, r1, #172032
   37d90: e59331f8     	ldr	r3, [r3, #0x1f8]
   37d94: e5933000     	ldr	r3, [r3]
   37d98: e3530003     	cmp	r3, #3
   37d9c: 0affffed     	beq	0x37d58
   37da0: e596318c     	ldr	r3, [r6, #0x18c]
   37da4: e5933000     	ldr	r3, [r3]
   37da8: e3530003     	cmp	r3, #3
   37dac: 0affffe9     	beq	0x37d58
   37db0: e59634a8     	ldr	r3, [r6, #0x4a8]
   37db4: e3a0c001     	mov	r12, #1
   37db8: ee073a10     	vmov	s14, r3
   37dbc: eddf7a36     	vldr	s15, [pc, #216]         @ 0x37e9c ; float 4095
   37dc0: e59020e8     	ldr	r2, [r0, #0xe8]
   37dc4: eeb87ac7     	vcvt.f32.s32	s14, s14
   37dc8: e5900020     	ldr	r0, [r0, #0x20]
   37dcc: e592e070     	ldr	lr, [r2, #0x70]
   37dd0: e2800915     	add	r0, r0, #344064
   37dd4: e5923074     	ldr	r3, [r2, #0x74]
   37dd8: eec76a27     	vdiv.f32	s13, s14, s15
   37ddc: e5c2c07c     	strb	r12, [r2, #0x7c]
   37de0: e043300e     	sub	r3, r3, lr
   37de4: e1a03143     	asr	r3, r3, #2
   37de8: ee073a90     	vmov	s15, r3
   37dec: eef87a67     	vcvt.f32.u32	s15, s15
   37df0: ee677aa6     	vmul.f32	s15, s15, s13
   37df4: eefc7ae7     	vcvt.u32.f32	s15, s15
   37df8: ee173a90     	vmov	r3, s15
   37dfc: e79e3103     	ldr	r3, [lr, r3, lsl #2]
   37e00: e5823080     	str	r3, [r2, #0x80]
   37e04: e2533000     	subs	r3, r3, #0
   37e08: 13a03001     	movne	r3, #1
   37e0c: e5c2306c     	strb	r3, [r2, #0x6c]
   37e10: e584c110     	str	r12, [r4, #0x110]
   37e14: e5d03abc     	ldrb	r3, [r0, #0xabc]
   37e18: e3530000     	cmp	r3, #0
   37e1c: e2843a29     	add	r3, r4, #167936
   37e20: 1581c110     	strne	r12, [r1, #0x110]
   37e24: e5933f5c     	ldr	r3, [r3, #0xf5c]
   37e28: e5921080     	ldr	r1, [r2, #0x80]
   37e2c: e5d42090     	ldrb	r2, [r4, #0x90]
   37e30: e5c32038     	strb	r2, [r3, #0x38]
   37e34: e583103c     	str	r1, [r3, #0x3c]
   37e38: eaffffc5     	b	0x37d54
   37e3c: e59634a8     	ldr	r3, [r6, #0x4a8]
   37e40: e2841a29     	add	r1, r4, #167936
   37e44: ee073a10     	vmov	s14, r3
   37e48: eddf7a13     	vldr	s15, [pc, #76]          @ 0x37e9c ; float 4095
   37e4c: e59624e8     	ldr	r2, [r6, #0x4e8]
   37e50: eeb87ac7     	vcvt.f32.s32	s14, s14
   37e54: e59634ec     	ldr	r3, [r6, #0x4ec]
   37e58: e5911f5c     	ldr	r1, [r1, #0xf5c]
   37e5c: e0433002     	sub	r3, r3, r2
   37e60: e5d4028a     	ldrb	r0, [r4, #0x28a]
   37e64: e5c40289     	strb	r0, [r4, #0x289]
   37e68: eec76a27     	vdiv.f32	s13, s14, s15
   37e6c: e1a03143     	asr	r3, r3, #2
   37e70: ee073a90     	vmov	s15, r3
   37e74: eef87a67     	vcvt.f32.u32	s15, s15
   37e78: ee677aa6     	vmul.f32	s15, s15, s13
   37e7c: eefc7ae7     	vcvt.u32.f32	s15, s15
   37e80: ee173a90     	vmov	r3, s15
   37e84: e7923103     	ldr	r3, [r2, r3, lsl #2]
   37e88: e58634d8     	str	r3, [r6, #0x4d8]
   37e8c: e58654b0     	str	r5, [r6, #0x4b0]
   37e90: e5813018     	str	r3, [r1, #0x18]
   37e94: eaffffae     	b	0x37d54
   37e98: 00 00 00 00  	.word	0x00000000
   37e9c: 00 f0 7f 45  	.word	0x457ff000
   37ea0: 66 66 66 3f  	.word	0x3f666666
   37ea4: ee075a90     	vmov	s15, r5
   37ea8: ed1f7a05     	vldr	s14, [pc, #-20]         @ 0x37e9c ; float 4095
   37eac: e59420e8     	ldr	r2, [r4, #0xe8]
   37eb0: e2844a29     	add	r4, r4, #167936
   37eb4: eef86ae7     	vcvt.f32.s32	s13, s15
   37eb8: e58654b8     	str	r5, [r6, #0x4b8]
   37ebc: e59230b0     	ldr	r3, [r2, #0xb0]
   37ec0: e5941f5c     	ldr	r1, [r4, #0xf5c]
   37ec4: eec67a87     	vdiv.f32	s15, s13, s14
   37ec8: e2833901     	add	r3, r3, #16384
   37ecc: ed937a9b     	vldr	s14, [r3, #620]
   37ed0: ee677a87     	vmul.f32	s15, s15, s14
   37ed4: edc27a29     	vstr	s15, [r2, #164]
   37ed8: edc17a07     	vstr	s15, [r1, #28]
   37edc: eaffff9c     	b	0x37d54
   37ee0: e59420e8     	ldr	r2, [r4, #0xe8]
   37ee4: e59230b0     	ldr	r3, [r2, #0xb0]
   37ee8: e593102c     	ldr	r1, [r3, #0x2c]
   37eec: e3510001     	cmp	r1, #1
   37ef0: 0a000016     	beq	0x37f50
   37ef4: e3510002     	cmp	r1, #2
   37ef8: 0afffeeb     	beq	0x37aac
   37efc: e3510000     	cmp	r1, #0
   37f00: 02844a29     	addeq	r4, r4, #167936
   37f04: 058654b4     	streq	r5, [r6, #0x4b4]
   37f08: 05943f5c     	ldreq	r3, [r4, #0xf5c]
   37f0c: 05835020     	streq	r5, [r3, #0x20]
   37f10: eaffff8f     	b	0x37d54
   37f14: e2844a29     	add	r4, r4, #167936
   37f18: e58654c4     	str	r5, [r6, #0x4c4]
   37f1c: e5943f5c     	ldr	r3, [r4, #0xf5c]
   37f20: e583502c     	str	r5, [r3, #0x2c]
   37f24: eaffff8a     	b	0x37d54
   37f28: e2844a29     	add	r4, r4, #167936
   37f2c: e58654c8     	str	r5, [r6, #0x4c8]
   37f30: e5943f5c     	ldr	r3, [r4, #0xf5c]
   37f34: e5835030     	str	r5, [r3, #0x30]
   37f38: eaffff85     	b	0x37d54
   37f3c: e2844a29     	add	r4, r4, #167936
   37f40: e58654cc     	str	r5, [r6, #0x4cc]
   37f44: e5943f5c     	ldr	r3, [r4, #0xf5c]
   37f48: e5835034     	str	r5, [r3, #0x34]
   37f4c: eaffff80     	b	0x37d54
   37f50: e58654c0     	str	r5, [r6, #0x4c0]
   37f54: ee075a10     	vmov	s14, r5
   37f58: ed5f6a31     	vldr	s13, [pc, #-196]        @ 0x37e9c ; float 4095
   37f5c: e2844a29     	add	r4, r4, #167936
   37f60: edd37a05     	vldr	s15, [r3, #20]
   37f64: eeb87ac7     	vcvt.f32.s32	s14, s14
   37f68: e5943f5c     	ldr	r3, [r4, #0xf5c]
   37f6c: eef87ae7     	vcvt.f32.s32	s15, s15
   37f70: ee677a87     	vmul.f32	s15, s15, s14
   37f74: ee877aa6     	vdiv.f32	s14, s15, s13
   37f78: ed827a2b     	vstr	s14, [r2, #172]
   37f7c: ed837a09     	vstr	s14, [r3, #36]
   37f80: eaffff73     	b	0x37d54
   37f84: e2847a29     	add	r7, r4, #167936
   37f88: e2844ba7     	add	r4, r4, #171008
   37f8c: e2873d3d     	add	r3, r7, #3904
   37f90: e3a02000     	mov	r2, #0
   37f94: eef07a47     	vmov.f32	s15, s14
   37f98: eeb08a47     	vmov.f32	s16, s14
   37f9c: e3060666     	movw	r0, #0x6666
   37fa0: e3430f66     	movt	r0, #0x3f66
   37fa4: e584033c     	str	r0, [r4, #0x33c]
   37fa8: ed837a00     	vstr	s14, [r3]
   37fac: ee072a10     	vmov	s14, r2
   37fb0: e5832004     	str	r2, [r3, #0x4]
   37fb4: ee876a88     	vdiv.f32	s12, s15, s16
   37fb8: eec77a08     	vdiv.f32	s15, s14, s16
   37fbc: eaffff5f     	b	0x37d40
   37fc0: ee377a67     	vsub.f32	s14, s14, s15
   37fc4: ee376aa7     	vadd.f32	s12, s15, s15
   37fc8: ed5f5a4c     	vldr	s11, [pc, #-304]        @ 0x37ea0 ; float 0.899999976158
   37fcc: e2847a29     	add	r7, r4, #167936
   37fd0: e2878d3d     	add	r8, r7, #3904
   37fd4: e2843ba7     	add	r3, r4, #171008
   37fd8: e2888004     	add	r8, r8, #4
   37fdc: e2879d3d     	add	r9, r7, #3904
   37fe0: ee270a07     	vmul.f32	s0, s14, s14
   37fe4: eeb46ae5     	vcmpe.f32	s12, s11
   37fe8: eea70aa7     	vfma.f32	s0, s15, s15
   37fec: eef1fa10     	vmrs	APSR_nzcv, fpscr
   37ff0: eeb50a40     	vcmp.f32	s0, #0
   37ff4: 5eb06a65     	vmovpl.f32	s12, s11
   37ff8: eeb18ac0     	vsqrt.f32	s16, s0
   37ffc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   38000: ed836acf     	vstr	s12, [r3, #828]
   38004: edc97a00     	vstr	s15, [r9]
   38008: ed887a00     	vstr	s14, [r8]
   3800c: 5affffe8     	bpl	0x37fb4
   38010: ebff76d6     	bl	0x15b70    @ imm = #-0x224a8 ; sqrtf
   38014: e59430e8     	ldr	r3, [r4, #0xe8]
   38018: edd97a00     	vldr	s15, [r9]
   3801c: ed987a00     	vldr	s14, [r8]
   38020: ee876a88     	vdiv.f32	s12, s15, s16
   38024: edd36a2a     	vldr	s13, [r3, #168]
   38028: eec77a08     	vdiv.f32	s15, s14, s16
   3802c: eaffff43     	b	0x37d40
