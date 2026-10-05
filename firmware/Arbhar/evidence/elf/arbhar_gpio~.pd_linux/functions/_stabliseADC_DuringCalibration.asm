00005578 <_stabliseADC_DuringCalibration>:
    5578: e30031b2     	movw	r3, #0x1b2
    557c: e92d4070     	push	{r4, r5, r6, lr}
    5580: e19040b3     	ldrh	r4, [r0, r3]
    5584: e3540064     	cmp	r4, #100
    5588: 8a000005     	bhi	0x55a4 <_stabliseADC_DuringCalibration+0x2c> @ imm = #0x14
    558c: e3540000     	cmp	r4, #0
    5590: 0a000090     	beq	0x57d8 <_stabliseADC_DuringCalibration+0x260> @ imm = #0x240
    5594: ee074a90     	vmov	s15, r4
    5598: e1a05004     	mov	r5, r4
    559c: eeb85a67     	vcvt.f32.u32	s10, s15
    55a0: ea000002     	b	0x55b0 <_stabliseADC_DuringCalibration+0x38> @ imm = #0x8
    55a4: ed9f5a9d     	vldr	s10, [pc, #628]         @ 0x5820 <_stabliseADC_DuringCalibration+0x2a8>
    55a8: e3a05064     	mov	r5, #100
    55ac: e1a04005     	mov	r4, r5
    55b0: e280ee1b     	add	lr, r0, #432
    55b4: ed900a01     	vldr	s0, [r0, #4]
    55b8: e1dec0b0     	ldrh	r12, [lr]
    55bc: edd06a03     	vldr	s13, [r0, #12]
    55c0: eef05a00     	vmov.f32	s11, #2.000000e+00
    55c4: ed9f6a96     	vldr	s12, [pc, #600]         @ 0x5824 <_stabliseADC_DuringCalibration+0x2ac>
    55c8: ee307a66     	vsub.f32	s14, s0, s13
    55cc: eefd0ac7     	vcvt.s32.f32	s1, s14
    55d0: ee102a90     	vmov	r2, s1
    55d4: eeb40ae5     	vcmpe.f32	s0, s11
    55d8: e3520000     	cmp	r2, #0
    55dc: b2622000     	rsblt	r2, r2, #0
    55e0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    55e4: eeb40ac6     	vcmpe.f32	s0, s12
    55e8: 43a03001     	movmi	r3, #1
    55ec: 53a03000     	movpl	r3, #0
    55f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    55f4: c3833001     	orrgt	r3, r3, #1
    55f8: e3520faf     	cmp	r2, #700
    55fc: c3833001     	orrgt	r3, r3, #1
    5600: e3530000     	cmp	r3, #0
    5604: 0a00007e     	beq	0x5804 <_stabliseADC_DuringCalibration+0x28c> @ imm = #0x1f8
    5608: e1a05105     	lsl	r5, r5, #2
    560c: e2802010     	add	r2, r0, #16
    5610: e2451004     	sub	r1, r5, #4
    5614: e0826005     	add	r6, r2, r5
    5618: e1a03002     	mov	r3, r2
    561c: e1a01121     	lsr	r1, r1, #2
    5620: e2811001     	add	r1, r1, #1
    5624: e2111007     	ands	r1, r1, #7
    5628: 0a000013     	beq	0x567c <_stabliseADC_DuringCalibration+0x104> @ imm = #0x4c
    562c: e3510001     	cmp	r1, #1
    5630: 0a00000e     	beq	0x5670 <_stabliseADC_DuringCalibration+0xf8> @ imm = #0x38
    5634: e3510002     	cmp	r1, #2
    5638: 0a00000b     	beq	0x566c <_stabliseADC_DuringCalibration+0xf4> @ imm = #0x2c
    563c: e3510003     	cmp	r1, #3
    5640: 0a000008     	beq	0x5668 <_stabliseADC_DuringCalibration+0xf0> @ imm = #0x20
    5644: e3510004     	cmp	r1, #4
    5648: 0a000005     	beq	0x5664 <_stabliseADC_DuringCalibration+0xec> @ imm = #0x14
    564c: e3510005     	cmp	r1, #5
    5650: 0a000002     	beq	0x5660 <_stabliseADC_DuringCalibration+0xe8> @ imm = #0x8
    5654: e3510006     	cmp	r1, #6
    5658: 1a000066     	bne	0x57f8 <_stabliseADC_DuringCalibration+0x280> @ imm = #0x198
    565c: eca30a01     	vstmia	r3!, {s0}
    5660: eca30a01     	vstmia	r3!, {s0}
    5664: eca30a01     	vstmia	r3!, {s0}
    5668: eca30a01     	vstmia	r3!, {s0}
    566c: eca30a01     	vstmia	r3!, {s0}
    5670: eca30a01     	vstmia	r3!, {s0}
    5674: e1560003     	cmp	r6, r3
    5678: 0a00000b     	beq	0x56ac <_stabliseADC_DuringCalibration+0x134> @ imm = #0x2c
    567c: e1a01003     	mov	r1, r3
    5680: e2833020     	add	r3, r3, #32
    5684: eca10a01     	vstmia	r1!, {s0}
    5688: ed030a07     	vstr	s0, [r3, #-28]
    568c: ed810a01     	vstr	s0, [r1, #4]
    5690: ed030a05     	vstr	s0, [r3, #-20]
    5694: ed030a04     	vstr	s0, [r3, #-16]
    5698: ed030a03     	vstr	s0, [r3, #-12]
    569c: ed030a02     	vstr	s0, [r3, #-8]
    56a0: ed030a01     	vstr	s0, [r3, #-4]
    56a4: e1560003     	cmp	r6, r3
    56a8: 1afffff3     	bne	0x567c <_stabliseADC_DuringCalibration+0x104> @ imm = #-0x34
    56ac: e28cc001     	add	r12, r12, #1
    56b0: e2453004     	sub	r3, r5, #4
    56b4: ed9f4a5b     	vldr	s8, [pc, #364]          @ 0x5828 <_stabliseADC_DuringCalibration+0x2b0>
    56b8: e0825005     	add	r5, r2, r5
    56bc: e6ff107c     	uxth	r1, r12
    56c0: e1a03123     	lsr	r3, r3, #2
    56c4: e1540001     	cmp	r4, r1
    56c8: e283c001     	add	r12, r3, #1
    56cc: 93a01000     	movls	r1, #0
    56d0: e21c3007     	ands	r3, r12, #7
    56d4: e1ce10b0     	strh	r1, [lr]
    56d8: 0a000019     	beq	0x5744 <_stabliseADC_DuringCalibration+0x1cc> @ imm = #0x64
    56dc: e3530001     	cmp	r3, #1
    56e0: 0a000013     	beq	0x5734 <_stabliseADC_DuringCalibration+0x1bc> @ imm = #0x4c
    56e4: e3530002     	cmp	r3, #2
    56e8: 0a00000f     	beq	0x572c <_stabliseADC_DuringCalibration+0x1b4> @ imm = #0x3c
    56ec: e3530003     	cmp	r3, #3
    56f0: 0a00000b     	beq	0x5724 <_stabliseADC_DuringCalibration+0x1ac> @ imm = #0x2c
    56f4: e3530004     	cmp	r3, #4
    56f8: 0a000007     	beq	0x571c <_stabliseADC_DuringCalibration+0x1a4> @ imm = #0x1c
    56fc: e3530005     	cmp	r3, #5
    5700: 0a000003     	beq	0x5714 <_stabliseADC_DuringCalibration+0x19c> @ imm = #0xc
    5704: e3530006     	cmp	r3, #6
    5708: 1a000036     	bne	0x57e8 <_stabliseADC_DuringCalibration+0x270> @ imm = #0xd8
    570c: ecb21a01     	vldmia	r2!, {s2}
    5710: ee344a01     	vadd.f32	s8, s8, s2
    5714: ecf21a01     	vldmia	r2!, {s3}
    5718: ee344a21     	vadd.f32	s8, s8, s3
    571c: ecb22a01     	vldmia	r2!, {s4}
    5720: ee344a02     	vadd.f32	s8, s8, s4
    5724: ecf22a01     	vldmia	r2!, {s5}
    5728: ee344a22     	vadd.f32	s8, s8, s5
    572c: ecb23a01     	vldmia	r2!, {s6}
    5730: ee344a03     	vadd.f32	s8, s8, s6
    5734: ecf23a01     	vldmia	r2!, {s7}
    5738: e1550002     	cmp	r5, r2
    573c: ee344a23     	vadd.f32	s8, s8, s7
    5740: 0a000013     	beq	0x5794 <_stabliseADC_DuringCalibration+0x21c> @ imm = #0x4c
    5744: e1a0c002     	mov	r12, r2
    5748: ed920a01     	vldr	s0, [r2, #4]
    574c: e2822020     	add	r2, r2, #32
    5750: ecfc4a01     	vldmia	r12!, {s9}
    5754: ed520a05     	vldr	s1, [r2, #-20]
    5758: ee346a24     	vadd.f32	s12, s8, s9
    575c: eddc7a01     	vldr	s15, [r12, #4]
    5760: ed121a04     	vldr	s2, [r2, #-16]
    5764: ed521a03     	vldr	s3, [r2, #-12]
    5768: ee367a00     	vadd.f32	s14, s12, s0
    576c: ed525a02     	vldr	s11, [r2, #-8]
    5770: ed122a01     	vldr	s4, [r2, #-4]
    5774: e1550002     	cmp	r5, r2
    5778: ee772a27     	vadd.f32	s5, s14, s15
    577c: ee323aa0     	vadd.f32	s6, s5, s1
    5780: ee733a01     	vadd.f32	s7, s6, s2
    5784: ee334aa1     	vadd.f32	s8, s7, s3
    5788: ee340a25     	vadd.f32	s0, s8, s11
    578c: ee304a02     	vadd.f32	s8, s0, s4
    5790: 1affffeb     	bne	0x5744 <_stabliseADC_DuringCalibration+0x1cc> @ imm = #-0x54
    5794: eec44a05     	vdiv.f32	s9, s8, s10
    5798: ee365ae4     	vsub.f32	s10, s13, s9
    579c: edc04a6a     	vstr	s9, [r0, #424]
    57a0: eefd0ac5     	vcvt.s32.f32	s1, s10
    57a4: ee103a90     	vmov	r3, s1
    57a8: e2831031     	add	r1, r3, #49
    57ac: e3510062     	cmp	r1, #98
    57b0: 9ef70ae6     	vcvtls.f64.f32	d16, s13
    57b4: 9ddf2b17     	vldrls	d18, [pc, #92]          @ 0x5818 <_stabliseADC_DuringCalibration+0x2a0>
    57b8: 9ef71ac5     	vcvtls.f64.f32	d17, s10
    57bc: 9e410ba2     	vmlals.f64	d16, d17, d18
    57c0: 9ef74be0     	vcvtls.f32.f64	s9, d16
    57c4: eefc6ae4     	vcvt.u32.f32	s13, s9
    57c8: edc04a03     	vstr	s9, [r0, #12]
    57cc: ee162a90     	vmov	r2, s13
    57d0: e1c020ba     	strh	r2, [r0, #10]
    57d4: e8bd8070     	pop	{r4, r5, r6, pc}
    57d8: e3a05001     	mov	r5, #1
    57dc: eeb75a00     	vmov.f32	s10, #1.000000e+00
    57e0: e1a04005     	mov	r4, r5
    57e4: eaffff71     	b	0x55b0 <_stabliseADC_DuringCalibration+0x38> @ imm = #-0x23c
    57e8: e5921000     	ldr	r1, [r2]
    57ec: e2802014     	add	r2, r0, #20
    57f0: ee041a10     	vmov	s8, r1
    57f4: eaffffc4     	b	0x570c <_stabliseADC_DuringCalibration+0x194> @ imm = #-0xf0
    57f8: ed820a00     	vstr	s0, [r2]
    57fc: e2803014     	add	r3, r0, #20
    5800: eaffff95     	b	0x565c <_stabliseADC_DuringCalibration+0xe4> @ imm = #-0x1ac
    5804: e080110c     	add	r1, r0, r12, lsl #2
    5808: e1a05105     	lsl	r5, r5, #2
    580c: e2802010     	add	r2, r0, #16
    5810: ed810a04     	vstr	s0, [r1, #16]
    5814: eaffffa4     	b	0x56ac <_stabliseADC_DuringCalibration+0x134> @ imm = #-0x170
    5818: cd cc cc cc  	.word	0xcccccccd
    581c: cc cc ec bf  	.word	0xbfeccccc
    5820: 00 00 c8 42  	.word	0x42c80000
    5824: 00 d0 7f 45  	.word	0x457fd000
    5828: 00 00 00 00  	.word	0x00000000

