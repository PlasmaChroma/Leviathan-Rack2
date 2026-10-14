0000a278 <_getAllAdcs>:
    a278: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    a27c: e3a08000     	mov	r8, #0
    a280: ed2d8b02     	vpush	{d8}
    a284: e30c7006     	movw	r7, #0xc006
    a288: e1a04000     	mov	r4, r0
    a28c: e34f7fff     	movt	r7, #0xffff
    a290: e1a09000     	mov	r9, r0
    a294: e1a06008     	mov	r6, r8
    a298: e24ddf75     	sub	sp, sp, #468
    a29c: e28d5f73     	add	r5, sp, #460
    a2a0: ea000003     	b	0xa2b4 <_getAllAdcs+0x3c> @ imm = #0xc
    a2a4: e3510005     	cmp	r1, #5
    a2a8: 0a00002b     	beq	0xa35c <_getAllAdcs+0xe4> @ imm = #0xac
    a2ac: e2888001     	add	r8, r8, #1
    a2b0: e2899f71     	add	r9, r9, #452
    a2b4: e5d9011c     	ldrb	r0, [r9, #0x11c]
    a2b8: e5cd61ce     	strb	r6, [sp, #0x1ce]
    a2bc: e2003007     	and	r3, r0, #7
    a2c0: e1a001a0     	lsr	r0, r0, #3
    a2c4: e1a02143     	asr	r2, r3, #2
    a2c8: e1a01303     	lsl	r1, r3, #6
    a2cc: e382c006     	orr	r12, r2, #6
    a2d0: e5cd11cd     	strb	r1, [sp, #0x1cd]
    a2d4: e5cdc1cc     	strb	r12, [sp, #0x1cc]
    a2d8: ebffe629     	bl	0x3b84 <.plt+0x488>     @ imm = #-0x675c  // CALL bcm2835_spi_chipSelect
    a2dc: e3a01003     	mov	r1, #3
    a2e0: e1a00005     	mov	r0, r5
    a2e4: ebffe563     	bl	0x3878 <.plt+0x17c>     @ imm = #-0x6a74  // CALL bcm2835_spi_transfern
    a2e8: e5dd01cd     	ldrb	r0, [sp, #0x1cd]
    a2ec: e5dd31ce     	ldrb	r3, [sp, #0x1ce]
    a2f0: e3580000     	cmp	r8, #0
    a2f4: e6ef1078     	uxtb	r1, r8
    a2f8: e1a02400     	lsl	r2, r0, #8
    a2fc: e202cc0f     	and	r12, r2, #3840
    a300: e183000c     	orr	r0, r3, r12
    a304: ee070a90     	vmov	s15, r0
    a308: eeb80a67     	vcvt.f32.u32	s0, s15
    a30c: ed890a48     	vstr	s0, [r9, #288]
    a310: 1affffe3     	bne	0xa2a4 <_getAllAdcs+0x2c> @ imm = #-0x74
    a314: e28d1f73     	add	r1, sp, #460
    a318: e1a00008     	mov	r0, r8
    a31c: e1c170b0     	strh	r7, [r1]
    a320: e5cd81ce     	strb	r8, [sp, #0x1ce]
    a324: ebffe616     	bl	0x3b84 <.plt+0x488>     @ imm = #-0x67a8  // CALL bcm2835_spi_chipSelect
    a328: e3a01003     	mov	r1, #3
    a32c: e1a00005     	mov	r0, r5
    a330: ebffe550     	bl	0x3878 <.plt+0x17c>     @ imm = #-0x6ac0  // CALL bcm2835_spi_transfern
    a334: e5dd21cd     	ldrb	r2, [sp, #0x1cd]
    a338: e5dd31ce     	ldrb	r3, [sp, #0x1ce]
    a33c: e284cd3d     	add	r12, r4, #3904
    a340: e1a00402     	lsl	r0, r2, #8
    a344: e2001c0f     	and	r1, r0, #3840
    a348: e1832001     	orr	r2, r3, r1
    a34c: ee062a90     	vmov	s13, r2
    a350: eeb87a66     	vcvt.f32.u32	s14, s13
    a354: ed8c7a00     	vstr	s14, [r12]
    a358: eaffffd3     	b	0xa2ac <_getAllAdcs+0x34> @ imm = #-0xb4
    a35c: e59f84d0     	ldr	r8, [pc, #0x4d0]        @ 0xa834 <_getAllAdcs+0x5bc>  // u32=0x1d044; f32?=1.66547125e-40
    a360: e3a0ef71     	mov	lr, #452
    a364: e5cd61ce     	strb	r6, [sp, #0x1ce]
    a368: e3001126     	movw	r1, #0x126
    a36c: e08f9008     	add	r9, pc, r8
    a370: e5d97050     	ldrb	r7, [r9, #0x50]
    a374: e2876006     	add	r6, r7, #6
    a378: e6ef6076     	uxtb	r6, r6
    a37c: e007069e     	mul	r7, lr, r6
    a380: e0893086     	add	r3, r9, r6, lsl #1
    a384: e0848007     	add	r8, r4, r7
    a388: e5d8c11c     	ldrb	r12, [r8, #0x11c]
    a38c: e19890b1     	ldrh	r9, [r8, r1]
    a390: e20c2007     	and	r2, r12, #7
    a394: e1a001ac     	lsr	r0, r12, #3
    a398: e1a0e142     	asr	lr, r2, #2
    a39c: e1c395b4     	strh	r9, [r3, #84]
    a3a0: e38e1006     	orr	r1, lr, #6
    a3a4: e1a03302     	lsl	r3, r2, #6
    a3a8: e5cd11cc     	strb	r1, [sp, #0x1cc]
    a3ac: e5cd31cd     	strb	r3, [sp, #0x1cd]
    a3b0: ebffe5f3     	bl	0x3b84 <.plt+0x488>     @ imm = #-0x6834  // CALL bcm2835_spi_chipSelect
    a3b4: e1a00005     	mov	r0, r5
    a3b8: e3a01003     	mov	r1, #3
    a3bc: ebffe52d     	bl	0x3878 <.plt+0x17c>     @ imm = #-0x6b4c  // CALL bcm2835_spi_transfern
    a3c0: e5dd51cd     	ldrb	r5, [sp, #0x1cd]
    a3c4: e5ddc1ce     	ldrb	r12, [sp, #0x1ce]
    a3c8: e2870f47     	add	r0, r7, #284
    a3cc: e0840000     	add	r0, r4, r0
    a3d0: e1a07405     	lsl	r7, r5, #8
    a3d4: e2079c0f     	and	r9, r7, #3840
    a3d8: e18c2009     	orr	r2, r12, r9
    a3dc: ee002a90     	vmov	s1, r2
    a3e0: eeb81a60     	vcvt.f32.u32	s2, s1
    a3e4: ed881a48     	vstr	s2, [r8, #288]
    a3e8: ebffe60c     	bl	0x3c20 <.plt+0x524>     @ imm = #-0x67d0  // CALL _stabliseADC
    a3ec: e3560006     	cmp	r6, #6
    a3f0: 0a0000b2     	beq	0xa6c0 <_getAllAdcs+0x448> @ imm = #0x2c8
    a3f4: e2840f47     	add	r0, r4, #284
    a3f8: e59f7438     	ldr	r7, [pc, #0x438]        @ 0xa838 <_getAllAdcs+0x5c0>  // u32=0x1cfa8; f32?=1.66328523e-40
    a3fc: ebffe607     	bl	0x3c20 <.plt+0x524>     @ imm = #-0x67e4  // CALL _stabliseADC
    a400: e3002126     	movw	r2, #0x126
    a404: e19480b2     	ldrh	r8, [r4, r2]
    a408: e08f9007     	add	r9, pc, r7
    a40c: e2840e2e     	add	r0, r4, #736
    a410: e2846a01     	add	r6, r4, #4096
    a414: e1c985b4     	strh	r8, [r9, #84]
    a418: ebffe600     	bl	0x3c20 <.plt+0x524>     @ imm = #-0x6800  // CALL _stabliseADC
    a41c: e30032ea     	movw	r3, #0x2ea
    a420: e2841e4a     	add	r1, r4, #1184
    a424: e19450b3     	ldrh	r5, [r4, r3]
    a428: e2810004     	add	r0, r1, #4
    a42c: e2848e82     	add	r8, r4, #2080
    a430: e1c955b6     	strh	r5, [r9, #86]
    a434: ebffe5f9     	bl	0x3c20 <.plt+0x524>     @ imm = #-0x681c  // CALL _stabliseADC
    a438: e300c4ae     	movw	r12, #0x4ae
    a43c: e2840e66     	add	r0, r4, #1632
    a440: e19470bc     	ldrh	r7, [r4, r12]
    a444: e2800008     	add	r0, r0, #8
    a448: e1c975b8     	strh	r7, [r9, #88]
    a44c: ebffe5f3     	bl	0x3c20 <.plt+0x524>     @ imm = #-0x6834  // CALL _stabliseADC
    a450: e3002672     	movw	r2, #0x672
    a454: e288000c     	add	r0, r8, #12
    a458: e19430b2     	ldrh	r3, [r4, r2]
    a45c: e30079fa     	movw	r7, #0x9fa
    a460: e1c935ba     	strh	r3, [r9, #90]
    a464: ebffe5ed     	bl	0x3c20 <.plt+0x524>     @ imm = #-0x684c  // CALL _stabliseADC
    a468: e3001836     	movw	r1, #0x836
    a46c: e2840e9f     	add	r0, r4, #2544
    a470: e19450b1     	ldrh	r5, [r4, r1]
    a474: e1c955bc     	strh	r5, [r9, #92]
    a478: ebffe5e8     	bl	0x3c20 <.plt+0x524>     @ imm = #-0x6860  // CALL _stabliseADC
    a47c: e300cba2     	movw	r12, #0xba2
    a480: e3002bbe     	movw	r2, #0xbbe
    a484: e3001f46     	movw	r1, #0xf46
    a488: e3003d82     	movw	r3, #0xd82
    a48c: e19600bc     	ldrh	r0, [r6, r12]
    a490: e300c10a     	movw	r12, #0x10a
    a494: e19480b7     	ldrh	r8, [r4, r7]
    a498: e19450b2     	ldrh	r5, [r4, r2]
    a49c: e3002492     	movw	r2, #0x492
    a4a0: e19470b1     	ldrh	r7, [r4, r1]
    a4a4: e19430b3     	ldrh	r3, [r4, r3]
    a4a8: e1c907b2     	strh	r0, [r9, #114]
    a4ac: e30002ce     	movw	r0, #0x2ce
    a4b0: e1c985be     	strh	r8, [r9, #94]
    a4b4: e3008656     	movw	r8, #0x656
    a4b8: e1c956b0     	strh	r5, [r9, #96]
    a4bc: e300581a     	movw	r5, #0x81a
    a4c0: e1c936b2     	strh	r3, [r9, #98]
    a4c4: e30039de     	movw	r3, #0x9de
    a4c8: e1c976b4     	strh	r7, [r9, #100]
    a4cc: e5d4703c     	ldrb	r7, [r4, #0x3c]
    a4d0: e196e0bc     	ldrh	lr, [r6, r12]
    a4d4: e19610b8     	ldrh	r1, [r6, r8]
    a4d8: e3570002     	cmp	r7, #2
    a4dc: e196c0b2     	ldrh	r12, [r6, r2]
    a4e0: e19600b0     	ldrh	r0, [r6, r0]
    a4e4: e19620b5     	ldrh	r2, [r6, r5]
    a4e8: e19680b3     	ldrh	r8, [r6, r3]
    a4ec: e1c9e6b6     	strh	lr, [r9, #102]
    a4f0: e1c906b8     	strh	r0, [r9, #104]
    a4f4: e1c9c6ba     	strh	r12, [r9, #106]
    a4f8: e1c916bc     	strh	r1, [r9, #108]
    a4fc: e1c926be     	strh	r2, [r9, #110]
    a500: e1c987b0     	strh	r8, [r9, #112]
    a504: 0a000008     	beq	0xa52c <_getAllAdcs+0x2b4> @ imm = #0x20
    a508: e2892054     	add	r2, r9, #84
    a50c: e3a03010     	mov	r3, #16
    a510: e3a01000     	mov	r1, #0
    a514: e1a00004     	mov	r0, r4
    a518: ebffe569     	bl	0x3ac4 <.plt+0x3c8>     @ imm = #-0x6a5c  // CALL writeChunkToSharedMem
    a51c: e1a00004     	mov	r0, r4
    a520: ebffe4a7     	bl	0x37c4 <.plt+0xc8>      @ imm = #-0x6d64  // CALL _setPlayParameters
    a524: e1a00004     	mov	r0, r4
    a528: ebffe5dd     	bl	0x3ca4 <.plt+0x5a8>     @ imm = #-0x688c  // CALL _setPlayAndRecLayer
    a52c: e594c0b4     	ldr	r12, [r4, #0xb4]
    a530: e5dc9004     	ldrb	r9, [r12, #0x4]
    a534: e3590000     	cmp	r9, #0
    a538: 1a000072     	bne	0xa708 <_getAllAdcs+0x490> @ imm = #0x1c8
    a53c: e5dc1005     	ldrb	r1, [r12, #0x5]
    a540: e5dc2003     	ldrb	r2, [r12, #0x3]
    a544: e3510000     	cmp	r1, #0
    a548: 0a000040     	beq	0xa650 <_getAllAdcs+0x3d8> @ imm = #0x100
    a54c: e3520000     	cmp	r2, #0
    a550: 1a000080     	bne	0xa758 <_getAllAdcs+0x4e0> @ imm = #0x200
    a554: e59f32e0     	ldr	r3, [pc, #0x2e0]        @ 0xa83c <_getAllAdcs+0x5c4>  // u32=0x1ce4c; f32?=1.65840871e-40
    a558: e30c7ccd     	movw	r7, #0xcccd
    a55c: e34c7ccc     	movt	r7, #0xcccc
    a560: e2866edd     	add	r6, r6, #3536
    a564: e08f5003     	add	r5, pc, r3
    a568: e3a0900a     	mov	r9, #10
    a56c: ed964a01     	vldr	s8, [r6, #4]
    a570: e5d50050     	ldrb	r0, [r5, #0x50]
    a574: e2808001     	add	r8, r0, #1
    a578: eeb54ac0     	vcmpe.f32	s8, #0
    a57c: e6efc078     	uxtb	r12, r8
    a580: e0810c97     	umull	r0, r1, r7, r12
    a584: eef1fa10     	vmrs	APSR_nzcv, fpscr
    a588: e1a021a1     	lsr	r2, r1, #3
    a58c: e063c299     	mls	r3, r9, r2, r12
    a590: e6ef0073     	uxtb	r0, r3
    a594: e5c50050     	strb	r0, [r5, #0x50]
    a598: ba000018     	blt	0xa600 <_getAllAdcs+0x388> @ imm = #0x60
    a59c: e3500000     	cmp	r0, #0
    a5a0: 0a000002     	beq	0xa5b0 <_getAllAdcs+0x338> @ imm = #0x8
    a5a4: e28ddf75     	add	sp, sp, #468
    a5a8: ecbd8b02     	vpop	{d8}
    a5ac: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    a5b0: eefd4ac4     	vcvt.s32.f32	s9, s8
    a5b4: e3a0ef71     	mov	lr, #452
    a5b8: e3008126     	movw	r8, #0x126
    a5bc: e59f727c     	ldr	r7, [pc, #0x27c]        @ 0xa840 <_getAllAdcs+0x5c8>  // u32=0xaf78; f32?=6.2946327e-41
    a5c0: e1a0200e     	mov	r2, lr
    a5c4: e1a0000d     	mov	r0, sp
    a5c8: e08f7007     	add	r7, pc, r7
    a5cc: ee146a90     	vmov	r6, s9
    a5d0: e02c469e     	mla	r12, lr, r6, r4
    a5d4: e0859086     	add	r9, r5, r6, lsl #1
    a5d8: e1d995b4     	ldrh	r9, [r9, #84]
    a5dc: e28c1f47     	add	r1, r12, #284
    a5e0: e19c80b8     	ldrh	r8, [r12, r8]
    a5e4: ebffe494     	bl	0x383c <.plt+0x140>     @ imm = #-0x6db0  // CALL memcpy
    a5e8: e1a00007     	mov	r0, r7
    a5ec: e1a03009     	mov	r3, r9
    a5f0: e1a01006     	mov	r1, r6
    a5f4: e1a02008     	mov	r2, r8
    a5f8: ebffe55e     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6a88  // CALL post
    a5fc: e5d50050     	ldrb	r0, [r5, #0x50]
    a600: e3500000     	cmp	r0, #0
    a604: 1affffe6     	bne	0xa5a4 <_getAllAdcs+0x32c> @ imm = #-0x68
    a608: e1a00004     	mov	r0, r4
    a60c: ed9f0a85     	vldr	s0, [pc, #532]          @ 0xa828 <_getAllAdcs+0x5b0>  // f32=109
    a610: ebffe474     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x6e30  // CALL readFromSharedMem
    a614: e5d4504c     	ldrb	r5, [r4, #0x4c]
    a618: eeb75a00     	vmov.f32	s10, #1.000000e+00
    a61c: e3550000     	cmp	r5, #0
    a620: 059450b4     	ldreq	r5, [r4, #0xb4]
    a624: 05d55007     	ldrbeq	r5, [r5, #0x7]
    a628: ee755a40     	vsub.f32	s11, s10, s0
    a62c: eebd6ae5     	vcvt.s32.f32	s12, s11
    a630: 0e164a10     	vmoveq	r4, s12
    a634: 00050495     	muleq	r5, r5, r4
    a638: 0e065a10     	vmoveq	s12, r5
    a63c: eeb80ac6     	vcvt.f32.s32	s0, s12
    a640: ebffe4cb     	bl	0x3974 <.plt+0x278>     @ imm = #-0x6cd4  // CALL _setPlusLed
    a644: e28ddf75     	add	sp, sp, #468
    a648: ecbd8b02     	vpop	{d8}
    a64c: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    a650: e3520000     	cmp	r2, #0
    a654: 0affffbe     	beq	0xa554 <_getAllAdcs+0x2dc> @ imm = #-0x108
    a658: e1a00004     	mov	r0, r4
    a65c: ed9f0a72     	vldr	s0, [pc, #456]          @ 0xa82c <_getAllAdcs+0x5b4>  // f32=105
    a660: ebffe460     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x6e80  // CALL readFromSharedMem
    a664: e1a00004     	mov	r0, r4
    a668: eeb08a40     	vmov.f32	s16, s0
    a66c: ed9f0a6d     	vldr	s0, [pc, #436]          @ 0xa828 <_getAllAdcs+0x5b0>  // f32=109
    a670: ebffe45c     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x6e90  // CALL readFromSharedMem
    a674: eebd2ac8     	vcvt.s32.f32	s4, s16
    a678: ee123a10     	vmov	r3, s4
    a67c: e3530000     	cmp	r3, #0
    a680: eefd2ac0     	vcvt.s32.f32	s5, s0
    a684: ee127a90     	vmov	r7, s5
    a688: ca000060     	bgt	0xa810 <_getAllAdcs+0x598> @ imm = #0x180
    a68c: e59fe1b0     	ldr	lr, [pc, #0x1b0]        @ 0xa844 <_getAllAdcs+0x5cc>  // u32=0x1cd20; f32?=1.65420481e-40
    a690: e08f900e     	add	r9, pc, lr
    a694: e5990078     	ldr	r0, [r9, #0x78]
    a698: e1500007     	cmp	r0, r7
    a69c: ba00005b     	blt	0xa810 <_getAllAdcs+0x598> @ imm = #0x16c
    a6a0: e59f51a0     	ldr	r5, [pc, #0x1a0]        @ 0xa848 <_getAllAdcs+0x5d0>  // u32=0x1cd00; f32?=1.6537564e-40
    a6a4: e3a02000     	mov	r2, #0
    a6a8: e3a01069     	mov	r1, #105
    a6ac: e1a00004     	mov	r0, r4
    a6b0: e08f8005     	add	r8, pc, r5
    a6b4: e5887078     	str	r7, [r8, #0x78]
    a6b8: ebffe50a     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6bd8  // CALL writeToSharedMem
    a6bc: eaffffa4     	b	0xa554 <_getAllAdcs+0x2dc> @ imm = #-0x170
    a6c0: e5d460bc     	ldrb	r6, [r4, #0xbc]
    a6c4: e3560000     	cmp	r6, #0
    a6c8: 1a00004a     	bne	0xa7f8 <_getAllAdcs+0x580> @ imm = #0x128
    a6cc: e5d4e0bd     	ldrb	lr, [r4, #0xbd]
    a6d0: e35e0000     	cmp	lr, #0
    a6d4: 1a000041     	bne	0xa7e0 <_getAllAdcs+0x568> @ imm = #0x104
    a6d8: e5d410be     	ldrb	r1, [r4, #0xbe]
    a6dc: e3510000     	cmp	r1, #0
    a6e0: 1a000038     	bne	0xa7c8 <_getAllAdcs+0x550> @ imm = #0xe0
    a6e4: e5d4c0bf     	ldrb	r12, [r4, #0xbf]
    a6e8: e35c0000     	cmp	r12, #0
    a6ec: 0affff40     	beq	0xa3f4 <_getAllAdcs+0x17c> @ imm = #-0x300
    a6f0: e3000bbe     	movw	r0, #0xbbe
    a6f4: e3a01035     	mov	r1, #53
    a6f8: e19420b0     	ldrh	r2, [r4, r0]
    a6fc: e1a00004     	mov	r0, r4
    a700: ebffe4f8     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6c20  // CALL writeToSharedMem
    a704: eaffff3a     	b	0xa3f4 <_getAllAdcs+0x17c> @ imm = #-0x318
    a708: e1a00004     	mov	r0, r4
    a70c: ed9f0a47     	vldr	s0, [pc, #284]          @ 0xa830 <_getAllAdcs+0x5b8>  // f32=117
    a710: ebffe434     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x6f30  // CALL readFromSharedMem
    a714: e59f0130     	ldr	r0, [pc, #0x130]        @ 0xa84c <_getAllAdcs+0x5d4>  // u32=0x1cc98; f32?=1.65229905e-40
    a718: e08f5000     	add	r5, pc, r0
    a71c: e595c074     	ldr	r12, [r5, #0x74]
    a720: eefd1ac0     	vcvt.s32.f32	s3, s0
    a724: ee118a90     	vmov	r8, s3
    a728: e15c0008     	cmp	r12, r8
    a72c: a5858074     	strge	r8, [r5, #0x74]
    a730: a594c0b4     	ldrge	r12, [r4, #0xb4]
    a734: aaffff80     	bge	0xa53c <_getAllAdcs+0x2c4> @ imm = #-0x200
    a738: eeb70a00     	vmov.f32	s0, #1.000000e+00
    a73c: ebffe4e3     	bl	0x3ad0 <.plt+0x3d4>     @ imm = #-0x6c74  // CALL _setTriggerOut
    a740: e59400d0     	ldr	r0, [r4, #0xd0]
    a744: e5858074     	str	r8, [r5, #0x74]
    a748: eeb20b00     	vmov.f64	d0, #8.000000e+00
    a74c: ebffe476     	bl	0x392c <.plt+0x230>     @ imm = #-0x6e28  // CALL clock_delay
    a750: e594c0b4     	ldr	r12, [r4, #0xb4]
    a754: eaffff78     	b	0xa53c <_getAllAdcs+0x2c4> @ imm = #-0x220
    a758: e1a00004     	mov	r0, r4
    a75c: ed9f0a31     	vldr	s0, [pc, #196]          @ 0xa828 <_getAllAdcs+0x5b0>  // f32=109
    a760: ebffe420     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x6f80  // CALL readFromSharedMem
    a764: e1a00004     	mov	r0, r4
    a768: eef08a40     	vmov.f32	s17, s0
    a76c: ed9f0a2e     	vldr	s0, [pc, #184]          @ 0xa82c <_getAllAdcs+0x5b4>  // f32=105
    a770: ebffe41c     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x6f90  // CALL readFromSharedMem
    a774: eebd3ae8     	vcvt.s32.f32	s6, s17
    a778: ee13ca10     	vmov	r12, s6
    a77c: e35c0000     	cmp	r12, #0
    a780: ca000006     	bgt	0xa7a0 <_getAllAdcs+0x528> @ imm = #0x18
    a784: eefd3ac0     	vcvt.s32.f32	s7, s0
    a788: ee131a90     	vmov	r1, s7
    a78c: e3510000     	cmp	r1, #0
    a790: da000007     	ble	0xa7b4 <_getAllAdcs+0x53c> @ imm = #0x1c
    a794: e5d420ba     	ldrb	r2, [r4, #0xba]
    a798: e3520000     	cmp	r2, #0
    a79c: 1a000004     	bne	0xa7b4 <_getAllAdcs+0x53c> @ imm = #0x10
    a7a0: eeb70a00     	vmov.f32	s0, #1.000000e+00
    a7a4: ebffe4c9     	bl	0x3ad0 <.plt+0x3d4>     @ imm = #-0x6cdc  // CALL _setTriggerOut
    a7a8: e59400d0     	ldr	r0, [r4, #0xd0]
    a7ac: eeb20b00     	vmov.f64	d0, #8.000000e+00
    a7b0: ebffe45d     	bl	0x392c <.plt+0x230>     @ imm = #-0x6e8c  // CALL clock_delay
    a7b4: e3a02000     	mov	r2, #0
    a7b8: e3a01069     	mov	r1, #105
    a7bc: e1a00004     	mov	r0, r4
    a7c0: ebffe4c8     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6ce0  // CALL writeToSharedMem
    a7c4: eaffff62     	b	0xa554 <_getAllAdcs+0x2dc> @ imm = #-0x278
    a7c8: e3005bbe     	movw	r5, #0xbbe
    a7cc: e3a01034     	mov	r1, #52
    a7d0: e19420b5     	ldrh	r2, [r4, r5]
    a7d4: e1a00004     	mov	r0, r4
    a7d8: ebffe4c2     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6cf8  // CALL writeToSharedMem
    a7dc: eaffffc0     	b	0xa6e4 <_getAllAdcs+0x46c> @ imm = #-0x100
    a7e0: e3003bbe     	movw	r3, #0xbbe
    a7e4: e3a01033     	mov	r1, #51
    a7e8: e19420b3     	ldrh	r2, [r4, r3]
    a7ec: e1a00004     	mov	r0, r4
    a7f0: ebffe4bc     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6d10  // CALL writeToSharedMem
    a7f4: eaffffb7     	b	0xa6d8 <_getAllAdcs+0x460> @ imm = #-0x124
    a7f8: e3008bbe     	movw	r8, #0xbbe
    a7fc: e3a01032     	mov	r1, #50
    a800: e19420b8     	ldrh	r2, [r4, r8]
    a804: e1a00004     	mov	r0, r4
    a808: ebffe4b6     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x6d28  // CALL writeToSharedMem
    a80c: eaffffae     	b	0xa6cc <_getAllAdcs+0x454> @ imm = #-0x148
    a810: eeb70a00     	vmov.f32	s0, #1.000000e+00
    a814: ebffe4ad     	bl	0x3ad0 <.plt+0x3d4>     @ imm = #-0x6d4c  // CALL _setTriggerOut
    a818: e59400d0     	ldr	r0, [r4, #0xd0]
    a81c: eeb20b00     	vmov.f64	d0, #8.000000e+00
    a820: ebffe441     	bl	0x392c <.plt+0x230>     @ imm = #-0x6efc  // CALL clock_delay
    a824: eaffff9d     	b	0xa6a0 <_getAllAdcs+0x428> @ imm = #-0x18c
    a828: 00 00 da 42  	.word	0x42da0000
    a82c: 00 00 d2 42  	.word	0x42d20000
    a830: 00 00 ea 42  	.word	0x42ea0000
    a834: 44 d0 01 00  	.word	0x0001d044
    a838: a8 cf 01 00  	.word	0x0001cfa8
    a83c: 4c ce 01 00  	.word	0x0001ce4c
    a840: 78 af 00 00  	.word	0x0000af78
    a844: 20 cd 01 00  	.word	0x0001cd20
    a848: 00 cd 01 00  	.word	0x0001cd00
    a84c: 98 cc 01 00  	.word	0x0001cc98

