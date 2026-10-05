; lubadh::TapeFilter::process(std::vector<float, std::allocator<float> >&)
; VA 0x4f524 size 956

   4f524: e92d4ff8     	push	{r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
   4f528: e1a05001     	mov	r5, r1
   4f52c: e1a04000     	mov	r4, r0
   4f530: e8910044     	ldm	r1, {r2, r6}
   4f534: e0466002     	sub	r6, r6, r2
   4f538: e1b07146     	asrs	r7, r6, #2
   4f53c: 0a000059     	beq	0x4f6a8
   4f540: e590e040     	ldr	lr, [r0, #0x40]
   4f544: e2473001     	sub	r3, r7, #1
   4f548: e5900058     	ldr	r0, [r0, #0x58]
   4f54c: e594c04c     	ldr	r12, [r4, #0x4c]
   4f550: e28e800f     	add	r8, lr, #15
   4f554: e0481000     	sub	r1, r8, r0
   4f558: e048900c     	sub	r9, r8, r12
   4f55c: e0488002     	sub	r8, r8, r2
   4f560: e351001e     	cmp	r1, #30
   4f564: 8359001e     	cmphi	r9, #30
   4f568: e28c900f     	add	r9, r12, #15
   4f56c: 83a01001     	movhi	r1, #1
   4f570: 93a01000     	movls	r1, #0
   4f574: e358001e     	cmp	r8, #30
   4f578: 93a01000     	movls	r1, #0
   4f57c: 82011001     	andhi	r1, r1, #1
   4f580: e0498000     	sub	r8, r9, r0
   4f584: e358001e     	cmp	r8, #30
   4f588: 93a01000     	movls	r1, #0
   4f58c: 82011001     	andhi	r1, r1, #1
   4f590: e0498002     	sub	r8, r9, r2
   4f594: e280900f     	add	r9, r0, #15
   4f598: e358001e     	cmp	r8, #30
   4f59c: 93a01000     	movls	r1, #0
   4f5a0: 82011001     	andhi	r1, r1, #1
   4f5a4: e0498002     	sub	r8, r9, r2
   4f5a8: e358001e     	cmp	r8, #30
   4f5ac: 93a01000     	movls	r1, #0
   4f5b0: 82011001     	andhi	r1, r1, #1
   4f5b4: e3530004     	cmp	r3, #4
   4f5b8: 93a03000     	movls	r3, #0
   4f5bc: 82013001     	andhi	r3, r1, #1
   4f5c0: e3530000     	cmp	r3, #0
   4f5c4: 0a0000b6     	beq	0x4f8a4
   4f5c8: e1a09127     	lsr	r9, r7, #2
   4f5cc: e1a03002     	mov	r3, r2
   4f5d0: e1a0800e     	mov	r8, lr
   4f5d4: e1a0600c     	mov	r6, r12
   4f5d8: e0829209     	add	r9, r2, r9, lsl #4
   4f5dc: e1a01000     	mov	r1, r0
   4f5e0: f4630a8f     	vld1.32	{d16, d17}, [r3]
   4f5e4: f4480a8d     	vst1.32	{d16, d17}, [r8]!
   4f5e8: f4630a8f     	vld1.32	{d16, d17}, [r3]
   4f5ec: f4460a8d     	vst1.32	{d16, d17}, [r6]!
   4f5f0: f4630a8d     	vld1.32	{d16, d17}, [r3]!
   4f5f4: e1530009     	cmp	r3, r9
   4f5f8: f4410a8d     	vst1.32	{d16, d17}, [r1]!
   4f5fc: 1afffff7     	bne	0x4f5e0
   4f600: e3170003     	tst	r7, #3
   4f604: e3c73003     	bic	r3, r7, #3
   4f608: 0a000026     	beq	0x4f6a8
   4f60c: e1a01103     	lsl	r1, r3, #2
   4f610: e2836001     	add	r6, r3, #1
   4f614: e0828001     	add	r8, r2, r1
   4f618: e08ea001     	add	r10, lr, r1
   4f61c: e08c9001     	add	r9, r12, r1
   4f620: e0801001     	add	r1, r0, r1
   4f624: e1570006     	cmp	r7, r6
   4f628: e598b000     	ldr	r11, [r8]
   4f62c: e58ab000     	str	r11, [r10]
   4f630: e598a000     	ldr	r10, [r8]
   4f634: e589a000     	str	r10, [r9]
   4f638: e5988000     	ldr	r8, [r8]
   4f63c: e5818000     	str	r8, [r1]
   4f640: 9a000018     	bls	0x4f6a8
   4f644: e1a06106     	lsl	r6, r6, #2
   4f648: e2833002     	add	r3, r3, #2
   4f64c: e0821006     	add	r1, r2, r6
   4f650: e08e9006     	add	r9, lr, r6
   4f654: e1570003     	cmp	r7, r3
   4f658: e08c8006     	add	r8, r12, r6
   4f65c: e0806006     	add	r6, r0, r6
   4f660: e5917000     	ldr	r7, [r1]
   4f664: e5897000     	str	r7, [r9]
   4f668: e5917000     	ldr	r7, [r1]
   4f66c: e5887000     	str	r7, [r8]
   4f670: e5911000     	ldr	r1, [r1]
   4f674: e5861000     	str	r1, [r6]
   4f678: 9a00000a     	bls	0x4f6a8
   4f67c: e1a01103     	lsl	r1, r3, #2
   4f680: e0823001     	add	r3, r2, r1
   4f684: e08ee001     	add	lr, lr, r1
   4f688: e08cc001     	add	r12, r12, r1
   4f68c: e0800001     	add	r0, r0, r1
   4f690: e5932000     	ldr	r2, [r3]
   4f694: e58e2000     	str	r2, [lr]
   4f698: e5932000     	ldr	r2, [r3]
   4f69c: e58c2000     	str	r2, [r12]
   4f6a0: e5933000     	ldr	r3, [r3]
   4f6a4: e5803000     	str	r3, [r0]
   4f6a8: e1a01005     	mov	r1, r5
   4f6ac: e2840004     	add	r0, r4, #4
   4f6b0: e2846040     	add	r6, r4, #64
   4f6b4: ebffee92     	bl	0x4b104
   4f6b8: e1a01006     	mov	r1, r6
   4f6bc: e2840010     	add	r0, r4, #16
   4f6c0: ebffee8f     	bl	0x4b104
   4f6c4: e5957000     	ldr	r7, [r5]
   4f6c8: e5952004     	ldr	r2, [r5, #0x4]
   4f6cc: e042c007     	sub	r12, r2, r7
   4f6d0: e1b0914c     	asrs	r9, r12, #2
   4f6d4: 0a00003e     	beq	0x4f7d4
   4f6d8: e5948040     	ldr	r8, [r4, #0x40]
   4f6dc: e1a01007     	mov	r1, r7
   4f6e0: e087e00c     	add	lr, r7, r12
   4f6e4: e1a03007     	mov	r3, r7
   4f6e8: e1a00008     	mov	r0, r8
   4f6ec: e1a02008     	mov	r2, r8
   4f6f0: eddf2b76     	vldr	d18, [pc, #472]         @ 0x4f8d0>&)+0x3ac> ; float 0.6
   4f6f4: ecf27a01     	vldmia	r2!, {s15}
   4f6f8: ed937a00     	vldr	s14, [r3]
   4f6fc: eef70ae7     	vcvt.f64.f32	d16, s15
   4f700: eeb77ac7     	vcvt.f64.f32	d7, s14
   4f704: ee600ba2     	vmul.f64	d16, d16, d18
   4f708: eee70b22     	vfma.f64	d16, d7, d18
   4f70c: eef77be0     	vcvt.f32.f64	s15, d16
   4f710: ece37a01     	vstmia	r3!, {s15}
   4f714: e153000e     	cmp	r3, lr
   4f718: 1afffff5     	bne	0x4f6f4
   4f71c: e288300f     	add	r3, r8, #15
   4f720: e2492001     	sub	r2, r9, #1
   4f724: e0433007     	sub	r3, r3, r7
   4f728: e353001e     	cmp	r3, #30
   4f72c: 83520007     	cmphi	r2, #7
   4f730: 9a000052     	bls	0x4f880
   4f734: e1a03129     	lsr	r3, r9, #2
   4f738: e0883203     	add	r3, r8, r3, lsl #4
   4f73c: f4610a8d     	vld1.32	{d16, d17}, [r1]!
   4f740: f4400a8d     	vst1.32	{d16, d17}, [r0]!
   4f744: e1500003     	cmp	r0, r3
   4f748: 1afffffb     	bne	0x4f73c
   4f74c: e3190003     	tst	r9, #3
   4f750: e3c93003     	bic	r3, r9, #3
   4f754: 0a000014     	beq	0x4f7ac
   4f758: e1a01103     	lsl	r1, r3, #2
   4f75c: e2832001     	add	r2, r3, #1
   4f760: e0870001     	add	r0, r7, r1
   4f764: e0881001     	add	r1, r8, r1
   4f768: e1520009     	cmp	r2, r9
   4f76c: e5900000     	ldr	r0, [r0]
   4f770: e5810000     	str	r0, [r1]
   4f774: 2a00000c     	bhs	0x4f7ac
   4f778: e1a02102     	lsl	r2, r2, #2
   4f77c: e2833002     	add	r3, r3, #2
   4f780: e0871002     	add	r1, r7, r2
   4f784: e0882002     	add	r2, r8, r2
   4f788: e1530009     	cmp	r3, r9
   4f78c: e5911000     	ldr	r1, [r1]
   4f790: e5821000     	str	r1, [r2]
   4f794: 2a000004     	bhs	0x4f7ac
   4f798: e1a03103     	lsl	r3, r3, #2
   4f79c: e0877003     	add	r7, r7, r3
   4f7a0: e0888003     	add	r8, r8, r3
   4f7a4: e5973000     	ldr	r3, [r7]
   4f7a8: e5883000     	str	r3, [r8]
   4f7ac: e594304c     	ldr	r3, [r4, #0x4c]
   4f7b0: eddf1b48     	vldr	d17, [pc, #288]         @ 0x4f8d8>&)+0x3b4> ; float 0.2
   4f7b4: e083200c     	add	r2, r3, r12
   4f7b8: edd37a00     	vldr	s15, [r3]
   4f7bc: eef70ae7     	vcvt.f64.f32	d16, s15
   4f7c0: ee600ba1     	vmul.f64	d16, d16, d17
   4f7c4: eef77be0     	vcvt.f32.f64	s15, d16
   4f7c8: ece37a01     	vstmia	r3!, {s15}
   4f7cc: e1530002     	cmp	r3, r2
   4f7d0: 1afffff8     	bne	0x4f7b8
   4f7d4: e1a01005     	mov	r1, r5
   4f7d8: e284001c     	add	r0, r4, #28
   4f7dc: ebffee33     	bl	0x4b0b0
   4f7e0: e1a01006     	mov	r1, r6
   4f7e4: e2840028     	add	r0, r4, #40
   4f7e8: ebffee30     	bl	0x4b0b0
   4f7ec: e284104c     	add	r1, r4, #76
   4f7f0: e2840034     	add	r0, r4, #52
   4f7f4: ebffee2d     	bl	0x4b0b0
   4f7f8: e5952000     	ldr	r2, [r5]
   4f7fc: e5951004     	ldr	r1, [r5, #0x4]
   4f800: e041c002     	sub	r12, r1, r2
   4f804: e1b0312c     	lsrs	r3, r12, #2
   4f808: 08bd8ff8     	popeq	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
   4f80c: e5940040     	ldr	r0, [r4, #0x40]
   4f810: e082e00c     	add	lr, r2, r12
   4f814: e594104c     	ldr	r1, [r4, #0x4c]
   4f818: e1a03002     	mov	r3, r2
   4f81c: eddf3b2b     	vldr	d19, [pc, #172]         @ 0x4f8d0>&)+0x3ac> ; float 0.6
   4f820: ecb07a01     	vldmia	r0!, {s14}
   4f824: edd36a00     	vldr	s13, [r3]
   4f828: ecf17a01     	vldmia	r1!, {s15}
   4f82c: eef70ac7     	vcvt.f64.f32	d16, s14
   4f830: eef72ae6     	vcvt.f64.f32	d18, s13
   4f834: eef71ae7     	vcvt.f64.f32	d17, s15
   4f838: ee600ba3     	vmul.f64	d16, d16, d19
   4f83c: eee20ba3     	vfma.f64	d16, d18, d19
   4f840: ee710ba0     	vadd.f64	d16, d17, d16
   4f844: eef77be0     	vcvt.f32.f64	s15, d16
   4f848: ece37a01     	vstmia	r3!, {s15}
   4f84c: e153000e     	cmp	r3, lr
   4f850: 1afffff2     	bne	0x4f820
   4f854: e5943058     	ldr	r3, [r4, #0x58]
   4f858: e083100c     	add	r1, r3, r12
   4f85c: ecf37a01     	vldmia	r3!, {s15}
   4f860: ed927a00     	vldr	s14, [r2]
   4f864: edd46a00     	vldr	s13, [r4]
   4f868: e1530001     	cmp	r3, r1
   4f86c: ee377a67     	vsub.f32	s14, s14, s15
   4f870: eee67a87     	vfma.f32	s15, s13, s14
   4f874: ece27a01     	vstmia	r2!, {s15}
   4f878: 1afffff7     	bne	0x4f85c
   4f87c: e8bd8ff8     	pop	{r3, r4, r5, r6, r7, r8, r9, r10, r11, pc}
   4f880: e4913004     	ldr	r3, [r1], #4
   4f884: e4803004     	str	r3, [r0], #4
   4f888: e151000e     	cmp	r1, lr
   4f88c: 0affffc6     	beq	0x4f7ac
   4f890: e4913004     	ldr	r3, [r1], #4
   4f894: e4803004     	str	r3, [r0], #4
   4f898: e151000e     	cmp	r1, lr
   4f89c: 1afffff7     	bne	0x4f880
   4f8a0: eaffffc1     	b	0x4f7ac
   4f8a4: e1a03002     	mov	r3, r2
   4f8a8: e0826006     	add	r6, r2, r6
   4f8ac: e5932000     	ldr	r2, [r3]
   4f8b0: e48e2004     	str	r2, [lr], #4
   4f8b4: e4932004     	ldr	r2, [r3], #4
   4f8b8: e48c2004     	str	r2, [r12], #4
   4f8bc: e1530006     	cmp	r3, r6
   4f8c0: e5132004     	ldr	r2, [r3, #-0x4]
   4f8c4: e4802004     	str	r2, [r0], #4
   4f8c8: 1afffff7     	bne	0x4f8ac
   4f8cc: eaffff75     	b	0x4f6a8
   4f8d0: 33 33 33 33  	.word	0x33333333
   4f8d4: 33 33 e3 3f  	.word	0x3fe33333
   4f8d8: 9a 99 99 99  	.word	0x9999999a
   4f8dc: 99 99 c9 3f  	.word	0x3fc99999
