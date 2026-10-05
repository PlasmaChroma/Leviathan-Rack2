; lubadh::Channel::interpretButtonPress(int)
; VA 0x3f1c4 size 2700

   3f1c4: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   3f1c8: e1a04000     	mov	r4, r0
   3f1cc: e24dd090     	sub	sp, sp, #144
   3f1d0: e3510017     	cmp	r1, #23
   3f1d4: 979ff101     	ldrls	pc, [pc, r1, lsl #2]
   3f1d8: ea000060     	b	0x3f360
   3f1dc: 68 f3 03 00  	.word	0x0003f368
   3f1e0: 68 f3 03 00  	.word	0x0003f368
   3f1e4: a8 f3 03 00  	.word	0x0003f3a8
   3f1e8: 60 f3 03 00  	.word	0x0003f360
   3f1ec: e8 f3 03 00  	.word	0x0003f3e8
   3f1f0: 0c f4 03 00  	.word	0x0003f40c
   3f1f4: 34 f4 03 00  	.word	0x0003f434
   3f1f8: 74 f4 03 00  	.word	0x0003f474
   3f1fc: e0 f4 03 00  	.word	0x0003f4e0
   3f200: 1c f6 03 00  	.word	0x0003f61c
   3f204: 60 f3 03 00  	.word	0x0003f360
   3f208: 3c f2 03 00  	.word	0x0003f23c
   3f20c: 5c f5 03 00  	.word	0x0003f55c
   3f210: 7c f6 03 00  	.word	0x0003f67c
   3f214: d8 f6 03 00  	.word	0x0003f6d8
   3f218: e8 f6 03 00  	.word	0x0003f6e8
   3f21c: 60 f3 03 00  	.word	0x0003f360
   3f220: 60 f3 03 00  	.word	0x0003f360
   3f224: 18 f7 03 00  	.word	0x0003f718
   3f228: 60 f7 03 00  	.word	0x0003f760
   3f22c: a8 f7 03 00  	.word	0x0003f7a8
   3f230: 60 f3 03 00  	.word	0x0003f360
   3f234: 14 f8 03 00  	.word	0x0003f814
   3f238: 24 f8 03 00  	.word	0x0003f824
   3f23c: e3a03001     	mov	r3, #1
   3f240: e5c030ad     	strb	r3, [r0, #0xad]
   3f244: ebffff23     	bl	0x3eed8
   3f248: e5943000     	ldr	r3, [r4]
   3f24c: e28d0018     	add	r0, sp, #24
   3f250: e3a02010     	mov	r2, #16
   3f254: e58d3000     	str	r3, [sp]
   3f258: e3061218     	movw	r1, #0x6218
   3f25c: e3401001     	movt	r1, #0x1
   3f260: e3003d28     	movw	r3, #0xd28
   3f264: e3403007     	movt	r3, #0x7
   3f268: ebffdf4e     	bl	0x36fa8
   3f26c: e3022920     	movw	r2, #0x2920
   3f270: e3402007     	movt	r2, #0x7
   3f274: e28d0018     	add	r0, sp, #24
   3f278: e3a01000     	mov	r1, #0
   3f27c: ebff5aa4     	bl	0x15d14    @ imm = #-0x29570 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6insertEjPKc
   3f280: e1a01000     	mov	r1, r0
   3f284: e28d0030     	add	r0, sp, #48
   3f288: ebff5a5c     	bl	0x15c00    @ imm = #-0x29690 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3f28c: e3021938     	movw	r1, #0x2938
   3f290: e3401007     	movt	r1, #0x7
   3f294: e28d0030     	add	r0, sp, #48
   3f298: ebff5c6b     	bl	0x1644c    @ imm = #-0x28e54 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   3f29c: e28d5048     	add	r5, sp, #72
   3f2a0: e1a01000     	mov	r1, r0
   3f2a4: e1a00005     	mov	r0, r5
   3f2a8: e28d6060     	add	r6, sp, #96
   3f2ac: ebff5a53     	bl	0x15c00    @ imm = #-0x296b4 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3f2b0: e5d430ad     	ldrb	r3, [r4, #0xad]
   3f2b4: e58d3000     	str	r3, [sp]
   3f2b8: e3061218     	movw	r1, #0x6218
   3f2bc: e3401001     	movt	r1, #0x1
   3f2c0: e3003d28     	movw	r3, #0xd28
   3f2c4: e3403007     	movt	r3, #0x7
   3f2c8: e1a00006     	mov	r0, r6
   3f2cc: e3a02010     	mov	r2, #16
   3f2d0: ebffdf34     	bl	0x36fa8
   3f2d4: e28d4078     	add	r4, sp, #120
   3f2d8: e1a02006     	mov	r2, r6
   3f2dc: e1a01005     	mov	r1, r5
   3f2e0: e1a00004     	mov	r0, r4
   3f2e4: ebffbd80     	bl	0x2e8ec
   3f2e8: e3090fec     	movw	r0, #0x9fec
   3f2ec: e3400009     	movt	r0, #0x9
   3f2f0: e1a01004     	mov	r1, r4
   3f2f4: e3a02000     	mov	r2, #0
   3f2f8: eb00c308     	bl	0x6ff20
   3f2fc: e59d0078     	ldr	r0, [sp, #0x78]
   3f300: e28d3080     	add	r3, sp, #128
   3f304: e1500003     	cmp	r0, r3
   3f308: 0a000000     	beq	0x3f310
   3f30c: ebff5acb     	bl	0x15e40    @ imm = #-0x294d4 ; _ZdlPv
   3f310: e59d0060     	ldr	r0, [sp, #0x60]
   3f314: e28d3068     	add	r3, sp, #104
   3f318: e1500003     	cmp	r0, r3
   3f31c: 0a000000     	beq	0x3f324
   3f320: ebff5ac6     	bl	0x15e40    @ imm = #-0x294e8 ; _ZdlPv
   3f324: e59d0048     	ldr	r0, [sp, #0x48]
   3f328: e28d3050     	add	r3, sp, #80
   3f32c: e1500003     	cmp	r0, r3
   3f330: 0a000000     	beq	0x3f338
   3f334: ebff5ac1     	bl	0x15e40    @ imm = #-0x294fc ; _ZdlPv
   3f338: e59d0030     	ldr	r0, [sp, #0x30]
   3f33c: e28d3038     	add	r3, sp, #56
   3f340: e1500003     	cmp	r0, r3
   3f344: 0a000000     	beq	0x3f34c
   3f348: ebff5abc     	bl	0x15e40    @ imm = #-0x29510 ; _ZdlPv
   3f34c: e59d0018     	ldr	r0, [sp, #0x18]
   3f350: e28d3020     	add	r3, sp, #32
   3f354: e1500003     	cmp	r0, r3
   3f358: 0a000000     	beq	0x3f360
   3f35c: ebff5ab7     	bl	0x15e40    @ imm = #-0x29524 ; _ZdlPv
   3f360: e28dd090     	add	sp, sp, #144
   3f364: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   3f368: e5d03284     	ldrb	r3, [r0, #0x284]
   3f36c: e1a05001     	mov	r5, r1
   3f370: e3530000     	cmp	r3, #0
   3f374: 1a000191     	bne	0x3f9c0
   3f378: e3a02001     	mov	r2, #1
   3f37c: e3a01000     	mov	r1, #0
   3f380: e1a00004     	mov	r0, r4
   3f384: ebffe0c8     	bl	0x376ac
   3f388: e5940278     	ldr	r0, [r4, #0x278]
   3f38c: e1a01005     	mov	r1, r5
   3f390: e5903000     	ldr	r3, [r0]
   3f394: e5933000     	ldr	r3, [r3]
   3f398: e12fff33     	blx	r3
   3f39c: e3a03001     	mov	r3, #1
   3f3a0: e5c43285     	strb	r3, [r4, #0x285]
   3f3a4: eaffffed     	b	0x3f360
   3f3a8: e2803a2a     	add	r3, r0, #172032
   3f3ac: e5d33328     	ldrb	r3, [r3, #0x328]
   3f3b0: e3530000     	cmp	r3, #0
   3f3b4: 1affffe9     	bne	0x3f360
   3f3b8: e5903110     	ldr	r3, [r0, #0x110]
   3f3bc: e3530000     	cmp	r3, #0
   3f3c0: 12433001     	subne	r3, r3, #1
   3f3c4: 15803110     	strne	r3, [r0, #0x110]
   3f3c8: 1affffe4     	bne	0x3f360
   3f3cc: e59030e8     	ldr	r3, [r0, #0xe8]
   3f3d0: e59360b0     	ldr	r6, [r3, #0xb0]
   3f3d4: e5962038     	ldr	r2, [r6, #0x38]
   3f3d8: e3520000     	cmp	r2, #0
   3f3dc: 1a00019a     	bne	0x3fa4c
   3f3e0: ebffe32e     	bl	0x380a0
   3f3e4: eaffffdd     	b	0x3f360
   3f3e8: e5900278     	ldr	r0, [r0, #0x278]
   3f3ec: e3a03001     	mov	r3, #1
   3f3f0: e5c43285     	strb	r3, [r4, #0x285]
   3f3f4: e3a01006     	mov	r1, #6
   3f3f8: e5c4328b     	strb	r3, [r4, #0x28b]
   3f3fc: e5903000     	ldr	r3, [r0]
   3f400: e5933000     	ldr	r3, [r3]
   3f404: e12fff33     	blx	r3
   3f408: eaffffd4     	b	0x3f360
   3f40c: e5900278     	ldr	r0, [r0, #0x278]
   3f410: e3a03000     	mov	r3, #0
   3f414: e5c4328b     	strb	r3, [r4, #0x28b]
   3f418: e3a02001     	mov	r2, #1
   3f41c: e3a01007     	mov	r1, #7
   3f420: e5c42285     	strb	r2, [r4, #0x285]
   3f424: e5903000     	ldr	r3, [r0]
   3f428: e5933000     	ldr	r3, [r3]
   3f42c: e12fff33     	blx	r3
   3f430: eaffffca     	b	0x3f360
   3f434: e59030e8     	ldr	r3, [r0, #0xe8]
   3f438: e59320b0     	ldr	r2, [r3, #0xb0]
   3f43c: e5922028     	ldr	r2, [r2, #0x28]
   3f440: e3520000     	cmp	r2, #0
   3f444: 1a000153     	bne	0x3f998
   3f448: e5900278     	ldr	r0, [r0, #0x278]
   3f44c: e3a02001     	mov	r2, #1
   3f450: e3a0e000     	mov	lr, #0
   3f454: e5c42285     	strb	r2, [r4, #0x285]
   3f458: e3a01006     	mov	r1, #6
   3f45c: e590c000     	ldr	r12, [r0]
   3f460: e5c422ac     	strb	r2, [r4, #0x2ac]
   3f464: e583e0a4     	str	lr, [r3, #0xa4]
   3f468: e59c3000     	ldr	r3, [r12]
   3f46c: e12fff33     	blx	r3
   3f470: eaffffba     	b	0x3f360
   3f474: e5900278     	ldr	r0, [r0, #0x278]
   3f478: e3a03000     	mov	r3, #0
   3f47c: e5c432ac     	strb	r3, [r4, #0x2ac]
   3f480: e3a02001     	mov	r2, #1
   3f484: e3a01007     	mov	r1, #7
   3f488: e5c42285     	strb	r2, [r4, #0x285]
   3f48c: e5903000     	ldr	r3, [r0]
   3f490: e5933000     	ldr	r3, [r3]
   3f494: e12fff33     	blx	r3
   3f498: e2843ba9     	add	r3, r4, #173056
   3f49c: ed9f6ae5     	vldr	s12, [pc, #916]         @ 0x3f838 ; float 4095
   3f4a0: e28330b8     	add	r3, r3, #184
   3f4a4: eddf6ae4     	vldr	s13, [pc, #912]         @ 0x3f83c ; float 0.899999976158
   3f4a8: ed937a00     	vldr	s14, [r3]
   3f4ac: eeb87ac7     	vcvt.f32.s32	s14, s14
   3f4b0: eec77a06     	vdiv.f32	s15, s14, s12
   3f4b4: eef47ae6     	vcmpe.f32	s15, s13
   3f4b8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3f4bc: 5ef07a66     	vmovpl.f32	s15, s13
   3f4c0: 5a000003     	bpl	0x3f4d4
   3f4c4: eef57ac0     	vcmpe.f32	s15, #0
   3f4c8: ed9f7adc     	vldr	s14, [pc, #880]         @ 0x3f840 ; float 0
   3f4cc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3f4d0: def07a47     	vmovle.f32	s15, s14
   3f4d4: e59430e8     	ldr	r3, [r4, #0xe8]
   3f4d8: edc37a29     	vstr	s15, [r3, #164]
   3f4dc: eaffff9f     	b	0x3f360
   3f4e0: e59030e8     	ldr	r3, [r0, #0xe8]
   3f4e4: e59320b0     	ldr	r2, [r3, #0xb0]
   3f4e8: e5922040     	ldr	r2, [r2, #0x40]
   3f4ec: e3520003     	cmp	r2, #3
   3f4f0: 1affff9a     	bne	0x3f360
   3f4f4: e59020ec     	ldr	r2, [r0, #0xec]
   3f4f8: e3520000     	cmp	r2, #0
   3f4fc: 0a000159     	beq	0x3fa68
   3f500: e3520001     	cmp	r2, #1
   3f504: 03a02000     	moveq	r2, #0
   3f508: 058020ec     	streq	r2, [r0, #0xec]
   3f50c: edd47a0b     	vldr	s15, [r4, #44]
   3f510: eeb76a00     	vmov.f32	s12, #1.000000e+00
   3f514: ed947ab6     	vldr	s14, [r4, #728]
   3f518: eddf5ac9     	vldr	s11, [pc, #804]         @ 0x3f844 ; float 2.70000004768
   3f51c: eef17a67     	vneg.f32	s15, s15
   3f520: eddf0bc2     	vldr	d16, [pc, #776]         @ 0x3f830 ; float 6.36598738204e-313
   3f524: edc40ba4     	vstr	d16, [r4, #656]
   3f528: edc47a0b     	vstr	s15, [r4, #44]
   3f52c: ee777ac7     	vsub.f32	s15, s15, s14
   3f530: edd36a2b     	vldr	s13, [r3, #172]
   3f534: eefd6ae6     	vcvt.s32.f32	s13, s13
   3f538: eef86ae6     	vcvt.f32.s32	s13, s13
   3f53c: ee867aa5     	vdiv.f32	s14, s13, s11
   3f540: ee377a06     	vadd.f32	s14, s14, s12
   3f544: eebd7ac7     	vcvt.s32.f32	s14, s14
   3f548: eef86ac7     	vcvt.f32.s32	s13, s14
   3f54c: ed847ab8     	vstr	s14, [r4, #736]
   3f550: ee877aa6     	vdiv.f32	s14, s15, s13
   3f554: ed847ab7     	vstr	s14, [r4, #732]
   3f558: eaffff80     	b	0x3f360
   3f55c: e5903000     	ldr	r3, [r0]
   3f560: e3a05000     	mov	r5, #0
   3f564: e58d3000     	str	r3, [sp]
   3f568: e28d0018     	add	r0, sp, #24
   3f56c: e3a02010     	mov	r2, #16
   3f570: e3003d28     	movw	r3, #0xd28
   3f574: e3403007     	movt	r3, #0x7
   3f578: e5c450ad     	strb	r5, [r4, #0xad]
   3f57c: e3061218     	movw	r1, #0x6218
   3f580: e3401001     	movt	r1, #0x1
   3f584: ebffde87     	bl	0x36fa8
   3f588: e3022920     	movw	r2, #0x2920
   3f58c: e3402007     	movt	r2, #0x7
   3f590: e1a01005     	mov	r1, r5
   3f594: e28d0018     	add	r0, sp, #24
   3f598: ebff59dd     	bl	0x15d14    @ imm = #-0x2988c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6insertEjPKc
   3f59c: e1a01000     	mov	r1, r0
   3f5a0: e28d0030     	add	r0, sp, #48
   3f5a4: ebff5995     	bl	0x15c00    @ imm = #-0x299ac ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3f5a8: e3021938     	movw	r1, #0x2938
   3f5ac: e3401007     	movt	r1, #0x7
   3f5b0: e28d0030     	add	r0, sp, #48
   3f5b4: ebff5ba4     	bl	0x1644c    @ imm = #-0x29170 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   3f5b8: e28d5048     	add	r5, sp, #72
   3f5bc: e1a01000     	mov	r1, r0
   3f5c0: e1a00005     	mov	r0, r5
   3f5c4: e28d6060     	add	r6, sp, #96
   3f5c8: ebff598c     	bl	0x15c00    @ imm = #-0x299d0 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3f5cc: e5d430ad     	ldrb	r3, [r4, #0xad]
   3f5d0: e58d3000     	str	r3, [sp]
   3f5d4: e3061218     	movw	r1, #0x6218
   3f5d8: e3401001     	movt	r1, #0x1
   3f5dc: e3003d28     	movw	r3, #0xd28
   3f5e0: e3403007     	movt	r3, #0x7
   3f5e4: e1a00006     	mov	r0, r6
   3f5e8: e3a02010     	mov	r2, #16
   3f5ec: ebffde6d     	bl	0x36fa8
   3f5f0: e28d4078     	add	r4, sp, #120
   3f5f4: e1a02006     	mov	r2, r6
   3f5f8: e1a01005     	mov	r1, r5
   3f5fc: e1a00004     	mov	r0, r4
   3f600: ebffbcb9     	bl	0x2e8ec
   3f604: e3090fec     	movw	r0, #0x9fec
   3f608: e3400009     	movt	r0, #0x9
   3f60c: e1a01004     	mov	r1, r4
   3f610: e3a02000     	mov	r2, #0
   3f614: eb00c241     	bl	0x6ff20
   3f618: eaffff37     	b	0x3f2fc
   3f61c: e59071f8     	ldr	r7, [r0, #0x1f8]
   3f620: e5906208     	ldr	r6, [r0, #0x208]
   3f624: e2875004     	add	r5, r7, #4
   3f628: e59081ec     	ldr	r8, [r0, #0x1ec]
   3f62c: e2866004     	add	r6, r6, #4
   3f630: e59091f0     	ldr	r9, [r0, #0x1f0]
   3f634: e590a1f4     	ldr	r10, [r0, #0x1f4]
   3f638: e1550006     	cmp	r5, r6
   3f63c: 2a000003     	bhs	0x3f650
   3f640: e4950004     	ldr	r0, [r5], #4
   3f644: ebff59fd     	bl	0x15e40    @ imm = #-0x2980c ; _ZdlPv
   3f648: e1560005     	cmp	r6, r5
   3f64c: 8afffffb     	bhi	0x3f640
   3f650: e58d8008     	str	r8, [sp, #0x8]
   3f654: e2842f7f     	add	r2, r4, #508
   3f658: e58d900c     	str	r9, [sp, #0xc]
   3f65c: e3a03001     	mov	r3, #1
   3f660: e58da010     	str	r10, [sp, #0x10]
   3f664: e58d7014     	str	r7, [sp, #0x14]
   3f668: eddd0b02     	vldr	d16, [sp, #8]
   3f66c: eddd1b04     	vldr	d17, [sp, #16]
   3f670: f4420a8f     	vst1.32	{d16, d17}, [r2]
   3f674: e5c431e0     	strb	r3, [r4, #0x1e0]
   3f678: eaffff38     	b	0x3f360
   3f67c: e2806a2a     	add	r6, r0, #172032
   3f680: e3a02001     	mov	r2, #1
   3f684: e3053556     	movw	r3, #0x5556
   3f688: e3453555     	movt	r3, #0x5555
   3f68c: e5c02285     	strb	r2, [r0, #0x285]
   3f690: e59654d0     	ldr	r5, [r6, #0x4d0]
   3f694: e5c02288     	strb	r2, [r0, #0x288]
   3f698: e0855002     	add	r5, r5, r2
   3f69c: e0c32593     	smull	r2, r3, r3, r5
   3f6a0: e0433fc5     	sub	r3, r3, r5, asr #31
   3f6a4: e0833083     	add	r3, r3, r3, lsl #1
   3f6a8: e0455003     	sub	r5, r5, r3
   3f6ac: e58654d0     	str	r5, [r6, #0x4d0]
   3f6b0: e3550001     	cmp	r5, #1
   3f6b4: 0a0000d6     	beq	0x3fa14
   3f6b8: e3550002     	cmp	r5, #2
   3f6bc: 0a0000a1     	beq	0x3f948
   3f6c0: e3550000     	cmp	r5, #0
   3f6c4: 0a00008e     	beq	0x3f904
   3f6c8: e2844a29     	add	r4, r4, #167936
   3f6cc: e5943f5c     	ldr	r3, [r4, #0xf5c]
   3f6d0: e5835014     	str	r5, [r3, #0x14]
   3f6d4: eaffff21     	b	0x3f360
   3f6d8: e2800a2a     	add	r0, r0, #172032
   3f6dc: e2800e1f     	add	r0, r0, #496
   3f6e0: ebffe76a     	bl	0x39490
   3f6e4: eaffff1d     	b	0x3f360
   3f6e8: e2804a2a     	add	r4, r0, #172032
   3f6ec: e59433fc     	ldr	r3, [r4, #0x3fc]
   3f6f0: e5933000     	ldr	r3, [r3]
   3f6f4: e3530001     	cmp	r3, #1
   3f6f8: 0affff18     	beq	0x3f360
   3f6fc: e5943460     	ldr	r3, [r4, #0x460]
   3f700: e5933000     	ldr	r3, [r3]
   3f704: e3530001     	cmp	r3, #1
   3f708: 0affff14     	beq	0x3f360
   3f70c: e2840fed     	add	r0, r4, #948
   3f710: ebffe705     	bl	0x3932c
   3f714: eaffff11     	b	0x3f360
   3f718: e5903020     	ldr	r3, [r0, #0x20]
   3f71c: e3a02001     	mov	r2, #1
   3f720: e5c02285     	strb	r2, [r0, #0x285]
   3f724: e2833915     	add	r3, r3, #344064
   3f728: e5d33abc     	ldrb	r3, [r3, #0xabc]
   3f72c: e3530000     	cmp	r3, #0
   3f730: 1590301c     	ldrne	r3, [r0, #0x1c]
   3f734: 15c32285     	strbne	r2, [r3, #0x285]
   3f738: e59030e8     	ldr	r3, [r0, #0xe8]
   3f73c: e593108c     	ldr	r1, [r3, #0x8c]
   3f740: e16f1f11     	clz	r1, r1
   3f744: e1a012a1     	lsr	r1, r1, #5
   3f748: ebffe8e6     	bl	0x39ae8
   3f74c: e2843a29     	add	r3, r4, #167936
   3f750: e59420b0     	ldr	r2, [r4, #0xb0]
   3f754: e5933f5c     	ldr	r3, [r3, #0xf5c]
   3f758: e583200c     	str	r2, [r3, #0xc]
   3f75c: eafffeff     	b	0x3f360
   3f760: e5903020     	ldr	r3, [r0, #0x20]
   3f764: e3a02001     	mov	r2, #1
   3f768: e5c02285     	strb	r2, [r0, #0x285]
   3f76c: e2833915     	add	r3, r3, #344064
   3f770: e5d33abc     	ldrb	r3, [r3, #0xabc]
   3f774: e3530000     	cmp	r3, #0
   3f778: 1590301c     	ldrne	r3, [r0, #0x1c]
   3f77c: 15c32285     	strbne	r2, [r3, #0x285]
   3f780: e59030e8     	ldr	r3, [r0, #0xe8]
   3f784: e5931098     	ldr	r1, [r3, #0x98]
   3f788: e16f1f11     	clz	r1, r1
   3f78c: e1a012a1     	lsr	r1, r1, #5
   3f790: ebffecf2     	bl	0x3ab60
   3f794: e2843a29     	add	r3, r4, #167936
   3f798: e59420bc     	ldr	r2, [r4, #0xbc]
   3f79c: e5933f5c     	ldr	r3, [r3, #0xf5c]
   3f7a0: e5832010     	str	r2, [r3, #0x10]
   3f7a4: eafffeed     	b	0x3f360
   3f7a8: e59030e8     	ldr	r3, [r0, #0xe8]
   3f7ac: e5935084     	ldr	r5, [r3, #0x84]
   3f7b0: e3550001     	cmp	r5, #1
   3f7b4: 0a000086     	beq	0x3f9d4
   3f7b8: e3550002     	cmp	r5, #2
   3f7bc: 0a000038     	beq	0x3f8a4
   3f7c0: e3550000     	cmp	r5, #0
   3f7c4: 0a00001f     	beq	0x3f848
   3f7c8: e5943020     	ldr	r3, [r4, #0x20]
   3f7cc: e3a02001     	mov	r2, #1
   3f7d0: e5c42285     	strb	r2, [r4, #0x285]
   3f7d4: e2833915     	add	r3, r3, #344064
   3f7d8: e5d33abc     	ldrb	r3, [r3, #0xabc]
   3f7dc: e3530000     	cmp	r3, #0
   3f7e0: 0a000006     	beq	0x3f800
   3f7e4: e594301c     	ldr	r3, [r4, #0x1c]
   3f7e8: e5c32285     	strb	r2, [r3, #0x285]
   3f7ec: e5d3127c     	ldrb	r1, [r3, #0x27c]
   3f7f0: e5d4227c     	ldrb	r2, [r4, #0x27c]
   3f7f4: e5c3227c     	strb	r2, [r3, #0x27c]
   3f7f8: e0222001     	eor	r2, r2, r1
   3f7fc: e5c3227d     	strb	r2, [r3, #0x27d]
   3f800: e2843a29     	add	r3, r4, #167936
   3f804: e59420a8     	ldr	r2, [r4, #0xa8]
   3f808: e5933f5c     	ldr	r3, [r3, #0xf5c]
   3f80c: e5832008     	str	r2, [r3, #0x8]
   3f810: eafffed2     	b	0x3f360
   3f814: e2800a2a     	add	r0, r0, #172032
   3f818: e2800048     	add	r0, r0, #72
   3f81c: ebffe60d     	bl	0x39058
   3f820: eafffece     	b	0x3f360
   3f824: e3a01001     	mov	r1, #1
   3f828: ebffe301     	bl	0x38434
   3f82c: eafffecb     	b	0x3f360
   3f830: b9 00 00 00  	.word	0x000000b9
   3f834: 1e 00 00 00  	.word	0x0000001e
   3f838: 00 f0 7f 45  	.word	0x457ff000
   3f83c: 66 66 66 3f  	.word	0x3f666666
   3f840: 00 00 00 00  	.word	0x00000000
   3f844: cd cc 2c 40  	.word	0x402ccccd
   3f848: e5d0227c     	ldrb	r2, [r0, #0x27c]
   3f84c: e3a01002     	mov	r1, #2
   3f850: e28d0078     	add	r0, sp, #120
   3f854: e5831084     	str	r1, [r3, #0x84]
   3f858: e2223001     	eor	r3, r2, #1
   3f85c: e2841004     	add	r1, r4, #4
   3f860: e5c4327d     	strb	r3, [r4, #0x27d]
   3f864: e3022910     	movw	r2, #0x2910
   3f868: e3402007     	movt	r2, #0x7
   3f86c: e3a03001     	mov	r3, #1
   3f870: e5c4327c     	strb	r3, [r4, #0x27c]
   3f874: ebffbcd3     	bl	0x2ebc8
   3f878: e3090fec     	movw	r0, #0x9fec
   3f87c: e3400009     	movt	r0, #0x9
   3f880: e1a02005     	mov	r2, r5
   3f884: e28d1078     	add	r1, sp, #120
   3f888: eb00c1a4     	bl	0x6ff20
   3f88c: e59d0078     	ldr	r0, [sp, #0x78]
   3f890: e28d3080     	add	r3, sp, #128
   3f894: e1500003     	cmp	r0, r3
   3f898: 0affffca     	beq	0x3f7c8
   3f89c: ebff5967     	bl	0x15e40    @ imm = #-0x29a64 ; _ZdlPv
   3f8a0: eaffffc8     	b	0x3f7c8
   3f8a4: e5901278     	ldr	r1, [r0, #0x278]
   3f8a8: e2802e25     	add	r2, r0, #592
   3f8ac: e3a00001     	mov	r0, #1
   3f8b0: e5830084     	str	r0, [r3, #0x84]
   3f8b4: e1510002     	cmp	r1, r2
   3f8b8: e5d4327c     	ldrb	r3, [r4, #0x27c]
   3f8bc: 0a00005f     	beq	0x3fa40
   3f8c0: e2842e26     	add	r2, r4, #608
   3f8c4: e1510002     	cmp	r1, r2
   3f8c8: 13a02000     	movne	r2, #0
   3f8cc: 0a00005b     	beq	0x3fa40
   3f8d0: e28d0078     	add	r0, sp, #120
   3f8d4: e2841004     	add	r1, r4, #4
   3f8d8: e5c4227c     	strb	r2, [r4, #0x27c]
   3f8dc: e30228e8     	movw	r2, #0x28e8
   3f8e0: e3402007     	movt	r2, #0x7
   3f8e4: e5c4327d     	strb	r3, [r4, #0x27d]
   3f8e8: ebffbcb6     	bl	0x2ebc8
   3f8ec: e3090fec     	movw	r0, #0x9fec
   3f8f0: e3400009     	movt	r0, #0x9
   3f8f4: e28d1078     	add	r1, sp, #120
   3f8f8: e3a02000     	mov	r2, #0
   3f8fc: eb00c187     	bl	0x6ff20
   3f900: eaffffe1     	b	0x3f88c
   3f904: e28d0078     	add	r0, sp, #120
   3f908: e2841004     	add	r1, r4, #4
   3f90c: e302293c     	movw	r2, #0x293c
   3f910: e3402007     	movt	r2, #0x7
   3f914: ebffbcab     	bl	0x2ebc8
   3f918: e3090fec     	movw	r0, #0x9fec
   3f91c: e3400009     	movt	r0, #0x9
   3f920: e1a02005     	mov	r2, r5
   3f924: e28d1078     	add	r1, sp, #120
   3f928: eb00c17c     	bl	0x6ff20
   3f92c: e59d0078     	ldr	r0, [sp, #0x78]
   3f930: e28d3080     	add	r3, sp, #128
   3f934: e1500003     	cmp	r0, r3
   3f938: 0a000000     	beq	0x3f940
   3f93c: ebff593f     	bl	0x15e40    @ imm = #-0x29b04 ; _ZdlPv
   3f940: e59654d0     	ldr	r5, [r6, #0x4d0]
   3f944: eaffff5f     	b	0x3f6c8
   3f948: e59030e8     	ldr	r3, [r0, #0xe8]
   3f94c: e59330b0     	ldr	r3, [r3, #0xb0]
   3f950: e593702c     	ldr	r7, [r3, #0x2c]
   3f954: e3570001     	cmp	r7, #1
   3f958: 0a000050     	beq	0x3faa0
   3f95c: e3570002     	cmp	r7, #2
   3f960: 0a000043     	beq	0x3fa74
   3f964: e3570000     	cmp	r7, #0
   3f968: 1affff56     	bne	0x3f6c8
   3f96c: e28d0078     	add	r0, sp, #120
   3f970: e2841004     	add	r1, r4, #4
   3f974: e302297c     	movw	r2, #0x297c
   3f978: e3402007     	movt	r2, #0x7
   3f97c: ebffbc91     	bl	0x2ebc8
   3f980: e3090fec     	movw	r0, #0x9fec
   3f984: e3400009     	movt	r0, #0x9
   3f988: e1a02007     	mov	r2, r7
   3f98c: e28d1078     	add	r1, sp, #120
   3f990: eb00c162     	bl	0x6ff20
   3f994: eaffffe4     	b	0x3f92c
   3f998: ebffdde0     	bl	0x37120
   3f99c: e2842fa7     	add	r2, r4, #668
   3f9a0: eddf0ba8     	vldr	d16, [pc, #672]         @ 0x3fc48 ; float 3.92569221347e-312
   3f9a4: e3a03001     	mov	r3, #1
   3f9a8: e3a01003     	mov	r1, #3
   3f9ac: e5c43285     	strb	r3, [r4, #0x285]
   3f9b0: e5c432a4     	strb	r3, [r4, #0x2a4]
   3f9b4: e58412a8     	str	r1, [r4, #0x2a8]
   3f9b8: f442078f     	vst1.32	{d16}, [r2]
   3f9bc: eafffe67     	b	0x3f360
   3f9c0: e3a01000     	mov	r1, #0
   3f9c4: ebffe993     	bl	0x3a018
   3f9c8: e3a03001     	mov	r3, #1
   3f9cc: e5c43286     	strb	r3, [r4, #0x286]
   3f9d0: eafffe68     	b	0x3f378
   3f9d4: e5d0127c     	ldrb	r1, [r0, #0x27c]
   3f9d8: e3a05000     	mov	r5, #0
   3f9dc: e28d0078     	add	r0, sp, #120
   3f9e0: e5835084     	str	r5, [r3, #0x84]
   3f9e4: e30228fc     	movw	r2, #0x28fc
   3f9e8: e3402007     	movt	r2, #0x7
   3f9ec: e5c4127d     	strb	r1, [r4, #0x27d]
   3f9f0: e2841004     	add	r1, r4, #4
   3f9f4: e5c4527c     	strb	r5, [r4, #0x27c]
   3f9f8: ebffbc72     	bl	0x2ebc8
   3f9fc: e3090fec     	movw	r0, #0x9fec
   3fa00: e3400009     	movt	r0, #0x9
   3fa04: e1a02005     	mov	r2, r5
   3fa08: e28d1078     	add	r1, sp, #120
   3fa0c: eb00c143     	bl	0x6ff20
   3fa10: eaffff9d     	b	0x3f88c
   3fa14: e28d0078     	add	r0, sp, #120
   3fa18: e2841004     	add	r1, r4, #4
   3fa1c: e302295c     	movw	r2, #0x295c
   3fa20: e3402007     	movt	r2, #0x7
   3fa24: ebffbc67     	bl	0x2ebc8
   3fa28: e3090fec     	movw	r0, #0x9fec
   3fa2c: e3400009     	movt	r0, #0x9
   3fa30: e28d1078     	add	r1, sp, #120
   3fa34: e3a02000     	mov	r2, #0
   3fa38: eb00c138     	bl	0x6ff20
   3fa3c: eaffffba     	b	0x3f92c
   3fa40: e2233001     	eor	r3, r3, #1
   3fa44: e3a02001     	mov	r2, #1
   3fa48: eaffffa0     	b	0x3f8d0
   3fa4c: e59032d0     	ldr	r3, [r0, #0x2d0]
   3fa50: e59012d4     	ldr	r1, [r0, #0x2d4]
   3fa54: e1530001     	cmp	r3, r1
   3fa58: 0a00001b     	beq	0x3facc
   3fa5c: e4832004     	str	r2, [r3], #4
   3fa60: e58032d0     	str	r3, [r0, #0x2d0]
   3fa64: eafffe3d     	b	0x3f360
   3fa68: e3a02001     	mov	r2, #1
   3fa6c: e58020ec     	str	r2, [r0, #0xec]
   3fa70: eafffea5     	b	0x3f50c
   3fa74: e28d0078     	add	r0, sp, #120
   3fa78: e2841004     	add	r1, r4, #4
   3fa7c: e30229cc     	movw	r2, #0x29cc
   3fa80: e3402007     	movt	r2, #0x7
   3fa84: ebffbc4f     	bl	0x2ebc8
   3fa88: e3090fec     	movw	r0, #0x9fec
   3fa8c: e3400009     	movt	r0, #0x9
   3fa90: e28d1078     	add	r1, sp, #120
   3fa94: e3a02000     	mov	r2, #0
   3fa98: eb00c120     	bl	0x6ff20
   3fa9c: eaffffa2     	b	0x3f92c
   3faa0: e28d0078     	add	r0, sp, #120
   3faa4: e2841004     	add	r1, r4, #4
   3faa8: e30229a4     	movw	r2, #0x29a4
   3faac: e3402007     	movt	r2, #0x7
   3fab0: ebffbc44     	bl	0x2ebc8
   3fab4: e3090fec     	movw	r0, #0x9fec
   3fab8: e3400009     	movt	r0, #0x9
   3fabc: e28d1078     	add	r1, sp, #120
   3fac0: e3a02000     	mov	r2, #0
   3fac4: eb00c115     	bl	0x6ff20
   3fac8: eaffff97     	b	0x3f92c
   3facc: e59082cc     	ldr	r8, [r0, #0x2cc]
   3fad0: e0435008     	sub	r5, r3, r8
   3fad4: e1a03145     	asr	r3, r5, #2
   3fad8: e373021e     	cmn	r3, #-536870911
   3fadc: 0a000027     	beq	0x3fb80
   3fae0: e3530000     	cmp	r3, #0
   3fae4: 0a000023     	beq	0x3fb78
   3fae8: e1530083     	cmp	r3, r3, lsl #1
   3faec: e1a03083     	lsl	r3, r3, #1
   3faf0: 83e0320e     	mvnhi	r3, #-536870912
   3faf4: 9a000017     	bls	0x3fb58
   3faf8: e1a09103     	lsl	r9, r3, #2
   3fafc: e1a00009     	mov	r0, r9
   3fb00: ebff5781     	bl	0x1590c     @ imm = #-0x2a1fc ; _Znwj
   3fb04: e1a07000     	mov	r7, r0
   3fb08: e5963038     	ldr	r3, [r6, #0x38]
   3fb0c: e2856004     	add	r6, r5, #4
   3fb10: e3550000     	cmp	r5, #0
   3fb14: e0876006     	add	r6, r7, r6
   3fb18: e7873005     	str	r3, [r7, r5]
   3fb1c: ca000006     	bgt	0x3fb3c
   3fb20: e3580000     	cmp	r8, #0
   3fb24: 1a000008     	bne	0x3fb4c
   3fb28: e0873009     	add	r3, r7, r9
   3fb2c: e58472cc     	str	r7, [r4, #0x2cc]
   3fb30: e58432d4     	str	r3, [r4, #0x2d4]
   3fb34: e58462d0     	str	r6, [r4, #0x2d0]
   3fb38: eafffe08     	b	0x3f360
   3fb3c: e1a02005     	mov	r2, r5
   3fb40: e1a01008     	mov	r1, r8
   3fb44: e1a00007     	mov	r0, r7
   3fb48: ebff57a5     	bl	0x159e4    @ imm = #-0x2a16c ; memmove
   3fb4c: e1a00008     	mov	r0, r8
   3fb50: ebff58ba     	bl	0x15e40    @ imm = #-0x29d18 ; _ZdlPv
   3fb54: eafffff3     	b	0x3fb28
   3fb58: e3530000     	cmp	r3, #0
   3fb5c: 01a07003     	moveq	r7, r3
   3fb60: 01a09007     	moveq	r9, r7
   3fb64: 0affffe7     	beq	0x3fb08
   3fb68: e3e0220e     	mvn	r2, #-536870912
   3fb6c: e1530002     	cmp	r3, r2
   3fb70: 21a03002     	movhs	r3, r2
   3fb74: eaffffdf     	b	0x3faf8
   3fb78: e3a03001     	mov	r3, #1
   3fb7c: eaffffdd     	b	0x3faf8
   3fb80: e3000c8c     	movw	r0, #0xc8c
   3fb84: e3400007     	movt	r0, #0x7
   3fb88: ebff580a     	bl	0x15bb8    @ imm = #-0x29fd8 ; _ZSt20__throw_length_errorPKc
   3fb8c: e59d0078     	ldr	r0, [sp, #0x78]
   3fb90: e28d3080     	add	r3, sp, #128
   3fb94: e1500003     	cmp	r0, r3
   3fb98: 0a000000     	beq	0x3fba0
   3fb9c: ebff58a7     	bl	0x15e40    @ imm = #-0x29d64 ; _ZdlPv
   3fba0: ebff58ee     	bl	0x15f60    @ imm = #-0x29c48 ; __cxa_end_cleanup
   3fba4: eafffff8     	b	0x3fb8c
   3fba8: e59d0078     	ldr	r0, [sp, #0x78]
   3fbac: e28d3080     	add	r3, sp, #128
   3fbb0: e1500003     	cmp	r0, r3
   3fbb4: 0a000000     	beq	0x3fbbc
   3fbb8: ebff58a0     	bl	0x15e40    @ imm = #-0x29d80 ; _ZdlPv
   3fbbc: e59d0060     	ldr	r0, [sp, #0x60]
   3fbc0: e28d3068     	add	r3, sp, #104
   3fbc4: e1500003     	cmp	r0, r3
   3fbc8: 0a000000     	beq	0x3fbd0
   3fbcc: ebff589b     	bl	0x15e40    @ imm = #-0x29d94 ; _ZdlPv
   3fbd0: e59d0048     	ldr	r0, [sp, #0x48]
   3fbd4: e28d3050     	add	r3, sp, #80
   3fbd8: e1500003     	cmp	r0, r3
   3fbdc: 0a000000     	beq	0x3fbe4
   3fbe0: ebff5896     	bl	0x15e40    @ imm = #-0x29da8 ; _ZdlPv
   3fbe4: e59d0030     	ldr	r0, [sp, #0x30]
   3fbe8: e28d3038     	add	r3, sp, #56
   3fbec: e1500003     	cmp	r0, r3
   3fbf0: 0a000000     	beq	0x3fbf8
   3fbf4: ebff5891     	bl	0x15e40    @ imm = #-0x29dbc ; _ZdlPv
   3fbf8: e59d0018     	ldr	r0, [sp, #0x18]
   3fbfc: e28d3020     	add	r3, sp, #32
   3fc00: e1500003     	cmp	r0, r3
   3fc04: 1affffe4     	bne	0x3fb9c
   3fc08: eaffffe4     	b	0x3fba0
   3fc0c: eaffffea     	b	0x3fbbc
   3fc10: eaffffe9     	b	0x3fbbc
   3fc14: eaffffed     	b	0x3fbd0
   3fc18: eafffff1     	b	0x3fbe4
   3fc1c: eafffff5     	b	0x3fbf8
   3fc20: eaffffe0     	b	0x3fba8
   3fc24: eaffffd8     	b	0x3fb8c
   3fc28: eafffff2     	b	0x3fbf8
   3fc2c: eaffffd6     	b	0x3fb8c
   3fc30: eaffffd5     	b	0x3fb8c
   3fc34: eaffffd4     	b	0x3fb8c
   3fc38: eaffffd3     	b	0x3fb8c
   3fc3c: eaffffd2     	b	0x3fb8c
   3fc40: eaffffe2     	b	0x3fbd0
   3fc44: eaffffe6     	b	0x3fbe4
   3fc48: 25 00 00 00  	.word	0x00000025
   3fc4c: b9 00 00 00  	.word	0x000000b9
