; lubadh::Channel::tempoClock()
; VA 0x3eed8 size 748

   3eed8: e92d40f0     	push	{r4, r5, r6, r7, lr}
   3eedc: e1a04000     	mov	r4, r0
   3eee0: ed2d8b02     	vpush	{d8}
   3eee4: e24dd02c     	sub	sp, sp, #44
   3eee8: e28d0020     	add	r0, sp, #32
   3eeec: ebff5a89     	bl	0x15918     @ imm = #-0x295dc ; _ZNSt6chrono3_V212system_clock3nowEv
   3eef0: e1cd62d0     	ldrd	r6, r7, [sp, #32]
   3eef4: e2841e1d     	add	r1, r4, #464
   3eef8: e1c100d0     	ldrd	r0, r1, [r1]
   3eefc: e0560000     	subs	r0, r6, r0
   3ef00: e0c71001     	sbc	r1, r7, r1
   3ef04: ebff5be8     	bl	0x15eac    @ imm = #-0x29060 ; __aeabi_l2f
   3ef08: eddf7aa8     	vldr	s15, [pc, #672]         @ 0x3f1b0 ; float 1000000000
   3ef0c: e5d431d8     	ldrb	r3, [r4, #0x1d8]
   3ef10: ee070a10     	vmov	s14, r0
   3ef14: e58461d0     	str	r6, [r4, #0x1d0]
   3ef18: e58471d4     	str	r7, [r4, #0x1d4]
   3ef1c: ee878a27     	vdiv.f32	s16, s14, s15
   3ef20: e3530000     	cmp	r3, #0
   3ef24: 0a000006     	beq	0x3ef44
   3ef28: e59430e8     	ldr	r3, [r4, #0xe8]
   3ef2c: e59330b0     	ldr	r3, [r3, #0xb0]
   3ef30: edd37a09     	vldr	s15, [r3, #36]
   3ef34: eef87ae7     	vcvt.f32.s32	s15, s15
   3ef38: eef47ac8     	vcmpe.f32	s15, s16
   3ef3c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3ef40: 4a000048     	bmi	0x3f068
   3ef44: e594322c     	ldr	r3, [r4, #0x22c]
   3ef48: e5941224     	ldr	r1, [r4, #0x224]
   3ef4c: e2433004     	sub	r3, r3, #4
   3ef50: e594221c     	ldr	r2, [r4, #0x21c]
   3ef54: e1510003     	cmp	r1, r3
   3ef58: e5943214     	ldr	r3, [r4, #0x214]
   3ef5c: 0a000064     	beq	0x3f0f4
   3ef60: eca18a01     	vstmia	r1!, {s16}
   3ef64: e5841224     	str	r1, [r4, #0x224]
   3ef68: e2421004     	sub	r1, r2, #4
   3ef6c: e1530001     	cmp	r3, r1
   3ef70: 0a000051     	beq	0x3f0bc
   3ef74: e5941220     	ldr	r1, [r4, #0x220]
   3ef78: e2833004     	add	r3, r3, #4
   3ef7c: e5843214     	str	r3, [r4, #0x214]
   3ef80: e5940224     	ldr	r0, [r4, #0x224]
   3ef84: eddf7a8a     	vldr	s15, [pc, #552]         @ 0x3f1b4 ; float 0
   3ef88: e1500003     	cmp	r0, r3
   3ef8c: 0a000005     	beq	0x3efa8
   3ef90: ecb37a01     	vldmia	r3!, {s14}
   3ef94: ee777a87     	vadd.f32	s15, s15, s14
   3ef98: e1520003     	cmp	r2, r3
   3ef9c: 0a000034     	beq	0x3f074
   3efa0: e1500003     	cmp	r0, r3
   3efa4: 1afffff9     	bne	0x3ef90
   3efa8: e59430e8     	ldr	r3, [r4, #0xe8]
   3efac: eddf5a81     	vldr	s11, [pc, #516]         @ 0x3f1b8 ; float 49170.2539062
   3efb0: e59320b0     	ldr	r2, [r3, #0xb0]
   3efb4: ed937a0a     	vldr	s14, [r3, #40]
   3efb8: e5921020     	ldr	r1, [r2, #0x20]
   3efbc: edd26a07     	vldr	s13, [r2, #28]
   3efc0: eeb87ac7     	vcvt.f32.s32	s14, s14
   3efc4: e3510000     	cmp	r1, #0
   3efc8: eeb86ae6     	vcvt.f32.s32	s12, s13
   3efcc: eec76a86     	vdiv.f32	s13, s15, s12
   3efd0: ee666aa5     	vmul.f32	s13, s13, s11
   3efd4: 0a00002c     	beq	0x3f08c
   3efd8: ee071a90     	vmov	s15, r1
   3efdc: eef87ae7     	vcvt.f32.s32	s15, s15
   3efe0: ee876a27     	vdiv.f32	s12, s14, s15
   3efe4: eec67a26     	vdiv.f32	s15, s12, s13
   3efe8: eeb17a00     	vmov.f32	s14, #4.000000e+00
   3efec: eef47ac7     	vcmpe.f32	s15, s14
   3eff0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3eff4: 5ef07a47     	vmovpl.f32	s15, s14
   3eff8: 5a000003     	bpl	0x3f00c
   3effc: ed9f7a6e     	vldr	s14, [pc, #440]         @ 0x3f1bc ; float 0.00999999977648
   3f000: eef47ac7     	vcmpe.f32	s15, s14
   3f004: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3f008: def07a47     	vmovle.f32	s15, s14
   3f00c: e5922040     	ldr	r2, [r2, #0x40]
   3f010: e3520003     	cmp	r2, #3
   3f014: 0a000024     	beq	0x3f0ac
   3f018: e2842a2a     	add	r2, r4, #172032
   3f01c: e5922498     	ldr	r2, [r2, #0x498]
   3f020: e3520b02     	cmp	r2, #2048
   3f024: aa000000     	bge	0x3f02c
   3f028: eef17a67     	vneg.f32	s15, s15
   3f02c: edc47a0b     	vstr	s15, [r4, #44]
   3f030: eeb76a00     	vmov.f32	s12, #1.000000e+00
   3f034: edd36a2b     	vldr	s13, [r3, #172]
   3f038: ed947ab6     	vldr	s14, [r4, #728]
   3f03c: eddf5a5f     	vldr	s11, [pc, #380]         @ 0x3f1c0 ; float 2.70000004768
   3f040: eefd6ae6     	vcvt.s32.f32	s13, s13
   3f044: ee777ac7     	vsub.f32	s15, s15, s14
   3f048: eef86ae6     	vcvt.f32.s32	s13, s13
   3f04c: ee867aa5     	vdiv.f32	s14, s13, s11
   3f050: ee377a06     	vadd.f32	s14, s14, s12
   3f054: eebd7ac7     	vcvt.s32.f32	s14, s14
   3f058: eef86ac7     	vcvt.f32.s32	s13, s14
   3f05c: ed847ab8     	vstr	s14, [r4, #736]
   3f060: ee877aa6     	vdiv.f32	s14, s15, s13
   3f064: ed847ab7     	vstr	s14, [r4, #732]
   3f068: e28dd02c     	add	sp, sp, #44
   3f06c: ecbd8b02     	vpop	{d8}
   3f070: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
   3f074: e5913004     	ldr	r3, [r1, #0x4]
   3f078: e2811004     	add	r1, r1, #4
   3f07c: e1500003     	cmp	r0, r3
   3f080: e2832c02     	add	r2, r3, #512
   3f084: 1affffc1     	bne	0x3ef90
   3f088: eaffffc6     	b	0x3efa8
   3f08c: e2841a2a     	add	r1, r4, #172032
   3f090: e59114d8     	ldr	r1, [r1, #0x4d8]
   3f094: e3510000     	cmp	r1, #0
   3f098: 1e071a90     	vmovne	s15, r1
   3f09c: 1ef87ae7     	vcvtne.f32.s32	s15, s15
   3f0a0: 1e877a27     	vdivne.f32	s14, s14, s15
   3f0a4: eec77a26     	vdiv.f32	s15, s14, s13
   3f0a8: eaffffce     	b	0x3efe8
   3f0ac: e59420ec     	ldr	r2, [r4, #0xec]
   3f0b0: e3520000     	cmp	r2, #0
   3f0b4: 0affffdc     	beq	0x3f02c
   3f0b8: eaffffda     	b	0x3f028
   3f0bc: e5940218     	ldr	r0, [r4, #0x218]
   3f0c0: ebff5b5e     	bl	0x15e40    @ imm = #-0x29288 ; _ZdlPv
   3f0c4: e5943220     	ldr	r3, [r4, #0x220]
   3f0c8: e2840f85     	add	r0, r4, #532
   3f0cc: e2831004     	add	r1, r3, #4
   3f0d0: e5933004     	ldr	r3, [r3, #0x4]
   3f0d4: e58d3000     	str	r3, [sp]
   3f0d8: e58d3004     	str	r3, [sp, #0x4]
   3f0dc: e2832c02     	add	r2, r3, #512
   3f0e0: e58d100c     	str	r1, [sp, #0xc]
   3f0e4: e58d2008     	str	r2, [sp, #0x8]
   3f0e8: f46d0adf     	vld1.64	{d16, d17}, [sp:64]
   3f0ec: f4400a8f     	vst1.32	{d16, d17}, [r0]
   3f0f0: eaffffa2     	b	0x3ef80
   3f0f4: e0423003     	sub	r3, r2, r3
   3f0f8: e5945230     	ldr	r5, [r4, #0x230]
   3f0fc: e5942220     	ldr	r2, [r4, #0x220]
   3f100: e5940228     	ldr	r0, [r4, #0x228]
   3f104: e0452002     	sub	r2, r5, r2
   3f108: e0411000     	sub	r1, r1, r0
   3f10c: e1a02142     	asr	r2, r2, #2
   3f110: e2420001     	sub	r0, r2, #1
   3f114: e1a02141     	asr	r2, r1, #2
   3f118: e0822380     	add	r2, r2, r0, lsl #7
   3f11c: e0823143     	add	r3, r2, r3, asr #2
   3f120: e373021e     	cmn	r3, #-536870911
   3f124: 0a00001e     	beq	0x3f1a4
   3f128: e594220c     	ldr	r2, [r4, #0x20c]
   3f12c: e5943210     	ldr	r3, [r4, #0x210]
   3f130: e0452002     	sub	r2, r5, r2
   3f134: e0433142     	sub	r3, r3, r2, asr #2
   3f138: e3530001     	cmp	r3, #1
   3f13c: 9a000012     	bls	0x3f18c
   3f140: e3a00c02     	mov	r0, #512
   3f144: ebff59f0     	bl	0x1590c     @ imm = #-0x29840 ; _Znwj
   3f148: e5850004     	str	r0, [r5, #0x4]
   3f14c: e5942230     	ldr	r2, [r4, #0x230]
   3f150: e2843f89     	add	r3, r4, #548
   3f154: e5940224     	ldr	r0, [r4, #0x224]
   3f158: e5b21004     	ldr	r1, [r2, #0x4]!
   3f15c: ed808a00     	vstr	s16, [r0]
   3f160: e2810c02     	add	r0, r1, #512
   3f164: e58d1010     	str	r1, [sp, #0x10]
   3f168: e58d1014     	str	r1, [sp, #0x14]
   3f16c: e58d0018     	str	r0, [sp, #0x18]
   3f170: e58d201c     	str	r2, [sp, #0x1c]
   3f174: eddd0b04     	vldr	d16, [sp, #16]
   3f178: eddd1b06     	vldr	d17, [sp, #24]
   3f17c: f4430a8f     	vst1.32	{d16, d17}, [r3]
   3f180: e5943214     	ldr	r3, [r4, #0x214]
   3f184: e594221c     	ldr	r2, [r4, #0x21c]
   3f188: eaffff76     	b	0x3ef68
   3f18c: e3a02000     	mov	r2, #0
   3f190: e3a01001     	mov	r1, #1
   3f194: e2840f83     	add	r0, r4, #524
   3f198: eb002099     	bl	0x47404
   3f19c: e5945230     	ldr	r5, [r4, #0x230]
   3f1a0: eaffffe6     	b	0x3f140
   3f1a4: e30208b8     	movw	r0, #0x28b8
   3f1a8: e3400007     	movt	r0, #0x7
   3f1ac: ebff5a81     	bl	0x15bb8    @ imm = #-0x295fc ; _ZSt20__throw_length_errorPKc
   3f1b0: 28 6b 6e 4e  	.word	0x4e6e6b28
   3f1b4: 00 00 00 00  	.word	0x00000000
   3f1b8: 41 12 40 47  	.word	0x47401241
   3f1bc: 0a d7 23 3c  	.word	0x3c23d70a
   3f1c0: cd cc 2c 40  	.word	0x402ccccd
