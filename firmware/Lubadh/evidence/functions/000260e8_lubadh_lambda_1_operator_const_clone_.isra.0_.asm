; lubadh::{lambda()#1}::operator()() const [clone .isra.0]
; VA 0x260e8 size 2760

   260e8: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   260ec: e3a01000     	mov	r1, #0
   260f0: e304229c     	movw	r2, #0x429c
   260f4: ed2d8b04     	vpush	{d8, d9}
   260f8: e24dd034     	sub	sp, sp, #52
   260fc: e1a05000     	mov	r5, r0
   26100: ebffbf1b     	bl	0x15d74    @ imm = #-0x10394 ; memset
   26104: eddf0bf5     	vldr	d16, [pc, #980]         @ 0x264e0 ; float 5.30498947741e-313
   26108: eddf1bf6     	vldr	d17, [pc, #984]         @ 0x264e8 ; float 8.48798316386e-314
   2610c: e1a03005     	mov	r3, r5
   26110: eddf2bf6     	vldr	d18, [pc, #984]         @ 0x264f0 ; float 3.16202013338e-322
   26114: eddf3bf7     	vldr	d19, [pc, #988]         @ 0x264f8 ; float 4.24399158193e-314
   26118: e2850020     	add	r0, r5, #32
   2611c: f2c04050     	vmov.i32	q10, #0x0
   26120: e2856044     	add	r6, r5, #68
   26124: e3a01000     	mov	r1, #0
   26128: e3a02c05     	mov	r2, #1280
   2612c: f4402a8f     	vst1.32	{d18, d19}, [r0]
   26130: e1a00006     	mov	r0, r6
   26134: e1a04001     	mov	r4, r1
   26138: f4434a8d     	vst1.32	{d20, d21}, [r3]!
   2613c: e30ccccd     	movw	r12, #0xcccd
   26140: e343cdcc     	movt	r12, #0x3dcc
   26144: f4430a8f     	vst1.32	{d16, d17}, [r3]
   26148: e1a08006     	mov	r8, r6
   2614c: e1a09004     	mov	r9, r4
   26150: e5852030     	str	r2, [r5, #0x30]
   26154: e3a02e1e     	mov	r2, #480
   26158: e585c034     	str	r12, [r5, #0x34]
   2615c: ebffbf04     	bl	0x15d74    @ imm = #-0x103f0 ; memset
   26160: eddf2be6     	vldr	d18, [pc, #920]         @ 0x26500 ; float -2.00000143424
   26164: eddf3be7     	vldr	d19, [pc, #924]         @ 0x26508 ; float -3.0517599896e-05
   26168: e2853058     	add	r3, r5, #88
   2616c: eddf0be7     	vldr	d16, [pc, #924]         @ 0x26510 ; float 0.00781250183354
   26170: eddf1be8     	vldr	d17, [pc, #928]         @ 0x26518 ; float 512.00012207
   26174: e1a07004     	mov	r7, r4
   26178: f4462a8f     	vst1.32	{d18, d19}, [r6]
   2617c: f4430a8f     	vst1.32	{d16, d17}, [r3]
   26180: ea00000e     	b	0x261c0
   26184: e1a0b009     	mov	r11, r9
   26188: ed988a00     	vldr	s16, [r8]
   2618c: e1a0a007     	mov	r10, r7
   26190: ecab8a01     	vstmia	r11!, {s16}
   26194: eeb18a48     	vneg.f32	s16, s16
   26198: e154000b     	cmp	r4, r11
   2619c: 0a000025     	beq	0x26238
   261a0: e1a0900b     	mov	r9, r11
   261a4: e1a0700a     	mov	r7, r10
   261a8: eca98a01     	vstmia	r9!, {s16}
   261ac: e2853f89     	add	r3, r5, #548
   261b0: e2888004     	add	r8, r8, #4
   261b4: e1580003     	cmp	r8, r3
   261b8: e58d300c     	str	r3, [sp, #0xc]
   261bc: 0a000058     	beq	0x26324
   261c0: e1540009     	cmp	r4, r9
   261c4: 1affffee     	bne	0x26184
   261c8: e0449007     	sub	r9, r4, r7
   261cc: e1a04149     	asr	r4, r9, #2
   261d0: e374021e     	cmn	r4, #-536870911
   261d4: 0a000252     	beq	0x26b24
   261d8: e3540000     	cmp	r4, #0
   261dc: 0a00003d     	beq	0x262d8
   261e0: e1540084     	cmp	r4, r4, lsl #1
   261e4: e1a04084     	lsl	r4, r4, #1
   261e8: 83e0410e     	mvnhi	r4, #-2147483645
   261ec: 9a000031     	bls	0x262b8
   261f0: e1a00004     	mov	r0, r4
   261f4: ebffbdc4     	bl	0x1590c     @ imm = #-0x108f0 ; _Znwj
   261f8: e1a0a000     	mov	r10, r0
   261fc: e0804004     	add	r4, r0, r4
   26200: ed988a00     	vldr	s16, [r8]
   26204: e08a3009     	add	r3, r10, r9
   26208: e289b004     	add	r11, r9, #4
   2620c: e3590000     	cmp	r9, #0
   26210: e08ab00b     	add	r11, r10, r11
   26214: ed838a00     	vstr	s16, [r3]
   26218: ca00001f     	bgt	0x2629c
   2621c: e3570000     	cmp	r7, #0
   26220: 0affffdb     	beq	0x26194
   26224: e1a00007     	mov	r0, r7
   26228: ebffbf04     	bl	0x15e40    @ imm = #-0x103f0 ; _ZdlPv
   2622c: eeb18a48     	vneg.f32	s16, s16
   26230: e154000b     	cmp	r4, r11
   26234: 1affffd9     	bne	0x261a0
   26238: e044b00a     	sub	r11, r4, r10
   2623c: e1a0414b     	asr	r4, r11, #2
   26240: e374021e     	cmn	r4, #-536870911
   26244: 0a000239     	beq	0x26b30
   26248: e3540000     	cmp	r4, #0
   2624c: 0a000032     	beq	0x2631c
   26250: e1540084     	cmp	r4, r4, lsl #1
   26254: e1a04084     	lsl	r4, r4, #1
   26258: 83e0410e     	mvnhi	r4, #-2147483645
   2625c: 9a000026     	bls	0x262fc
   26260: e1a00004     	mov	r0, r4
   26264: ebffbda8     	bl	0x1590c     @ imm = #-0x10960 ; _Znwj
   26268: e1a07000     	mov	r7, r0
   2626c: e0804004     	add	r4, r0, r4
   26270: e087300b     	add	r3, r7, r11
   26274: e28b9004     	add	r9, r11, #4
   26278: e35b0000     	cmp	r11, #0
   2627c: e0879009     	add	r9, r7, r9
   26280: ed838a00     	vstr	s16, [r3]
   26284: ca000015     	bgt	0x262e0
   26288: e35a0000     	cmp	r10, #0
   2628c: 0affffc6     	beq	0x261ac
   26290: e1a0000a     	mov	r0, r10
   26294: ebffbee9     	bl	0x15e40    @ imm = #-0x1045c ; _ZdlPv
   26298: eaffffc3     	b	0x261ac
   2629c: e1a02009     	mov	r2, r9
   262a0: e1a01007     	mov	r1, r7
   262a4: e1a0000a     	mov	r0, r10
   262a8: ebffbdcd     	bl	0x159e4    @ imm = #-0x108cc ; memmove
   262ac: e1a00007     	mov	r0, r7
   262b0: ebffbee2     	bl	0x15e40    @ imm = #-0x10478 ; _ZdlPv
   262b4: eaffffdc     	b	0x2622c
   262b8: e3540000     	cmp	r4, #0
   262bc: 01a0a004     	moveq	r10, r4
   262c0: 0affffce     	beq	0x26200
   262c4: e3e0320e     	mvn	r3, #-536870912
   262c8: e1540003     	cmp	r4, r3
   262cc: 21a04003     	movhs	r4, r3
   262d0: e1a04104     	lsl	r4, r4, #2
   262d4: eaffffc5     	b	0x261f0
   262d8: e3a04004     	mov	r4, #4
   262dc: eaffffc3     	b	0x261f0
   262e0: e1a0200b     	mov	r2, r11
   262e4: e1a0100a     	mov	r1, r10
   262e8: e1a00007     	mov	r0, r7
   262ec: ebffbdbc     	bl	0x159e4    @ imm = #-0x10910 ; memmove
   262f0: e1a0000a     	mov	r0, r10
   262f4: ebffbed1     	bl	0x15e40    @ imm = #-0x104bc ; _ZdlPv
   262f8: eaffffab     	b	0x261ac
   262fc: e3540000     	cmp	r4, #0
   26300: 01a07004     	moveq	r7, r4
   26304: 0affffd9     	beq	0x26270
   26308: e3e0320e     	mvn	r3, #-536870912
   2630c: e1540003     	cmp	r4, r3
   26310: 21a04003     	movhs	r4, r3
   26314: e1a04104     	lsl	r4, r4, #2
   26318: eaffffd0     	b	0x26260
   2631c: e3a04004     	mov	r4, #4
   26320: eaffffce     	b	0x26260
   26324: e1590007     	cmp	r9, r7
   26328: 0a000039     	beq	0x26414
   2632c: e0494007     	sub	r4, r9, r7
   26330: e1a01009     	mov	r1, r9
   26334: e1a00007     	mov	r0, r7
   26338: e3a03000     	mov	r3, #0
   2633c: e1a02144     	asr	r2, r4, #2
   26340: e2878004     	add	r8, r7, #4
   26344: e16f2f12     	clz	r2, r2
   26348: e262201f     	rsb	r2, r2, #31
   2634c: e1a02082     	lsl	r2, r2, #1
   26350: eb002283     	bl	0x2ed64
   26354: e3540040     	cmp	r4, #64
   26358: ca0000ea     	bgt	0x26708
   2635c: e1580009     	cmp	r8, r9
   26360: 0a00012c     	beq	0x26818
   26364: e1a04008     	mov	r4, r8
   26368: ea000009     	b	0x26394
   2636c: e1540007     	cmp	r4, r7
   26370: 0a000003     	beq	0x26384
   26374: e0442007     	sub	r2, r4, r7
   26378: e1a01007     	mov	r1, r7
   2637c: e2870004     	add	r0, r7, #4
   26380: ebffbd97     	bl	0x159e4    @ imm = #-0x109a4 ; memmove
   26384: ed878a00     	vstr	s16, [r7]
   26388: e2844004     	add	r4, r4, #4
   2638c: e1540009     	cmp	r4, r9
   26390: 0a000120     	beq	0x26818
   26394: ed948a00     	vldr	s16, [r4]
   26398: edd77a00     	vldr	s15, [r7]
   2639c: eeb48ae7     	vcmpe.f32	s16, s15
   263a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   263a4: 4afffff0     	bmi	0x2636c
   263a8: ed547a01     	vldr	s15, [r4, #-4]
   263ac: e2443004     	sub	r3, r4, #4
   263b0: eeb48ae7     	vcmpe.f32	s16, s15
   263b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   263b8: 5a0001d2     	bpl	0x26b08
   263bc: e1a02003     	mov	r2, r3
   263c0: edc37a01     	vstr	s15, [r3, #4]
   263c4: ed737a01     	vldmdb	r3!, {s15}
   263c8: eeb48ae7     	vcmpe.f32	s16, s15
   263cc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   263d0: 4afffff9     	bmi	0x263bc
   263d4: ed828a00     	vstr	s16, [r2]
   263d8: eaffffea     	b	0x26388
   263dc: e1530009     	cmp	r3, r9
   263e0: 0a00000b     	beq	0x26414
   263e4: e2832008     	add	r2, r3, #8
   263e8: e1590002     	cmp	r9, r2
   263ec: 0a000007     	beq	0x26410
   263f0: ecf27a01     	vldmia	r2!, {s15}
   263f4: ed937a00     	vldr	s14, [r3]
   263f8: eeb47a67     	vcmp.f32	s14, s15
   263fc: eef1fa10     	vmrs	APSR_nzcv, fpscr
   26400: 1dc37a01     	vstrne	s15, [r3, #4]
   26404: 12833004     	addne	r3, r3, #4
   26408: e1590002     	cmp	r9, r2
   2640c: 1afffff7     	bne	0x263f0
   26410: e2839004     	add	r9, r3, #4
   26414: e3a02e1e     	mov	r2, #480
   26418: e3a01000     	mov	r1, #0
   2641c: e1a00006     	mov	r0, r6
   26420: ebffbe53     	bl	0x15d74    @ imm = #-0x106b4 ; memset
   26424: e1590007     	cmp	r9, r7
   26428: 0a000003     	beq	0x2643c
   2642c: e0492007     	sub	r2, r9, r7
   26430: e1a01007     	mov	r1, r7
   26434: e1a00006     	mov	r0, r6
   26438: ebffbf01     	bl	0x16044    @ imm = #-0x103fc ; memcpy
   2643c: e3570000     	cmp	r7, #0
   26440: 0a000001     	beq	0x2644c
   26444: e1a00007     	mov	r0, r7
   26448: ebffbe7c     	bl	0x15e40    @ imm = #-0x10610 ; _ZdlPv
   2644c: f2c00010     	vmov.i32	d16, #0x0
   26450: e3a00e7a     	mov	r0, #1952
   26454: e3a03000     	mov	r3, #0
   26458: e58d3018     	str	r3, [sp, #0x18]
   2645c: edcd0b04     	vstr	d16, [sp, #16]
   26460: ebffbd29     	bl	0x1590c     @ imm = #-0x10b5c ; _Znwj
   26464: e59d7010     	ldr	r7, [sp, #0x10]
   26468: e1a04000     	mov	r4, r0
   2646c: e59d2014     	ldr	r2, [sp, #0x14]
   26470: e0422007     	sub	r2, r2, r7
   26474: e3520000     	cmp	r2, #0
   26478: ca0001a4     	bgt	0x26b10
   2647c: e3570000     	cmp	r7, #0
   26480: 1a0001a4     	bne	0x26b18
   26484: f2c00010     	vmov.i32	d16, #0x0
   26488: ee814b90     	vdup.32	d17, r4
   2648c: e28d2024     	add	r2, sp, #36
   26490: e2840e7a     	add	r0, r4, #1952
   26494: e28d1020     	add	r1, sp, #32
   26498: e58d0018     	str	r0, [sp, #0x18]
   2649c: e28d0010     	add	r0, sp, #16
   264a0: e3a03000     	mov	r3, #0
   264a4: e34c3080     	movt	r3, #0xc080
   264a8: f442078f     	vst1.32	{d16}, [r2]
   264ac: edcd1b04     	vstr	d17, [sp, #16]
   264b0: e58d3020     	str	r3, [sp, #0x20]
   264b4: e3a03000     	mov	r3, #0
   264b8: e58d302c     	str	r3, [sp, #0x2c]
   264bc: ebffe2e0     	bl	0x1f044
   264c0: e59d8014     	ldr	r8, [sp, #0x14]
   264c4: eef18a00     	vmov.f32	s17, #4.000000e+00
   264c8: e59d4018     	ldr	r4, [sp, #0x18]
   264cc: ed9f9a13     	vldr	s18, [pc, #76]          @ 0x26520 ; float 4095
   264d0: eddf9a13     	vldr	s19, [pc, #76]          @ 0x26524 ; float 0
   264d4: e58d6004     	str	r6, [sp, #0x4]
   264d8: ea00001e     	b	0x26558
   264dc: e320f000     	nop
   264e0: 00 00 00 00  	.word	0x00000000
   264e4: 19 00 00 00  	.word	0x00000019
   264e8: 00 00 00 00  	.word	0x00000000
   264ec: 04 00 00 00  	.word	0x00000004
   264f0: 40 00 00 00  	.word	0x00000040
   264f4: 00 00 00 00  	.word	0x00000000
   264f8: 00 00 00 00  	.word	0x00000000
   264fc: 02 00 00 00  	.word	0x00000002
   26500: 00 00 80 c0  	.word	0xc0800000
   26504: 00 00 00 c0  	.word	0xc0000000
   26508: 00 00 80 bf  	.word	0xbf800000
   2650c: 00 00 00 bf  	.word	0xbf000000
   26510: 00 00 00 3f  	.word	0x3f000000
   26514: 00 00 80 3f  	.word	0x3f800000
   26518: 00 00 00 40  	.word	0x40000000
   2651c: 00 00 80 40  	.word	0x40800000
   26520: 00 f0 7f 45  	.word	0x457ff000
   26524: 00 00 00 00  	.word	0x00000000
   26528: 38 36 07 00  	.word	0x00073638
   2652c: 48 36 07 00  	.word	0x00073648
   26530: e59d300c     	ldr	r3, [sp, #0xc]
   26534: e2888010     	add	r8, r8, #16
   26538: e59d2004     	ldr	r2, [sp, #0x4]
   2653c: ed088a04     	vstr	s16, [r8, #-16]
   26540: e508600c     	str	r6, [r8, #-0xc]
   26544: e1530002     	cmp	r3, r2
   26548: ed487a02     	vstr	s15, [r8, #-8]
   2654c: e5087004     	str	r7, [r8, #-0x4]
   26550: e58d8014     	str	r8, [sp, #0x14]
   26554: 0a000037     	beq	0x26638
   26558: e59d3004     	ldr	r3, [sp, #0x4]
   2655c: eeb47a00     	vmov.f32	s14, #1.250000e-01
   26560: e3002fff     	movw	r2, #0xfff
   26564: ecb38a01     	vldmia	r3!, {s16}
   26568: ee787a28     	vadd.f32	s15, s16, s17
   2656c: e58d3004     	str	r3, [sp, #0x4]
   26570: ee677a87     	vmul.f32	s15, s15, s14
   26574: eeb07a69     	vmov.f32	s14, s19
   26578: eea77a89     	vfma.f32	s14, s15, s18
   2657c: eefd7ac7     	vcvt.s32.f32	s15, s14
   26580: ee173a90     	vmov	r3, s15
   26584: edcd7a02     	vstr	s15, [sp, #8]
   26588: e2436032     	sub	r6, r3, #50
   2658c: e2837032     	add	r7, r3, #50
   26590: e1560002     	cmp	r6, r2
   26594: a1a06002     	movge	r6, r2
   26598: e1570002     	cmp	r7, r2
   2659c: a1a07002     	movge	r7, r2
   265a0: e1580004     	cmp	r8, r4
   265a4: e1c66fc6     	bic	r6, r6, r6, asr #31
   265a8: e1c77fc7     	bic	r7, r7, r7, asr #31
   265ac: 1affffdf     	bne	0x26530
   265b0: e59da010     	ldr	r10, [sp, #0x10]
   265b4: e048900a     	sub	r9, r8, r10
   265b8: e1a04249     	asr	r4, r9, #4
   265bc: e374037e     	cmn	r4, #-134217727
   265c0: 0a00015d     	beq	0x26b3c
   265c4: e3540000     	cmp	r4, #0
   265c8: 0a00007c     	beq	0x267c0
   265cc: e1540084     	cmp	r4, r4, lsl #1
   265d0: e1a04084     	lsl	r4, r4, #1
   265d4: 83e0413e     	mvnhi	r4, #-2147483633
   265d8: 9a000070     	bls	0x267a0
   265dc: e1a00004     	mov	r0, r4
   265e0: ebffbcc9     	bl	0x1590c     @ imm = #-0x10cdc ; _Znwj
   265e4: e1a0b000     	mov	r11, r0
   265e8: e0804004     	add	r4, r0, r4
   265ec: e08b2009     	add	r2, r11, r9
   265f0: e2893010     	add	r3, r9, #16
   265f4: e08b8003     	add	r8, r11, r3
   265f8: e59d3008     	ldr	r3, [sp, #0x8]
   265fc: e3590000     	cmp	r9, #0
   26600: e5826004     	str	r6, [r2, #0x4]
   26604: e5823008     	str	r3, [r2, #0x8]
   26608: e582700c     	str	r7, [r2, #0xc]
   2660c: ed828a00     	vstr	s16, [r2]
   26610: ca00005b     	bgt	0x26784
   26614: e35a0000     	cmp	r10, #0
   26618: 1a00005d     	bne	0x26794
   2661c: e59d300c     	ldr	r3, [sp, #0xc]
   26620: e59d2004     	ldr	r2, [sp, #0x4]
   26624: e58db010     	str	r11, [sp, #0x10]
   26628: e1530002     	cmp	r3, r2
   2662c: e58d8014     	str	r8, [sp, #0x14]
   26630: e58d4018     	str	r4, [sp, #0x18]
   26634: 1affffc7     	bne	0x26558
   26638: e51f3118     	ldr	r3, [pc, #-0x118]       @ 0x26528
   2663c: e28dc020     	add	r12, sp, #32
   26640: e893000f     	ldm	r3, {r0, r1, r2, r3}
   26644: e88c000f     	stm	r12, {r0, r1, r2, r3}
   26648: e28d0010     	add	r0, sp, #16
   2664c: e1a0100c     	mov	r1, r12
   26650: ebffe27b     	bl	0x1f044
   26654: e59d4010     	ldr	r4, [sp, #0x10]
   26658: e59d6014     	ldr	r6, [sp, #0x14]
   2665c: e1540006     	cmp	r4, r6
   26660: 0a000079     	beq	0x2684c
   26664: e0467004     	sub	r7, r6, r4
   26668: e1a01006     	mov	r1, r6
   2666c: e1a00004     	mov	r0, r4
   26670: e3a03000     	mov	r3, #0
   26674: e1a02247     	asr	r2, r7, #4
   26678: e16f2f12     	clz	r2, r2
   2667c: e262201f     	rsb	r2, r2, #31
   26680: e1a02082     	lsl	r2, r2, #1
   26684: ebffe2de     	bl	0x1f204
   26688: e3570c01     	cmp	r7, #256
   2668c: da0000e1     	ble	0x26a18
   26690: e2847c01     	add	r7, r4, #256
   26694: e1a00004     	mov	r0, r4
   26698: e1a01007     	mov	r1, r7
   2669c: ebffe2a4     	bl	0x1f134
   266a0: e1a0e007     	mov	lr, r7
   266a4: e156000e     	cmp	r6, lr
   266a8: e1a0700e     	mov	r7, lr
   266ac: 0a0000dc     	beq	0x26a24
   266b0: e28dc020     	add	r12, sp, #32
   266b4: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   266b8: e88c000f     	stm	r12, {r0, r1, r2, r3}
   266bc: e59e8008     	ldr	r8, [lr, #0x8]
   266c0: e51e3008     	ldr	r3, [lr, #-0x8]
   266c4: e1580003     	cmp	r8, r3
   266c8: aa000008     	bge	0x266f0
   266cc: e24ec010     	sub	r12, lr, #16
   266d0: e28c4010     	add	r4, r12, #16
   266d4: e1a0700c     	mov	r7, r12
   266d8: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   266dc: e24cc010     	sub	r12, r12, #16
   266e0: e884000f     	stm	r4, {r0, r1, r2, r3}
   266e4: e59c3008     	ldr	r3, [r12, #0x8]
   266e8: e1530008     	cmp	r3, r8
   266ec: cafffff7     	bgt	0x266d0
   266f0: e28d3020     	add	r3, sp, #32
   266f4: e58d8028     	str	r8, [sp, #0x28]
   266f8: e28ee010     	add	lr, lr, #16
   266fc: e893000f     	ldm	r3, {r0, r1, r2, r3}
   26700: e887000f     	stm	r7, {r0, r1, r2, r3}
   26704: eaffffe6     	b	0x266a4
   26708: e287a040     	add	r10, r7, #64
   2670c: e1a04008     	mov	r4, r8
   26710: ea000009     	b	0x2673c
   26714: e1540007     	cmp	r4, r7
   26718: 0a000003     	beq	0x2672c
   2671c: e0442007     	sub	r2, r4, r7
   26720: e1a01007     	mov	r1, r7
   26724: e2870004     	add	r0, r7, #4
   26728: ebffbcad     	bl	0x159e4    @ imm = #-0x10d4c ; memmove
   2672c: ed878a00     	vstr	s16, [r7]
   26730: e2844004     	add	r4, r4, #4
   26734: e15a0004     	cmp	r10, r4
   26738: 0a000022     	beq	0x267c8
   2673c: ed948a00     	vldr	s16, [r4]
   26740: edd77a00     	vldr	s15, [r7]
   26744: eeb48ae7     	vcmpe.f32	s16, s15
   26748: eef1fa10     	vmrs	APSR_nzcv, fpscr
   2674c: 4afffff0     	bmi	0x26714
   26750: ed547a01     	vldr	s15, [r4, #-4]
   26754: e2443004     	sub	r3, r4, #4
   26758: eeb48ae7     	vcmpe.f32	s16, s15
   2675c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   26760: 5a0000e6     	bpl	0x26b00
   26764: e1a02003     	mov	r2, r3
   26768: edc37a01     	vstr	s15, [r3, #4]
   2676c: ed737a01     	vldmdb	r3!, {s15}
   26770: eeb48ae7     	vcmpe.f32	s16, s15
   26774: eef1fa10     	vmrs	APSR_nzcv, fpscr
   26778: 4afffff9     	bmi	0x26764
   2677c: ed828a00     	vstr	s16, [r2]
   26780: eaffffea     	b	0x26730
   26784: e1a02009     	mov	r2, r9
   26788: e1a0100a     	mov	r1, r10
   2678c: e1a0000b     	mov	r0, r11
   26790: ebffbc93     	bl	0x159e4    @ imm = #-0x10db4 ; memmove
   26794: e1a0000a     	mov	r0, r10
   26798: ebffbda8     	bl	0x15e40    @ imm = #-0x10960 ; _ZdlPv
   2679c: eaffff9e     	b	0x2661c
   267a0: e3540000     	cmp	r4, #0
   267a4: 01a0b004     	moveq	r11, r4
   267a8: 0affff8f     	beq	0x265ec
   267ac: e3e0333e     	mvn	r3, #-134217728
   267b0: e1540003     	cmp	r4, r3
   267b4: 21a04003     	movhs	r4, r3
   267b8: e1a04204     	lsl	r4, r4, #4
   267bc: eaffff86     	b	0x265dc
   267c0: e3a04010     	mov	r4, #16
   267c4: eaffff84     	b	0x265dc
   267c8: e15a0009     	cmp	r10, r9
   267cc: 0a000011     	beq	0x26818
   267d0: e1a0300a     	mov	r3, r10
   267d4: e287103c     	add	r1, r7, #60
   267d8: e1a00003     	mov	r0, r3
   267dc: e1a02001     	mov	r2, r1
   267e0: ecb37a01     	vldmia	r3!, {s14}
   267e4: ecf17a01     	vldmia	r1!, {s15}
   267e8: eeb47ae7     	vcmpe.f32	s14, s15
   267ec: eef1fa10     	vmrs	APSR_nzcv, fpscr
   267f0: 5a000005     	bpl	0x2680c
   267f4: e1a00002     	mov	r0, r2
   267f8: edc27a01     	vstr	s15, [r2, #4]
   267fc: ed727a01     	vldmdb	r2!, {s15}
   26800: eeb47ae7     	vcmpe.f32	s14, s15
   26804: eef1fa10     	vmrs	APSR_nzcv, fpscr
   26808: 4afffff9     	bmi	0x267f4
   2680c: e1590003     	cmp	r9, r3
   26810: ed807a00     	vstr	s14, [r0]
   26814: 1affffef     	bne	0x267d8
   26818: e1a02008     	mov	r2, r8
   2681c: e1a01007     	mov	r1, r7
   26820: ea000005     	b	0x2683c
   26824: edd27a00     	vldr	s15, [r2]
   26828: e2822004     	add	r2, r2, #4
   2682c: ecb17a01     	vldmia	r1!, {s14}
   26830: eeb47a67     	vcmp.f32	s14, s15
   26834: eef1fa10     	vmrs	APSR_nzcv, fpscr
   26838: 0afffee7     	beq	0x263dc
   2683c: e1a03001     	mov	r3, r1
   26840: e1520009     	cmp	r2, r9
   26844: 1afffff6     	bne	0x26824
   26848: eafffef1     	b	0x26414
   2684c: e0466004     	sub	r6, r6, r4
   26850: e1a03246     	asr	r3, r6, #4
   26854: e3530001     	cmp	r3, #1
   26858: 0a00004c     	beq	0x26990
   2685c: e2446010     	sub	r6, r4, #16
   26860: e1a01004     	mov	r1, r4
   26864: e0866203     	add	r6, r6, r3, lsl #4
   26868: e1a03004     	mov	r3, r4
   2686c: e593000c     	ldr	r0, [r3, #0xc]
   26870: e5932014     	ldr	r2, [r3, #0x14]
   26874: e1500002     	cmp	r0, r2
   26878: da000008     	ble	0x268a0
   2687c: e593c008     	ldr	r12, [r3, #0x8]
   26880: e5930018     	ldr	r0, [r3, #0x18]
   26884: e040200c     	sub	r2, r0, r12
   26888: e0822fa2     	add	r2, r2, r2, lsr #31
   2688c: e1a020c2     	asr	r2, r2, #1
   26890: e082c00c     	add	r12, r2, r12
   26894: e0402002     	sub	r2, r0, r2
   26898: e583c00c     	str	r12, [r3, #0xc]
   2689c: e5832014     	str	r2, [r3, #0x14]
   268a0: e2833010     	add	r3, r3, #16
   268a4: e1530006     	cmp	r3, r6
   268a8: 1affffef     	bne	0x2686c
   268ac: e5913004     	ldr	r3, [r1, #0x4]
   268b0: e591200c     	ldr	r2, [r1, #0xc]
   268b4: e1530002     	cmp	r3, r2
   268b8: aa00001c     	bge	0x26930
   268bc: e042e003     	sub	lr, r2, r3
   268c0: e591c000     	ldr	r12, [r1]
   268c4: e24e0001     	sub	r0, lr, #1
   268c8: e3500002     	cmp	r0, #2
   268cc: 9a00000c     	bls	0x26904
   268d0: e2830089     	add	r0, r3, #137
   268d4: eea0cb90     	vdup.32	q8, r12
   268d8: e1a0812e     	lsr	r8, lr, #2
   268dc: e3a07000     	mov	r7, #0
   268e0: e0850100     	add	r0, r5, r0, lsl #2
   268e4: e2877001     	add	r7, r7, #1
   268e8: f4400a8d     	vst1.32	{d16, d17}, [r0]!
   268ec: e1570008     	cmp	r7, r8
   268f0: 1afffffb     	bne	0x268e4
   268f4: e3ce0003     	bic	r0, lr, #3
   268f8: e0833000     	add	r3, r3, r0
   268fc: e15e0000     	cmp	lr, r0
   26900: 0a00000a     	beq	0x26930
   26904: e085e103     	add	lr, r5, r3, lsl #2
   26908: e2830001     	add	r0, r3, #1
   2690c: e1520000     	cmp	r2, r0
   26910: e58ec224     	str	r12, [lr, #0x224]
   26914: da000005     	ble	0x26930
   26918: e0850100     	add	r0, r5, r0, lsl #2
   2691c: e2833002     	add	r3, r3, #2
   26920: e1520003     	cmp	r2, r3
   26924: e580c224     	str	r12, [r0, #0x224]
   26928: c0853103     	addgt	r3, r5, r3, lsl #2
   2692c: c583c224     	strgt	r12, [r3, #0x224]
   26930: e5913014     	ldr	r3, [r1, #0x14]
   26934: e1530002     	cmp	r3, r2
   26938: da000011     	ble	0x26984
   2693c: ed917a00     	vldr	s14, [r1]
   26940: e0433002     	sub	r3, r3, r2
   26944: edd17a04     	vldr	s15, [r1, #16]
   26948: ee063a90     	vmov	s13, r3
   2694c: e2822089     	add	r2, r2, #137
   26950: e3a00000     	mov	r0, #0
   26954: eef86ae6     	vcvt.f32.s32	s13, s13
   26958: ee777ac7     	vsub.f32	s15, s15, s14
   2695c: e0852102     	add	r2, r5, r2, lsl #2
   26960: ee060a10     	vmov	s12, r0
   26964: e2800001     	add	r0, r0, #1
   26968: e1500003     	cmp	r0, r3
   2696c: eef85ac6     	vcvt.f32.s32	s11, s12
   26970: ee856aa6     	vdiv.f32	s12, s11, s13
   26974: eef05a47     	vmov.f32	s11, s14
   26978: eee65a27     	vfma.f32	s11, s12, s15
   2697c: ece25a01     	vstmia	r2!, {s11}
   26980: 1afffff6     	bne	0x26960
   26984: e2811010     	add	r1, r1, #16
   26988: e1510006     	cmp	r1, r6
   2698c: 1affffc6     	bne	0x268ac
   26990: e3540000     	cmp	r4, #0
   26994: 0a000001     	beq	0x269a0
   26998: e1a00004     	mov	r0, r4
   2699c: ebffbd27     	bl	0x15e40    @ imm = #-0x10b64 ; _ZdlPv
   269a0: e51fe47c     	ldr	lr, [pc, #-0x47c]       @ 0x2652c
   269a4: e2854901     	add	r4, r5, #16384
   269a8: e284cf8a     	add	r12, r4, #552
   269ac: e2849f9b     	add	r9, r4, #620
   269b0: eddf4b72     	vldr	d20, [pc, #456]         @ 0x26b80 ; float 3.05175853327e-05
   269b4: eddf5b73     	vldr	d21, [pc, #460]         @ 0x26b88 ; float 4.65661395381e-10
   269b8: e2848f9f     	add	r8, r4, #636
   269bc: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   269c0: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   269c4: e2847fa3     	add	r7, r4, #652
   269c8: eddf2b70     	vldr	d18, [pc, #448]         @ 0x26b90 ; float 1.3411048233e-08
   269cc: eddf3b71     	vldr	d19, [pc, #452]         @ 0x26b98 ; float 1.34110464955e-08
   269d0: e3a06003     	mov	r6, #3
   269d4: eddf0b71     	vldr	d16, [pc, #452]         @ 0x26ba0 ; float 3.43322837737e-06
   269d8: eddf1b72     	vldr	d17, [pc, #456]         @ 0x26ba8 ; float 0.000878906470662
   269dc: e5846224     	str	r6, [r4, #0x224]
   269e0: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   269e4: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   269e8: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   269ec: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   269f0: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   269f4: e88c000f     	stm	r12, {r0, r1, r2, r3}
   269f8: e1a00005     	mov	r0, r5
   269fc: e5846268     	str	r6, [r4, #0x268]
   26a00: f4494a8f     	vst1.32	{d20, d21}, [r9]
   26a04: f4482a8f     	vst1.32	{d18, d19}, [r8]
   26a08: f4470a8f     	vst1.32	{d16, d17}, [r7]
   26a0c: e28dd034     	add	sp, sp, #52
   26a10: ecbd8b04     	vpop	{d8, d9}
   26a14: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   26a18: e1a01006     	mov	r1, r6
   26a1c: e1a00004     	mov	r0, r4
   26a20: ebffe1c3     	bl	0x1f134
   26a24: e59d4010     	ldr	r4, [sp, #0x10]
   26a28: e59d6014     	ldr	r6, [sp, #0x14]
   26a2c: e1540006     	cmp	r4, r6
   26a30: 0affff85     	beq	0x2684c
   26a34: e2843010     	add	r3, r4, #16
   26a38: e1560003     	cmp	r6, r3
   26a3c: 0affff82     	beq	0x2684c
   26a40: ed947a00     	vldr	s14, [r4]
   26a44: e1a03004     	mov	r3, r4
   26a48: edd37a04     	vldr	s15, [r3, #16]
   26a4c: e1a07003     	mov	r7, r3
   26a50: eeb47a67     	vcmp.f32	s14, s15
   26a54: eef1fa10     	vmrs	APSR_nzcv, fpscr
   26a58: 1a000003     	bne	0x26a6c
   26a5c: e5931008     	ldr	r1, [r3, #0x8]
   26a60: e5932018     	ldr	r2, [r3, #0x18]
   26a64: e1510002     	cmp	r1, r2
   26a68: 0a000005     	beq	0x26a84
   26a6c: e2832020     	add	r2, r3, #32
   26a70: e2833010     	add	r3, r3, #16
   26a74: e1560002     	cmp	r6, r2
   26a78: 0affff73     	beq	0x2684c
   26a7c: eeb07a67     	vmov.f32	s14, s15
   26a80: eafffff0     	b	0x26a48
   26a84: e1560003     	cmp	r6, r3
   26a88: 0affff6f     	beq	0x2684c
   26a8c: e2833020     	add	r3, r3, #32
   26a90: e1560003     	cmp	r6, r3
   26a94: 0a000010     	beq	0x26adc
   26a98: e287c030     	add	r12, r7, #48
   26a9c: ed977a00     	vldr	s14, [r7]
   26aa0: ed5c7a04     	vldr	s15, [r12, #-16]
   26aa4: eeb47a67     	vcmp.f32	s14, s15
   26aa8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   26aac: 1a000003     	bne	0x26ac0
   26ab0: e5972008     	ldr	r2, [r7, #0x8]
   26ab4: e51c3008     	ldr	r3, [r12, #-0x8]
   26ab8: e1520003     	cmp	r2, r3
   26abc: 0a000003     	beq	0x26ad0
   26ac0: e287e010     	add	lr, r7, #16
   26ac4: e91c000f     	ldmdb	r12, {r0, r1, r2, r3}
   26ac8: e1a0700e     	mov	r7, lr
   26acc: e88e000f     	stm	lr, {r0, r1, r2, r3}
   26ad0: e156000c     	cmp	r6, r12
   26ad4: e28cc010     	add	r12, r12, #16
   26ad8: 1affffef     	bne	0x26a9c
   26adc: e2877010     	add	r7, r7, #16
   26ae0: e1560007     	cmp	r6, r7
   26ae4: 11a06007     	movne	r6, r7
   26ae8: 158d7014     	strne	r7, [sp, #0x14]
   26aec: e0466004     	sub	r6, r6, r4
   26af0: e1a03246     	asr	r3, r6, #4
   26af4: e3530001     	cmp	r3, #1
   26af8: 1affff57     	bne	0x2685c
   26afc: eaffffa5     	b	0x26998
   26b00: e1a02004     	mov	r2, r4
   26b04: eaffff1c     	b	0x2677c
   26b08: e1a02004     	mov	r2, r4
   26b0c: eafffe30     	b	0x263d4
   26b10: e1a01007     	mov	r1, r7
   26b14: ebffbbb2     	bl	0x159e4    @ imm = #-0x11138 ; memmove
   26b18: e1a00007     	mov	r0, r7
   26b1c: ebffbcc7     	bl	0x15e40    @ imm = #-0x10ce4 ; _ZdlPv
   26b20: eafffe57     	b	0x26484
   26b24: e3000c8c     	movw	r0, #0xc8c
   26b28: e3400007     	movt	r0, #0x7
   26b2c: ebffbc21     	bl	0x15bb8    @ imm = #-0x10f7c ; _ZSt20__throw_length_errorPKc
   26b30: e3000c8c     	movw	r0, #0xc8c
   26b34: e3400007     	movt	r0, #0x7
   26b38: ebffbc1e     	bl	0x15bb8    @ imm = #-0x10f88 ; _ZSt20__throw_length_errorPKc
   26b3c: e3000c8c     	movw	r0, #0xc8c
   26b40: e3400007     	movt	r0, #0x7
   26b44: ebffbc1b     	bl	0x15bb8    @ imm = #-0x10f94 ; _ZSt20__throw_length_errorPKc
   26b48: e59d0010     	ldr	r0, [sp, #0x10]
   26b4c: e3500000     	cmp	r0, #0
   26b50: 0a000000     	beq	0x26b58
   26b54: ebffbcb9     	bl	0x15e40    @ imm = #-0x10d1c ; _ZdlPv
   26b58: ebffbd00     	bl	0x15f60    @ imm = #-0x10c00 ; __cxa_end_cleanup
   26b5c: e1a0a007     	mov	r10, r7
   26b60: e35a0000     	cmp	r10, #0
   26b64: 0afffffb     	beq	0x26b58
   26b68: e1a0000a     	mov	r0, r10
   26b6c: ebffbcb3     	bl	0x15e40    @ imm = #-0x10d34 ; _ZdlPv
   26b70: eafffff8     	b	0x26b58
   26b74: eafffff8     	b	0x26b5c
   26b78: eafffff8     	b	0x26b60
   26b7c: e320f000     	nop
   26b80: 66 66 66 3f  	.word	0x3f666666
   26b84: 00 00 00 3f  	.word	0x3f000000
   26b88: cd cc 4c 3e  	.word	0x3e4ccccd
   26b8c: 00 00 00 3e  	.word	0x3e000000
   26b90: 9a 99 99 3e  	.word	0x3e99999a
   26b94: cd cc 4c 3e  	.word	0x3e4ccccd
   26b98: 00 00 00 00  	.word	0x00000000
   26b9c: cd cc 4c 3e  	.word	0x3e4ccccd
   26ba0: 00 00 c8 42  	.word	0x42c80000
   26ba4: cd cc cc 3e  	.word	0x3ecccccd
   26ba8: 00 40 1c 46  	.word	0x461c4000
   26bac: cd cc 4c 3f  	.word	0x3f4ccccd
