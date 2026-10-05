; lubadh::Channel::setPotSpeed(int)
; VA 0x37510 size 412

   37510: e2802a2a     	add	r2, r0, #172032
   37514: e6ec1011     	usat	r1, #0xc, r1
   37518: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   3751c: e1a04000     	mov	r4, r0
   37520: e592349c     	ldr	r3, [r2, #0x49c]
   37524: e5821498     	str	r1, [r2, #0x498]
   37528: e0413003     	sub	r3, r1, r3
   3752c: e3530000     	cmp	r3, #0
   37530: b2633000     	rsblt	r3, r3, #0
   37534: e3530009     	cmp	r3, #9
   37538: da00001f     	ble	0x375bc
   3753c: e59070e8     	ldr	r7, [r0, #0xe8]
   37540: e5905104     	ldr	r5, [r0, #0x104]
   37544: e582149c     	str	r1, [r2, #0x49c]
   37548: e59730b0     	ldr	r3, [r7, #0xb0]
   3754c: e2452001     	sub	r2, r5, #1
   37550: e3520001     	cmp	r2, #1
   37554: e0831101     	add	r1, r3, r1, lsl #2
   37558: e5936040     	ldr	r6, [r3, #0x40]
   3755c: ed910a89     	vldr	s0, [r1, #548]
   37560: ed800aae     	vstr	s0, [r0, #696]
   37564: 9a00002e     	bls	0x37624
   37568: e3550000     	cmp	r5, #0
   3756c: 1a00003a     	bne	0x3765c
   37570: e3560001     	cmp	r6, #1
   37574: 0a000048     	beq	0x3769c
   37578: e3560003     	cmp	r6, #3
   3757c: 0a00003f     	beq	0x37680
   37580: ed840a0b     	vstr	s0, [r4, #44]
   37584: eef76a00     	vmov.f32	s13, #1.000000e+00
   37588: ed977a2b     	vldr	s14, [r7, #172]
   3758c: edd47ab6     	vldr	s15, [r4, #728]
   37590: ed9f6a44     	vldr	s12, [pc, #272]         @ 0x376a8 ; float 2.70000004768
   37594: eebd7ac7     	vcvt.s32.f32	s14, s14
   37598: ee300a67     	vsub.f32	s0, s0, s15
   3759c: eeb87ac7     	vcvt.f32.s32	s14, s14
   375a0: eec77a06     	vdiv.f32	s15, s14, s12
   375a4: ee777aa6     	vadd.f32	s15, s15, s13
   375a8: eefd7ae7     	vcvt.s32.f32	s15, s15
   375ac: eeb87ae7     	vcvt.f32.s32	s14, s15
   375b0: edc47ab8     	vstr	s15, [r4, #736]
   375b4: eec07a07     	vdiv.f32	s15, s0, s14
   375b8: edc47ab7     	vstr	s15, [r4, #732]
   375bc: e5d4310e     	ldrb	r3, [r4, #0x10e]
   375c0: e3530000     	cmp	r3, #0
   375c4: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
   375c8: e5d432bc     	ldrb	r3, [r4, #0x2bc]
   375cc: e3530000     	cmp	r3, #0
   375d0: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
   375d4: e59420e8     	ldr	r2, [r4, #0xe8]
   375d8: eeb76a00     	vmov.f32	s12, #1.000000e+00
   375dc: edd46aae     	vldr	s13, [r4, #696]
   375e0: e3a03000     	mov	r3, #0
   375e4: edd47ab6     	vldr	s15, [r4, #728]
   375e8: eddf5a2e     	vldr	s11, [pc, #184]         @ 0x376a8 ; float 2.70000004768
   375ec: edc46a0b     	vstr	s13, [r4, #44]
   375f0: ed927a2b     	vldr	s14, [r2, #172]
   375f4: ee766ae7     	vsub.f32	s13, s13, s15
   375f8: e5c432bc     	strb	r3, [r4, #0x2bc]
   375fc: eebd7ac7     	vcvt.s32.f32	s14, s14
   37600: eeb87ac7     	vcvt.f32.s32	s14, s14
   37604: eec77a25     	vdiv.f32	s15, s14, s11
   37608: ee777a86     	vadd.f32	s15, s15, s12
   3760c: eefd7ae7     	vcvt.s32.f32	s15, s15
   37610: eeb87ae7     	vcvt.f32.s32	s14, s15
   37614: edc47ab8     	vstr	s15, [r4, #736]
   37618: eec67a87     	vdiv.f32	s15, s13, s14
   3761c: edc47ab7     	vstr	s15, [r4, #732]
   37620: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   37624: e3560001     	cmp	r6, #1
   37628: 0a000001     	beq	0x37634
   3762c: ebffff96     	bl	0x3748c
   37630: ed840aae     	vstr	s0, [r4, #696]
   37634: e5943020     	ldr	r3, [r4, #0x20]
   37638: e3a02001     	mov	r2, #1
   3763c: e5842110     	str	r2, [r4, #0x110]
   37640: e2833915     	add	r3, r3, #344064
   37644: e5d33abc     	ldrb	r3, [r3, #0xabc]
   37648: e3530000     	cmp	r3, #0
   3764c: 1594301c     	ldrne	r3, [r4, #0x1c]
   37650: 15832110     	strne	r2, [r3, #0x110]
   37654: e3a03001     	mov	r3, #1
   37658: e5c432bc     	strb	r3, [r4, #0x2bc]
   3765c: e3560003     	cmp	r6, #3
   37660: 1affffd5     	bne	0x375bc
   37664: e59430ec     	ldr	r3, [r4, #0xec]
   37668: e3530001     	cmp	r3, #1
   3766c: 0a000006     	beq	0x3768c
   37670: e3550000     	cmp	r5, #0
   37674: 1affffd0     	bne	0x375bc
   37678: ed940aae     	vldr	s0, [r4, #696]
   3767c: eaffffbf     	b	0x37580
   37680: e59030ec     	ldr	r3, [r0, #0xec]
   37684: e3530001     	cmp	r3, #1
   37688: 1afffffa     	bne	0x37678
   3768c: edd47aae     	vldr	s15, [r4, #696]
   37690: eef17a67     	vneg.f32	s15, s15
   37694: edc47aae     	vstr	s15, [r4, #696]
   37698: eafffff4     	b	0x37670
   3769c: ebffff7a     	bl	0x3748c
   376a0: ed840aae     	vstr	s0, [r4, #696]
   376a4: eafffff3     	b	0x37678
   376a8: cd cc 2c 40  	.word	0x402ccccd
