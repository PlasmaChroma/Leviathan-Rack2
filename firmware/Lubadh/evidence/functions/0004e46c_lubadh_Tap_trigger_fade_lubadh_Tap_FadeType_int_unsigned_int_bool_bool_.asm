; lubadh::Tap::trigger_fade(lubadh::Tap::FadeType, int, unsigned int, bool, bool)
; VA 0x4e46c size 2188

   4e46c: e3a0c018     	mov	r12, #24
   4e470: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   4e474: e1a06001     	mov	r6, r1
   4e478: e1a04000     	mov	r4, r0
   4e47c: e280001c     	add	r0, r0, #28
   4e480: e001019c     	mul	r1, r12, r1
   4e484: ed2d8b02     	vpush	{d8}
   4e488: e24dd0d4     	sub	sp, sp, #212
   4e48c: ee083a10     	vmov	s16, r3
   4e490: e1a07002     	mov	r7, r2
   4e494: e0805001     	add	r5, r0, r1
   4e498: e7d03001     	ldrb	r3, [r0, r1]
   4e49c: e5dd8100     	ldrb	r8, [sp, #0x100]
   4e4a0: e5dd9104     	ldrb	r9, [sp, #0x104]
   4e4a4: e3530000     	cmp	r3, #0
   4e4a8: 1a0000c4     	bne	0x4e7c0
   4e4ac: eeb88a48     	vcvt.f32.u32	s16, s16
   4e4b0: eddf7abf     	vldr	s15, [pc, #764]         @ 0x4e7b4 ; float 256
   4e4b4: e5943004     	ldr	r3, [r4, #0x4]
   4e4b8: e594200c     	ldr	r2, [r4, #0xc]
   4e4bc: ee877a88     	vdiv.f32	s14, s15, s16
   4e4c0: e1520003     	cmp	r2, r3
   4e4c4: b1a0c002     	movlt	r12, r2
   4e4c8: a1a0c003     	movge	r12, r3
   4e4cc: a1a03002     	movge	r3, r2
   4e4d0: e15c0007     	cmp	r12, r7
   4e4d4: c3a0c000     	movgt	r12, #0
   4e4d8: d3a0c001     	movle	r12, #1
   4e4dc: e1530007     	cmp	r3, r7
   4e4e0: b3a0c000     	movlt	r12, #0
   4e4e4: e1580009     	cmp	r8, r9
   4e4e8: 1eb17a47     	vnegne.f32	s14, s14
   4e4ec: e3580000     	cmp	r8, #0
   4e4f0: 0a000042     	beq	0x4e600
   4e4f4: e35c0000     	cmp	r12, #0
   4e4f8: 0a00009f     	beq	0x4e77c
   4e4fc: e0473002     	sub	r3, r7, r2
   4e500: ee073a90     	vmov	s15, r3
   4e504: e3a02018     	mov	r2, #24
   4e508: e3a03000     	mov	r3, #0
   4e50c: eef87ae7     	vcvt.f32.s32	s15, s15
   4e510: e3a01001     	mov	r1, #1
   4e514: e0224692     	mla	r2, r2, r6, r4
   4e518: ee677a87     	vmul.f32	s15, s15, s14
   4e51c: e282c028     	add	r12, r2, #40
   4e520: e5c2101c     	strb	r1, [r2, #0x1c]
   4e524: e5853008     	str	r3, [r5, #0x8]
   4e528: eefd7ae7     	vcvt.s32.f32	s15, s15
   4e52c: ee173a90     	vmov	r3, s15
   4e530: e2633000     	rsb	r3, r3, #0
   4e534: e5853004     	str	r3, [r5, #0x4]
   4e538: e1c202d0     	ldrd	r0, r1, [r2, #32]
   4e53c: e88c0003     	stm	r12, {r0, r1}
   4e540: ed827a0c     	vstr	s14, [r2, #48]
   4e544: edd47a01     	vldr	s15, [r4, #4]
   4e548: e3a01018     	mov	r1, #24
   4e54c: ed946a04     	vldr	s12, [r4, #16]
   4e550: eeb77a00     	vmov.f32	s14, #1.000000e+00
   4e554: edd45a02     	vldr	s11, [r4, #8]
   4e558: eef86ae7     	vcvt.f32.s32	s13, s15
   4e55c: edd47a03     	vldr	s15, [r4, #12]
   4e560: e0244691     	mla	r4, r1, r6, r4
   4e564: eef87ae7     	vcvt.f32.s32	s15, s15
   4e568: e2843028     	add	r3, r4, #40
   4e56c: ee766aa5     	vadd.f32	s13, s13, s11
   4e570: e1c402d0     	ldrd	r0, r1, [r4, #32]
   4e574: e8830003     	stm	r3, {r0, r1}
   4e578: ee777a86     	vadd.f32	s15, s15, s12
   4e57c: ed946a0c     	vldr	s12, [r4, #48]
   4e580: ee766ae7     	vsub.f32	s13, s13, s15
   4e584: edd57a02     	vldr	s15, [r5, #8]
   4e588: eee67a86     	vfma.f32	s15, s13, s12
   4e58c: eef47ac7     	vcmpe.f32	s15, s14
   4e590: edc57a02     	vstr	s15, [r5, #8]
   4e594: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e598: a5953004     	ldrge	r3, [r5, #0x4]
   4e59c: ba000006     	blt	0x4e5bc
   4e5a0: ee777ac7     	vsub.f32	s15, s15, s14
   4e5a4: e2833001     	add	r3, r3, #1
   4e5a8: eef47ac7     	vcmpe.f32	s15, s14
   4e5ac: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e5b0: aafffffa     	bge	0x4e5a0
   4e5b4: e5853004     	str	r3, [r5, #0x4]
   4e5b8: edc57a02     	vstr	s15, [r5, #8]
   4e5bc: eef57ac0     	vcmpe.f32	s15, #0
   4e5c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e5c4: 45953004     	ldrmi	r3, [r5, #0x4]
   4e5c8: 4eb77a00     	vmovmi.f32	s14, #1.000000e+00
   4e5cc: 42433001     	submi	r3, r3, #1
   4e5d0: 5a000007     	bpl	0x4e5f4
   4e5d4: ee777a87     	vadd.f32	s15, s15, s14
   4e5d8: e1a02003     	mov	r2, r3
   4e5dc: e2433001     	sub	r3, r3, #1
   4e5e0: eef57ac0     	vcmpe.f32	s15, #0
   4e5e4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e5e8: 4afffff9     	bmi	0x4e5d4
   4e5ec: e5852004     	str	r2, [r5, #0x4]
   4e5f0: edc57a02     	vstr	s15, [r5, #8]
   4e5f4: e28dd0d4     	add	sp, sp, #212
   4e5f8: ecbd8b02     	vpop	{d8}
   4e5fc: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   4e600: e35c0000     	cmp	r12, #0
   4e604: 1a000172     	bne	0x4ebd4
   4e608: e3a03018     	mov	r3, #24
   4e60c: e3a01000     	mov	r1, #0
   4e610: e3a000ff     	mov	r0, #255
   4e614: e3a07001     	mov	r7, #1
   4e618: e3a0e5fe     	mov	lr, #1065353216
   4e61c: e0234693     	mla	r3, r3, r6, r4
   4e620: e2832028     	add	r2, r3, #40
   4e624: e5c3701c     	strb	r7, [r3, #0x1c]
   4e628: e5850004     	str	r0, [r5, #0x4]
   4e62c: e5851008     	str	r1, [r5, #0x8]
   4e630: e1c302d0     	ldrd	r0, r1, [r3, #32]
   4e634: e8820003     	stm	r2, {r0, r1}
   4e638: e583e030     	str	lr, [r3, #0x30]
   4e63c: e3a03018     	mov	r3, #24
   4e640: ed9f6a5c     	vldr	s12, [pc, #368]         @ 0x4e7b8 ; float -9.99999997475e-07
   4e644: eef76a00     	vmov.f32	s13, #1.000000e+00
   4e648: e0234693     	mla	r3, r3, r6, r4
   4e64c: e2832028     	add	r2, r3, #40
   4e650: e1c302d0     	ldrd	r0, r1, [r3, #32]
   4e654: e8820003     	stm	r2, {r0, r1}
   4e658: edd35a0c     	vldr	s11, [r3, #48]
   4e65c: edd57a02     	vldr	s15, [r5, #8]
   4e660: eee57a86     	vfma.f32	s15, s11, s12
   4e664: eef47ae6     	vcmpe.f32	s15, s13
   4e668: edc57a02     	vstr	s15, [r5, #8]
   4e66c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e670: a5953004     	ldrge	r3, [r5, #0x4]
   4e674: ba000006     	blt	0x4e694
   4e678: ee777ae6     	vsub.f32	s15, s15, s13
   4e67c: e2833001     	add	r3, r3, #1
   4e680: eef47ae6     	vcmpe.f32	s15, s13
   4e684: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e688: aafffffa     	bge	0x4e678
   4e68c: e5853004     	str	r3, [r5, #0x4]
   4e690: edc57a02     	vstr	s15, [r5, #8]
   4e694: eef57ac0     	vcmpe.f32	s15, #0
   4e698: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e69c: 45953004     	ldrmi	r3, [r5, #0x4]
   4e6a0: 4ef76a00     	vmovmi.f32	s13, #1.000000e+00
   4e6a4: 5a000006     	bpl	0x4e6c4
   4e6a8: ee777aa6     	vadd.f32	s15, s15, s13
   4e6ac: e2433001     	sub	r3, r3, #1
   4e6b0: eef57ac0     	vcmpe.f32	s15, #0
   4e6b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e6b8: 4afffffa     	bmi	0x4e6a8
   4e6bc: e5853004     	str	r3, [r5, #0x4]
   4e6c0: edc57a02     	vstr	s15, [r5, #8]
   4e6c4: e3a03018     	mov	r3, #24
   4e6c8: ed9f6a3b     	vldr	s12, [pc, #236]         @ 0x4e7bc ; float 0
   4e6cc: eef76a00     	vmov.f32	s13, #1.000000e+00
   4e6d0: e0234693     	mla	r3, r3, r6, r4
   4e6d4: e2832028     	add	r2, r3, #40
   4e6d8: e1c302d0     	ldrd	r0, r1, [r3, #32]
   4e6dc: e8820003     	stm	r2, {r0, r1}
   4e6e0: edd35a0c     	vldr	s11, [r3, #48]
   4e6e4: edd57a02     	vldr	s15, [r5, #8]
   4e6e8: eee57a86     	vfma.f32	s15, s11, s12
   4e6ec: eef47ae6     	vcmpe.f32	s15, s13
   4e6f0: edc57a02     	vstr	s15, [r5, #8]
   4e6f4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e6f8: a5953004     	ldrge	r3, [r5, #0x4]
   4e6fc: a2833001     	addge	r3, r3, #1
   4e700: ba000007     	blt	0x4e724
   4e704: ee777ae6     	vsub.f32	s15, s15, s13
   4e708: e1a02003     	mov	r2, r3
   4e70c: e2833001     	add	r3, r3, #1
   4e710: eef47ae6     	vcmpe.f32	s15, s13
   4e714: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e718: aafffff9     	bge	0x4e704
   4e71c: e5852004     	str	r2, [r5, #0x4]
   4e720: edc57a02     	vstr	s15, [r5, #8]
   4e724: eef57ac0     	vcmpe.f32	s15, #0
   4e728: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e72c: 45953004     	ldrmi	r3, [r5, #0x4]
   4e730: 4ef76a00     	vmovmi.f32	s13, #1.000000e+00
   4e734: 42433001     	submi	r3, r3, #1
   4e738: 5a000007     	bpl	0x4e75c
   4e73c: ee777aa6     	vadd.f32	s15, s15, s13
   4e740: e1a02003     	mov	r2, r3
   4e744: e2433001     	sub	r3, r3, #1
   4e748: eef57ac0     	vcmpe.f32	s15, #0
   4e74c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4e750: 4afffff9     	bmi	0x4e73c
   4e754: e5852004     	str	r2, [r5, #0x4]
   4e758: edc57a02     	vstr	s15, [r5, #8]
   4e75c: e3a03018     	mov	r3, #24
   4e760: e35c0000     	cmp	r12, #0
   4e764: e0234693     	mla	r3, r3, r6, r4
   4e768: ed837a0c     	vstr	s14, [r3, #48]
   4e76c: 1affff74     	bne	0x4e544
   4e770: e28dd0d4     	add	sp, sp, #212
   4e774: ecbd8b02     	vpop	{d8}
   4e778: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   4e77c: e3a01018     	mov	r1, #24
   4e780: e3a02000     	mov	r2, #0
   4e784: e3a00001     	mov	r0, #1
   4e788: e0244691     	mla	r4, r1, r6, r4
   4e78c: e2843028     	add	r3, r4, #40
   4e790: e5c4001c     	strb	r0, [r4, #0x1c]
   4e794: e585c004     	str	r12, [r5, #0x4]
   4e798: e5852008     	str	r2, [r5, #0x8]
   4e79c: e1c402d0     	ldrd	r0, r1, [r4, #32]
   4e7a0: e8830003     	stm	r3, {r0, r1}
   4e7a4: ed847a0c     	vstr	s14, [r4, #48]
   4e7a8: e28dd0d4     	add	sp, sp, #212
   4e7ac: ecbd8b02     	vpop	{d8}
   4e7b0: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   4e7b4: 00 00 80 43  	.word	0x43800000
   4e7b8: bd 37 86 b5  	.word	0xb58637bd
   4e7bc: 00 00 00 00  	.word	0x00000000
   4e7c0: e594307c     	ldr	r3, [r4, #0x7c]
   4e7c4: e28d0010     	add	r0, sp, #16
   4e7c8: e3a02010     	mov	r2, #16
   4e7cc: e58d3000     	str	r3, [sp]
   4e7d0: e3061218     	movw	r1, #0x6218
   4e7d4: e3401001     	movt	r1, #0x1
   4e7d8: e3023ab0     	movw	r3, #0x2ab0
   4e7dc: e3403007     	movt	r3, #0x7
   4e7e0: ebffeea8     	bl	0x4a288
   4e7e4: e3a02000     	mov	r2, #0
   4e7e8: e3a0c007     	mov	r12, #7
   4e7ec: e3033698     	movw	r3, #0x3698
   4e7f0: e3403007     	movt	r3, #0x7
   4e7f4: e28d0010     	add	r0, sp, #16
   4e7f8: e1a01002     	mov	r1, r2
   4e7fc: e58dc000     	str	r12, [sp]
   4e800: ebff1cbc     	bl	0x15af8    @ imm = #-0x38d10 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_replaceEjjPKcj
   4e804: e1a0e000     	mov	lr, r0
   4e808: e28d3030     	add	r3, sp, #48
   4e80c: e58d3028     	str	r3, [sp, #0x28]
   4e810: e1a0c000     	mov	r12, r0
   4e814: e49e3008     	ldr	r3, [lr], #8
   4e818: e153000e     	cmp	r3, lr
   4e81c: 158d3028     	strne	r3, [sp, #0x28]
   4e820: 028da030     	addeq	r10, sp, #48
   4e824: 059e1004     	ldreq	r1, [lr, #0x4]
   4e828: 059e2008     	ldreq	r2, [lr, #0x8]
   4e82c: 059e300c     	ldreq	r3, [lr, #0xc]
   4e830: 059e0000     	ldreq	r0, [lr]
   4e834: 159c2008     	ldrne	r2, [r12, #0x8]
   4e838: 08aa000f     	stmeq	r10!, {r0, r1, r2, r3}
   4e83c: 158d2030     	strne	r2, [sp, #0x30]
   4e840: e3a02000     	mov	r2, #0
   4e844: e5cc2008     	strb	r2, [r12, #0x8]
   4e848: e59c3004     	ldr	r3, [r12, #0x4]
   4e84c: e58d302c     	str	r3, [sp, #0x2c]
   4e850: e3e03103     	mvn	r3, #-1073741824
   4e854: e58c2004     	str	r2, [r12, #0x4]
   4e858: e59d102c     	ldr	r1, [sp, #0x2c]
   4e85c: e58ce000     	str	lr, [r12]
   4e860: e0433001     	sub	r3, r3, r1
   4e864: e3530005     	cmp	r3, #5
   4e868: 9a0000f0     	bls	0x4ec30
   4e86c: e30316a0     	movw	r1, #0x36a0
   4e870: e3401007     	movt	r1, #0x7
   4e874: e3a02006     	mov	r2, #6
   4e878: e28d0028     	add	r0, sp, #40
   4e87c: ebff1d81     	bl	0x15e88    @ imm = #-0x389fc ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   4e880: e1a0e000     	mov	lr, r0
   4e884: e28db048     	add	r11, sp, #72
   4e888: e58db040     	str	r11, [sp, #0x40]
   4e88c: e1a0c000     	mov	r12, r0
   4e890: e49e3008     	ldr	r3, [lr], #8
   4e894: e153000e     	cmp	r3, lr
   4e898: 158d3040     	strne	r3, [sp, #0x40]
   4e89c: 01a0a00b     	moveq	r10, r11
   4e8a0: 059e0000     	ldreq	r0, [lr]
   4e8a4: 059e1004     	ldreq	r1, [lr, #0x4]
   4e8a8: 059e2008     	ldreq	r2, [lr, #0x8]
   4e8ac: 059e300c     	ldreq	r3, [lr, #0xc]
   4e8b0: 159c2008     	ldrne	r2, [r12, #0x8]
   4e8b4: 08aa000f     	stmeq	r10!, {r0, r1, r2, r3}
   4e8b8: e3a03000     	mov	r3, #0
   4e8bc: e3061218     	movw	r1, #0x6218
   4e8c0: e3401001     	movt	r1, #0x1
   4e8c4: 158d2048     	strne	r2, [sp, #0x48]
   4e8c8: e5cc3008     	strb	r3, [r12, #0x8]
   4e8cc: e28d0058     	add	r0, sp, #88
   4e8d0: e59c2004     	ldr	r2, [r12, #0x4]
   4e8d4: e58d2044     	str	r2, [sp, #0x44]
   4e8d8: e58c3004     	str	r3, [r12, #0x4]
   4e8dc: e3023ab0     	movw	r3, #0x2ab0
   4e8e0: e3403007     	movt	r3, #0x7
   4e8e4: e58ce000     	str	lr, [r12]
   4e8e8: e5942080     	ldr	r2, [r4, #0x80]
   4e8ec: e58d2000     	str	r2, [sp]
   4e8f0: e3a02010     	mov	r2, #16
   4e8f4: ebffee63     	bl	0x4a288
   4e8f8: e59d3040     	ldr	r3, [sp, #0x40]
   4e8fc: e59dc044     	ldr	r12, [sp, #0x44]
   4e900: e153000b     	cmp	r3, r11
   4e904: e59d205c     	ldr	r2, [sp, #0x5c]
   4e908: 03a0100f     	moveq	r1, #15
   4e90c: e08c0002     	add	r0, r12, r2
   4e910: 159d1048     	ldrne	r1, [sp, #0x48]
   4e914: e1500001     	cmp	r0, r1
   4e918: e59d1058     	ldr	r1, [sp, #0x58]
   4e91c: 928d3060     	addls	r3, sp, #96
   4e920: 958d3008     	strls	r3, [sp, #0x8]
   4e924: 9a000006     	bls	0x4e944
   4e928: e28de060     	add	lr, sp, #96
   4e92c: e58de008     	str	lr, [sp, #0x8]
   4e930: e151000e     	cmp	r1, lr
   4e934: 03a0e00f     	moveq	lr, #15
   4e938: 159de060     	ldrne	lr, [sp, #0x60]
   4e93c: e150000e     	cmp	r0, lr
   4e940: 9a00009d     	bls	0x4ebbc
   4e944: e28d0040     	add	r0, sp, #64
   4e948: ebff1d4e     	bl	0x15e88    @ imm = #-0x38ac8 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   4e94c: e1a0e000     	mov	lr, r0
   4e950: e28d3078     	add	r3, sp, #120
   4e954: e58d3070     	str	r3, [sp, #0x70]
   4e958: e1a0c000     	mov	r12, r0
   4e95c: e49e3008     	ldr	r3, [lr], #8
   4e960: e153000e     	cmp	r3, lr
   4e964: 158d3070     	strne	r3, [sp, #0x70]
   4e968: 028da078     	addeq	r10, sp, #120
   4e96c: 059e1004     	ldreq	r1, [lr, #0x4]
   4e970: 059e2008     	ldreq	r2, [lr, #0x8]
   4e974: 059e300c     	ldreq	r3, [lr, #0xc]
   4e978: 059e0000     	ldreq	r0, [lr]
   4e97c: 159c2008     	ldrne	r2, [r12, #0x8]
   4e980: 08aa000f     	stmeq	r10!, {r0, r1, r2, r3}
   4e984: 158d2078     	strne	r2, [sp, #0x78]
   4e988: e3a02000     	mov	r2, #0
   4e98c: e5cc2008     	strb	r2, [r12, #0x8]
   4e990: e59c3004     	ldr	r3, [r12, #0x4]
   4e994: e58d3074     	str	r3, [sp, #0x74]
   4e998: e3e03103     	mvn	r3, #-1073741824
   4e99c: e58c2004     	str	r2, [r12, #0x4]
   4e9a0: e59d1074     	ldr	r1, [sp, #0x74]
   4e9a4: e58ce000     	str	lr, [r12]
   4e9a8: e0433001     	sub	r3, r3, r1
   4e9ac: e353002a     	cmp	r3, #42
   4e9b0: 9a00009b     	bls	0x4ec24
   4e9b4: e30316cc     	movw	r1, #0x36cc
   4e9b8: e3401007     	movt	r1, #0x7
   4e9bc: e3a0202b     	mov	r2, #43
   4e9c0: e28d0070     	add	r0, sp, #112
   4e9c4: ebff1d2f     	bl	0x15e88    @ imm = #-0x38b44 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   4e9c8: e1a0e000     	mov	lr, r0
   4e9cc: e28d3090     	add	r3, sp, #144
   4e9d0: e58d3088     	str	r3, [sp, #0x88]
   4e9d4: e1a0c000     	mov	r12, r0
   4e9d8: e49e3008     	ldr	r3, [lr], #8
   4e9dc: e153000e     	cmp	r3, lr
   4e9e0: 158d3088     	strne	r3, [sp, #0x88]
   4e9e4: 028d3090     	addeq	r3, sp, #144
   4e9e8: 01a0a003     	moveq	r10, r3
   4e9ec: 059e0000     	ldreq	r0, [lr]
   4e9f0: 059e1004     	ldreq	r1, [lr, #0x4]
   4e9f4: 059e2008     	ldreq	r2, [lr, #0x8]
   4e9f8: 059e300c     	ldreq	r3, [lr, #0xc]
   4e9fc: 159c2008     	ldrne	r2, [r12, #0x8]
   4ea00: 08aa000f     	stmeq	r10!, {r0, r1, r2, r3}
   4ea04: e3061218     	movw	r1, #0x6218
   4ea08: e3401001     	movt	r1, #0x1
   4ea0c: 158d2090     	strne	r2, [sp, #0x90]
   4ea10: e28d00a0     	add	r0, sp, #160
   4ea14: e3a02000     	mov	r2, #0
   4ea18: e5cc2008     	strb	r2, [r12, #0x8]
   4ea1c: e59c3004     	ldr	r3, [r12, #0x4]
   4ea20: e58d308c     	str	r3, [sp, #0x8c]
   4ea24: e3003d28     	movw	r3, #0xd28
   4ea28: e3403007     	movt	r3, #0x7
   4ea2c: e58c2004     	str	r2, [r12, #0x4]
   4ea30: e58ce000     	str	lr, [r12]
   4ea34: e3a02010     	mov	r2, #16
   4ea38: e58d6000     	str	r6, [sp]
   4ea3c: ebffee11     	bl	0x4a288
   4ea40: e59d3088     	ldr	r3, [sp, #0x88]
   4ea44: e28d1090     	add	r1, sp, #144
   4ea48: e59dc08c     	ldr	r12, [sp, #0x8c]
   4ea4c: e1530001     	cmp	r3, r1
   4ea50: e59d20a4     	ldr	r2, [sp, #0xa4]
   4ea54: 03a0100f     	moveq	r1, #15
   4ea58: e08c0002     	add	r0, r12, r2
   4ea5c: 159d1090     	ldrne	r1, [sp, #0x90]
   4ea60: e1500001     	cmp	r0, r1
   4ea64: e59d10a0     	ldr	r1, [sp, #0xa0]
   4ea68: 928d30a8     	addls	r3, sp, #168
   4ea6c: 958d300c     	strls	r3, [sp, #0xc]
   4ea70: 9a000006     	bls	0x4ea90
   4ea74: e28de0a8     	add	lr, sp, #168
   4ea78: e58de00c     	str	lr, [sp, #0xc]
   4ea7c: e151000e     	cmp	r1, lr
   4ea80: 03a0e00f     	moveq	lr, #15
   4ea84: 159de0a8     	ldrne	lr, [sp, #0xa8]
   4ea88: e150000e     	cmp	r0, lr
   4ea8c: 9a000044     	bls	0x4eba4
   4ea90: e28d0088     	add	r0, sp, #136
   4ea94: ebff1cfb     	bl	0x15e88    @ imm = #-0x38c14 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   4ea98: e1a0e000     	mov	lr, r0
   4ea9c: e28d30c0     	add	r3, sp, #192
   4eaa0: e58d30b8     	str	r3, [sp, #0xb8]
   4eaa4: e1a0c000     	mov	r12, r0
   4eaa8: e49e3008     	ldr	r3, [lr], #8
   4eaac: e153000e     	cmp	r3, lr
   4eab0: 158d30b8     	strne	r3, [sp, #0xb8]
   4eab4: 028d30c0     	addeq	r3, sp, #192
   4eab8: 01a0a003     	moveq	r10, r3
   4eabc: 059e0000     	ldreq	r0, [lr]
   4eac0: 059e1004     	ldreq	r1, [lr, #0x4]
   4eac4: 059e2008     	ldreq	r2, [lr, #0x8]
   4eac8: 059e300c     	ldreq	r3, [lr, #0xc]
   4eacc: 159c2008     	ldrne	r2, [r12, #0x8]
   4ead0: 08aa000f     	stmeq	r10!, {r0, r1, r2, r3}
   4ead4: e3a03000     	mov	r3, #0
   4ead8: e3090fec     	movw	r0, #0x9fec
   4eadc: e3400009     	movt	r0, #0x9
   4eae0: 158d20c0     	strne	r2, [sp, #0xc0]
   4eae4: e28d10b8     	add	r1, sp, #184
   4eae8: e5cc3008     	strb	r3, [r12, #0x8]
   4eaec: e59c2004     	ldr	r2, [r12, #0x4]
   4eaf0: e58d20bc     	str	r2, [sp, #0xbc]
   4eaf4: e3a02001     	mov	r2, #1
   4eaf8: e58ce000     	str	lr, [r12]
   4eafc: e58c3004     	str	r3, [r12, #0x4]
   4eb00: eb008506     	bl	0x6ff20
   4eb04: e59d00b8     	ldr	r0, [sp, #0xb8]
   4eb08: e28d30c0     	add	r3, sp, #192
   4eb0c: e1500003     	cmp	r0, r3
   4eb10: 0a000000     	beq	0x4eb18
   4eb14: ebff1cc9     	bl	0x15e40    @ imm = #-0x38cdc ; _ZdlPv
   4eb18: e59d00a0     	ldr	r0, [sp, #0xa0]
   4eb1c: e59d300c     	ldr	r3, [sp, #0xc]
   4eb20: e1500003     	cmp	r0, r3
   4eb24: 0a000000     	beq	0x4eb2c
   4eb28: ebff1cc4     	bl	0x15e40    @ imm = #-0x38cf0 ; _ZdlPv
   4eb2c: e59d0088     	ldr	r0, [sp, #0x88]
   4eb30: e28d3090     	add	r3, sp, #144
   4eb34: e1500003     	cmp	r0, r3
   4eb38: 0a000000     	beq	0x4eb40
   4eb3c: ebff1cbf     	bl	0x15e40    @ imm = #-0x38d04 ; _ZdlPv
   4eb40: e59d0070     	ldr	r0, [sp, #0x70]
   4eb44: e28d3078     	add	r3, sp, #120
   4eb48: e1500003     	cmp	r0, r3
   4eb4c: 0a000000     	beq	0x4eb54
   4eb50: ebff1cba     	bl	0x15e40    @ imm = #-0x38d18 ; _ZdlPv
   4eb54: e59d0058     	ldr	r0, [sp, #0x58]
   4eb58: e59d3008     	ldr	r3, [sp, #0x8]
   4eb5c: e1500003     	cmp	r0, r3
   4eb60: 0a000000     	beq	0x4eb68
   4eb64: ebff1cb5     	bl	0x15e40    @ imm = #-0x38d2c ; _ZdlPv
   4eb68: e59d0040     	ldr	r0, [sp, #0x40]
   4eb6c: e150000b     	cmp	r0, r11
   4eb70: 0a000000     	beq	0x4eb78
   4eb74: ebff1cb1     	bl	0x15e40    @ imm = #-0x38d3c ; _ZdlPv
   4eb78: e59d0028     	ldr	r0, [sp, #0x28]
   4eb7c: e28d3030     	add	r3, sp, #48
   4eb80: e1500003     	cmp	r0, r3
   4eb84: 0a000000     	beq	0x4eb8c
   4eb88: ebff1cac     	bl	0x15e40    @ imm = #-0x38d50 ; _ZdlPv
   4eb8c: e59d0010     	ldr	r0, [sp, #0x10]
   4eb90: e28d3018     	add	r3, sp, #24
   4eb94: e1500003     	cmp	r0, r3
   4eb98: 0afffe43     	beq	0x4e4ac
   4eb9c: ebff1ca7     	bl	0x15e40    @ imm = #-0x38d64 ; _ZdlPv
   4eba0: eafffe41     	b	0x4e4ac
   4eba4: e3a02000     	mov	r2, #0
   4eba8: e28d00a0     	add	r0, sp, #160
   4ebac: e1a01002     	mov	r1, r2
   4ebb0: e58dc000     	str	r12, [sp]
   4ebb4: ebff1bcf     	bl	0x15af8    @ imm = #-0x390c4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_replaceEjjPKcj
   4ebb8: eaffffb6     	b	0x4ea98
   4ebbc: e3a02000     	mov	r2, #0
   4ebc0: e28d0058     	add	r0, sp, #88
   4ebc4: e1a01002     	mov	r1, r2
   4ebc8: e58dc000     	str	r12, [sp]
   4ebcc: ebff1bc9     	bl	0x15af8    @ imm = #-0x390dc ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_replaceEjjPKcj
   4ebd0: eaffff5d     	b	0x4e94c
   4ebd4: e0473002     	sub	r3, r7, r2
   4ebd8: ee073a90     	vmov	s15, r3
   4ebdc: e3a02018     	mov	r2, #24
   4ebe0: e3a03000     	mov	r3, #0
   4ebe4: eef87ae7     	vcvt.f32.s32	s15, s15
   4ebe8: e3a01001     	mov	r1, #1
   4ebec: e3a075fe     	mov	r7, #1065353216
   4ebf0: e0224692     	mla	r2, r2, r6, r4
   4ebf4: ee677a87     	vmul.f32	s15, s15, s14
   4ebf8: e282e028     	add	lr, r2, #40
   4ebfc: e5c2101c     	strb	r1, [r2, #0x1c]
   4ec00: e5853008     	str	r3, [r5, #0x8]
   4ec04: eefd7ae7     	vcvt.s32.f32	s15, s15
   4ec08: ee173a90     	vmov	r3, s15
   4ec0c: e26330ff     	rsb	r3, r3, #255
   4ec10: e5853004     	str	r3, [r5, #0x4]
   4ec14: e1c202d0     	ldrd	r0, r1, [r2, #32]
   4ec18: e88e0003     	stm	lr, {r0, r1}
   4ec1c: e5827030     	str	r7, [r2, #0x30]
   4ec20: eafffe85     	b	0x4e63c
   4ec24: e3010b08     	movw	r0, #0x1b08
   4ec28: e3400007     	movt	r0, #0x7
   4ec2c: ebff1be1     	bl	0x15bb8    @ imm = #-0x3907c ; _ZSt20__throw_length_errorPKc
   4ec30: e3010b08     	movw	r0, #0x1b08
   4ec34: e3400007     	movt	r0, #0x7
   4ec38: ebff1bde     	bl	0x15bb8    @ imm = #-0x39088 ; _ZSt20__throw_length_errorPKc
   4ec3c: e59d0070     	ldr	r0, [sp, #0x70]
   4ec40: e28d3078     	add	r3, sp, #120
   4ec44: e1500003     	cmp	r0, r3
   4ec48: 0a000000     	beq	0x4ec50
   4ec4c: ebff1c7b     	bl	0x15e40    @ imm = #-0x38e14 ; _ZdlPv
   4ec50: e59d0058     	ldr	r0, [sp, #0x58]
   4ec54: e59d3008     	ldr	r3, [sp, #0x8]
   4ec58: e1500003     	cmp	r0, r3
   4ec5c: 0a000000     	beq	0x4ec64
   4ec60: ebff1c76     	bl	0x15e40    @ imm = #-0x38e28 ; _ZdlPv
   4ec64: e59d0040     	ldr	r0, [sp, #0x40]
   4ec68: e150000b     	cmp	r0, r11
   4ec6c: 0a000000     	beq	0x4ec74
   4ec70: ebff1c72     	bl	0x15e40    @ imm = #-0x38e38 ; _ZdlPv
   4ec74: e59d0028     	ldr	r0, [sp, #0x28]
   4ec78: e28d3030     	add	r3, sp, #48
   4ec7c: e1500003     	cmp	r0, r3
   4ec80: 0a000000     	beq	0x4ec88
   4ec84: ebff1c6d     	bl	0x15e40    @ imm = #-0x38e4c ; _ZdlPv
   4ec88: e59d0010     	ldr	r0, [sp, #0x10]
   4ec8c: e28d3018     	add	r3, sp, #24
   4ec90: e1500003     	cmp	r0, r3
   4ec94: 0a000000     	beq	0x4ec9c
   4ec98: ebff1c68     	bl	0x15e40    @ imm = #-0x38e60 ; _ZdlPv
   4ec9c: ebff1caf     	bl	0x15f60    @ imm = #-0x38d44 ; __cxa_end_cleanup
   4eca0: eaffffea     	b	0x4ec50
   4eca4: eaffffee     	b	0x4ec64
   4eca8: eafffff1     	b	0x4ec74
   4ecac: e59d0088     	ldr	r0, [sp, #0x88]
   4ecb0: e28d3090     	add	r3, sp, #144
   4ecb4: e1500003     	cmp	r0, r3
   4ecb8: 0affffdf     	beq	0x4ec3c
   4ecbc: ebff1c5f     	bl	0x15e40    @ imm = #-0x38e84 ; _ZdlPv
   4ecc0: eaffffdd     	b	0x4ec3c
   4ecc4: e59d00a0     	ldr	r0, [sp, #0xa0]
   4ecc8: e59d300c     	ldr	r3, [sp, #0xc]
   4eccc: e1500003     	cmp	r0, r3
   4ecd0: 0afffff5     	beq	0x4ecac
   4ecd4: ebff1c59     	bl	0x15e40    @ imm = #-0x38e9c ; _ZdlPv
   4ecd8: eafffff3     	b	0x4ecac
   4ecdc: e59d00b8     	ldr	r0, [sp, #0xb8]
   4ece0: e28d30c0     	add	r3, sp, #192
   4ece4: e1500003     	cmp	r0, r3
   4ece8: 0afffff5     	beq	0x4ecc4
   4ecec: ebff1c53     	bl	0x15e40    @ imm = #-0x38eb4 ; _ZdlPv
   4ecf0: eafffff3     	b	0x4ecc4
   4ecf4: eaffffe3     	b	0x4ec88
