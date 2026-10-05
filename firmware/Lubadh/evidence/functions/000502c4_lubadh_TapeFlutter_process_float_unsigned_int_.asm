; lubadh::TapeFlutter::process(float, unsigned int)
; VA 0x502c4 size 584

   502c4: ed9f5a89     	vldr	s10, [pc, #548]         @ 0x504f0 ; float 49170.2539062
   502c8: ee051a90     	vmov	s11, r1
   502cc: edd04a00     	vldr	s9, [r0]
   502d0: eef77a00     	vmov.f32	s15, #1.000000e+00
   502d4: eef85a65     	vcvt.f32.u32	s11, s11
   502d8: ed9f6a85     	vldr	s12, [pc, #532]         @ 0x504f4 ; float 255
   502dc: ed907a02     	vldr	s14, [r0, #8]
   502e0: eec46a85     	vdiv.f32	s13, s9, s10
   502e4: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   502e8: e5902004     	ldr	r2, [r0, #0x4]
   502ec: ed2d8b04     	vpush	{d8, d9}
   502f0: e1a05000     	mov	r5, r0
   502f4: ee666aa5     	vmul.f32	s13, s13, s11
   502f8: eea67a86     	vfma.f32	s14, s13, s12
   502fc: eeb47ae7     	vcmpe.f32	s14, s15
   50300: ed807a02     	vstr	s14, [r0, #8]
   50304: eef1fa10     	vmrs	APSR_nzcv, fpscr
   50308: ba000005     	blt	0x50324
   5030c: ee377a67     	vsub.f32	s14, s14, s15
   50310: e2822001     	add	r2, r2, #1
   50314: eeb47ae7     	vcmpe.f32	s14, s15
   50318: eef1fa10     	vmrs	APSR_nzcv, fpscr
   5031c: aafffffa     	bge	0x5030c
   50320: ed857a02     	vstr	s14, [r5, #8]
   50324: ed955a03     	vldr	s10, [r5, #12]
   50328: e3081081     	movw	r1, #0x8081
   5032c: e3481080     	movt	r1, #0x8080
   50330: ed9f6a6e     	vldr	s12, [pc, #440]         @ 0x504f0 ; float 49170.2539062
   50334: ed9f4a6e     	vldr	s8, [pc, #440]          @ 0x504f4 ; float 255
   50338: eef76a00     	vmov.f32	s13, #1.000000e+00
   5033c: edd57a05     	vldr	s15, [r5, #20]
   50340: eec54a06     	vdiv.f32	s9, s10, s12
   50344: e0813291     	umull	r3, r1, r1, r2
   50348: e5953010     	ldr	r3, [r5, #0x10]
   5034c: e1a013a1     	lsr	r1, r1, #7
   50350: e0611401     	rsb	r1, r1, r1, lsl #8
   50354: e0421001     	sub	r1, r2, r1
   50358: e5851004     	str	r1, [r5, #0x4]
   5035c: e0852101     	add	r2, r5, r1, lsl #2
   50360: ed926a1f     	vldr	s12, [r2, #124]
   50364: ed925a20     	vldr	s10, [r2, #128]
   50368: ee645aa5     	vmul.f32	s11, s9, s11
   5036c: ee355a46     	vsub.f32	s10, s10, s12
   50370: eee57a84     	vfma.f32	s15, s11, s8
   50374: eea56a07     	vfma.f32	s12, s10, s14
   50378: eef47ae6     	vcmpe.f32	s15, s13
   5037c: edc57a05     	vstr	s15, [r5, #20]
   50380: eef1fa10     	vmrs	APSR_nzcv, fpscr
   50384: ba000008     	blt	0x503ac
   50388: eeb07a66     	vmov.f32	s14, s13
   5038c: e2832001     	add	r2, r3, #1
   50390: ee777ac7     	vsub.f32	s15, s15, s14
   50394: e1a03002     	mov	r3, r2
   50398: e2822001     	add	r2, r2, #1
   5039c: eef47ac7     	vcmpe.f32	s15, s14
   503a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   503a4: aafffff9     	bge	0x50390
   503a8: edc57a05     	vstr	s15, [r5, #20]
   503ac: e3082081     	movw	r2, #0x8081
   503b0: e3482080     	movt	r2, #0x8080
   503b4: edd55a06     	vldr	s11, [r5, #24]
   503b8: eeb68a00     	vmov.f32	s16, #5.000000e-01
   503bc: e2856a01     	add	r6, r5, #4096
   503c0: e0821392     	umull	r1, r2, r2, r3
   503c4: ee257a88     	vmul.f32	s14, s11, s16
   503c8: e596480c     	ldr	r4, [r6, #0x80c]
   503cc: e1a023a2     	lsr	r2, r2, #7
   503d0: e5967810     	ldr	r7, [r6, #0x810]
   503d4: e0622402     	rsb	r2, r2, r2, lsl #8
   503d8: e1540007     	cmp	r4, r7
   503dc: e0432002     	sub	r2, r3, r2
   503e0: e5852010     	str	r2, [r5, #0x10]
   503e4: e0853102     	add	r3, r5, r2, lsl #2
   503e8: ed938a1f     	vldr	s16, [r3, #124]
   503ec: edd36a20     	vldr	s13, [r3, #128]
   503f0: ee766ac8     	vsub.f32	s13, s13, s16
   503f4: eea68aa7     	vfma.f32	s16, s13, s15
   503f8: ee278a08     	vmul.f32	s16, s14, s16
   503fc: eea58a86     	vfma.f32	s16, s11, s12
   50400: 0a000025     	beq	0x5049c
   50404: e2858d39     	add	r8, r5, #3648
   50408: ed9f9a3a     	vldr	s18, [pc, #232]         @ 0x504f8 ; float 0
   5040c: eddf8a3a     	vldr	s17, [pc, #232]         @ 0x504fc ; float 2.32830643654e-10
   50410: eddf9a3a     	vldr	s19, [pc, #232]         @ 0x50500 ; float 0.999999940395
   50414: ea000007     	b	0x50438
   50418: ed927a00     	vldr	s14, [r2]
   5041c: edd36a00     	vldr	s13, [r3]
   50420: ee766ac7     	vsub.f32	s13, s13, s14
   50424: eea77aa6     	vfma.f32	s14, s15, s13
   50428: ee377a46     	vsub.f32	s14, s14, s12
   5042c: eca47a01     	vstmia	r4!, {s14}
   50430: e1570004     	cmp	r7, r4
   50434: 0a000018     	beq	0x5049c
   50438: e1a00008     	mov	r0, r8
   5043c: eb000043     	bl	0x50550
   50440: ee070a90     	vmov	s15, r0
   50444: eef76a00     	vmov.f32	s13, #1.000000e+00
   50448: e2862b02     	add	r2, r6, #2048
   5044c: eef87a67     	vcvt.f32.u32	s15, s15
   50450: e1a03002     	mov	r3, r2
   50454: e2833008     	add	r3, r3, #8
   50458: e2822004     	add	r2, r2, #4
   5045c: eeb66a00     	vmov.f32	s12, #5.000000e-01
   50460: ee777a89     	vadd.f32	s15, s15, s18
   50464: ee677aa8     	vmul.f32	s15, s15, s17
   50468: eef47ae6     	vcmpe.f32	s15, s13
   5046c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   50470: baffffe8     	blt	0x50418
   50474: e2862b02     	add	r2, r6, #2048
   50478: eef66a00     	vmov.f32	s13, #5.000000e-01
   5047c: edd27a01     	vldr	s15, [r2, #4]
   50480: ed927a02     	vldr	s14, [r2, #8]
   50484: ee377a67     	vsub.f32	s14, s14, s15
   50488: eee77a29     	vfma.f32	s15, s14, s19
   5048c: ee777ae6     	vsub.f32	s15, s15, s13
   50490: ece47a01     	vstmia	r4!, {s15}
   50494: e1570004     	cmp	r7, r4
   50498: 1affffe6     	bne	0x50438
   5049c: e2854b06     	add	r4, r5, #6144
   504a0: e2850024     	add	r0, r5, #36
   504a4: e284400c     	add	r4, r4, #12
   504a8: e1a01004     	mov	r1, r4
   504ac: ebffeadb     	bl	0x4b020
   504b0: e1a01004     	mov	r1, r4
   504b4: e2850050     	add	r0, r5, #80
   504b8: ebffead8     	bl	0x4b020
   504bc: e596380c     	ldr	r3, [r6, #0x80c]
   504c0: edd55a07     	vldr	s11, [r5, #28]
   504c4: eeb07a04     	vmov.f32	s14, #2.500000e+00
   504c8: eef16a04     	vmov.f32	s13, #5.000000e+00
   504cc: eddf7a0c     	vldr	s15, [pc, #48]          @ 0x50504 ; float 0.0833339691162
   504d0: ed9f0a0c     	vldr	s0, [pc, #48]           @ 0x50508 ; float 0.958333015442
   504d4: ed936a00     	vldr	s12, [r3]
   504d8: eea58a86     	vfma.f32	s16, s11, s12
   504dc: ee388a07     	vadd.f32	s16, s16, s14
   504e0: ee887a26     	vdiv.f32	s14, s16, s13
   504e4: ecbd8b04     	vpop	{d8, d9}
   504e8: eea70a27     	vfma.f32	s0, s14, s15
   504ec: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   504f0: 41 12 40 47  	.word	0x47401241
   504f4: 00 00 7f 43  	.word	0x437f0000
   504f8: 00 00 00 00  	.word	0x00000000
   504fc: 00 00 80 2f  	.word	0x2f800000
   50500: ff ff 7f 3f  	.word	0x3f7fffff
   50504: 00 ab aa 3d  	.word	0x3daaab00
   50508: 50 55 75 3f  	.word	0x3f755550
