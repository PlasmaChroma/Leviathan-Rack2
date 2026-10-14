000034dc <makingWndArray>:
    34dc: eef76a00     	vmov.f32	s13, #1.000000e+00
    34e0: e59f3420     	ldr	r3, [pc, #0x420]        @ 0x3908 <makingWndArray+0x42c>  // u32=0x16b10; f32?=1.30242284e-40
    34e4: e59f2420     	ldr	r2, [pc, #0x420]        @ 0x390c <makingWndArray+0x430>  // u32=0x158; f32?=4.82046672e-43
    34e8: e08f3003     	add	r3, pc, r3
    34ec: e59f041c     	ldr	r0, [pc, #0x41c]        @ 0x3910 <makingWndArray+0x434>  // u32=0x467c; f32?=2.52850295e-41
    34f0: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    34f4: e08f4000     	add	r4, pc, r0
    34f8: e793c002     	ldr	r12, [r3, r2]
    34fc: e2845b02     	add	r5, r4, #2048
    3500: eddf3bf8     	vldr	d19, [pc, #992]         @ 0x38e8 <makingWndArray+0x40c>  // f64=0.02
    3504: e285000c     	add	r0, r5, #12
    3508: e28c6bcb     	add	r6, r12, #207872
    350c: e3e0e031     	mvn	lr, #49
    3510: e28650bc     	add	r5, r6, #188
    3514: ed9f6af9     	vldr	s12, [pc, #996]         @ 0x3900 <makingWndArray+0x424>  // f32=200
    3518: eddf2bf4     	vldr	d18, [pc, #976]         @ 0x38f0 <makingWndArray+0x414>  // f64=0.90000000000000002
    351c: eddf1bf5     	vldr	d17, [pc, #980]         @ 0x38f8 <makingWndArray+0x41c>  // f64=0.10000000000000001
    3520: ee07ea90     	vmov	s15, lr
    3524: eeb85be7     	vcvt.f64.s32	d5, s15
    3528: ee255b23     	vmul.f64	d5, d5, d19
    352c: eeb70bc5     	vcvt.f32.f64	s0, d5
    3530: eeb14a40     	vneg.f32	s8, s0
    3534: ee705a26     	vadd.f32	s11, s0, s13
    3538: ee764ac0     	vsub.f32	s9, s13, s0
    353c: eef74ac4     	vcvt.f64.f32	d20, s8
    3540: eeb50ac0     	vcmpe.f32	s0, #0
    3544: ee241ba2     	vmul.f64	d1, d20, d18
    3548: eef1fa10     	vmrs	APSR_nzcv, fpscr
    354c: ee640aa5     	vmul.f32	s1, s9, s11
    3550: ee602a06     	vmul.f32	s5, s0, s12
    3554: 9a00005b     	bls	0x36c8 <makingWndArray+0x1ec> @ imm = #0x16c
    3558: e59f13b4     	ldr	r1, [pc, #0x3b4]        @ 0x3914 <makingWndArray+0x438>  // u32=0x4608; f32?=2.51224789e-41
    355c: e1a0700c     	mov	r7, r12
    3560: eddf1ae7     	vldr	s3, [pc, #924]          @ 0x3904 <makingWndArray+0x428>  // f32=0
    3564: e3004202     	movw	r4, #0x202
    3568: e08f2001     	add	r2, pc, r1
    356c: e3a08001     	mov	r8, #1
    3570: e3001203     	movw	r1, #0x203
    3574: e4923004     	ldr	r3, [r2], #4
    3578: ece71a01     	vstmia	r7!, {s3}
    357c: ea000039     	b	0x3668 <makingWndArray+0x18c> @ imm = #0xe4
    3580: e3580f7e     	cmp	r8, #504
    3584: def03a66     	vmovle.f32	s7, s13
    3588: ca000048     	bgt	0x36b0 <makingWndArray+0x1d4> @ imm = #0x120
    358c: ee018a10     	vmov	s2, r8
    3590: e2883001     	add	r3, r8, #1
    3594: e1a06002     	mov	r6, r2
    3598: ecf64a01     	vldmia	r6!, {s9}
    359c: eeb82ac1     	vcvt.f32.s32	s4, s2
    35a0: ee327a22     	vadd.f32	s14, s4, s5
    35a4: eebd3ac7     	vcvt.s32.f32	s6, s14
    35a8: ee138a10     	vmov	r8, s6
    35ac: ee607a24     	vmul.f32	s15, s0, s9
    35b0: e1580001     	cmp	r8, r1
    35b4: a1a08001     	movge	r8, r1
    35b8: e0802108     	add	r2, r0, r8, lsl #2
    35bc: ed925a00     	vldr	s10, [r2]
    35c0: ee407a85     	vmla.f32	s15, s1, s10
    35c4: eef47ae6     	vcmpe.f32	s15, s13
    35c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    35cc: 8ef07a66     	vmovhi.f32	s15, s13
    35d0: eef57ac0     	vcmpe.f32	s15, #0
    35d4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    35d8: bef07a61     	vmovlt.f32	s15, s3
    35dc: e3530009     	cmp	r3, #9
    35e0: ee274aa3     	vmul.f32	s8, s15, s7
    35e4: eca74a01     	vstmia	r7!, {s8}
    35e8: da00002b     	ble	0x369c <makingWndArray+0x1c0> @ imm = #0xac
    35ec: e3530f7e     	cmp	r3, #504
    35f0: c0442003     	subgt	r2, r4, r3
    35f4: def03a66     	vmovle.f32	s7, s13
    35f8: ce042a10     	vmovgt	s8, r2
    35fc: cef80bc4     	vcvtgt.f64.s32	d16, s8
    3600: ce600ba1     	vmulgt.f64	d16, d16, d17
    3604: cef73be0     	vcvtgt.f32.f64	s7, d16
    3608: ee013a10     	vmov	s2, r3
    360c: e2838001     	add	r8, r3, #1
    3610: e1a02006     	mov	r2, r6
    3614: ecf25a01     	vldmia	r2!, {s11}
    3618: eeb82ac1     	vcvt.f32.s32	s4, s2
    361c: ee724a22     	vadd.f32	s9, s4, s5
    3620: eebd7ae4     	vcvt.s32.f32	s14, s9
    3624: ee173a10     	vmov	r3, s14
    3628: ee203a25     	vmul.f32	s6, s0, s11
    362c: e1530001     	cmp	r3, r1
    3630: a1a03001     	movge	r3, r1
    3634: e0803103     	add	r3, r0, r3, lsl #2
    3638: edd37a00     	vldr	s15, [r3]
    363c: ee003aa7     	vmla.f32	s6, s1, s15
    3640: eeb43ae6     	vcmpe.f32	s6, s13
    3644: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3648: 8eb03a66     	vmovhi.f32	s6, s13
    364c: eeb53ac0     	vcmpe.f32	s6, #0
    3650: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3654: beb03a61     	vmovlt.f32	s6, s3
    3658: e1580001     	cmp	r8, r1
    365c: ee235a23     	vmul.f32	s10, s6, s7
    3660: eca75a01     	vstmia	r7!, {s10}
    3664: 0a000006     	beq	0x3684 <makingWndArray+0x1a8> @ imm = #0x18
    3668: e3580009     	cmp	r8, #9
    366c: caffffc3     	bgt	0x3580 <makingWndArray+0xa4> @ imm = #-0xf4
    3670: ee058a90     	vmov	s11, r8
    3674: eef8abe5     	vcvt.f64.s32	d26, s11
    3678: ee6a0ba1     	vmul.f64	d16, d26, d17
    367c: eef73be0     	vcvt.f32.f64	s7, d16
    3680: eaffffc1     	b	0x358c <makingWndArray+0xb0> @ imm = #-0xfc
    3684: e28ccb02     	add	r12, r12, #2048
    3688: e28ee001     	add	lr, lr, #1
    368c: e28cc00c     	add	r12, r12, #12
    3690: e155000c     	cmp	r5, r12
    3694: 1affffa1     	bne	0x3520 <makingWndArray+0x44> @ imm = #-0x17c
    3698: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    369c: ee033a90     	vmov	s7, r3
    36a0: eef8bbe3     	vcvt.f64.s32	d27, s7
    36a4: ee6b0ba1     	vmul.f64	d16, d27, d17
    36a8: eef73be0     	vcvt.f32.f64	s7, d16
    36ac: eaffffd5     	b	0x3608 <makingWndArray+0x12c> @ imm = #-0xac
    36b0: e0446008     	sub	r6, r4, r8
    36b4: ee036a90     	vmov	s7, r6
    36b8: eef89be3     	vcvt.f64.s32	d25, s7
    36bc: ee690ba1     	vmul.f64	d16, d25, d17
    36c0: eef73be0     	vcvt.f32.f64	s7, d16
    36c4: eaffffb0     	b	0x358c <makingWndArray+0xb0> @ imm = #-0x140
    36c8: e59f7248     	ldr	r7, [pc, #0x248]        @ 0x3918 <makingWndArray+0x43c>  // u32=0x449c; f32?=2.46124062e-41
    36cc: e1a0900c     	mov	r9, r12
    36d0: e59f3244     	ldr	r3, [pc, #0x244]        @ 0x391c <makingWndArray+0x440>  // u32=0x648c; f32?=3.60694225e-41
    36d4: e08f8007     	add	r8, pc, r7
    36d8: ed9f2a89     	vldr	s4, [pc, #548]          @ 0x3904 <makingWndArray+0x428>  // f32=0
    36dc: e08f4003     	add	r4, pc, r3
    36e0: e2882b02     	add	r2, r8, #2048
    36e4: e3a03000     	mov	r3, #0
    36e8: e282200c     	add	r2, r2, #12
    36ec: e2446efe     	sub	r6, r4, #4064
    36f0: e3008202     	movw	r8, #0x202
    36f4: e3007203     	movw	r7, #0x203
    36f8: ea000072     	b	0x38c8 <makingWndArray+0x3ec> @ imm = #0x1c8
    36fc: e3530f7e     	cmp	r3, #504
    3700: c0481003     	subgt	r1, r8, r3
    3704: def02a66     	vmovle.f32	s5, s13
    3708: ce041a10     	vmovgt	s8, r1
    370c: cef80bc4     	vcvtgt.f64.s32	d16, s8
    3710: ce600ba1     	vmulgt.f64	d16, d16, d17
    3714: cef72be0     	vcvtgt.f32.f64	s5, d16
    3718: ecb27a01     	vldmia	r2!, {s14}
    371c: e1a04006     	mov	r4, r6
    3720: e2833001     	add	r3, r3, #1
    3724: e1a01009     	mov	r1, r9
    3728: ecb43a01     	vldmia	r4!, {s6}
    372c: ee657a87     	vmul.f32	s15, s11, s14
    3730: eef75ac3     	vcvt.f64.f32	d21, s6
    3734: eef76ae7     	vcvt.f64.f32	d22, s15
    3738: ee456b81     	vmla.f64	d22, d21, d1
    373c: eeb75be6     	vcvt.f32.f64	s10, d22
    3740: eeb55ac0     	vcmpe.f32	s10, #0
    3744: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3748: beb05a42     	vmovlt.f32	s10, s4
    374c: e3530009     	cmp	r3, #9
    3750: ee250a22     	vmul.f32	s0, s10, s5
    3754: eca10a01     	vstmia	r1!, {s0}
    3758: da00007f     	ble	0x395c <makingWndArray+0x480> @ imm = #0x1fc
    375c: e3530f7e     	cmp	r3, #504
    3760: c0486003     	subgt	r6, r8, r3
    3764: def02a66     	vmovle.f32	s5, s13
    3768: ce006a10     	vmovgt	s0, r6
    376c: cef86bc0     	vcvtgt.f64.s32	d22, s0
    3770: ce666ba1     	vmulgt.f64	d22, d22, d17
    3774: cef72be6     	vcvtgt.f32.f64	s5, d22
    3778: edd20a00     	vldr	s1, [r2]
    377c: e2839001     	add	r9, r3, #1
    3780: edd44a00     	vldr	s9, [r4]
    3784: ee254aa0     	vmul.f32	s8, s11, s1
    3788: eef79ae4     	vcvt.f64.f32	d25, s9
    378c: eef7aac4     	vcvt.f64.f32	d26, s8
    3790: ee49ab81     	vmla.f64	d26, d25, d1
    3794: eeb77bea     	vcvt.f32.f64	s14, d26
    3798: eeb57ac0     	vcmpe.f32	s14, #0
    379c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    37a0: beb07a42     	vmovlt.f32	s14, s4
    37a4: e3590009     	cmp	r9, #9
    37a8: ee273a22     	vmul.f32	s6, s14, s5
    37ac: ed813a00     	vstr	s6, [r1]
    37b0: da000064     	ble	0x3948 <makingWndArray+0x46c> @ imm = #0x190
    37b4: e3590f7e     	cmp	r9, #504
    37b8: c0489009     	subgt	r9, r8, r9
    37bc: def04a66     	vmovle.f32	s9, s13
    37c0: ce039a10     	vmovgt	s6, r9
    37c4: cef8abc3     	vcvtgt.f64.s32	d26, s6
    37c8: ce6aaba1     	vmulgt.f64	d26, d26, d17
    37cc: cef74bea     	vcvtgt.f32.f64	s9, d26
    37d0: ed925a01     	vldr	s10, [r2, #4]
    37d4: e2836002     	add	r6, r3, #2
    37d8: ed940a01     	vldr	s0, [r4, #4]
    37dc: ee650a85     	vmul.f32	s1, s11, s10
    37e0: eef7dac0     	vcvt.f64.f32	d29, s0
    37e4: eef7eae0     	vcvt.f64.f32	d30, s1
    37e8: ee4deb81     	vmla.f64	d30, d29, d1
    37ec: eeb74bee     	vcvt.f32.f64	s8, d30
    37f0: eeb54ac0     	vcmpe.f32	s8, #0
    37f4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    37f8: beb04a42     	vmovlt.f32	s8, s4
    37fc: e3560009     	cmp	r6, #9
    3800: ee247a24     	vmul.f32	s14, s8, s9
    3804: ed817a01     	vstr	s14, [r1, #4]
    3808: da000049     	ble	0x3934 <makingWndArray+0x458> @ imm = #0x124
    380c: e3560f7e     	cmp	r6, #504
    3810: c0486006     	subgt	r6, r8, r6
    3814: def04a66     	vmovle.f32	s9, s13
    3818: ce076a10     	vmovgt	s14, r6
    381c: cef8ebc7     	vcvtgt.f64.s32	d30, s14
    3820: ce6eeba1     	vmulgt.f64	d30, d30, d17
    3824: cef74bee     	vcvtgt.f32.f64	s9, d30
    3828: ed923a02     	vldr	s6, [r2, #8]
    382c: e2839003     	add	r9, r3, #3
    3830: edd47a02     	vldr	s15, [r4, #8]
    3834: ee255a83     	vmul.f32	s10, s11, s6
    3838: eef75ae7     	vcvt.f64.f32	d21, s15
    383c: eef70ac5     	vcvt.f64.f32	d16, s10
    3840: ee450b81     	vmla.f64	d16, d21, d1
    3844: eeb70be0     	vcvt.f32.f64	s0, d16
    3848: eeb50ac0     	vcmpe.f32	s0, #0
    384c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    3850: beb00a42     	vmovlt.f32	s0, s4
    3854: e3590009     	cmp	r9, #9
    3858: ee600a24     	vmul.f32	s1, s0, s9
    385c: edc10a02     	vstr	s1, [r1, #8]
    3860: da00002e     	ble	0x3920 <makingWndArray+0x444> @ imm = #0xb8
    3864: e3590f7e     	cmp	r9, #504
    3868: c0489009     	subgt	r9, r8, r9
    386c: def04a66     	vmovle.f32	s9, s13
    3870: ce009a90     	vmovgt	s1, r9
    3874: cef80be0     	vcvtgt.f64.s32	d16, s1
    3878: ce600ba1     	vmulgt.f64	d16, d16, d17
    387c: cef74be0     	vcvtgt.f32.f64	s9, d16
    3880: ed927a03     	vldr	s14, [r2, #12]
    3884: e2833004     	add	r3, r3, #4
    3888: e2822010     	add	r2, r2, #16
    388c: e2846010     	add	r6, r4, #16
    3890: e2819010     	add	r9, r1, #16
    3894: ed943a03     	vldr	s6, [r4, #12]
    3898: ee657a87     	vmul.f32	s15, s11, s14
    389c: eef78ac3     	vcvt.f64.f32	d24, s6
    38a0: eef70ae7     	vcvt.f64.f32	d16, s15
    38a4: ee480b81     	vmla.f64	d16, d24, d1
    38a8: eeb75be0     	vcvt.f32.f64	s10, d16
    38ac: eeb55ac0     	vcmpe.f32	s10, #0
    38b0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    38b4: beb05a42     	vmovlt.f32	s10, s4
    38b8: e1530007     	cmp	r3, r7
    38bc: ee254a24     	vmul.f32	s8, s10, s9
    38c0: ed814a03     	vstr	s8, [r1, #12]
    38c4: 0affff6e     	beq	0x3684 <makingWndArray+0x1a8> @ imm = #-0x248
    38c8: e3530009     	cmp	r3, #9
    38cc: caffff8a     	bgt	0x36fc <makingWndArray+0x220> @ imm = #-0x1d8
    38d0: ee023a90     	vmov	s5, r3
    38d4: eef80be2     	vcvt.f64.s32	d16, s5
    38d8: ee204ba1     	vmul.f64	d4, d16, d17
    38dc: eef72bc4     	vcvt.f32.f64	s5, d4
    38e0: eaffff8c     	b	0x3718 <makingWndArray+0x23c> @ imm = #-0x1d0
    38e4: e320f000     	nop
    38e8: 7b 14 ae 47  	.word	0x47ae147b
    38ec: e1 7a 94 3f  	.word	0x3f947ae1
    38f0: cd cc cc cc  	.word	0xcccccccd
    38f4: cc cc ec 3f  	.word	0x3feccccc
    38f8: 9a 99 99 99  	.word	0x9999999a
    38fc: 99 99 b9 3f  	.word	0x3fb99999
    3900: 00 00 48 43  	.word	0x43480000
    3904: 00 00 00 00  	.word	0x00000000
    3908: 10 6b 01 00  	.word	0x00016b10
    390c: 58 01 00 00  	.word	0x00000158
    3910: 7c 46 00 00  	.word	0x0000467c
    3914: 08 46 00 00  	.word	0x00004608
    3918: 9c 44 00 00  	.word	0x0000449c
    391c: 8c 64 00 00  	.word	0x0000648c
    3920: ee049a10     	vmov	s8, r9
    3924: eef86bc4     	vcvt.f64.s32	d22, s8
    3928: ee667ba1     	vmul.f64	d23, d22, d17
    392c: eef74be7     	vcvt.f32.f64	s9, d23
    3930: eaffffd2     	b	0x3880 <makingWndArray+0x3a4> @ imm = #-0xb8
    3934: ee046a90     	vmov	s9, r6
    3938: eef8fbe4     	vcvt.f64.s32	d31, s9
    393c: ee6f4ba1     	vmul.f64	d20, d31, d17
    3940: eef74be4     	vcvt.f32.f64	s9, d20
    3944: eaffffb7     	b	0x3828 <makingWndArray+0x34c> @ imm = #-0x124
    3948: ee079a90     	vmov	s15, r9
    394c: eef8bbe7     	vcvt.f64.s32	d27, s15
    3950: ee6bcba1     	vmul.f64	d28, d27, d17
    3954: eef74bec     	vcvt.f32.f64	s9, d28
    3958: eaffff9c     	b	0x37d0 <makingWndArray+0x2f4> @ imm = #-0x190
    395c: ee043a90     	vmov	s9, r3
    3960: eef87be4     	vcvt.f64.s32	d23, s9
    3964: ee678ba1     	vmul.f64	d24, d23, d17
    3968: eef72be8     	vcvt.f32.f64	s5, d24
    396c: eaffff81     	b	0x3778 <makingWndArray+0x29c> @ imm = #-0x1fc

