; lubadh::{lambda()#1}::operator()() const [clone .isra.0]
; VA 0x18814 size 2756

   18814: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   18818: e3a01000     	mov	r1, #0
   1881c: e304229c     	movw	r2, #0x429c
   18820: ed2d8b04     	vpush	{d8, d9}
   18824: e24dd034     	sub	sp, sp, #52
   18828: e1a05000     	mov	r5, r0
   1882c: ebfff550     	bl	0x15d74    @ imm = #-0x2ac0 ; memset
   18830: eddf0bf4     	vldr	d16, [pc, #976]         @ 0x18c08 ; float 5.30498947741e-313
   18834: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x18c10 ; float 8.48798316386e-314
   18838: e1a03005     	mov	r3, r5
   1883c: eddf2bf5     	vldr	d18, [pc, #980]         @ 0x18c18 ; float 3.16202013338e-322
   18840: eddf3bf6     	vldr	d19, [pc, #984]         @ 0x18c20 ; float 4.24399158193e-314
   18844: e2850020     	add	r0, r5, #32
   18848: f2c04050     	vmov.i32	q10, #0x0
   1884c: e2856044     	add	r6, r5, #68
   18850: e3a01000     	mov	r1, #0
   18854: e3a02c05     	mov	r2, #1280
   18858: f4402a8f     	vst1.32	{d18, d19}, [r0]
   1885c: e1a00006     	mov	r0, r6
   18860: e1a04001     	mov	r4, r1
   18864: f4434a8d     	vst1.32	{d20, d21}, [r3]!
   18868: e30ccccd     	movw	r12, #0xcccd
   1886c: e343cdcc     	movt	r12, #0x3dcc
   18870: f4430a8f     	vst1.32	{d16, d17}, [r3]
   18874: e1a08006     	mov	r8, r6
   18878: e1a09004     	mov	r9, r4
   1887c: e5852030     	str	r2, [r5, #0x30]
   18880: e3a02e1e     	mov	r2, #480
   18884: e585c034     	str	r12, [r5, #0x34]
   18888: ebfff539     	bl	0x15d74    @ imm = #-0x2b1c ; memset
   1888c: eddf2be5     	vldr	d18, [pc, #916]         @ 0x18c28 ; float -2.00000143424
   18890: eddf3be6     	vldr	d19, [pc, #920]         @ 0x18c30 ; float -3.0517599896e-05
   18894: e2853058     	add	r3, r5, #88
   18898: eddf0be6     	vldr	d16, [pc, #920]         @ 0x18c38 ; float 0.00781250183354
   1889c: eddf1be7     	vldr	d17, [pc, #924]         @ 0x18c40 ; float 512.00012207
   188a0: e1a07004     	mov	r7, r4
   188a4: f4462a8f     	vst1.32	{d18, d19}, [r6]
   188a8: f4430a8f     	vst1.32	{d16, d17}, [r3]
   188ac: ea00000e     	b	0x188ec
   188b0: e1a0b009     	mov	r11, r9
   188b4: ed988a00     	vldr	s16, [r8]
   188b8: e1a0a007     	mov	r10, r7
   188bc: ecab8a01     	vstmia	r11!, {s16}
   188c0: eeb18a48     	vneg.f32	s16, s16
   188c4: e154000b     	cmp	r4, r11
   188c8: 0a000025     	beq	0x18964
   188cc: e1a0900b     	mov	r9, r11
   188d0: e1a0700a     	mov	r7, r10
   188d4: eca98a01     	vstmia	r9!, {s16}
   188d8: e2853f89     	add	r3, r5, #548
   188dc: e2888004     	add	r8, r8, #4
   188e0: e1580003     	cmp	r8, r3
   188e4: e58d300c     	str	r3, [sp, #0xc]
   188e8: 0a000058     	beq	0x18a50
   188ec: e1540009     	cmp	r4, r9
   188f0: 1affffee     	bne	0x188b0
   188f4: e0449007     	sub	r9, r4, r7
   188f8: e1a04149     	asr	r4, r9, #2
   188fc: e374021e     	cmn	r4, #-536870911
   18900: 0a000251     	beq	0x1924c
   18904: e3540000     	cmp	r4, #0
   18908: 0a00003d     	beq	0x18a04
   1890c: e1540084     	cmp	r4, r4, lsl #1
   18910: e1a04084     	lsl	r4, r4, #1
   18914: 83e0410e     	mvnhi	r4, #-2147483645
   18918: 9a000031     	bls	0x189e4
   1891c: e1a00004     	mov	r0, r4
   18920: ebfff3f9     	bl	0x1590c     @ imm = #-0x301c ; _Znwj
   18924: e1a0a000     	mov	r10, r0
   18928: e0804004     	add	r4, r0, r4
   1892c: ed988a00     	vldr	s16, [r8]
   18930: e08a3009     	add	r3, r10, r9
   18934: e289b004     	add	r11, r9, #4
   18938: e3590000     	cmp	r9, #0
   1893c: e08ab00b     	add	r11, r10, r11
   18940: ed838a00     	vstr	s16, [r3]
   18944: ca00001f     	bgt	0x189c8
   18948: e3570000     	cmp	r7, #0
   1894c: 0affffdb     	beq	0x188c0
   18950: e1a00007     	mov	r0, r7
   18954: ebfff539     	bl	0x15e40    @ imm = #-0x2b1c ; _ZdlPv
   18958: eeb18a48     	vneg.f32	s16, s16
   1895c: e154000b     	cmp	r4, r11
   18960: 1affffd9     	bne	0x188cc
   18964: e044b00a     	sub	r11, r4, r10
   18968: e1a0414b     	asr	r4, r11, #2
   1896c: e374021e     	cmn	r4, #-536870911
   18970: 0a000238     	beq	0x19258
   18974: e3540000     	cmp	r4, #0
   18978: 0a000032     	beq	0x18a48
   1897c: e1540084     	cmp	r4, r4, lsl #1
   18980: e1a04084     	lsl	r4, r4, #1
   18984: 83e0410e     	mvnhi	r4, #-2147483645
   18988: 9a000026     	bls	0x18a28
   1898c: e1a00004     	mov	r0, r4
   18990: ebfff3dd     	bl	0x1590c     @ imm = #-0x308c ; _Znwj
   18994: e1a07000     	mov	r7, r0
   18998: e0804004     	add	r4, r0, r4
   1899c: e087300b     	add	r3, r7, r11
   189a0: e28b9004     	add	r9, r11, #4
   189a4: e35b0000     	cmp	r11, #0
   189a8: e0879009     	add	r9, r7, r9
   189ac: ed838a00     	vstr	s16, [r3]
   189b0: ca000015     	bgt	0x18a0c
   189b4: e35a0000     	cmp	r10, #0
   189b8: 0affffc6     	beq	0x188d8
   189bc: e1a0000a     	mov	r0, r10
   189c0: ebfff51e     	bl	0x15e40    @ imm = #-0x2b88 ; _ZdlPv
   189c4: eaffffc3     	b	0x188d8
   189c8: e1a02009     	mov	r2, r9
   189cc: e1a01007     	mov	r1, r7
   189d0: e1a0000a     	mov	r0, r10
   189d4: ebfff402     	bl	0x159e4    @ imm = #-0x2ff8 ; memmove
   189d8: e1a00007     	mov	r0, r7
   189dc: ebfff517     	bl	0x15e40    @ imm = #-0x2ba4 ; _ZdlPv
   189e0: eaffffdc     	b	0x18958
   189e4: e3540000     	cmp	r4, #0
   189e8: 01a0a004     	moveq	r10, r4
   189ec: 0affffce     	beq	0x1892c
   189f0: e3e0320e     	mvn	r3, #-536870912
   189f4: e1540003     	cmp	r4, r3
   189f8: 21a04003     	movhs	r4, r3
   189fc: e1a04104     	lsl	r4, r4, #2
   18a00: eaffffc5     	b	0x1891c
   18a04: e3a04004     	mov	r4, #4
   18a08: eaffffc3     	b	0x1891c
   18a0c: e1a0200b     	mov	r2, r11
   18a10: e1a0100a     	mov	r1, r10
   18a14: e1a00007     	mov	r0, r7
   18a18: ebfff3f1     	bl	0x159e4    @ imm = #-0x303c ; memmove
   18a1c: e1a0000a     	mov	r0, r10
   18a20: ebfff506     	bl	0x15e40    @ imm = #-0x2be8 ; _ZdlPv
   18a24: eaffffab     	b	0x188d8
   18a28: e3540000     	cmp	r4, #0
   18a2c: 01a07004     	moveq	r7, r4
   18a30: 0affffd9     	beq	0x1899c
   18a34: e3e0320e     	mvn	r3, #-536870912
   18a38: e1540003     	cmp	r4, r3
   18a3c: 21a04003     	movhs	r4, r3
   18a40: e1a04104     	lsl	r4, r4, #2
   18a44: eaffffd0     	b	0x1898c
   18a48: e3a04004     	mov	r4, #4
   18a4c: eaffffce     	b	0x1898c
   18a50: e1590007     	cmp	r9, r7
   18a54: 0a000039     	beq	0x18b40
   18a58: e0494007     	sub	r4, r9, r7
   18a5c: e1a01009     	mov	r1, r9
   18a60: e1a00007     	mov	r0, r7
   18a64: e3a03000     	mov	r3, #0
   18a68: e1a02144     	asr	r2, r4, #2
   18a6c: e2878004     	add	r8, r7, #4
   18a70: e16f2f12     	clz	r2, r2
   18a74: e262201f     	rsb	r2, r2, #31
   18a78: e1a02082     	lsl	r2, r2, #1
   18a7c: eb0058b8     	bl	0x2ed64
   18a80: e3540040     	cmp	r4, #64
   18a84: ca0000e9     	bgt	0x18e30
   18a88: e1580009     	cmp	r8, r9
   18a8c: 0a00012b     	beq	0x18f40
   18a90: e1a04008     	mov	r4, r8
   18a94: ea000009     	b	0x18ac0
   18a98: e1540007     	cmp	r4, r7
   18a9c: 0a000003     	beq	0x18ab0
   18aa0: e0442007     	sub	r2, r4, r7
   18aa4: e1a01007     	mov	r1, r7
   18aa8: e2870004     	add	r0, r7, #4
   18aac: ebfff3cc     	bl	0x159e4    @ imm = #-0x30d0 ; memmove
   18ab0: ed878a00     	vstr	s16, [r7]
   18ab4: e2844004     	add	r4, r4, #4
   18ab8: e1540009     	cmp	r4, r9
   18abc: 0a00011f     	beq	0x18f40
   18ac0: ed948a00     	vldr	s16, [r4]
   18ac4: edd77a00     	vldr	s15, [r7]
   18ac8: eeb48ae7     	vcmpe.f32	s16, s15
   18acc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18ad0: 4afffff0     	bmi	0x18a98
   18ad4: ed547a01     	vldr	s15, [r4, #-4]
   18ad8: e2443004     	sub	r3, r4, #4
   18adc: eeb48ae7     	vcmpe.f32	s16, s15
   18ae0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18ae4: 5a0001d1     	bpl	0x19230
   18ae8: e1a02003     	mov	r2, r3
   18aec: edc37a01     	vstr	s15, [r3, #4]
   18af0: ed737a01     	vldmdb	r3!, {s15}
   18af4: eeb48ae7     	vcmpe.f32	s16, s15
   18af8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18afc: 4afffff9     	bmi	0x18ae8
   18b00: ed828a00     	vstr	s16, [r2]
   18b04: eaffffea     	b	0x18ab4
   18b08: e1530009     	cmp	r3, r9
   18b0c: 0a00000b     	beq	0x18b40
   18b10: e2832008     	add	r2, r3, #8
   18b14: e1590002     	cmp	r9, r2
   18b18: 0a000007     	beq	0x18b3c
   18b1c: ecf27a01     	vldmia	r2!, {s15}
   18b20: ed937a00     	vldr	s14, [r3]
   18b24: eeb47a67     	vcmp.f32	s14, s15
   18b28: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18b2c: 1dc37a01     	vstrne	s15, [r3, #4]
   18b30: 12833004     	addne	r3, r3, #4
   18b34: e1590002     	cmp	r9, r2
   18b38: 1afffff7     	bne	0x18b1c
   18b3c: e2839004     	add	r9, r3, #4
   18b40: e3a02e1e     	mov	r2, #480
   18b44: e3a01000     	mov	r1, #0
   18b48: e1a00006     	mov	r0, r6
   18b4c: ebfff488     	bl	0x15d74    @ imm = #-0x2de0 ; memset
   18b50: e1590007     	cmp	r9, r7
   18b54: 0a000003     	beq	0x18b68
   18b58: e0492007     	sub	r2, r9, r7
   18b5c: e1a01007     	mov	r1, r7
   18b60: e1a00006     	mov	r0, r6
   18b64: ebfff536     	bl	0x16044    @ imm = #-0x2b28 ; memcpy
   18b68: e3570000     	cmp	r7, #0
   18b6c: 0a000001     	beq	0x18b78
   18b70: e1a00007     	mov	r0, r7
   18b74: ebfff4b1     	bl	0x15e40    @ imm = #-0x2d3c ; _ZdlPv
   18b78: f2c00010     	vmov.i32	d16, #0x0
   18b7c: e3a00e7a     	mov	r0, #1952
   18b80: e3a03000     	mov	r3, #0
   18b84: e58d3018     	str	r3, [sp, #0x18]
   18b88: edcd0b04     	vstr	d16, [sp, #16]
   18b8c: ebfff35e     	bl	0x1590c     @ imm = #-0x3288 ; _Znwj
   18b90: e59d7010     	ldr	r7, [sp, #0x10]
   18b94: e1a04000     	mov	r4, r0
   18b98: e59d2014     	ldr	r2, [sp, #0x14]
   18b9c: e0422007     	sub	r2, r2, r7
   18ba0: e3520000     	cmp	r2, #0
   18ba4: ca0001a3     	bgt	0x19238
   18ba8: e3570000     	cmp	r7, #0
   18bac: 1a0001a3     	bne	0x19240
   18bb0: f2c00010     	vmov.i32	d16, #0x0
   18bb4: ee814b90     	vdup.32	d17, r4
   18bb8: e28d2024     	add	r2, sp, #36
   18bbc: e2840e7a     	add	r0, r4, #1952
   18bc0: e28d1020     	add	r1, sp, #32
   18bc4: e58d0018     	str	r0, [sp, #0x18]
   18bc8: e28d0010     	add	r0, sp, #16
   18bcc: e3a03000     	mov	r3, #0
   18bd0: e34c3080     	movt	r3, #0xc080
   18bd4: f442078f     	vst1.32	{d16}, [r2]
   18bd8: edcd1b04     	vstr	d17, [sp, #16]
   18bdc: e58d3020     	str	r3, [sp, #0x20]
   18be0: e3a03000     	mov	r3, #0
   18be4: e58d302c     	str	r3, [sp, #0x2c]
   18be8: ebfffde7     	bl	0x1838c
   18bec: e59d8014     	ldr	r8, [sp, #0x14]
   18bf0: eef18a00     	vmov.f32	s17, #4.000000e+00
   18bf4: e59d4018     	ldr	r4, [sp, #0x18]
   18bf8: ed9f9a12     	vldr	s18, [pc, #72]          @ 0x18c48 ; float 4095
   18bfc: eddf9a12     	vldr	s19, [pc, #72]          @ 0x18c4c ; float 0
   18c00: e58d6004     	str	r6, [sp, #0x4]
   18c04: ea00001c     	b	0x18c7c
   18c08: 00 00 00 00  	.word	0x00000000
   18c0c: 19 00 00 00  	.word	0x00000019
   18c10: 00 00 00 00  	.word	0x00000000
   18c14: 04 00 00 00  	.word	0x00000004
   18c18: 40 00 00 00  	.word	0x00000040
   18c1c: 00 00 00 00  	.word	0x00000000
   18c20: 00 00 00 00  	.word	0x00000000
   18c24: 02 00 00 00  	.word	0x00000002
   18c28: 00 00 80 c0  	.word	0xc0800000
   18c2c: 00 00 00 c0  	.word	0xc0000000
   18c30: 00 00 80 bf  	.word	0xbf800000
   18c34: 00 00 00 bf  	.word	0xbf000000
   18c38: 00 00 00 3f  	.word	0x3f000000
   18c3c: 00 00 80 3f  	.word	0x3f800000
   18c40: 00 00 00 40  	.word	0x40000000
   18c44: 00 00 80 40  	.word	0x40800000
   18c48: 00 f0 7f 45  	.word	0x457ff000
   18c4c: 00 00 00 00  	.word	0x00000000
   18c50: 08 22 07 00  	.word	0x00072208
   18c54: e59d300c     	ldr	r3, [sp, #0xc]
   18c58: e2888010     	add	r8, r8, #16
   18c5c: e59d2004     	ldr	r2, [sp, #0x4]
   18c60: ed088a04     	vstr	s16, [r8, #-16]
   18c64: e508600c     	str	r6, [r8, #-0xc]
   18c68: e1530002     	cmp	r3, r2
   18c6c: ed487a02     	vstr	s15, [r8, #-8]
   18c70: e5087004     	str	r7, [r8, #-0x4]
   18c74: e58d8014     	str	r8, [sp, #0x14]
   18c78: 0a000037     	beq	0x18d5c
   18c7c: e59d3004     	ldr	r3, [sp, #0x4]
   18c80: eeb47a00     	vmov.f32	s14, #1.250000e-01
   18c84: e3002fff     	movw	r2, #0xfff
   18c88: ecb38a01     	vldmia	r3!, {s16}
   18c8c: ee787a28     	vadd.f32	s15, s16, s17
   18c90: e58d3004     	str	r3, [sp, #0x4]
   18c94: ee677a87     	vmul.f32	s15, s15, s14
   18c98: eeb07a69     	vmov.f32	s14, s19
   18c9c: eea77a89     	vfma.f32	s14, s15, s18
   18ca0: eefd7ac7     	vcvt.s32.f32	s15, s14
   18ca4: ee173a90     	vmov	r3, s15
   18ca8: edcd7a02     	vstr	s15, [sp, #8]
   18cac: e2436032     	sub	r6, r3, #50
   18cb0: e2837032     	add	r7, r3, #50
   18cb4: e1560002     	cmp	r6, r2
   18cb8: a1a06002     	movge	r6, r2
   18cbc: e1570002     	cmp	r7, r2
   18cc0: a1a07002     	movge	r7, r2
   18cc4: e1580004     	cmp	r8, r4
   18cc8: e1c66fc6     	bic	r6, r6, r6, asr #31
   18ccc: e1c77fc7     	bic	r7, r7, r7, asr #31
   18cd0: 1affffdf     	bne	0x18c54
   18cd4: e59da010     	ldr	r10, [sp, #0x10]
   18cd8: e048900a     	sub	r9, r8, r10
   18cdc: e1a04249     	asr	r4, r9, #4
   18ce0: e374037e     	cmn	r4, #-134217727
   18ce4: 0a00015e     	beq	0x19264
   18ce8: e3540000     	cmp	r4, #0
   18cec: 0a00007d     	beq	0x18ee8
   18cf0: e1540084     	cmp	r4, r4, lsl #1
   18cf4: e1a04084     	lsl	r4, r4, #1
   18cf8: 83e0413e     	mvnhi	r4, #-2147483633
   18cfc: 9a000071     	bls	0x18ec8
   18d00: e1a00004     	mov	r0, r4
   18d04: ebfff300     	bl	0x1590c     @ imm = #-0x3400 ; _Znwj
   18d08: e1a0b000     	mov	r11, r0
   18d0c: e0804004     	add	r4, r0, r4
   18d10: e08b2009     	add	r2, r11, r9
   18d14: e2893010     	add	r3, r9, #16
   18d18: e08b8003     	add	r8, r11, r3
   18d1c: e59d3008     	ldr	r3, [sp, #0x8]
   18d20: e3590000     	cmp	r9, #0
   18d24: e5826004     	str	r6, [r2, #0x4]
   18d28: e5823008     	str	r3, [r2, #0x8]
   18d2c: e582700c     	str	r7, [r2, #0xc]
   18d30: ed828a00     	vstr	s16, [r2]
   18d34: ca00005c     	bgt	0x18eac
   18d38: e35a0000     	cmp	r10, #0
   18d3c: 1a00005e     	bne	0x18ebc
   18d40: e59d300c     	ldr	r3, [sp, #0xc]
   18d44: e59d2004     	ldr	r2, [sp, #0x4]
   18d48: e58db010     	str	r11, [sp, #0x10]
   18d4c: e1530002     	cmp	r3, r2
   18d50: e58d8014     	str	r8, [sp, #0x14]
   18d54: e58d4018     	str	r4, [sp, #0x18]
   18d58: 1affffc7     	bne	0x18c7c
   18d5c: e30231f8     	movw	r3, #0x21f8
   18d60: e3403007     	movt	r3, #0x7
   18d64: e28dc020     	add	r12, sp, #32
   18d68: e893000f     	ldm	r3, {r0, r1, r2, r3}
   18d6c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   18d70: e28d0010     	add	r0, sp, #16
   18d74: e1a0100c     	mov	r1, r12
   18d78: ebfffd83     	bl	0x1838c
   18d7c: e59d4010     	ldr	r4, [sp, #0x10]
   18d80: e59d6014     	ldr	r6, [sp, #0x14]
   18d84: e1540006     	cmp	r4, r6
   18d88: 0a000079     	beq	0x18f74
   18d8c: e0467004     	sub	r7, r6, r4
   18d90: e1a01006     	mov	r1, r6
   18d94: e1a00004     	mov	r0, r4
   18d98: e3a03000     	mov	r3, #0
   18d9c: e1a02247     	asr	r2, r7, #4
   18da0: e16f2f12     	clz	r2, r2
   18da4: e262201f     	rsb	r2, r2, #31
   18da8: e1a02082     	lsl	r2, r2, #1
   18dac: ebfffe12     	bl	0x185fc
   18db0: e3570c01     	cmp	r7, #256
   18db4: da0000e1     	ble	0x19140
   18db8: e2847c01     	add	r7, r4, #256
   18dbc: e1a00004     	mov	r0, r4
   18dc0: e1a01007     	mov	r1, r7
   18dc4: ebfffdd8     	bl	0x1852c
   18dc8: e1a0e007     	mov	lr, r7
   18dcc: e156000e     	cmp	r6, lr
   18dd0: e1a0700e     	mov	r7, lr
   18dd4: 0a0000dc     	beq	0x1914c
   18dd8: e28dc020     	add	r12, sp, #32
   18ddc: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   18de0: e88c000f     	stm	r12, {r0, r1, r2, r3}
   18de4: e59e8008     	ldr	r8, [lr, #0x8]
   18de8: e51e3008     	ldr	r3, [lr, #-0x8]
   18dec: e1580003     	cmp	r8, r3
   18df0: aa000008     	bge	0x18e18
   18df4: e24ec010     	sub	r12, lr, #16
   18df8: e28c4010     	add	r4, r12, #16
   18dfc: e1a0700c     	mov	r7, r12
   18e00: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   18e04: e24cc010     	sub	r12, r12, #16
   18e08: e884000f     	stm	r4, {r0, r1, r2, r3}
   18e0c: e59c3008     	ldr	r3, [r12, #0x8]
   18e10: e1530008     	cmp	r3, r8
   18e14: cafffff7     	bgt	0x18df8
   18e18: e28d3020     	add	r3, sp, #32
   18e1c: e58d8028     	str	r8, [sp, #0x28]
   18e20: e28ee010     	add	lr, lr, #16
   18e24: e893000f     	ldm	r3, {r0, r1, r2, r3}
   18e28: e887000f     	stm	r7, {r0, r1, r2, r3}
   18e2c: eaffffe6     	b	0x18dcc
   18e30: e287a040     	add	r10, r7, #64
   18e34: e1a04008     	mov	r4, r8
   18e38: ea000009     	b	0x18e64
   18e3c: e1540007     	cmp	r4, r7
   18e40: 0a000003     	beq	0x18e54
   18e44: e0442007     	sub	r2, r4, r7
   18e48: e1a01007     	mov	r1, r7
   18e4c: e2870004     	add	r0, r7, #4
   18e50: ebfff2e3     	bl	0x159e4    @ imm = #-0x3474 ; memmove
   18e54: ed878a00     	vstr	s16, [r7]
   18e58: e2844004     	add	r4, r4, #4
   18e5c: e15a0004     	cmp	r10, r4
   18e60: 0a000022     	beq	0x18ef0
   18e64: ed948a00     	vldr	s16, [r4]
   18e68: edd77a00     	vldr	s15, [r7]
   18e6c: eeb48ae7     	vcmpe.f32	s16, s15
   18e70: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18e74: 4afffff0     	bmi	0x18e3c
   18e78: ed547a01     	vldr	s15, [r4, #-4]
   18e7c: e2443004     	sub	r3, r4, #4
   18e80: eeb48ae7     	vcmpe.f32	s16, s15
   18e84: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18e88: 5a0000e6     	bpl	0x19228
   18e8c: e1a02003     	mov	r2, r3
   18e90: edc37a01     	vstr	s15, [r3, #4]
   18e94: ed737a01     	vldmdb	r3!, {s15}
   18e98: eeb48ae7     	vcmpe.f32	s16, s15
   18e9c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18ea0: 4afffff9     	bmi	0x18e8c
   18ea4: ed828a00     	vstr	s16, [r2]
   18ea8: eaffffea     	b	0x18e58
   18eac: e1a02009     	mov	r2, r9
   18eb0: e1a0100a     	mov	r1, r10
   18eb4: e1a0000b     	mov	r0, r11
   18eb8: ebfff2c9     	bl	0x159e4    @ imm = #-0x34dc ; memmove
   18ebc: e1a0000a     	mov	r0, r10
   18ec0: ebfff3de     	bl	0x15e40    @ imm = #-0x3088 ; _ZdlPv
   18ec4: eaffff9d     	b	0x18d40
   18ec8: e3540000     	cmp	r4, #0
   18ecc: 01a0b004     	moveq	r11, r4
   18ed0: 0affff8e     	beq	0x18d10
   18ed4: e3e0333e     	mvn	r3, #-134217728
   18ed8: e1540003     	cmp	r4, r3
   18edc: 21a04003     	movhs	r4, r3
   18ee0: e1a04204     	lsl	r4, r4, #4
   18ee4: eaffff85     	b	0x18d00
   18ee8: e3a04010     	mov	r4, #16
   18eec: eaffff83     	b	0x18d00
   18ef0: e15a0009     	cmp	r10, r9
   18ef4: 0a000011     	beq	0x18f40
   18ef8: e1a0300a     	mov	r3, r10
   18efc: e287103c     	add	r1, r7, #60
   18f00: e1a00003     	mov	r0, r3
   18f04: e1a02001     	mov	r2, r1
   18f08: ecb37a01     	vldmia	r3!, {s14}
   18f0c: ecf17a01     	vldmia	r1!, {s15}
   18f10: eeb47ae7     	vcmpe.f32	s14, s15
   18f14: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18f18: 5a000005     	bpl	0x18f34
   18f1c: e1a00002     	mov	r0, r2
   18f20: edc27a01     	vstr	s15, [r2, #4]
   18f24: ed727a01     	vldmdb	r2!, {s15}
   18f28: eeb47ae7     	vcmpe.f32	s14, s15
   18f2c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18f30: 4afffff9     	bmi	0x18f1c
   18f34: e1590003     	cmp	r9, r3
   18f38: ed807a00     	vstr	s14, [r0]
   18f3c: 1affffef     	bne	0x18f00
   18f40: e1a02008     	mov	r2, r8
   18f44: e1a01007     	mov	r1, r7
   18f48: ea000005     	b	0x18f64
   18f4c: edd27a00     	vldr	s15, [r2]
   18f50: e2822004     	add	r2, r2, #4
   18f54: ecb17a01     	vldmia	r1!, {s14}
   18f58: eeb47a67     	vcmp.f32	s14, s15
   18f5c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   18f60: 0afffee8     	beq	0x18b08
   18f64: e1a03001     	mov	r3, r1
   18f68: e1520009     	cmp	r2, r9
   18f6c: 1afffff6     	bne	0x18f4c
   18f70: eafffef2     	b	0x18b40
   18f74: e0466004     	sub	r6, r6, r4
   18f78: e1a03246     	asr	r3, r6, #4
   18f7c: e3530001     	cmp	r3, #1
   18f80: 0a00004c     	beq	0x190b8
   18f84: e2446010     	sub	r6, r4, #16
   18f88: e1a01004     	mov	r1, r4
   18f8c: e0866203     	add	r6, r6, r3, lsl #4
   18f90: e1a03004     	mov	r3, r4
   18f94: e593000c     	ldr	r0, [r3, #0xc]
   18f98: e5932014     	ldr	r2, [r3, #0x14]
   18f9c: e1500002     	cmp	r0, r2
   18fa0: da000008     	ble	0x18fc8
   18fa4: e593c008     	ldr	r12, [r3, #0x8]
   18fa8: e5930018     	ldr	r0, [r3, #0x18]
   18fac: e040200c     	sub	r2, r0, r12
   18fb0: e0822fa2     	add	r2, r2, r2, lsr #31
   18fb4: e1a020c2     	asr	r2, r2, #1
   18fb8: e082c00c     	add	r12, r2, r12
   18fbc: e0402002     	sub	r2, r0, r2
   18fc0: e583c00c     	str	r12, [r3, #0xc]
   18fc4: e5832014     	str	r2, [r3, #0x14]
   18fc8: e2833010     	add	r3, r3, #16
   18fcc: e1530006     	cmp	r3, r6
   18fd0: 1affffef     	bne	0x18f94
   18fd4: e5913004     	ldr	r3, [r1, #0x4]
   18fd8: e591200c     	ldr	r2, [r1, #0xc]
   18fdc: e1530002     	cmp	r3, r2
   18fe0: aa00001c     	bge	0x19058
   18fe4: e042e003     	sub	lr, r2, r3
   18fe8: e591c000     	ldr	r12, [r1]
   18fec: e24e0001     	sub	r0, lr, #1
   18ff0: e3500002     	cmp	r0, #2
   18ff4: 9a00000c     	bls	0x1902c
   18ff8: e2830089     	add	r0, r3, #137
   18ffc: eea0cb90     	vdup.32	q8, r12
   19000: e1a0812e     	lsr	r8, lr, #2
   19004: e3a07000     	mov	r7, #0
   19008: e0850100     	add	r0, r5, r0, lsl #2
   1900c: e2877001     	add	r7, r7, #1
   19010: f4400a8d     	vst1.32	{d16, d17}, [r0]!
   19014: e1570008     	cmp	r7, r8
   19018: 1afffffb     	bne	0x1900c
   1901c: e3ce0003     	bic	r0, lr, #3
   19020: e0833000     	add	r3, r3, r0
   19024: e15e0000     	cmp	lr, r0
   19028: 0a00000a     	beq	0x19058
   1902c: e085e103     	add	lr, r5, r3, lsl #2
   19030: e2830001     	add	r0, r3, #1
   19034: e1520000     	cmp	r2, r0
   19038: e58ec224     	str	r12, [lr, #0x224]
   1903c: da000005     	ble	0x19058
   19040: e0850100     	add	r0, r5, r0, lsl #2
   19044: e2833002     	add	r3, r3, #2
   19048: e1520003     	cmp	r2, r3
   1904c: e580c224     	str	r12, [r0, #0x224]
   19050: c0853103     	addgt	r3, r5, r3, lsl #2
   19054: c583c224     	strgt	r12, [r3, #0x224]
   19058: e5913014     	ldr	r3, [r1, #0x14]
   1905c: e1530002     	cmp	r3, r2
   19060: da000011     	ble	0x190ac
   19064: ed917a00     	vldr	s14, [r1]
   19068: e0433002     	sub	r3, r3, r2
   1906c: edd17a04     	vldr	s15, [r1, #16]
   19070: ee063a90     	vmov	s13, r3
   19074: e2822089     	add	r2, r2, #137
   19078: e3a00000     	mov	r0, #0
   1907c: eef86ae6     	vcvt.f32.s32	s13, s13
   19080: ee777ac7     	vsub.f32	s15, s15, s14
   19084: e0852102     	add	r2, r5, r2, lsl #2
   19088: ee060a10     	vmov	s12, r0
   1908c: e2800001     	add	r0, r0, #1
   19090: e1500003     	cmp	r0, r3
   19094: eef85ac6     	vcvt.f32.s32	s11, s12
   19098: ee856aa6     	vdiv.f32	s12, s11, s13
   1909c: eef05a47     	vmov.f32	s11, s14
   190a0: eee65a27     	vfma.f32	s11, s12, s15
   190a4: ece25a01     	vstmia	r2!, {s11}
   190a8: 1afffff6     	bne	0x19088
   190ac: e2811010     	add	r1, r1, #16
   190b0: e1510006     	cmp	r1, r6
   190b4: 1affffc6     	bne	0x18fd4
   190b8: e3540000     	cmp	r4, #0
   190bc: 0a000001     	beq	0x190c8
   190c0: e1a00004     	mov	r0, r4
   190c4: ebfff35d     	bl	0x15e40    @ imm = #-0x328c ; _ZdlPv
   190c8: e51fe480     	ldr	lr, [pc, #-0x480]       @ 0x18c50
   190cc: e2854901     	add	r4, r5, #16384
   190d0: e284cf8a     	add	r12, r4, #552
   190d4: e2849f9b     	add	r9, r4, #620
   190d8: eddf4b72     	vldr	d20, [pc, #456]         @ 0x192a8 ; float 3.05175853327e-05
   190dc: eddf5b73     	vldr	d21, [pc, #460]         @ 0x192b0 ; float 4.65661395381e-10
   190e0: e2848f9f     	add	r8, r4, #636
   190e4: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   190e8: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   190ec: e2847fa3     	add	r7, r4, #652
   190f0: eddf2b70     	vldr	d18, [pc, #448]         @ 0x192b8 ; float 1.3411048233e-08
   190f4: eddf3b71     	vldr	d19, [pc, #452]         @ 0x192c0 ; float 1.34110464955e-08
   190f8: e3a06003     	mov	r6, #3
   190fc: eddf0b71     	vldr	d16, [pc, #452]         @ 0x192c8 ; float 3.43322837737e-06
   19100: eddf1b72     	vldr	d17, [pc, #456]         @ 0x192d0 ; float 0.000878906470662
   19104: e5846224     	str	r6, [r4, #0x224]
   19108: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1910c: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   19110: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   19114: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   19118: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1911c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   19120: e1a00005     	mov	r0, r5
   19124: e5846268     	str	r6, [r4, #0x268]
   19128: f4494a8f     	vst1.32	{d20, d21}, [r9]
   1912c: f4482a8f     	vst1.32	{d18, d19}, [r8]
   19130: f4470a8f     	vst1.32	{d16, d17}, [r7]
   19134: e28dd034     	add	sp, sp, #52
   19138: ecbd8b04     	vpop	{d8, d9}
   1913c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   19140: e1a01006     	mov	r1, r6
   19144: e1a00004     	mov	r0, r4
   19148: ebfffcf7     	bl	0x1852c
   1914c: e59d4010     	ldr	r4, [sp, #0x10]
   19150: e59d6014     	ldr	r6, [sp, #0x14]
   19154: e1540006     	cmp	r4, r6
   19158: 0affff85     	beq	0x18f74
   1915c: e2843010     	add	r3, r4, #16
   19160: e1560003     	cmp	r6, r3
   19164: 0affff82     	beq	0x18f74
   19168: ed947a00     	vldr	s14, [r4]
   1916c: e1a03004     	mov	r3, r4
   19170: edd37a04     	vldr	s15, [r3, #16]
   19174: e1a07003     	mov	r7, r3
   19178: eeb47a67     	vcmp.f32	s14, s15
   1917c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   19180: 1a000003     	bne	0x19194
   19184: e5931008     	ldr	r1, [r3, #0x8]
   19188: e5932018     	ldr	r2, [r3, #0x18]
   1918c: e1510002     	cmp	r1, r2
   19190: 0a000005     	beq	0x191ac
   19194: e2832020     	add	r2, r3, #32
   19198: e2833010     	add	r3, r3, #16
   1919c: e1560002     	cmp	r6, r2
   191a0: 0affff73     	beq	0x18f74
   191a4: eeb07a67     	vmov.f32	s14, s15
   191a8: eafffff0     	b	0x19170
   191ac: e1560003     	cmp	r6, r3
   191b0: 0affff6f     	beq	0x18f74
   191b4: e2833020     	add	r3, r3, #32
   191b8: e1560003     	cmp	r6, r3
   191bc: 0a000010     	beq	0x19204
   191c0: e287c030     	add	r12, r7, #48
   191c4: ed977a00     	vldr	s14, [r7]
   191c8: ed5c7a04     	vldr	s15, [r12, #-16]
   191cc: eeb47a67     	vcmp.f32	s14, s15
   191d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   191d4: 1a000003     	bne	0x191e8
   191d8: e5972008     	ldr	r2, [r7, #0x8]
   191dc: e51c3008     	ldr	r3, [r12, #-0x8]
   191e0: e1520003     	cmp	r2, r3
   191e4: 0a000003     	beq	0x191f8
   191e8: e287e010     	add	lr, r7, #16
   191ec: e91c000f     	ldmdb	r12, {r0, r1, r2, r3}
   191f0: e1a0700e     	mov	r7, lr
   191f4: e88e000f     	stm	lr, {r0, r1, r2, r3}
   191f8: e156000c     	cmp	r6, r12
   191fc: e28cc010     	add	r12, r12, #16
   19200: 1affffef     	bne	0x191c4
   19204: e2877010     	add	r7, r7, #16
   19208: e1560007     	cmp	r6, r7
   1920c: 11a06007     	movne	r6, r7
   19210: 158d7014     	strne	r7, [sp, #0x14]
   19214: e0466004     	sub	r6, r6, r4
   19218: e1a03246     	asr	r3, r6, #4
   1921c: e3530001     	cmp	r3, #1
   19220: 1affff57     	bne	0x18f84
   19224: eaffffa5     	b	0x190c0
   19228: e1a02004     	mov	r2, r4
   1922c: eaffff1c     	b	0x18ea4
   19230: e1a02004     	mov	r2, r4
   19234: eafffe31     	b	0x18b00
   19238: e1a01007     	mov	r1, r7
   1923c: ebfff1e8     	bl	0x159e4    @ imm = #-0x3860 ; memmove
   19240: e1a00007     	mov	r0, r7
   19244: ebfff2fd     	bl	0x15e40    @ imm = #-0x340c ; _ZdlPv
   19248: eafffe58     	b	0x18bb0
   1924c: e3000c8c     	movw	r0, #0xc8c
   19250: e3400007     	movt	r0, #0x7
   19254: ebfff257     	bl	0x15bb8    @ imm = #-0x36a4 ; _ZSt20__throw_length_errorPKc
   19258: e3000c8c     	movw	r0, #0xc8c
   1925c: e3400007     	movt	r0, #0x7
   19260: ebfff254     	bl	0x15bb8    @ imm = #-0x36b0 ; _ZSt20__throw_length_errorPKc
   19264: e3000c8c     	movw	r0, #0xc8c
   19268: e3400007     	movt	r0, #0x7
   1926c: ebfff251     	bl	0x15bb8    @ imm = #-0x36bc ; _ZSt20__throw_length_errorPKc
   19270: e59d0010     	ldr	r0, [sp, #0x10]
   19274: e3500000     	cmp	r0, #0
   19278: 0a000000     	beq	0x19280
   1927c: ebfff2ef     	bl	0x15e40    @ imm = #-0x3444 ; _ZdlPv
   19280: ebfff336     	bl	0x15f60    @ imm = #-0x3328 ; __cxa_end_cleanup
   19284: e1a0a007     	mov	r10, r7
   19288: e35a0000     	cmp	r10, #0
   1928c: 0afffffb     	beq	0x19280
   19290: e1a0000a     	mov	r0, r10
   19294: ebfff2e9     	bl	0x15e40    @ imm = #-0x345c ; _ZdlPv
   19298: eafffff8     	b	0x19280
   1929c: eafffff8     	b	0x19284
   192a0: eafffff8     	b	0x19288
   192a4: e320f000     	nop
   192a8: 66 66 66 3f  	.word	0x3f666666
   192ac: 00 00 00 3f  	.word	0x3f000000
   192b0: cd cc 4c 3e  	.word	0x3e4ccccd
   192b4: 00 00 00 3e  	.word	0x3e000000
   192b8: 9a 99 99 3e  	.word	0x3e99999a
   192bc: cd cc 4c 3e  	.word	0x3e4ccccd
   192c0: 00 00 00 00  	.word	0x00000000
   192c4: cd cc 4c 3e  	.word	0x3e4ccccd
   192c8: 00 00 c8 42  	.word	0x42c80000
   192cc: cd cc cc 3e  	.word	0x3ecccccd
   192d0: 00 40 1c 46  	.word	0x461c4000
   192d4: cd cc 4c 3f  	.word	0x3f4ccccd
