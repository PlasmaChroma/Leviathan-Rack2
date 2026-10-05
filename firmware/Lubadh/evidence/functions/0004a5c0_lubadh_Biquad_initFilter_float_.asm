; lubadh::Biquad::initFilter(float)
; VA 0x4a5c0 size 944

   4a5c0: e92d4010     	push	{r4, lr}
   4a5c4: e1a04000     	mov	r4, r0
   4a5c8: eddf0be2     	vldr	d16, [pc, #904]         @ 0x4a958 ; float 3.14159265359
   4a5cc: ed2d8b06     	vpush	{d8, d9, d10}
   4a5d0: eeb79ac0     	vcvt.f64.f32	d9, s0
   4a5d4: ee290b20     	vmul.f64	d0, d9, d16
   4a5d8: eeb70bc0     	vcvt.f32.f64	s0, d0
   4a5dc: ebff2cfd     	bl	0x159d8    @ imm = #-0x34c0c ; tanf
   4a5e0: e5943028     	ldr	r3, [r4, #0x28]
   4a5e4: eeb08a40     	vmov.f32	s16, s0
   4a5e8: ed94aa00     	vldr	s20, [r4]
   4a5ec: e3530005     	cmp	r3, #5
   4a5f0: 979ff103     	ldrls	pc, [pc, r3, lsl #2]
   4a5f4: ea00003a     	b	0x4a6e4
   4a5f8: 14 a8 04 00  	.word	0x0004a814
   4a5fc: c4 a7 04 00  	.word	0x0004a7c4
   4a600: e4 a6 04 00  	.word	0x0004a6e4
   4a604: e4 a6 04 00  	.word	0x0004a6e4
   4a608: ec a6 04 00  	.word	0x0004a6ec
   4a60c: 10 a6 04 00  	.word	0x0004a610
   4a610: eef18aca     	vsqrt.f32	s17, s20
   4a614: eeb5aa40     	vcmp.f32	s20, #0
   4a618: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4a61c: 5a000001     	bpl	0x4a628
   4a620: eeb00a4a     	vmov.f32	s0, s20
   4a624: ebff2d51     	bl	0x15b70    @ imm = #-0x34abc ; sqrtf
   4a628: eef72ac8     	vcvt.f64.f32	d18, s16
   4a62c: eddf6bcb     	vldr	d22, [pc, #812]         @ 0x4a960 ; float 1.41421356237
   4a630: eef76a00     	vmov.f32	s13, #1.000000e+00
   4a634: eef78b00     	vmov.f64	d24, #1.000000e+00
   4a638: ee288a08     	vmul.f32	s16, s16, s16
   4a63c: eef71ae8     	vcvt.f64.f32	d17, s17
   4a640: eeb4aae6     	vcmpe.f32	s20, s13
   4a644: eef04b68     	vmov.f64	d20, d24
   4a648: eef05b68     	vmov.f64	d21, d24
   4a64c: eee24ba6     	vfma.f64	d20, d18, d22
   4a650: eee25be6     	vfms.f64	d21, d18, d22
   4a654: e5942010     	ldr	r2, [r4, #0x10]
   4a658: eef77ac8     	vcvt.f64.f32	d23, s16
   4a65c: ee387a66     	vsub.f32	s14, s16, s13
   4a660: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4a664: e5943004     	ldr	r3, [r4, #0x4]
   4a668: ee377a07     	vadd.f32	s14, s14, s14
   4a66c: ee744ba7     	vadd.f64	d20, d20, d23
   4a670: ee755ba7     	vadd.f64	d21, d21, d23
   4a674: aa000079     	bge	0x4a860
   4a678: eec63ba1     	vdiv.f64	d19, d22, d17
   4a67c: eec67a8a     	vdiv.f32	s15, s13, s20
   4a680: eef71ae7     	vcvt.f64.f32	d17, s15
   4a684: ee388a67     	vsub.f32	s16, s16, s15
   4a688: eef00b61     	vmov.f64	d16, d17
   4a68c: ee787a08     	vadd.f32	s15, s16, s16
   4a690: eee30ba2     	vfma.f64	d16, d19, d18
   4a694: eee31be2     	vfms.f64	d17, d19, d18
   4a698: ee702ba7     	vadd.f64	d18, d16, d23
   4a69c: ee710ba7     	vadd.f64	d16, d17, d23
   4a6a0: ee888ba2     	vdiv.f64	d8, d24, d18
   4a6a4: eeb78bc8     	vcvt.f32.f64	s16, d8
   4a6a8: eef71ac8     	vcvt.f64.f32	d17, s16
   4a6ac: ee287a07     	vmul.f32	s14, s16, s14
   4a6b0: ee278a88     	vmul.f32	s16, s15, s16
   4a6b4: ee614ba4     	vmul.f64	d20, d17, d20
   4a6b8: ee615ba5     	vmul.f64	d21, d17, d21
   4a6bc: ee600ba1     	vmul.f64	d16, d16, d17
   4a6c0: ed827a01     	vstr	s14, [r2, #4]
   4a6c4: eeb76be4     	vcvt.f32.f64	s12, d20
   4a6c8: eeb77be5     	vcvt.f32.f64	s14, d21
   4a6cc: eef77be0     	vcvt.f32.f64	s15, d16
   4a6d0: ed826a00     	vstr	s12, [r2]
   4a6d4: ed827a02     	vstr	s14, [r2, #8]
   4a6d8: ed838a01     	vstr	s16, [r3, #4]
   4a6dc: edc37a02     	vstr	s15, [r3, #8]
   4a6e0: edc36a00     	vstr	s13, [r3]
   4a6e4: ecbd8b06     	vpop	{d8, d9, d10}
   4a6e8: e8bd8010     	pop	{r4, pc}
   4a6ec: eef18aca     	vsqrt.f32	s17, s20
   4a6f0: eeb5aa40     	vcmp.f32	s20, #0
   4a6f4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4a6f8: 4a000092     	bmi	0x4a948
   4a6fc: eef76ac8     	vcvt.f64.f32	d22, s16
   4a700: eddf5b96     	vldr	d21, [pc, #600]         @ 0x4a960 ; float 1.41421356237
   4a704: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4a708: eef73b00     	vmov.f64	d19, #1.000000e+00
   4a70c: ee686a08     	vmul.f32	s13, s16, s16
   4a710: eef70ae8     	vcvt.f64.f32	d16, s17
   4a714: eeb4aac6     	vcmpe.f32	s20, s12
   4a718: eef02b63     	vmov.f64	d18, d19
   4a71c: eef04b63     	vmov.f64	d20, d19
   4a720: eee62ba5     	vfma.f64	d18, d22, d21
   4a724: eee64be5     	vfms.f64	d20, d22, d21
   4a728: eef71ae6     	vcvt.f64.f32	d17, s13
   4a72c: ee367ac6     	vsub.f32	s14, s13, s12
   4a730: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4a734: ee377a07     	vadd.f32	s14, s14, s14
   4a738: ee712ba2     	vadd.f64	d18, d17, d18
   4a73c: ee714ba4     	vadd.f64	d20, d17, d20
   4a740: ba000062     	blt	0x4a8d0
   4a744: eec31ba2     	vdiv.f64	d17, d19, d18
   4a748: ee280a0a     	vmul.f32	s0, s16, s20
   4a74c: ee600ba5     	vmul.f64	d16, d16, d21
   4a750: eef02b63     	vmov.f64	d18, d19
   4a754: e5942010     	ldr	r2, [r4, #0x10]
   4a758: e5943004     	ldr	r3, [r4, #0x4]
   4a75c: ee208a08     	vmul.f32	s16, s0, s16
   4a760: eee02ba6     	vfma.f64	d18, d16, d22
   4a764: eee03be6     	vfms.f64	d19, d16, d22
   4a768: eef70ac8     	vcvt.f64.f32	d16, s16
   4a76c: ee388a46     	vsub.f32	s16, s16, s12
   4a770: ee787a08     	vadd.f32	s15, s16, s16
   4a774: ee722ba0     	vadd.f64	d18, d18, d16
   4a778: ee700ba3     	vadd.f64	d16, d16, d19
   4a77c: eeb78be1     	vcvt.f32.f64	s16, d17
   4a780: eef71ac8     	vcvt.f64.f32	d17, s16
   4a784: ee685a07     	vmul.f32	s11, s16, s14
   4a788: ee278a88     	vmul.f32	s16, s15, s16
   4a78c: ee622ba1     	vmul.f64	d18, d18, d17
   4a790: ee600ba1     	vmul.f64	d16, d16, d17
   4a794: ee611ba4     	vmul.f64	d17, d17, d20
   4a798: ed828a01     	vstr	s16, [r2, #4]
   4a79c: ecbd8b06     	vpop	{d8, d9, d10}
   4a7a0: eef76be2     	vcvt.f32.f64	s13, d18
   4a7a4: eef77be0     	vcvt.f32.f64	s15, d16
   4a7a8: eeb77be1     	vcvt.f32.f64	s14, d17
   4a7ac: edc26a00     	vstr	s13, [r2]
   4a7b0: edc27a02     	vstr	s15, [r2, #8]
   4a7b4: edc35a01     	vstr	s11, [r3, #4]
   4a7b8: ed837a02     	vstr	s14, [r3, #8]
   4a7bc: ed836a00     	vstr	s12, [r3]
   4a7c0: e8bd8010     	pop	{r4, pc}
   4a7c4: eeb60b00     	vmov.f64	d0, #5.000000e-01
   4a7c8: eddf0b66     	vldr	d16, [pc, #408]         @ 0x4a968 ; float -6.28318530718
   4a7cc: e5943004     	ldr	r3, [r4, #0x4]
   4a7d0: eeb78a00     	vmov.f32	s16, #1.000000e+00
   4a7d4: ee300b49     	vsub.f64	d0, d0, d9
   4a7d8: ed838a00     	vstr	s16, [r3]
   4a7dc: ee200b20     	vmul.f64	d0, d0, d16
   4a7e0: eeb70bc0     	vcvt.f32.f64	s0, d0
   4a7e4: ebff2edf     	bl	0x16368    @ imm = #-0x34484 ; expf
   4a7e8: e5941004     	ldr	r1, [r4, #0x4]
   4a7ec: e5943010     	ldr	r3, [r4, #0x10]
   4a7f0: ee388a40     	vsub.f32	s16, s16, s0
   4a7f4: e3a02000     	mov	r2, #0
   4a7f8: e5812008     	str	r2, [r1, #0x8]
   4a7fc: ed810a01     	vstr	s0, [r1, #4]
   4a800: ed838a00     	vstr	s16, [r3]
   4a804: ecbd8b06     	vpop	{d8, d9, d10}
   4a808: e5832004     	str	r2, [r3, #0x4]
   4a80c: e5832008     	str	r2, [r3, #0x8]
   4a810: e8bd8010     	pop	{r4, pc}
   4a814: ed9f0b53     	vldr	d0, [pc, #332]          @ 0x4a968 ; float -6.28318530718
   4a818: eeb78a00     	vmov.f32	s16, #1.000000e+00
   4a81c: e5943004     	ldr	r3, [r4, #0x4]
   4a820: ee290b00     	vmul.f64	d0, d9, d0
   4a824: ed838a00     	vstr	s16, [r3]
   4a828: eeb70bc0     	vcvt.f32.f64	s0, d0
   4a82c: ebff2ecd     	bl	0x16368    @ imm = #-0x344cc ; expf
   4a830: e5941004     	ldr	r1, [r4, #0x4]
   4a834: e5943010     	ldr	r3, [r4, #0x10]
   4a838: ee388a40     	vsub.f32	s16, s16, s0
   4a83c: eeb10a40     	vneg.f32	s0, s0
   4a840: e3a02000     	mov	r2, #0
   4a844: e5812008     	str	r2, [r1, #0x8]
   4a848: ed810a01     	vstr	s0, [r1, #4]
   4a84c: ed838a00     	vstr	s16, [r3]
   4a850: ecbd8b06     	vpop	{d8, d9, d10}
   4a854: e5832004     	str	r2, [r3, #0x4]
   4a858: e5832008     	str	r2, [r3, #0x8]
   4a85c: e8bd8010     	pop	{r4, pc}
   4a860: eec83ba4     	vdiv.f64	d19, d24, d20
   4a864: eef70aca     	vcvt.f64.f32	d16, s20
   4a868: ee611ba6     	vmul.f64	d17, d17, d22
   4a86c: ee388a4a     	vsub.f32	s16, s16, s20
   4a870: eef04b60     	vmov.f64	d20, d16
   4a874: eee14ba2     	vfma.f64	d20, d17, d18
   4a878: eee10be2     	vfms.f64	d16, d17, d18
   4a87c: ee388a08     	vadd.f32	s16, s16, s16
   4a880: ee741ba7     	vadd.f64	d17, d20, d23
   4a884: ee700ba7     	vadd.f64	d16, d16, d23
   4a888: eef77be3     	vcvt.f32.f64	s15, d19
   4a88c: eef72ae7     	vcvt.f64.f32	d18, s15
   4a890: ee288a27     	vmul.f32	s16, s16, s15
   4a894: ee277a87     	vmul.f32	s14, s15, s14
   4a898: ee625ba5     	vmul.f64	d21, d18, d21
   4a89c: ee611ba2     	vmul.f64	d17, d17, d18
   4a8a0: ee600ba2     	vmul.f64	d16, d16, d18
   4a8a4: ed828a01     	vstr	s16, [r2, #4]
   4a8a8: ecbd8b06     	vpop	{d8, d9, d10}
   4a8ac: eef75be5     	vcvt.f32.f64	s11, d21
   4a8b0: eeb76be1     	vcvt.f32.f64	s12, d17
   4a8b4: eef77be0     	vcvt.f32.f64	s15, d16
   4a8b8: ed826a00     	vstr	s12, [r2]
   4a8bc: edc27a02     	vstr	s15, [r2, #8]
   4a8c0: ed837a01     	vstr	s14, [r3, #4]
   4a8c4: edc35a02     	vstr	s11, [r3, #8]
   4a8c8: edc36a00     	vstr	s13, [r3]
   4a8cc: e8bd8010     	pop	{r4, pc}
   4a8d0: eec57ba0     	vdiv.f64	d23, d21, d16
   4a8d4: eef01b63     	vmov.f64	d17, d19
   4a8d8: eec67a8a     	vdiv.f32	s15, s13, s20
   4a8dc: eef05b63     	vmov.f64	d21, d19
   4a8e0: e5943010     	ldr	r3, [r4, #0x10]
   4a8e4: e5942004     	ldr	r2, [r4, #0x4]
   4a8e8: eef70ae7     	vcvt.f64.f32	d16, s15
   4a8ec: ee376ac6     	vsub.f32	s12, s15, s12
   4a8f0: ee767a06     	vadd.f32	s15, s12, s12
   4a8f4: eee71ba6     	vfma.f64	d17, d23, d22
   4a8f8: eee75be6     	vfms.f64	d21, d23, d22
   4a8fc: ee711ba0     	vadd.f64	d17, d17, d16
   4a900: ee700ba5     	vadd.f64	d16, d16, d21
   4a904: ee836ba1     	vdiv.f64	d6, d19, d17
   4a908: eeb76bc6     	vcvt.f32.f64	s12, d6
   4a90c: eef73ac6     	vcvt.f64.f32	d19, s12
   4a910: ee267a07     	vmul.f32	s14, s12, s14
   4a914: ee276a86     	vmul.f32	s12, s15, s12
   4a918: ee632ba2     	vmul.f64	d18, d19, d18
   4a91c: ee634ba4     	vmul.f64	d20, d19, d20
   4a920: ee600ba3     	vmul.f64	d16, d16, d19
   4a924: ed837a01     	vstr	s14, [r3, #4]
   4a928: eef76be2     	vcvt.f32.f64	s13, d18
   4a92c: eeb77be4     	vcvt.f32.f64	s14, d20
   4a930: eef77be0     	vcvt.f32.f64	s15, d16
   4a934: edc36a00     	vstr	s13, [r3]
   4a938: ed837a02     	vstr	s14, [r3, #8]
   4a93c: ed826a00     	vstr	s12, [r2]
   4a940: edc27a01     	vstr	s15, [r2, #4]
   4a944: eaffff66     	b	0x4a6e4
   4a948: eeb00a4a     	vmov.f32	s0, s20
   4a94c: ebff2c87     	bl	0x15b70    @ imm = #-0x34de4 ; sqrtf
   4a950: eaffff69     	b	0x4a6fc
   4a954: e320f000     	nop
   4a958: 18 2d 44 54  	.word	0x54442d18
   4a95c: fb 21 09 40  	.word	0x400921fb
   4a960: cd 3b 7f 66  	.word	0x667f3bcd
   4a964: 9e a0 f6 3f  	.word	0x3ff6a09e
   4a968: 18 2d 44 54  	.word	0x54442d18
   4a96c: fb 21 19 c0  	.word	0xc01921fb
