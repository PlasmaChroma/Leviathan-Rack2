; lubadh::Biquad::initFilter(float, float)
; VA 0x4a970 size 1712

   4a970: eef77a00     	vmov.f32	s15, #1.000000e+00
   4a974: e92d4010     	push	{r4, lr}
   4a978: e1a04000     	mov	r4, r0
   4a97c: eeb40ae7     	vcmpe.f32	s0, s15
   4a980: ed2d8b04     	vpush	{d8, d9}
   4a984: e24dd008     	sub	sp, sp, #8
   4a988: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4a98c: 5a000052     	bpl	0x4aadc
   4a990: eeb50ac0     	vcmpe.f32	s0, #0
   4a994: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4a998: dd9f9bc8     	vldrle	d9, [pc, #800]          @ 0x4acc0 ; float 0
   4a99c: dd9f8acd     	vldrle	s16, [pc, #820]         @ 0x4acd8 ; float 0
   4a9a0: ca000190     	bgt	0x4afe8
   4a9a4: eddf7acc     	vldr	s15, [pc, #816]         @ 0x4acdc ; float 0.800000011921
   4a9a8: eef40ae7     	vcmpe.f32	s1, s15
   4a9ac: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4a9b0: 5ef00a67     	vmovpl.f32	s1, s15
   4a9b4: 5a000003     	bpl	0x4a9c8
   4a9b8: eddf7ac8     	vldr	s15, [pc, #800]         @ 0x4ace0 ; float 0.20000000298
   4a9bc: eef40ae7     	vcmpe.f32	s1, s15
   4a9c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4a9c4: def00a67     	vmovle.f32	s1, s15
   4a9c8: e5943028     	ldr	r3, [r4, #0x28]
   4a9cc: edd48a00     	vldr	s17, [r4]
   4a9d0: e3530008     	cmp	r3, #8
   4a9d4: 979ff103     	ldrls	pc, [pc, r3, lsl #2]
   4a9d8: ea00003c     	b	0x4aad0
   4a9dc: 54 ab 04 00  	.word	0x0004ab54
   4a9e0: a4 ab 04 00  	.word	0x0004aba4
   4a9e4: f8 ab 04 00  	.word	0x0004abf8
   4a9e8: 5c ac 04 00  	.word	0x0004ac5c
   4a9ec: 00 aa 04 00  	.word	0x0004aa00
   4a9f0: e8 ac 04 00  	.word	0x0004ace8
   4a9f4: b4 ad 04 00  	.word	0x0004adb4
   4a9f8: 1c ae 04 00  	.word	0x0004ae1c
   4a9fc: e8 aa 04 00  	.word	0x0004aae8
   4aa00: eeb19ae8     	vsqrt.f32	s18, s17
   4aa04: eef58a40     	vcmp.f32	s17, #0
   4aa08: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4aa0c: 4a000172     	bmi	0x4afdc
   4aa10: eef76ac8     	vcvt.f64.f32	d22, s16
   4aa14: eddf0bab     	vldr	d16, [pc, #684]         @ 0x4acc8 ; float 1.41421356237
   4aa18: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4aa1c: eef73b00     	vmov.f64	d19, #1.000000e+00
   4aa20: ee686a08     	vmul.f32	s13, s16, s16
   4aa24: eeb79ac9     	vcvt.f64.f32	d9, s18
   4aa28: eef48ac6     	vcmpe.f32	s17, s12
   4aa2c: eef02b63     	vmov.f64	d18, d19
   4aa30: eef04b63     	vmov.f64	d20, d19
   4aa34: eee62ba0     	vfma.f64	d18, d22, d16
   4aa38: eee64be0     	vfms.f64	d20, d22, d16
   4aa3c: eef71ae6     	vcvt.f64.f32	d17, s13
   4aa40: ee367ac6     	vsub.f32	s14, s13, s12
   4aa44: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4aa48: ee377a07     	vadd.f32	s14, s14, s14
   4aa4c: ee712ba2     	vadd.f64	d18, d17, d18
   4aa50: ee714ba4     	vadd.f64	d20, d17, d20
   4aa54: ba000111     	blt	0x4aea0
   4aa58: eec31ba2     	vdiv.f64	d17, d19, d18
   4aa5c: ee688a88     	vmul.f32	s17, s17, s16
   4aa60: ee690b20     	vmul.f64	d16, d9, d16
   4aa64: eef02b63     	vmov.f64	d18, d19
   4aa68: e5942010     	ldr	r2, [r4, #0x10]
   4aa6c: e5943004     	ldr	r3, [r4, #0x4]
   4aa70: ee288a88     	vmul.f32	s16, s17, s16
   4aa74: eee02ba6     	vfma.f64	d18, d16, d22
   4aa78: eee03be6     	vfms.f64	d19, d16, d22
   4aa7c: eef70ac8     	vcvt.f64.f32	d16, s16
   4aa80: ee388a46     	vsub.f32	s16, s16, s12
   4aa84: ee787a08     	vadd.f32	s15, s16, s16
   4aa88: ee722ba0     	vadd.f64	d18, d18, d16
   4aa8c: ee700ba3     	vadd.f64	d16, d16, d19
   4aa90: eeb78be1     	vcvt.f32.f64	s16, d17
   4aa94: eef71ac8     	vcvt.f64.f32	d17, s16
   4aa98: ee685a07     	vmul.f32	s11, s16, s14
   4aa9c: ee278a88     	vmul.f32	s16, s15, s16
   4aaa0: ee622ba1     	vmul.f64	d18, d18, d17
   4aaa4: ee600ba1     	vmul.f64	d16, d16, d17
   4aaa8: ee611ba4     	vmul.f64	d17, d17, d20
   4aaac: ed828a01     	vstr	s16, [r2, #4]
   4aab0: eef76be2     	vcvt.f32.f64	s13, d18
   4aab4: eef77be0     	vcvt.f32.f64	s15, d16
   4aab8: eeb77be1     	vcvt.f32.f64	s14, d17
   4aabc: edc26a00     	vstr	s13, [r2]
   4aac0: edc27a02     	vstr	s15, [r2, #8]
   4aac4: edc35a01     	vstr	s11, [r3, #4]
   4aac8: ed837a02     	vstr	s14, [r3, #8]
   4aacc: ed836a00     	vstr	s12, [r3]
   4aad0: e28dd008     	add	sp, sp, #8
   4aad4: ecbd8b04     	vpop	{d8, d9}
   4aad8: e8bd8010     	pop	{r4, pc}
   4aadc: ed9f8a80     	vldr	s16, [pc, #512]         @ 0x4ace4 ; float 8.74227765735e-08
   4aae0: eeb79b00     	vmov.f64	d9, #1.000000e+00
   4aae4: eaffffae     	b	0x4a9a4
   4aae8: eec87a20     	vdiv.f32	s15, s16, s1
   4aaec: eef76a00     	vmov.f32	s13, #1.000000e+00
   4aaf0: eebf7a00     	vmov.f32	s14, #-1.000000e+00
   4aaf4: e5943010     	ldr	r3, [r4, #0x10]
   4aaf8: eea87a08     	vfma.f32	s14, s16, s16
   4aafc: eeb06a66     	vmov.f32	s12, s13
   4ab00: eea86a08     	vfma.f32	s12, s16, s16
   4ab04: e5942004     	ldr	r2, [r4, #0x4]
   4ab08: ee377a07     	vadd.f32	s14, s14, s14
   4ab0c: ee375aa6     	vadd.f32	s10, s15, s13
   4ab10: ee767ae7     	vsub.f32	s15, s13, s15
   4ab14: eea85a08     	vfma.f32	s10, s16, s16
   4ab18: eee87a08     	vfma.f32	s15, s16, s16
   4ab1c: eec65a85     	vdiv.f32	s11, s13, s10
   4ab20: ee266a25     	vmul.f32	s12, s12, s11
   4ab24: ee277a25     	vmul.f32	s14, s14, s11
   4ab28: ee677aa5     	vmul.f32	s15, s15, s11
   4ab2c: ed837a01     	vstr	s14, [r3, #4]
   4ab30: ed836a00     	vstr	s12, [r3]
   4ab34: ed836a02     	vstr	s12, [r3, #8]
   4ab38: edc26a00     	vstr	s13, [r2]
   4ab3c: e5933004     	ldr	r3, [r3, #0x4]
   4ab40: e5823004     	str	r3, [r2, #0x4]
   4ab44: edc27a02     	vstr	s15, [r2, #8]
   4ab48: e28dd008     	add	sp, sp, #8
   4ab4c: ecbd8b04     	vpop	{d8, d9}
   4ab50: e8bd8010     	pop	{r4, pc}
   4ab54: ed9f0b5d     	vldr	d0, [pc, #372]          @ 0x4acd0 ; float -6.28318530718
   4ab58: eeb78a00     	vmov.f32	s16, #1.000000e+00
   4ab5c: e5943004     	ldr	r3, [r4, #0x4]
   4ab60: ee290b00     	vmul.f64	d0, d9, d0
   4ab64: ed838a00     	vstr	s16, [r3]
   4ab68: eeb70bc0     	vcvt.f32.f64	s0, d0
   4ab6c: ebff2dfd     	bl	0x16368    @ imm = #-0x3480c ; expf
   4ab70: e5941004     	ldr	r1, [r4, #0x4]
   4ab74: e5943010     	ldr	r3, [r4, #0x10]
   4ab78: ee388a40     	vsub.f32	s16, s16, s0
   4ab7c: eeb10a40     	vneg.f32	s0, s0
   4ab80: e3a02000     	mov	r2, #0
   4ab84: e5812008     	str	r2, [r1, #0x8]
   4ab88: ed810a01     	vstr	s0, [r1, #4]
   4ab8c: e5832004     	str	r2, [r3, #0x4]
   4ab90: e5832008     	str	r2, [r3, #0x8]
   4ab94: ed838a00     	vstr	s16, [r3]
   4ab98: e28dd008     	add	sp, sp, #8
   4ab9c: ecbd8b04     	vpop	{d8, d9}
   4aba0: e8bd8010     	pop	{r4, pc}
   4aba4: eeb60b00     	vmov.f64	d0, #5.000000e-01
   4aba8: eddf0b48     	vldr	d16, [pc, #288]         @ 0x4acd0 ; float -6.28318530718
   4abac: e5943004     	ldr	r3, [r4, #0x4]
   4abb0: eeb78a00     	vmov.f32	s16, #1.000000e+00
   4abb4: ee300b49     	vsub.f64	d0, d0, d9
   4abb8: ed838a00     	vstr	s16, [r3]
   4abbc: ee200b20     	vmul.f64	d0, d0, d16
   4abc0: eeb70bc0     	vcvt.f32.f64	s0, d0
   4abc4: ebff2de7     	bl	0x16368    @ imm = #-0x34864 ; expf
   4abc8: e5941004     	ldr	r1, [r4, #0x4]
   4abcc: e5943010     	ldr	r3, [r4, #0x10]
   4abd0: ee388a40     	vsub.f32	s16, s16, s0
   4abd4: e3a02000     	mov	r2, #0
   4abd8: e5812008     	str	r2, [r1, #0x8]
   4abdc: ed810a01     	vstr	s0, [r1, #4]
   4abe0: e5832004     	str	r2, [r3, #0x4]
   4abe4: e5832008     	str	r2, [r3, #0x8]
   4abe8: ed838a00     	vstr	s16, [r3]
   4abec: e28dd008     	add	sp, sp, #8
   4abf0: ecbd8b04     	vpop	{d8, d9}
   4abf4: e8bd8010     	pop	{r4, pc}
   4abf8: eec87a20     	vdiv.f32	s15, s16, s1
   4abfc: eef76a00     	vmov.f32	s13, #1.000000e+00
   4ac00: ee288a08     	vmul.f32	s16, s16, s16
   4ac04: e5942010     	ldr	r2, [r4, #0x10]
   4ac08: e5943004     	ldr	r3, [r4, #0x4]
   4ac0c: ee387a66     	vsub.f32	s14, s16, s13
   4ac10: ee377a07     	vadd.f32	s14, s14, s14
   4ac14: ee775aa6     	vadd.f32	s11, s15, s13
   4ac18: ee767ae7     	vsub.f32	s15, s13, s15
   4ac1c: ee755a88     	vadd.f32	s11, s11, s16
   4ac20: ee777a88     	vadd.f32	s15, s15, s16
   4ac24: ee866aa5     	vdiv.f32	s12, s13, s11
   4ac28: ee288a06     	vmul.f32	s16, s16, s12
   4ac2c: ee677a86     	vmul.f32	s15, s15, s12
   4ac30: ee277a06     	vmul.f32	s14, s14, s12
   4ac34: ee386a08     	vadd.f32	s12, s16, s16
   4ac38: ed828a00     	vstr	s16, [r2]
   4ac3c: ed828a02     	vstr	s16, [r2, #8]
   4ac40: ed826a01     	vstr	s12, [r2, #4]
   4ac44: edc37a02     	vstr	s15, [r3, #8]
   4ac48: ed837a01     	vstr	s14, [r3, #4]
   4ac4c: edc36a00     	vstr	s13, [r3]
   4ac50: e28dd008     	add	sp, sp, #8
   4ac54: ecbd8b04     	vpop	{d8, d9}
   4ac58: e8bd8010     	pop	{r4, pc}
   4ac5c: ee887a20     	vdiv.f32	s14, s16, s1
   4ac60: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4ac64: eeff7a00     	vmov.f32	s15, #-1.000000e+00
   4ac68: eef85a00     	vmov.f32	s11, #-2.000000e+00
   4ac6c: eee87a08     	vfma.f32	s15, s16, s16
   4ac70: e5942010     	ldr	r2, [r4, #0x10]
   4ac74: e5943004     	ldr	r3, [r4, #0x4]
   4ac78: ee777aa7     	vadd.f32	s15, s15, s15
   4ac7c: ee375a06     	vadd.f32	s10, s14, s12
   4ac80: ee367a47     	vsub.f32	s14, s12, s14
   4ac84: eea85a08     	vfma.f32	s10, s16, s16
   4ac88: eea87a08     	vfma.f32	s14, s16, s16
   4ac8c: eec66a05     	vdiv.f32	s13, s12, s10
   4ac90: ee277a26     	vmul.f32	s14, s14, s13
   4ac94: ee665aa5     	vmul.f32	s11, s13, s11
   4ac98: ee677aa6     	vmul.f32	s15, s15, s13
   4ac9c: edc26a00     	vstr	s13, [r2]
   4aca0: edc26a02     	vstr	s13, [r2, #8]
   4aca4: edc25a01     	vstr	s11, [r2, #4]
   4aca8: ed837a02     	vstr	s14, [r3, #8]
   4acac: ed836a00     	vstr	s12, [r3]
   4acb0: edc37a01     	vstr	s15, [r3, #4]
   4acb4: e28dd008     	add	sp, sp, #8
   4acb8: ecbd8b04     	vpop	{d8, d9}
   4acbc: e8bd8010     	pop	{r4, pc}
   4acc0: 00 00 00 00  	.word	0x00000000
   4acc4: 00 00 00 00  	.word	0x00000000
   4acc8: cd 3b 7f 66  	.word	0x667f3bcd
   4accc: 9e a0 f6 3f  	.word	0x3ff6a09e
   4acd0: 18 2d 44 54  	.word	0x54442d18
   4acd4: fb 21 19 c0  	.word	0xc01921fb
   4acd8: 00 00 00 00  	.word	0x00000000
   4acdc: cd cc 4c 3f  	.word	0x3f4ccccd
   4ace0: cd cc 4c 3e  	.word	0x3e4ccccd
   4ace4: 2e bd bb 33  	.word	0x33bbbd2e
   4ace8: eeb19ae8     	vsqrt.f32	s18, s17
   4acec: eef58a40     	vcmp.f32	s17, #0
   4acf0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4acf4: 4a0000b5     	bmi	0x4afd0
   4acf8: eef72ac8     	vcvt.f64.f32	d18, s16
   4acfc: eddf1bc3     	vldr	d17, [pc, #780]         @ 0x4b010 ; float 1.41421356237
   4ad00: eef76a00     	vmov.f32	s13, #1.000000e+00
   4ad04: eef78b00     	vmov.f64	d24, #1.000000e+00
   4ad08: ee288a08     	vmul.f32	s16, s16, s16
   4ad0c: eeb79ac9     	vcvt.f64.f32	d9, s18
   4ad10: eef48ae6     	vcmpe.f32	s17, s13
   4ad14: eef04b68     	vmov.f64	d20, d24
   4ad18: eef05b68     	vmov.f64	d21, d24
   4ad1c: eee24ba1     	vfma.f64	d20, d18, d17
   4ad20: eee25be1     	vfms.f64	d21, d18, d17
   4ad24: e5942010     	ldr	r2, [r4, #0x10]
   4ad28: eef77ac8     	vcvt.f64.f32	d23, s16
   4ad2c: ee387a66     	vsub.f32	s14, s16, s13
   4ad30: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ad34: e5943004     	ldr	r3, [r4, #0x4]
   4ad38: ee377a07     	vadd.f32	s14, s14, s14
   4ad3c: ee774ba4     	vadd.f64	d20, d23, d20
   4ad40: ee775ba5     	vadd.f64	d21, d23, d21
   4ad44: ba000085     	blt	0x4af60
   4ad48: eec83ba4     	vdiv.f64	d19, d24, d20
   4ad4c: eef70ae8     	vcvt.f64.f32	d16, s17
   4ad50: ee691b21     	vmul.f64	d17, d9, d17
   4ad54: ee388a68     	vsub.f32	s16, s16, s17
   4ad58: eef04b60     	vmov.f64	d20, d16
   4ad5c: eee14ba2     	vfma.f64	d20, d17, d18
   4ad60: eee10be2     	vfms.f64	d16, d17, d18
   4ad64: ee388a08     	vadd.f32	s16, s16, s16
   4ad68: ee741ba7     	vadd.f64	d17, d20, d23
   4ad6c: ee700ba7     	vadd.f64	d16, d16, d23
   4ad70: eef77be3     	vcvt.f32.f64	s15, d19
   4ad74: eef72ae7     	vcvt.f64.f32	d18, s15
   4ad78: ee288a27     	vmul.f32	s16, s16, s15
   4ad7c: ee277a87     	vmul.f32	s14, s15, s14
   4ad80: ee625ba5     	vmul.f64	d21, d18, d21
   4ad84: ee611ba2     	vmul.f64	d17, d17, d18
   4ad88: ee600ba2     	vmul.f64	d16, d16, d18
   4ad8c: ed828a01     	vstr	s16, [r2, #4]
   4ad90: eef75be5     	vcvt.f32.f64	s11, d21
   4ad94: eeb76be1     	vcvt.f32.f64	s12, d17
   4ad98: eef77be0     	vcvt.f32.f64	s15, d16
   4ad9c: ed826a00     	vstr	s12, [r2]
   4ada0: edc27a02     	vstr	s15, [r2, #8]
   4ada4: ed837a01     	vstr	s14, [r3, #4]
   4ada8: edc35a02     	vstr	s11, [r3, #8]
   4adac: edc36a00     	vstr	s13, [r3]
   4adb0: eaffff46     	b	0x4aad0
   4adb4: ee887a20     	vdiv.f32	s14, s16, s1
   4adb8: eeb76a00     	vmov.f32	s12, #1.000000e+00
   4adbc: eeff7a00     	vmov.f32	s15, #-1.000000e+00
   4adc0: e5942010     	ldr	r2, [r4, #0x10]
   4adc4: eee87a08     	vfma.f32	s15, s16, s16
   4adc8: e5943004     	ldr	r3, [r4, #0x4]
   4adcc: e3a01000     	mov	r1, #0
   4add0: e5821004     	str	r1, [r2, #0x4]
   4add4: ee777aa7     	vadd.f32	s15, s15, s15
   4add8: ee375a06     	vadd.f32	s10, s14, s12
   4addc: ee766a47     	vsub.f32	s13, s12, s14
   4ade0: eea85a08     	vfma.f32	s10, s16, s16
   4ade4: eee86a08     	vfma.f32	s13, s16, s16
   4ade8: eec65a05     	vdiv.f32	s11, s12, s10
   4adec: ee277a25     	vmul.f32	s14, s14, s11
   4adf0: ee666aa5     	vmul.f32	s13, s13, s11
   4adf4: ee677aa5     	vmul.f32	s15, s15, s11
   4adf8: eef15a47     	vneg.f32	s11, s14
   4adfc: ed827a00     	vstr	s14, [r2]
   4ae00: edc25a02     	vstr	s11, [r2, #8]
   4ae04: edc36a02     	vstr	s13, [r3, #8]
   4ae08: ed836a00     	vstr	s12, [r3]
   4ae0c: edc37a01     	vstr	s15, [r3, #4]
   4ae10: e28dd008     	add	sp, sp, #8
   4ae14: ecbd8b04     	vpop	{d8, d9}
   4ae18: e8bd8010     	pop	{r4, pc}
   4ae1c: eec85a20     	vdiv.f32	s11, s16, s1
   4ae20: eef77a00     	vmov.f32	s15, #1.000000e+00
   4ae24: ee288a08     	vmul.f32	s16, s16, s16
   4ae28: e5943010     	ldr	r3, [r4, #0x10]
   4ae2c: eef48ae7     	vcmpe.f32	s17, s15
   4ae30: e5942004     	ldr	r2, [r4, #0x4]
   4ae34: ee387a67     	vsub.f32	s14, s16, s15
   4ae38: eef1fa10     	vmrs	APSR_nzcv, fpscr
   4ae3c: ee377a07     	vadd.f32	s14, s14, s14
   4ae40: ee356aa7     	vadd.f32	s12, s11, s15
   4ae44: ee776ae5     	vsub.f32	s13, s15, s11
   4ae48: ee366a08     	vadd.f32	s12, s12, s16
   4ae4c: ee766a88     	vadd.f32	s13, s13, s16
   4ae50: ba000030     	blt	0x4af18
   4ae54: ee875a86     	vdiv.f32	s10, s15, s12
   4ae58: eef04a67     	vmov.f32	s9, s15
   4ae5c: eeb06a67     	vmov.f32	s12, s15
   4ae60: eee84aa5     	vfma.f32	s9, s17, s11
   4ae64: eea86ae5     	vfms.f32	s12, s17, s11
   4ae68: ee745a88     	vadd.f32	s11, s9, s16
   4ae6c: ee368a08     	vadd.f32	s16, s12, s16
   4ae70: ee257a07     	vmul.f32	s14, s10, s14
   4ae74: ee256a85     	vmul.f32	s12, s11, s10
   4ae78: ee288a05     	vmul.f32	s16, s16, s10
   4ae7c: ee656a26     	vmul.f32	s13, s10, s13
   4ae80: ed837a01     	vstr	s14, [r3, #4]
   4ae84: ed836a00     	vstr	s12, [r3]
   4ae88: ed838a02     	vstr	s16, [r3, #8]
   4ae8c: edc27a00     	vstr	s15, [r2]
   4ae90: e5933004     	ldr	r3, [r3, #0x4]
   4ae94: e5823004     	str	r3, [r2, #0x4]
   4ae98: edc26a02     	vstr	s13, [r2, #8]
   4ae9c: eaffff0b     	b	0x4aad0
   4aea0: eec07b89     	vdiv.f64	d23, d16, d9
   4aea4: eef01b63     	vmov.f64	d17, d19
   4aea8: eec67aa8     	vdiv.f32	s15, s13, s17
   4aeac: eef05b63     	vmov.f64	d21, d19
   4aeb0: e5943010     	ldr	r3, [r4, #0x10]
   4aeb4: e5942004     	ldr	r2, [r4, #0x4]
   4aeb8: eef70ae7     	vcvt.f64.f32	d16, s15
   4aebc: ee376ac6     	vsub.f32	s12, s15, s12
   4aec0: ee767a06     	vadd.f32	s15, s12, s12
   4aec4: eee71ba6     	vfma.f64	d17, d23, d22
   4aec8: eee75be6     	vfms.f64	d21, d23, d22
   4aecc: ee711ba0     	vadd.f64	d17, d17, d16
   4aed0: ee700ba5     	vadd.f64	d16, d16, d21
   4aed4: ee836ba1     	vdiv.f64	d6, d19, d17
   4aed8: eeb76bc6     	vcvt.f32.f64	s12, d6
   4aedc: eef73ac6     	vcvt.f64.f32	d19, s12
   4aee0: ee267a07     	vmul.f32	s14, s12, s14
   4aee4: ee276a86     	vmul.f32	s12, s15, s12
   4aee8: ee632ba2     	vmul.f64	d18, d19, d18
   4aeec: ee634ba4     	vmul.f64	d20, d19, d20
   4aef0: ee600ba3     	vmul.f64	d16, d16, d19
   4aef4: ed837a01     	vstr	s14, [r3, #4]
   4aef8: eef76be2     	vcvt.f32.f64	s13, d18
   4aefc: eeb77be4     	vcvt.f32.f64	s14, d20
   4af00: eef77be0     	vcvt.f32.f64	s15, d16
   4af04: edc36a00     	vstr	s13, [r3]
   4af08: ed837a02     	vstr	s14, [r3, #8]
   4af0c: ed826a00     	vstr	s12, [r2]
   4af10: edc27a01     	vstr	s15, [r2, #4]
   4af14: eafffeed     	b	0x4aad0
   4af18: ee855aa8     	vdiv.f32	s10, s11, s17
   4af1c: ee754a27     	vadd.f32	s9, s10, s15
   4af20: ee375ac5     	vsub.f32	s10, s15, s10
   4af24: ee744a88     	vadd.f32	s9, s9, s16
   4af28: ee358a08     	vadd.f32	s16, s10, s16
   4af2c: eec75aa4     	vdiv.f32	s11, s15, s9
   4af30: ee256a86     	vmul.f32	s12, s11, s12
   4af34: ee257a87     	vmul.f32	s14, s11, s14
   4af38: ee656aa6     	vmul.f32	s13, s11, s13
   4af3c: ee288a25     	vmul.f32	s16, s16, s11
   4af40: ed836a00     	vstr	s12, [r3]
   4af44: ed837a01     	vstr	s14, [r3, #4]
   4af48: edc36a02     	vstr	s13, [r3, #8]
   4af4c: edc27a00     	vstr	s15, [r2]
   4af50: e5933004     	ldr	r3, [r3, #0x4]
   4af54: e5823004     	str	r3, [r2, #0x4]
   4af58: ed828a02     	vstr	s16, [r2, #8]
   4af5c: eafffedb     	b	0x4aad0
   4af60: eec13b89     	vdiv.f64	d19, d17, d9
   4af64: eec67aa8     	vdiv.f32	s15, s13, s17
   4af68: eef71ae7     	vcvt.f64.f32	d17, s15
   4af6c: ee388a67     	vsub.f32	s16, s16, s15
   4af70: eef00b61     	vmov.f64	d16, d17
   4af74: ee788a08     	vadd.f32	s17, s16, s16
   4af78: eee30ba2     	vfma.f64	d16, d19, d18
   4af7c: eee31be2     	vfms.f64	d17, d19, d18
   4af80: ee702ba7     	vadd.f64	d18, d16, d23
   4af84: ee710ba7     	vadd.f64	d16, d17, d23
   4af88: eec81ba2     	vdiv.f64	d17, d24, d18
   4af8c: eeb78be1     	vcvt.f32.f64	s16, d17
   4af90: eef71ac8     	vcvt.f64.f32	d17, s16
   4af94: ee287a07     	vmul.f32	s14, s16, s14
   4af98: ee288a88     	vmul.f32	s16, s17, s16
   4af9c: ee614ba4     	vmul.f64	d20, d17, d20
   4afa0: ee615ba5     	vmul.f64	d21, d17, d21
   4afa4: ee600ba1     	vmul.f64	d16, d16, d17
   4afa8: ed827a01     	vstr	s14, [r2, #4]
   4afac: eeb76be4     	vcvt.f32.f64	s12, d20
   4afb0: eeb77be5     	vcvt.f32.f64	s14, d21
   4afb4: eef77be0     	vcvt.f32.f64	s15, d16
   4afb8: ed826a00     	vstr	s12, [r2]
   4afbc: ed827a02     	vstr	s14, [r2, #8]
   4afc0: ed838a01     	vstr	s16, [r3, #4]
   4afc4: edc37a02     	vstr	s15, [r3, #8]
   4afc8: edc36a00     	vstr	s13, [r3]
   4afcc: eafffebf     	b	0x4aad0
   4afd0: eeb00a68     	vmov.f32	s0, s17
   4afd4: ebff2ae5     	bl	0x15b70    @ imm = #-0x3546c ; sqrtf
   4afd8: eaffff46     	b	0x4acf8
   4afdc: eeb00a68     	vmov.f32	s0, s17
   4afe0: ebff2ae2     	bl	0x15b70    @ imm = #-0x35478 ; sqrtf
   4afe4: eafffe89     	b	0x4aa10
   4afe8: eeb79ac0     	vcvt.f64.f32	d9, s0
   4afec: eddf0b09     	vldr	d16, [pc, #36]          @ 0x4b018 ; float 3.14159265359
   4aff0: edcd0a01     	vstr	s1, [sp, #4]
   4aff4: ee690b20     	vmul.f64	d16, d9, d16
   4aff8: eeb70be0     	vcvt.f32.f64	s0, d16
   4affc: ebff2a75     	bl	0x159d8    @ imm = #-0x3562c ; tanf
   4b000: eddd0a01     	vldr	s1, [sp, #4]
   4b004: eeb08a40     	vmov.f32	s16, s0
   4b008: eafffe65     	b	0x4a9a4
   4b00c: e320f000     	nop
   4b010: cd 3b 7f 66  	.word	0x667f3bcd
   4b014: 9e a0 f6 3f  	.word	0x3ff6a09e
   4b018: 18 2d 44 54  	.word	0x54442d18
   4b01c: fb 21 09 40  	.word	0x400921fb
