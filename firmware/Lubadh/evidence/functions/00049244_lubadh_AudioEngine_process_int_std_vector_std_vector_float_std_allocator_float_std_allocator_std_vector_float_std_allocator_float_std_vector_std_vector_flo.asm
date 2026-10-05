; lubadh::AudioEngine::process(int, std::vector<std::vector<float, std::allocator<float> >, std::allocator<std::vector<float, std::allocator<float> > > >&, std::vector<std::vector<float, std::allocator<float> >, std::allocator<std::vector<float, std::allocator<float> > > >&)
; VA 0x49244 size 1092

   49244: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   49248: e1a06002     	mov	r6, r2
   4924c: e590208c     	ldr	r2, [r0, #0x8c]
   49250: e24dd040     	sub	sp, sp, #64
   49254: e1a05003     	mov	r5, r3
   49258: e1a04000     	mov	r4, r0
   4925c: e3a03001     	mov	r3, #1
   49260: e1520001     	cmp	r2, r1
   49264: e5c03088     	strb	r3, [r0, #0x88]
   49268: e58d100c     	str	r1, [sp, #0xc]
   4926c: 1a000096     	bne	0x494cc
   49270: e8954001     	ldm	r5, {r0, lr}
   49274: e150000e     	cmp	r0, lr
   49278: 0a00000b     	beq	0x492ac
   4927c: e8901002     	ldm	r0, {r1, r12}
   49280: e151000c     	cmp	r1, r12
   49284: 0a000005     	beq	0x492a0
   49288: edd17a00     	vldr	s15, [r1]
   4928c: ed947a24     	vldr	s14, [r4, #144]
   49290: ee677a87     	vmul.f32	s15, s15, s14
   49294: ece17a01     	vstmia	r1!, {s15}
   49298: e15c0001     	cmp	r12, r1
   4929c: 1afffff9     	bne	0x49288
   492a0: e280000c     	add	r0, r0, #12
   492a4: e15e0000     	cmp	lr, r0
   492a8: 1afffff3     	bne	0x4927c
   492ac: e5947084     	ldr	r7, [r4, #0x84]
   492b0: e2870a2a     	add	r0, r7, #172032
   492b4: e2800090     	add	r0, r0, #144
   492b8: ebffd591     	bl	0x3e904
   492bc: e30405c8     	movw	r0, #0x45c8
   492c0: e3400005     	movt	r0, #0x5
   492c4: e0870000     	add	r0, r7, r0
   492c8: ebffd58d     	bl	0x3e904
   492cc: e5940084     	ldr	r0, [r4, #0x84]
   492d0: ebff80af     	bl	0x29594
   492d4: e5947084     	ldr	r7, [r4, #0x84]
   492d8: e2870a2a     	add	r0, r7, #172032
   492dc: e2878048     	add	r8, r7, #72
   492e0: e2800f8e     	add	r0, r0, #568
   492e4: ebffc93d     	bl	0x3b7e0
   492e8: e2880a2a     	add	r0, r8, #172032
   492ec: e2878ba9     	add	r8, r7, #173056
   492f0: e2800fed     	add	r0, r0, #948
   492f4: ebffc294     	bl	0x39d4c
   492f8: e2870915     	add	r0, r7, #344064
   492fc: e2800e77     	add	r0, r0, #1904
   49300: ebffc936     	bl	0x3b7e0
   49304: e2880ba9     	add	r0, r8, #173056
   49308: e2800f4d     	add	r0, r0, #308
   4930c: ebffc28e     	bl	0x39d4c
   49310: e3a00014     	mov	r0, #20
   49314: e3a03000     	mov	r3, #0
   49318: e58d3010     	str	r3, [sp, #0x10]
   4931c: ebff317a     	bl	0x1590c     @ imm = #-0x33a18 ; _Znwj
   49320: e1a0c000     	mov	r12, r0
   49324: e59f3358     	ldr	r3, [pc, #0x358]        @ 0x49684>, std::allocator<std::vector<float, std::allocator<float>>>>&, std::vector<std::vector<float, std::allocator<float>>, std::allocator<std::vector<float, std::allocator<float>>>>&)+0x440>
   49328: e28d7010     	add	r7, sp, #16
   4932c: e3052aa4     	movw	r2, #0x5aa4
   49330: e3402001     	movt	r2, #0x1
   49334: e1a00007     	mov	r0, r7
   49338: e58c3000     	str	r3, [r12]
   4933c: e28d1028     	add	r1, sp, #40
   49340: e28d300c     	add	r3, sp, #12
   49344: e58c4004     	str	r4, [r12, #0x4]
   49348: e58c600c     	str	r6, [r12, #0xc]
   4934c: e58c5010     	str	r5, [r12, #0x10]
   49350: e58c3008     	str	r3, [r12, #0x8]
   49354: e58dc028     	str	r12, [sp, #0x28]
   49358: ebff32a6     	bl	0x15df8    @ imm = #-0x33568 ; _ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE
   4935c: e59d0028     	ldr	r0, [sp, #0x28]
   49360: e3500000     	cmp	r0, #0
   49364: 0a000002     	beq	0x49374
   49368: e5903000     	ldr	r3, [r0]
   4936c: e5933004     	ldr	r3, [r3, #0x4]
   49370: e12fff33     	blx	r3
   49374: e5941084     	ldr	r1, [r4, #0x84]
   49378: e1a00004     	mov	r0, r4
   4937c: e5952000     	ldr	r2, [r5]
   49380: e5963000     	ldr	r3, [r6]
   49384: e2811ba9     	add	r1, r1, #173056
   49388: e282200c     	add	r2, r2, #12
   4938c: e2811d06     	add	r1, r1, #384
   49390: e58d2000     	str	r2, [sp]
   49394: e283300c     	add	r3, r3, #12
   49398: e59d200c     	ldr	r2, [sp, #0xc]
   4939c: ebfffa19     	bl	0x47c08
   493a0: e59d3010     	ldr	r3, [sp, #0x10]
   493a4: e3530000     	cmp	r3, #0
   493a8: 0a000001     	beq	0x493b4
   493ac: e1a00007     	mov	r0, r7
   493b0: ebff33d7     	bl	0x16314    @ imm = #-0x330a4 ; _ZNSt6thread4joinEv
   493b4: e5940084     	ldr	r0, [r4, #0x84]
   493b8: ebff8108     	bl	0x297e0
   493bc: e5946084     	ldr	r6, [r4, #0x84]
   493c0: e3047ab8     	movw	r7, #0x4ab8
   493c4: e3407005     	movt	r7, #0x5
   493c8: e0867007     	add	r7, r6, r7
   493cc: e2866048     	add	r6, r6, #72
   493d0: e59652cc     	ldr	r5, [r6, #0x2cc]
   493d4: e59682d0     	ldr	r8, [r6, #0x2d0]
   493d8: e1550008     	cmp	r5, r8
   493dc: 1a00000d     	bne	0x49418
   493e0: e2866ba9     	add	r6, r6, #173056
   493e4: e2866f4e     	add	r6, r6, #312
   493e8: e1570006     	cmp	r7, r6
   493ec: 1afffff7     	bne	0x493d0
   493f0: e59d3010     	ldr	r3, [sp, #0x10]
   493f4: e3530000     	cmp	r3, #0
   493f8: 1a00008b     	bne	0x4962c
   493fc: e5c43088     	strb	r3, [r4, #0x88]
   49400: e28dd040     	add	sp, sp, #64
   49404: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   49408: e1a00006     	mov	r0, r6
   4940c: ebffbb23     	bl	0x380a0
   49410: e1580005     	cmp	r8, r5
   49414: 0a000006     	beq	0x49434
   49418: e5953000     	ldr	r3, [r5]
   4941c: e2433001     	sub	r3, r3, #1
   49420: e4853004     	str	r3, [r5], #4
   49424: e3530000     	cmp	r3, #0
   49428: 0afffff6     	beq	0x49408
   4942c: e1580005     	cmp	r8, r5
   49430: 1afffff8     	bne	0x49418
   49434: e59632cc     	ldr	r3, [r6, #0x2cc]
   49438: e59602d0     	ldr	r0, [r6, #0x2d0]
   4943c: e0402003     	sub	r2, r0, r3
   49440: e1a01242     	asr	r1, r2, #4
   49444: e1a02142     	asr	r2, r2, #2
   49448: e3510000     	cmp	r1, #0
   4944c: da00005a     	ble	0x495bc
   49450: e0831201     	add	r1, r3, r1, lsl #4
   49454: ea00000b     	b	0x49488
   49458: e5932004     	ldr	r2, [r3, #0x4]
   4945c: e3520000     	cmp	r2, #0
   49460: 0a000061     	beq	0x495ec
   49464: e5932008     	ldr	r2, [r3, #0x8]
   49468: e3520000     	cmp	r2, #0
   4946c: 0a00005c     	beq	0x495e4
   49470: e593200c     	ldr	r2, [r3, #0xc]
   49474: e3520000     	cmp	r2, #0
   49478: 0a00005d     	beq	0x495f4
   4947c: e2833010     	add	r3, r3, #16
   49480: e1510003     	cmp	r1, r3
   49484: 0a00004a     	beq	0x495b4
   49488: e5932000     	ldr	r2, [r3]
   4948c: e3520000     	cmp	r2, #0
   49490: 1afffff0     	bne	0x49458
   49494: e1530000     	cmp	r3, r0
   49498: 0affffd0     	beq	0x493e0
   4949c: e2832004     	add	r2, r3, #4
   494a0: e1500002     	cmp	r0, r2
   494a4: 0a000006     	beq	0x494c4
   494a8: e4921004     	ldr	r1, [r2], #4
   494ac: e3510000     	cmp	r1, #0
   494b0: 14831004     	strne	r1, [r3], #4
   494b4: e1500002     	cmp	r0, r2
   494b8: 1afffffa     	bne	0x494a8
   494bc: e1500003     	cmp	r0, r3
   494c0: 0affffc6     	beq	0x493e0
   494c4: e58632d0     	str	r3, [r6, #0x2d0]
   494c8: eaffffc4     	b	0x493e0
   494cc: e28d7010     	add	r7, sp, #16
   494d0: e58d1000     	str	r1, [sp]
   494d4: e3003d28     	movw	r3, #0xd28
   494d8: e3403007     	movt	r3, #0x7
   494dc: e3061218     	movw	r1, #0x6218
   494e0: e3401001     	movt	r1, #0x1
   494e4: e1a00007     	mov	r0, r7
   494e8: e3a02010     	mov	r2, #16
   494ec: eb000365     	bl	0x4a288
   494f0: e3a02000     	mov	r2, #0
   494f4: e3a0301a     	mov	r3, #26
   494f8: e1a00007     	mov	r0, r7
   494fc: e58d3000     	str	r3, [sp]
   49500: e1a01002     	mov	r1, r2
   49504: e30331ec     	movw	r3, #0x31ec
   49508: e3403007     	movt	r3, #0x7
   4950c: ebff3179     	bl	0x15af8    @ imm = #-0x33a1c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_replaceEjjPKcj
   49510: e1a0e000     	mov	lr, r0
   49514: e28d7030     	add	r7, sp, #48
   49518: e58d7028     	str	r7, [sp, #0x28]
   4951c: e1a0c000     	mov	r12, r0
   49520: e49e3008     	ldr	r3, [lr], #8
   49524: e153000e     	cmp	r3, lr
   49528: 158d3028     	strne	r3, [sp, #0x28]
   4952c: 01a08007     	moveq	r8, r7
   49530: 059e0000     	ldreq	r0, [lr]
   49534: 059e1004     	ldreq	r1, [lr, #0x4]
   49538: 059e2008     	ldreq	r2, [lr, #0x8]
   4953c: 059e300c     	ldreq	r3, [lr, #0xc]
   49540: 159c3008     	ldrne	r3, [r12, #0x8]
   49544: 08a8000f     	stmeq	r8!, {r0, r1, r2, r3}
   49548: e3090fec     	movw	r0, #0x9fec
   4954c: e3400009     	movt	r0, #0x9
   49550: 158d3030     	strne	r3, [sp, #0x30]
   49554: e28d1028     	add	r1, sp, #40
   49558: e3a03000     	mov	r3, #0
   4955c: e59c2004     	ldr	r2, [r12, #0x4]
   49560: e58d202c     	str	r2, [sp, #0x2c]
   49564: e3a02001     	mov	r2, #1
   49568: e58ce000     	str	lr, [r12]
   4956c: e58c3004     	str	r3, [r12, #0x4]
   49570: e5cc3008     	strb	r3, [r12, #0x8]
   49574: eb009a69     	bl	0x6ff20
   49578: e59d0028     	ldr	r0, [sp, #0x28]
   4957c: e1500007     	cmp	r0, r7
   49580: 0a000000     	beq	0x49588
   49584: ebff322d     	bl	0x15e40    @ imm = #-0x3374c ; _ZdlPv
   49588: e59d0010     	ldr	r0, [sp, #0x10]
   4958c: e28d3018     	add	r3, sp, #24
   49590: e1500003     	cmp	r0, r3
   49594: 0a000000     	beq	0x4959c
   49598: ebff3228     	bl	0x15e40    @ imm = #-0x33760 ; _ZdlPv
   4959c: e5943000     	ldr	r3, [r4]
   495a0: e1a00004     	mov	r0, r4
   495a4: e59d100c     	ldr	r1, [sp, #0xc]
   495a8: e5933004     	ldr	r3, [r3, #0x4]
   495ac: e12fff33     	blx	r3
   495b0: eaffff2e     	b	0x49270
   495b4: e0402003     	sub	r2, r0, r3
   495b8: e1a02142     	asr	r2, r2, #2
   495bc: e3520002     	cmp	r2, #2
   495c0: 0a000011     	beq	0x4960c
   495c4: e3520003     	cmp	r2, #3
   495c8: 0a00000b     	beq	0x495fc
   495cc: e3520001     	cmp	r2, #1
   495d0: 1affff82     	bne	0x493e0
   495d4: e5932000     	ldr	r2, [r3]
   495d8: e3520000     	cmp	r2, #0
   495dc: 1affff7f     	bne	0x493e0
   495e0: eaffffab     	b	0x49494
   495e4: e2833008     	add	r3, r3, #8
   495e8: eaffffa9     	b	0x49494
   495ec: e2833004     	add	r3, r3, #4
   495f0: eaffffa7     	b	0x49494
   495f4: e283300c     	add	r3, r3, #12
   495f8: eaffffa5     	b	0x49494
   495fc: e5932000     	ldr	r2, [r3]
   49600: e3520000     	cmp	r2, #0
   49604: 0affffa2     	beq	0x49494
   49608: e2833004     	add	r3, r3, #4
   4960c: e5932000     	ldr	r2, [r3]
   49610: e3520000     	cmp	r2, #0
   49614: 0affff9e     	beq	0x49494
   49618: e2833004     	add	r3, r3, #4
   4961c: eaffffec     	b	0x495d4
   49620: e59d3010     	ldr	r3, [sp, #0x10]
   49624: e3530000     	cmp	r3, #0
   49628: 0a000006     	beq	0x49648
   4962c: ebff3101     	bl	0x15a38    @ imm = #-0x33bfc ; _ZSt9terminatev
   49630: e59d0028     	ldr	r0, [sp, #0x28]
   49634: e3500000     	cmp	r0, #0
   49638: 0a000002     	beq	0x49648
   4963c: e5903000     	ldr	r3, [r0]
   49640: e5933004     	ldr	r3, [r3, #0x4]
   49644: e12fff33     	blx	r3
   49648: e3a03000     	mov	r3, #0
   4964c: e5c43088     	strb	r3, [r4, #0x88]
   49650: ebff3242     	bl	0x15f60    @ imm = #-0x336f8 ; __cxa_end_cleanup
   49654: ea000004     	b	0x4966c
   49658: eafffffa     	b	0x49648
   4965c: e59d0028     	ldr	r0, [sp, #0x28]
   49660: e1500007     	cmp	r0, r7
   49664: 0a000000     	beq	0x4966c
   49668: ebff31f4     	bl	0x15e40    @ imm = #-0x33830 ; _ZdlPv
   4966c: e59d0010     	ldr	r0, [sp, #0x10]
   49670: e28d3018     	add	r3, sp, #24
   49674: e1500003     	cmp	r0, r3
   49678: 0afffff2     	beq	0x49648
   4967c: ebff31ef     	bl	0x15e40    @ imm = #-0x33844 ; _ZdlPv
   49680: eafffff0     	b	0x49648
   49684: 2c 32 07 00  	.word	0x0007322c
