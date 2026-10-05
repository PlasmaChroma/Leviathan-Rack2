; lubadh::{lambda()#1}::operator()() const [clone .isra.0]
; VA 0x1a49c size 2756

   1a49c: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   1a4a0: e3a01000     	mov	r1, #0
   1a4a4: e304229c     	movw	r2, #0x429c
   1a4a8: ed2d8b04     	vpush	{d8, d9}
   1a4ac: e24dd034     	sub	sp, sp, #52
   1a4b0: e1a05000     	mov	r5, r0
   1a4b4: ebffee2e     	bl	0x15d74    @ imm = #-0x4748 ; memset
   1a4b8: eddf0bf4     	vldr	d16, [pc, #976]         @ 0x1a890 ; float 5.30498947741e-313
   1a4bc: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x1a898 ; float 8.48798316386e-314
   1a4c0: e1a03005     	mov	r3, r5
   1a4c4: eddf2bf5     	vldr	d18, [pc, #980]         @ 0x1a8a0 ; float 3.16202013338e-322
   1a4c8: eddf3bf6     	vldr	d19, [pc, #984]         @ 0x1a8a8 ; float 4.24399158193e-314
   1a4cc: e2850020     	add	r0, r5, #32
   1a4d0: f2c04050     	vmov.i32	q10, #0x0
   1a4d4: e2856044     	add	r6, r5, #68
   1a4d8: e3a01000     	mov	r1, #0
   1a4dc: e3a02c05     	mov	r2, #1280
   1a4e0: f4402a8f     	vst1.32	{d18, d19}, [r0]
   1a4e4: e1a00006     	mov	r0, r6
   1a4e8: e1a04001     	mov	r4, r1
   1a4ec: f4434a8d     	vst1.32	{d20, d21}, [r3]!
   1a4f0: e30ccccd     	movw	r12, #0xcccd
   1a4f4: e343cdcc     	movt	r12, #0x3dcc
   1a4f8: f4430a8f     	vst1.32	{d16, d17}, [r3]
   1a4fc: e1a08006     	mov	r8, r6
   1a500: e1a09004     	mov	r9, r4
   1a504: e5852030     	str	r2, [r5, #0x30]
   1a508: e3a02e1e     	mov	r2, #480
   1a50c: e585c034     	str	r12, [r5, #0x34]
   1a510: ebffee17     	bl	0x15d74    @ imm = #-0x47a4 ; memset
   1a514: eddf2be5     	vldr	d18, [pc, #916]         @ 0x1a8b0 ; float -2.00000143424
   1a518: eddf3be6     	vldr	d19, [pc, #920]         @ 0x1a8b8 ; float -3.0517599896e-05
   1a51c: e2853058     	add	r3, r5, #88
   1a520: eddf0be6     	vldr	d16, [pc, #920]         @ 0x1a8c0 ; float 0.00781250183354
   1a524: eddf1be7     	vldr	d17, [pc, #924]         @ 0x1a8c8 ; float 512.00012207
   1a528: e1a07004     	mov	r7, r4
   1a52c: f4462a8f     	vst1.32	{d18, d19}, [r6]
   1a530: f4430a8f     	vst1.32	{d16, d17}, [r3]
   1a534: ea00000e     	b	0x1a574
   1a538: e1a0b009     	mov	r11, r9
   1a53c: ed988a00     	vldr	s16, [r8]
   1a540: e1a0a007     	mov	r10, r7
   1a544: ecab8a01     	vstmia	r11!, {s16}
   1a548: eeb18a48     	vneg.f32	s16, s16
   1a54c: e154000b     	cmp	r4, r11
   1a550: 0a000025     	beq	0x1a5ec
   1a554: e1a0900b     	mov	r9, r11
   1a558: e1a0700a     	mov	r7, r10
   1a55c: eca98a01     	vstmia	r9!, {s16}
   1a560: e2853f89     	add	r3, r5, #548
   1a564: e2888004     	add	r8, r8, #4
   1a568: e1580003     	cmp	r8, r3
   1a56c: e58d300c     	str	r3, [sp, #0xc]
   1a570: 0a000058     	beq	0x1a6d8
   1a574: e1540009     	cmp	r4, r9
   1a578: 1affffee     	bne	0x1a538
   1a57c: e0449007     	sub	r9, r4, r7
   1a580: e1a04149     	asr	r4, r9, #2
   1a584: e374021e     	cmn	r4, #-536870911
   1a588: 0a000251     	beq	0x1aed4
   1a58c: e3540000     	cmp	r4, #0
   1a590: 0a00003d     	beq	0x1a68c
   1a594: e1540084     	cmp	r4, r4, lsl #1
   1a598: e1a04084     	lsl	r4, r4, #1
   1a59c: 83e0410e     	mvnhi	r4, #-2147483645
   1a5a0: 9a000031     	bls	0x1a66c
   1a5a4: e1a00004     	mov	r0, r4
   1a5a8: ebffecd7     	bl	0x1590c     @ imm = #-0x4ca4 ; _Znwj
   1a5ac: e1a0a000     	mov	r10, r0
   1a5b0: e0804004     	add	r4, r0, r4
   1a5b4: ed988a00     	vldr	s16, [r8]
   1a5b8: e08a3009     	add	r3, r10, r9
   1a5bc: e289b004     	add	r11, r9, #4
   1a5c0: e3590000     	cmp	r9, #0
   1a5c4: e08ab00b     	add	r11, r10, r11
   1a5c8: ed838a00     	vstr	s16, [r3]
   1a5cc: ca00001f     	bgt	0x1a650
   1a5d0: e3570000     	cmp	r7, #0
   1a5d4: 0affffdb     	beq	0x1a548
   1a5d8: e1a00007     	mov	r0, r7
   1a5dc: ebffee17     	bl	0x15e40    @ imm = #-0x47a4 ; _ZdlPv
   1a5e0: eeb18a48     	vneg.f32	s16, s16
   1a5e4: e154000b     	cmp	r4, r11
   1a5e8: 1affffd9     	bne	0x1a554
   1a5ec: e044b00a     	sub	r11, r4, r10
   1a5f0: e1a0414b     	asr	r4, r11, #2
   1a5f4: e374021e     	cmn	r4, #-536870911
   1a5f8: 0a000238     	beq	0x1aee0
   1a5fc: e3540000     	cmp	r4, #0
   1a600: 0a000032     	beq	0x1a6d0
   1a604: e1540084     	cmp	r4, r4, lsl #1
   1a608: e1a04084     	lsl	r4, r4, #1
   1a60c: 83e0410e     	mvnhi	r4, #-2147483645
   1a610: 9a000026     	bls	0x1a6b0
   1a614: e1a00004     	mov	r0, r4
   1a618: ebffecbb     	bl	0x1590c     @ imm = #-0x4d14 ; _Znwj
   1a61c: e1a07000     	mov	r7, r0
   1a620: e0804004     	add	r4, r0, r4
   1a624: e087300b     	add	r3, r7, r11
   1a628: e28b9004     	add	r9, r11, #4
   1a62c: e35b0000     	cmp	r11, #0
   1a630: e0879009     	add	r9, r7, r9
   1a634: ed838a00     	vstr	s16, [r3]
   1a638: ca000015     	bgt	0x1a694
   1a63c: e35a0000     	cmp	r10, #0
   1a640: 0affffc6     	beq	0x1a560
   1a644: e1a0000a     	mov	r0, r10
   1a648: ebffedfc     	bl	0x15e40    @ imm = #-0x4810 ; _ZdlPv
   1a64c: eaffffc3     	b	0x1a560
   1a650: e1a02009     	mov	r2, r9
   1a654: e1a01007     	mov	r1, r7
   1a658: e1a0000a     	mov	r0, r10
   1a65c: ebffece0     	bl	0x159e4    @ imm = #-0x4c80 ; memmove
   1a660: e1a00007     	mov	r0, r7
   1a664: ebffedf5     	bl	0x15e40    @ imm = #-0x482c ; _ZdlPv
   1a668: eaffffdc     	b	0x1a5e0
   1a66c: e3540000     	cmp	r4, #0
   1a670: 01a0a004     	moveq	r10, r4
   1a674: 0affffce     	beq	0x1a5b4
   1a678: e3e0320e     	mvn	r3, #-536870912
   1a67c: e1540003     	cmp	r4, r3
   1a680: 21a04003     	movhs	r4, r3
   1a684: e1a04104     	lsl	r4, r4, #2
   1a688: eaffffc5     	b	0x1a5a4
   1a68c: e3a04004     	mov	r4, #4
   1a690: eaffffc3     	b	0x1a5a4
   1a694: e1a0200b     	mov	r2, r11
   1a698: e1a0100a     	mov	r1, r10
   1a69c: e1a00007     	mov	r0, r7
   1a6a0: ebffeccf     	bl	0x159e4    @ imm = #-0x4cc4 ; memmove
   1a6a4: e1a0000a     	mov	r0, r10
   1a6a8: ebffede4     	bl	0x15e40    @ imm = #-0x4870 ; _ZdlPv
   1a6ac: eaffffab     	b	0x1a560
   1a6b0: e3540000     	cmp	r4, #0
   1a6b4: 01a07004     	moveq	r7, r4
   1a6b8: 0affffd9     	beq	0x1a624
   1a6bc: e3e0320e     	mvn	r3, #-536870912
   1a6c0: e1540003     	cmp	r4, r3
   1a6c4: 21a04003     	movhs	r4, r3
   1a6c8: e1a04104     	lsl	r4, r4, #2
   1a6cc: eaffffd0     	b	0x1a614
   1a6d0: e3a04004     	mov	r4, #4
   1a6d4: eaffffce     	b	0x1a614
   1a6d8: e1590007     	cmp	r9, r7
   1a6dc: 0a000039     	beq	0x1a7c8
   1a6e0: e0494007     	sub	r4, r9, r7
   1a6e4: e1a01009     	mov	r1, r9
   1a6e8: e1a00007     	mov	r0, r7
   1a6ec: e3a03000     	mov	r3, #0
   1a6f0: e1a02144     	asr	r2, r4, #2
   1a6f4: e2878004     	add	r8, r7, #4
   1a6f8: e16f2f12     	clz	r2, r2
   1a6fc: e262201f     	rsb	r2, r2, #31
   1a700: e1a02082     	lsl	r2, r2, #1
   1a704: eb005196     	bl	0x2ed64
   1a708: e3540040     	cmp	r4, #64
   1a70c: ca0000e9     	bgt	0x1aab8
   1a710: e1580009     	cmp	r8, r9
   1a714: 0a00012b     	beq	0x1abc8
   1a718: e1a04008     	mov	r4, r8
   1a71c: ea000009     	b	0x1a748
   1a720: e1540007     	cmp	r4, r7
   1a724: 0a000003     	beq	0x1a738
   1a728: e0442007     	sub	r2, r4, r7
   1a72c: e1a01007     	mov	r1, r7
   1a730: e2870004     	add	r0, r7, #4
   1a734: ebffecaa     	bl	0x159e4    @ imm = #-0x4d58 ; memmove
   1a738: ed878a00     	vstr	s16, [r7]
   1a73c: e2844004     	add	r4, r4, #4
   1a740: e1540009     	cmp	r4, r9
   1a744: 0a00011f     	beq	0x1abc8
   1a748: ed948a00     	vldr	s16, [r4]
   1a74c: edd77a00     	vldr	s15, [r7]
   1a750: eeb48ae7     	vcmpe.f32	s16, s15
   1a754: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1a758: 4afffff0     	bmi	0x1a720
   1a75c: ed547a01     	vldr	s15, [r4, #-4]
   1a760: e2443004     	sub	r3, r4, #4
   1a764: eeb48ae7     	vcmpe.f32	s16, s15
   1a768: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1a76c: 5a0001d1     	bpl	0x1aeb8
   1a770: e1a02003     	mov	r2, r3
   1a774: edc37a01     	vstr	s15, [r3, #4]
   1a778: ed737a01     	vldmdb	r3!, {s15}
   1a77c: eeb48ae7     	vcmpe.f32	s16, s15
   1a780: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1a784: 4afffff9     	bmi	0x1a770
   1a788: ed828a00     	vstr	s16, [r2]
   1a78c: eaffffea     	b	0x1a73c
   1a790: e1530009     	cmp	r3, r9
   1a794: 0a00000b     	beq	0x1a7c8
   1a798: e2832008     	add	r2, r3, #8
   1a79c: e1590002     	cmp	r9, r2
   1a7a0: 0a000007     	beq	0x1a7c4
   1a7a4: ecf27a01     	vldmia	r2!, {s15}
   1a7a8: ed937a00     	vldr	s14, [r3]
   1a7ac: eeb47a67     	vcmp.f32	s14, s15
   1a7b0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1a7b4: 1dc37a01     	vstrne	s15, [r3, #4]
   1a7b8: 12833004     	addne	r3, r3, #4
   1a7bc: e1590002     	cmp	r9, r2
   1a7c0: 1afffff7     	bne	0x1a7a4
   1a7c4: e2839004     	add	r9, r3, #4
   1a7c8: e3a02e1e     	mov	r2, #480
   1a7cc: e3a01000     	mov	r1, #0
   1a7d0: e1a00006     	mov	r0, r6
   1a7d4: ebffed66     	bl	0x15d74    @ imm = #-0x4a68 ; memset
   1a7d8: e1590007     	cmp	r9, r7
   1a7dc: 0a000003     	beq	0x1a7f0
   1a7e0: e0492007     	sub	r2, r9, r7
   1a7e4: e1a01007     	mov	r1, r7
   1a7e8: e1a00006     	mov	r0, r6
   1a7ec: ebffee14     	bl	0x16044    @ imm = #-0x47b0 ; memcpy
   1a7f0: e3570000     	cmp	r7, #0
   1a7f4: 0a000001     	beq	0x1a800
   1a7f8: e1a00007     	mov	r0, r7
   1a7fc: ebffed8f     	bl	0x15e40    @ imm = #-0x49c4 ; _ZdlPv
   1a800: f2c00010     	vmov.i32	d16, #0x0
   1a804: e3a00e7a     	mov	r0, #1952
   1a808: e3a03000     	mov	r3, #0
   1a80c: e58d3018     	str	r3, [sp, #0x18]
   1a810: edcd0b04     	vstr	d16, [sp, #16]
   1a814: ebffec3c     	bl	0x1590c     @ imm = #-0x4f10 ; _Znwj
   1a818: e59d7010     	ldr	r7, [sp, #0x10]
   1a81c: e1a04000     	mov	r4, r0
   1a820: e59d2014     	ldr	r2, [sp, #0x14]
   1a824: e0422007     	sub	r2, r2, r7
   1a828: e3520000     	cmp	r2, #0
   1a82c: ca0001a3     	bgt	0x1aec0
   1a830: e3570000     	cmp	r7, #0
   1a834: 1a0001a3     	bne	0x1aec8
   1a838: f2c00010     	vmov.i32	d16, #0x0
   1a83c: ee814b90     	vdup.32	d17, r4
   1a840: e28d2024     	add	r2, sp, #36
   1a844: e2840e7a     	add	r0, r4, #1952
   1a848: e28d1020     	add	r1, sp, #32
   1a84c: e58d0018     	str	r0, [sp, #0x18]
   1a850: e28d0010     	add	r0, sp, #16
   1a854: e3a03000     	mov	r3, #0
   1a858: e34c3080     	movt	r3, #0xc080
   1a85c: f442078f     	vst1.32	{d16}, [r2]
   1a860: edcd1b04     	vstr	d17, [sp, #16]
   1a864: e58d3020     	str	r3, [sp, #0x20]
   1a868: e3a03000     	mov	r3, #0
   1a86c: e58d302c     	str	r3, [sp, #0x2c]
   1a870: ebfffde7     	bl	0x1a014
   1a874: e59d8014     	ldr	r8, [sp, #0x14]
   1a878: eef18a00     	vmov.f32	s17, #4.000000e+00
   1a87c: e59d4018     	ldr	r4, [sp, #0x18]
   1a880: ed9f9a12     	vldr	s18, [pc, #72]          @ 0x1a8d0 ; float 4095
   1a884: eddf9a12     	vldr	s19, [pc, #72]          @ 0x1a8d4 ; float 0
   1a888: e58d6004     	str	r6, [sp, #0x4]
   1a88c: ea00001c     	b	0x1a904
   1a890: 00 00 00 00  	.word	0x00000000
   1a894: 19 00 00 00  	.word	0x00000019
   1a898: 00 00 00 00  	.word	0x00000000
   1a89c: 04 00 00 00  	.word	0x00000004
   1a8a0: 40 00 00 00  	.word	0x00000040
   1a8a4: 00 00 00 00  	.word	0x00000000
   1a8a8: 00 00 00 00  	.word	0x00000000
   1a8ac: 02 00 00 00  	.word	0x00000002
   1a8b0: 00 00 80 c0  	.word	0xc0800000
   1a8b4: 00 00 00 c0  	.word	0xc0000000
   1a8b8: 00 00 80 bf  	.word	0xbf800000
   1a8bc: 00 00 00 bf  	.word	0xbf000000
   1a8c0: 00 00 00 3f  	.word	0x3f000000
   1a8c4: 00 00 80 3f  	.word	0x3f800000
   1a8c8: 00 00 00 40  	.word	0x40000000
   1a8cc: 00 00 80 40  	.word	0x40800000
   1a8d0: 00 f0 7f 45  	.word	0x457ff000
   1a8d4: 00 00 00 00  	.word	0x00000000
   1a8d8: 28 23 07 00  	.word	0x00072328
   1a8dc: e59d300c     	ldr	r3, [sp, #0xc]
   1a8e0: e2888010     	add	r8, r8, #16
   1a8e4: e59d2004     	ldr	r2, [sp, #0x4]
   1a8e8: ed088a04     	vstr	s16, [r8, #-16]
   1a8ec: e508600c     	str	r6, [r8, #-0xc]
   1a8f0: e1530002     	cmp	r3, r2
   1a8f4: ed487a02     	vstr	s15, [r8, #-8]
   1a8f8: e5087004     	str	r7, [r8, #-0x4]
   1a8fc: e58d8014     	str	r8, [sp, #0x14]
   1a900: 0a000037     	beq	0x1a9e4
   1a904: e59d3004     	ldr	r3, [sp, #0x4]
   1a908: eeb47a00     	vmov.f32	s14, #1.250000e-01
   1a90c: e3002fff     	movw	r2, #0xfff
   1a910: ecb38a01     	vldmia	r3!, {s16}
   1a914: ee787a28     	vadd.f32	s15, s16, s17
   1a918: e58d3004     	str	r3, [sp, #0x4]
   1a91c: ee677a87     	vmul.f32	s15, s15, s14
   1a920: eeb07a69     	vmov.f32	s14, s19
   1a924: eea77a89     	vfma.f32	s14, s15, s18
   1a928: eefd7ac7     	vcvt.s32.f32	s15, s14
   1a92c: ee173a90     	vmov	r3, s15
   1a930: edcd7a02     	vstr	s15, [sp, #8]
   1a934: e2436032     	sub	r6, r3, #50
   1a938: e2837032     	add	r7, r3, #50
   1a93c: e1560002     	cmp	r6, r2
   1a940: a1a06002     	movge	r6, r2
   1a944: e1570002     	cmp	r7, r2
   1a948: a1a07002     	movge	r7, r2
   1a94c: e1580004     	cmp	r8, r4
   1a950: e1c66fc6     	bic	r6, r6, r6, asr #31
   1a954: e1c77fc7     	bic	r7, r7, r7, asr #31
   1a958: 1affffdf     	bne	0x1a8dc
   1a95c: e59da010     	ldr	r10, [sp, #0x10]
   1a960: e048900a     	sub	r9, r8, r10
   1a964: e1a04249     	asr	r4, r9, #4
   1a968: e374037e     	cmn	r4, #-134217727
   1a96c: 0a00015e     	beq	0x1aeec
   1a970: e3540000     	cmp	r4, #0
   1a974: 0a00007d     	beq	0x1ab70
   1a978: e1540084     	cmp	r4, r4, lsl #1
   1a97c: e1a04084     	lsl	r4, r4, #1
   1a980: 83e0413e     	mvnhi	r4, #-2147483633
   1a984: 9a000071     	bls	0x1ab50
   1a988: e1a00004     	mov	r0, r4
   1a98c: ebffebde     	bl	0x1590c     @ imm = #-0x5088 ; _Znwj
   1a990: e1a0b000     	mov	r11, r0
   1a994: e0804004     	add	r4, r0, r4
   1a998: e08b2009     	add	r2, r11, r9
   1a99c: e2893010     	add	r3, r9, #16
   1a9a0: e08b8003     	add	r8, r11, r3
   1a9a4: e59d3008     	ldr	r3, [sp, #0x8]
   1a9a8: e3590000     	cmp	r9, #0
   1a9ac: e5826004     	str	r6, [r2, #0x4]
   1a9b0: e5823008     	str	r3, [r2, #0x8]
   1a9b4: e582700c     	str	r7, [r2, #0xc]
   1a9b8: ed828a00     	vstr	s16, [r2]
   1a9bc: ca00005c     	bgt	0x1ab34
   1a9c0: e35a0000     	cmp	r10, #0
   1a9c4: 1a00005e     	bne	0x1ab44
   1a9c8: e59d300c     	ldr	r3, [sp, #0xc]
   1a9cc: e59d2004     	ldr	r2, [sp, #0x4]
   1a9d0: e58db010     	str	r11, [sp, #0x10]
   1a9d4: e1530002     	cmp	r3, r2
   1a9d8: e58d8014     	str	r8, [sp, #0x14]
   1a9dc: e58d4018     	str	r4, [sp, #0x18]
   1a9e0: 1affffc7     	bne	0x1a904
   1a9e4: e3023318     	movw	r3, #0x2318
   1a9e8: e3403007     	movt	r3, #0x7
   1a9ec: e28dc020     	add	r12, sp, #32
   1a9f0: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1a9f4: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1a9f8: e28d0010     	add	r0, sp, #16
   1a9fc: e1a0100c     	mov	r1, r12
   1aa00: ebfffd83     	bl	0x1a014
   1aa04: e59d4010     	ldr	r4, [sp, #0x10]
   1aa08: e59d6014     	ldr	r6, [sp, #0x14]
   1aa0c: e1540006     	cmp	r4, r6
   1aa10: 0a000079     	beq	0x1abfc
   1aa14: e0467004     	sub	r7, r6, r4
   1aa18: e1a01006     	mov	r1, r6
   1aa1c: e1a00004     	mov	r0, r4
   1aa20: e3a03000     	mov	r3, #0
   1aa24: e1a02247     	asr	r2, r7, #4
   1aa28: e16f2f12     	clz	r2, r2
   1aa2c: e262201f     	rsb	r2, r2, #31
   1aa30: e1a02082     	lsl	r2, r2, #1
   1aa34: ebfffe12     	bl	0x1a284
   1aa38: e3570c01     	cmp	r7, #256
   1aa3c: da0000e1     	ble	0x1adc8
   1aa40: e2847c01     	add	r7, r4, #256
   1aa44: e1a00004     	mov	r0, r4
   1aa48: e1a01007     	mov	r1, r7
   1aa4c: ebfffdd8     	bl	0x1a1b4
   1aa50: e1a0e007     	mov	lr, r7
   1aa54: e156000e     	cmp	r6, lr
   1aa58: e1a0700e     	mov	r7, lr
   1aa5c: 0a0000dc     	beq	0x1add4
   1aa60: e28dc020     	add	r12, sp, #32
   1aa64: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1aa68: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1aa6c: e59e8008     	ldr	r8, [lr, #0x8]
   1aa70: e51e3008     	ldr	r3, [lr, #-0x8]
   1aa74: e1580003     	cmp	r8, r3
   1aa78: aa000008     	bge	0x1aaa0
   1aa7c: e24ec010     	sub	r12, lr, #16
   1aa80: e28c4010     	add	r4, r12, #16
   1aa84: e1a0700c     	mov	r7, r12
   1aa88: e89c000f     	ldm	r12, {r0, r1, r2, r3}
   1aa8c: e24cc010     	sub	r12, r12, #16
   1aa90: e884000f     	stm	r4, {r0, r1, r2, r3}
   1aa94: e59c3008     	ldr	r3, [r12, #0x8]
   1aa98: e1530008     	cmp	r3, r8
   1aa9c: cafffff7     	bgt	0x1aa80
   1aaa0: e28d3020     	add	r3, sp, #32
   1aaa4: e58d8028     	str	r8, [sp, #0x28]
   1aaa8: e28ee010     	add	lr, lr, #16
   1aaac: e893000f     	ldm	r3, {r0, r1, r2, r3}
   1aab0: e887000f     	stm	r7, {r0, r1, r2, r3}
   1aab4: eaffffe6     	b	0x1aa54
   1aab8: e287a040     	add	r10, r7, #64
   1aabc: e1a04008     	mov	r4, r8
   1aac0: ea000009     	b	0x1aaec
   1aac4: e1540007     	cmp	r4, r7
   1aac8: 0a000003     	beq	0x1aadc
   1aacc: e0442007     	sub	r2, r4, r7
   1aad0: e1a01007     	mov	r1, r7
   1aad4: e2870004     	add	r0, r7, #4
   1aad8: ebffebc1     	bl	0x159e4    @ imm = #-0x50fc ; memmove
   1aadc: ed878a00     	vstr	s16, [r7]
   1aae0: e2844004     	add	r4, r4, #4
   1aae4: e15a0004     	cmp	r10, r4
   1aae8: 0a000022     	beq	0x1ab78
   1aaec: ed948a00     	vldr	s16, [r4]
   1aaf0: edd77a00     	vldr	s15, [r7]
   1aaf4: eeb48ae7     	vcmpe.f32	s16, s15
   1aaf8: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1aafc: 4afffff0     	bmi	0x1aac4
   1ab00: ed547a01     	vldr	s15, [r4, #-4]
   1ab04: e2443004     	sub	r3, r4, #4
   1ab08: eeb48ae7     	vcmpe.f32	s16, s15
   1ab0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1ab10: 5a0000e6     	bpl	0x1aeb0
   1ab14: e1a02003     	mov	r2, r3
   1ab18: edc37a01     	vstr	s15, [r3, #4]
   1ab1c: ed737a01     	vldmdb	r3!, {s15}
   1ab20: eeb48ae7     	vcmpe.f32	s16, s15
   1ab24: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1ab28: 4afffff9     	bmi	0x1ab14
   1ab2c: ed828a00     	vstr	s16, [r2]
   1ab30: eaffffea     	b	0x1aae0
   1ab34: e1a02009     	mov	r2, r9
   1ab38: e1a0100a     	mov	r1, r10
   1ab3c: e1a0000b     	mov	r0, r11
   1ab40: ebffeba7     	bl	0x159e4    @ imm = #-0x5164 ; memmove
   1ab44: e1a0000a     	mov	r0, r10
   1ab48: ebffecbc     	bl	0x15e40    @ imm = #-0x4d10 ; _ZdlPv
   1ab4c: eaffff9d     	b	0x1a9c8
   1ab50: e3540000     	cmp	r4, #0
   1ab54: 01a0b004     	moveq	r11, r4
   1ab58: 0affff8e     	beq	0x1a998
   1ab5c: e3e0333e     	mvn	r3, #-134217728
   1ab60: e1540003     	cmp	r4, r3
   1ab64: 21a04003     	movhs	r4, r3
   1ab68: e1a04204     	lsl	r4, r4, #4
   1ab6c: eaffff85     	b	0x1a988
   1ab70: e3a04010     	mov	r4, #16
   1ab74: eaffff83     	b	0x1a988
   1ab78: e15a0009     	cmp	r10, r9
   1ab7c: 0a000011     	beq	0x1abc8
   1ab80: e1a0300a     	mov	r3, r10
   1ab84: e287103c     	add	r1, r7, #60
   1ab88: e1a00003     	mov	r0, r3
   1ab8c: e1a02001     	mov	r2, r1
   1ab90: ecb37a01     	vldmia	r3!, {s14}
   1ab94: ecf17a01     	vldmia	r1!, {s15}
   1ab98: eeb47ae7     	vcmpe.f32	s14, s15
   1ab9c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1aba0: 5a000005     	bpl	0x1abbc
   1aba4: e1a00002     	mov	r0, r2
   1aba8: edc27a01     	vstr	s15, [r2, #4]
   1abac: ed727a01     	vldmdb	r2!, {s15}
   1abb0: eeb47ae7     	vcmpe.f32	s14, s15
   1abb4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1abb8: 4afffff9     	bmi	0x1aba4
   1abbc: e1590003     	cmp	r9, r3
   1abc0: ed807a00     	vstr	s14, [r0]
   1abc4: 1affffef     	bne	0x1ab88
   1abc8: e1a02008     	mov	r2, r8
   1abcc: e1a01007     	mov	r1, r7
   1abd0: ea000005     	b	0x1abec
   1abd4: edd27a00     	vldr	s15, [r2]
   1abd8: e2822004     	add	r2, r2, #4
   1abdc: ecb17a01     	vldmia	r1!, {s14}
   1abe0: eeb47a67     	vcmp.f32	s14, s15
   1abe4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1abe8: 0afffee8     	beq	0x1a790
   1abec: e1a03001     	mov	r3, r1
   1abf0: e1520009     	cmp	r2, r9
   1abf4: 1afffff6     	bne	0x1abd4
   1abf8: eafffef2     	b	0x1a7c8
   1abfc: e0466004     	sub	r6, r6, r4
   1ac00: e1a03246     	asr	r3, r6, #4
   1ac04: e3530001     	cmp	r3, #1
   1ac08: 0a00004c     	beq	0x1ad40
   1ac0c: e2446010     	sub	r6, r4, #16
   1ac10: e1a01004     	mov	r1, r4
   1ac14: e0866203     	add	r6, r6, r3, lsl #4
   1ac18: e1a03004     	mov	r3, r4
   1ac1c: e593000c     	ldr	r0, [r3, #0xc]
   1ac20: e5932014     	ldr	r2, [r3, #0x14]
   1ac24: e1500002     	cmp	r0, r2
   1ac28: da000008     	ble	0x1ac50
   1ac2c: e593c008     	ldr	r12, [r3, #0x8]
   1ac30: e5930018     	ldr	r0, [r3, #0x18]
   1ac34: e040200c     	sub	r2, r0, r12
   1ac38: e0822fa2     	add	r2, r2, r2, lsr #31
   1ac3c: e1a020c2     	asr	r2, r2, #1
   1ac40: e082c00c     	add	r12, r2, r12
   1ac44: e0402002     	sub	r2, r0, r2
   1ac48: e583c00c     	str	r12, [r3, #0xc]
   1ac4c: e5832014     	str	r2, [r3, #0x14]
   1ac50: e2833010     	add	r3, r3, #16
   1ac54: e1530006     	cmp	r3, r6
   1ac58: 1affffef     	bne	0x1ac1c
   1ac5c: e5913004     	ldr	r3, [r1, #0x4]
   1ac60: e591200c     	ldr	r2, [r1, #0xc]
   1ac64: e1530002     	cmp	r3, r2
   1ac68: aa00001c     	bge	0x1ace0
   1ac6c: e042e003     	sub	lr, r2, r3
   1ac70: e591c000     	ldr	r12, [r1]
   1ac74: e24e0001     	sub	r0, lr, #1
   1ac78: e3500002     	cmp	r0, #2
   1ac7c: 9a00000c     	bls	0x1acb4
   1ac80: e2830089     	add	r0, r3, #137
   1ac84: eea0cb90     	vdup.32	q8, r12
   1ac88: e1a0812e     	lsr	r8, lr, #2
   1ac8c: e3a07000     	mov	r7, #0
   1ac90: e0850100     	add	r0, r5, r0, lsl #2
   1ac94: e2877001     	add	r7, r7, #1
   1ac98: f4400a8d     	vst1.32	{d16, d17}, [r0]!
   1ac9c: e1570008     	cmp	r7, r8
   1aca0: 1afffffb     	bne	0x1ac94
   1aca4: e3ce0003     	bic	r0, lr, #3
   1aca8: e0833000     	add	r3, r3, r0
   1acac: e15e0000     	cmp	lr, r0
   1acb0: 0a00000a     	beq	0x1ace0
   1acb4: e085e103     	add	lr, r5, r3, lsl #2
   1acb8: e2830001     	add	r0, r3, #1
   1acbc: e1520000     	cmp	r2, r0
   1acc0: e58ec224     	str	r12, [lr, #0x224]
   1acc4: da000005     	ble	0x1ace0
   1acc8: e0850100     	add	r0, r5, r0, lsl #2
   1accc: e2833002     	add	r3, r3, #2
   1acd0: e1520003     	cmp	r2, r3
   1acd4: e580c224     	str	r12, [r0, #0x224]
   1acd8: c0853103     	addgt	r3, r5, r3, lsl #2
   1acdc: c583c224     	strgt	r12, [r3, #0x224]
   1ace0: e5913014     	ldr	r3, [r1, #0x14]
   1ace4: e1530002     	cmp	r3, r2
   1ace8: da000011     	ble	0x1ad34
   1acec: ed917a00     	vldr	s14, [r1]
   1acf0: e0433002     	sub	r3, r3, r2
   1acf4: edd17a04     	vldr	s15, [r1, #16]
   1acf8: ee063a90     	vmov	s13, r3
   1acfc: e2822089     	add	r2, r2, #137
   1ad00: e3a00000     	mov	r0, #0
   1ad04: eef86ae6     	vcvt.f32.s32	s13, s13
   1ad08: ee777ac7     	vsub.f32	s15, s15, s14
   1ad0c: e0852102     	add	r2, r5, r2, lsl #2
   1ad10: ee060a10     	vmov	s12, r0
   1ad14: e2800001     	add	r0, r0, #1
   1ad18: e1500003     	cmp	r0, r3
   1ad1c: eef85ac6     	vcvt.f32.s32	s11, s12
   1ad20: ee856aa6     	vdiv.f32	s12, s11, s13
   1ad24: eef05a47     	vmov.f32	s11, s14
   1ad28: eee65a27     	vfma.f32	s11, s12, s15
   1ad2c: ece25a01     	vstmia	r2!, {s11}
   1ad30: 1afffff6     	bne	0x1ad10
   1ad34: e2811010     	add	r1, r1, #16
   1ad38: e1510006     	cmp	r1, r6
   1ad3c: 1affffc6     	bne	0x1ac5c
   1ad40: e3540000     	cmp	r4, #0
   1ad44: 0a000001     	beq	0x1ad50
   1ad48: e1a00004     	mov	r0, r4
   1ad4c: ebffec3b     	bl	0x15e40    @ imm = #-0x4f14 ; _ZdlPv
   1ad50: e51fe480     	ldr	lr, [pc, #-0x480]       @ 0x1a8d8
   1ad54: e2854901     	add	r4, r5, #16384
   1ad58: e284cf8a     	add	r12, r4, #552
   1ad5c: e2849f9b     	add	r9, r4, #620
   1ad60: eddf4b72     	vldr	d20, [pc, #456]         @ 0x1af30 ; float 3.05175853327e-05
   1ad64: eddf5b73     	vldr	d21, [pc, #460]         @ 0x1af38 ; float 4.65661395381e-10
   1ad68: e2848f9f     	add	r8, r4, #636
   1ad6c: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1ad70: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   1ad74: e2847fa3     	add	r7, r4, #652
   1ad78: eddf2b70     	vldr	d18, [pc, #448]         @ 0x1af40 ; float 1.3411048233e-08
   1ad7c: eddf3b71     	vldr	d19, [pc, #452]         @ 0x1af48 ; float 1.34110464955e-08
   1ad80: e3a06003     	mov	r6, #3
   1ad84: eddf0b71     	vldr	d16, [pc, #452]         @ 0x1af50 ; float 3.43322837737e-06
   1ad88: eddf1b72     	vldr	d17, [pc, #456]         @ 0x1af58 ; float 0.000878906470662
   1ad8c: e5846224     	str	r6, [r4, #0x224]
   1ad90: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1ad94: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   1ad98: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   1ad9c: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   1ada0: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   1ada4: e88c000f     	stm	r12, {r0, r1, r2, r3}
   1ada8: e1a00005     	mov	r0, r5
   1adac: e5846268     	str	r6, [r4, #0x268]
   1adb0: f4494a8f     	vst1.32	{d20, d21}, [r9]
   1adb4: f4482a8f     	vst1.32	{d18, d19}, [r8]
   1adb8: f4470a8f     	vst1.32	{d16, d17}, [r7]
   1adbc: e28dd034     	add	sp, sp, #52
   1adc0: ecbd8b04     	vpop	{d8, d9}
   1adc4: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   1adc8: e1a01006     	mov	r1, r6
   1adcc: e1a00004     	mov	r0, r4
   1add0: ebfffcf7     	bl	0x1a1b4
   1add4: e59d4010     	ldr	r4, [sp, #0x10]
   1add8: e59d6014     	ldr	r6, [sp, #0x14]
   1addc: e1540006     	cmp	r4, r6
   1ade0: 0affff85     	beq	0x1abfc
   1ade4: e2843010     	add	r3, r4, #16
   1ade8: e1560003     	cmp	r6, r3
   1adec: 0affff82     	beq	0x1abfc
   1adf0: ed947a00     	vldr	s14, [r4]
   1adf4: e1a03004     	mov	r3, r4
   1adf8: edd37a04     	vldr	s15, [r3, #16]
   1adfc: e1a07003     	mov	r7, r3
   1ae00: eeb47a67     	vcmp.f32	s14, s15
   1ae04: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1ae08: 1a000003     	bne	0x1ae1c
   1ae0c: e5931008     	ldr	r1, [r3, #0x8]
   1ae10: e5932018     	ldr	r2, [r3, #0x18]
   1ae14: e1510002     	cmp	r1, r2
   1ae18: 0a000005     	beq	0x1ae34
   1ae1c: e2832020     	add	r2, r3, #32
   1ae20: e2833010     	add	r3, r3, #16
   1ae24: e1560002     	cmp	r6, r2
   1ae28: 0affff73     	beq	0x1abfc
   1ae2c: eeb07a67     	vmov.f32	s14, s15
   1ae30: eafffff0     	b	0x1adf8
   1ae34: e1560003     	cmp	r6, r3
   1ae38: 0affff6f     	beq	0x1abfc
   1ae3c: e2833020     	add	r3, r3, #32
   1ae40: e1560003     	cmp	r6, r3
   1ae44: 0a000010     	beq	0x1ae8c
   1ae48: e287c030     	add	r12, r7, #48
   1ae4c: ed977a00     	vldr	s14, [r7]
   1ae50: ed5c7a04     	vldr	s15, [r12, #-16]
   1ae54: eeb47a67     	vcmp.f32	s14, s15
   1ae58: eef1fa10     	vmrs	APSR_nzcv, fpscr
   1ae5c: 1a000003     	bne	0x1ae70
   1ae60: e5972008     	ldr	r2, [r7, #0x8]
   1ae64: e51c3008     	ldr	r3, [r12, #-0x8]
   1ae68: e1520003     	cmp	r2, r3
   1ae6c: 0a000003     	beq	0x1ae80
   1ae70: e287e010     	add	lr, r7, #16
   1ae74: e91c000f     	ldmdb	r12, {r0, r1, r2, r3}
   1ae78: e1a0700e     	mov	r7, lr
   1ae7c: e88e000f     	stm	lr, {r0, r1, r2, r3}
   1ae80: e156000c     	cmp	r6, r12
   1ae84: e28cc010     	add	r12, r12, #16
   1ae88: 1affffef     	bne	0x1ae4c
   1ae8c: e2877010     	add	r7, r7, #16
   1ae90: e1560007     	cmp	r6, r7
   1ae94: 11a06007     	movne	r6, r7
   1ae98: 158d7014     	strne	r7, [sp, #0x14]
   1ae9c: e0466004     	sub	r6, r6, r4
   1aea0: e1a03246     	asr	r3, r6, #4
   1aea4: e3530001     	cmp	r3, #1
   1aea8: 1affff57     	bne	0x1ac0c
   1aeac: eaffffa5     	b	0x1ad48
   1aeb0: e1a02004     	mov	r2, r4
   1aeb4: eaffff1c     	b	0x1ab2c
   1aeb8: e1a02004     	mov	r2, r4
   1aebc: eafffe31     	b	0x1a788
   1aec0: e1a01007     	mov	r1, r7
   1aec4: ebffeac6     	bl	0x159e4    @ imm = #-0x54e8 ; memmove
   1aec8: e1a00007     	mov	r0, r7
   1aecc: ebffebdb     	bl	0x15e40    @ imm = #-0x5094 ; _ZdlPv
   1aed0: eafffe58     	b	0x1a838
   1aed4: e3000c8c     	movw	r0, #0xc8c
   1aed8: e3400007     	movt	r0, #0x7
   1aedc: ebffeb35     	bl	0x15bb8    @ imm = #-0x532c ; _ZSt20__throw_length_errorPKc
   1aee0: e3000c8c     	movw	r0, #0xc8c
   1aee4: e3400007     	movt	r0, #0x7
   1aee8: ebffeb32     	bl	0x15bb8    @ imm = #-0x5338 ; _ZSt20__throw_length_errorPKc
   1aeec: e3000c8c     	movw	r0, #0xc8c
   1aef0: e3400007     	movt	r0, #0x7
   1aef4: ebffeb2f     	bl	0x15bb8    @ imm = #-0x5344 ; _ZSt20__throw_length_errorPKc
   1aef8: e59d0010     	ldr	r0, [sp, #0x10]
   1aefc: e3500000     	cmp	r0, #0
   1af00: 0a000000     	beq	0x1af08
   1af04: ebffebcd     	bl	0x15e40    @ imm = #-0x50cc ; _ZdlPv
   1af08: ebffec14     	bl	0x15f60    @ imm = #-0x4fb0 ; __cxa_end_cleanup
   1af0c: e1a0a007     	mov	r10, r7
   1af10: e35a0000     	cmp	r10, #0
   1af14: 0afffffb     	beq	0x1af08
   1af18: e1a0000a     	mov	r0, r10
   1af1c: ebffebc7     	bl	0x15e40    @ imm = #-0x50e4 ; _ZdlPv
   1af20: eafffff8     	b	0x1af08
   1af24: eafffff8     	b	0x1af0c
   1af28: eafffff8     	b	0x1af10
   1af2c: e320f000     	nop
   1af30: 66 66 66 3f  	.word	0x3f666666
   1af34: 00 00 00 3f  	.word	0x3f000000
   1af38: cd cc 4c 3e  	.word	0x3e4ccccd
   1af3c: 00 00 00 3e  	.word	0x3e000000
   1af40: 9a 99 99 3e  	.word	0x3e99999a
   1af44: cd cc 4c 3e  	.word	0x3e4ccccd
   1af48: 00 00 00 00  	.word	0x00000000
   1af4c: cd cc 4c 3e  	.word	0x3e4ccccd
   1af50: 00 00 c8 42  	.word	0x42c80000
   1af54: cd cc cc 3e  	.word	0x3ecccccd
   1af58: 00 40 1c 46  	.word	0x461c4000
   1af5c: cd cc 4c 3f  	.word	0x3f4ccccd
