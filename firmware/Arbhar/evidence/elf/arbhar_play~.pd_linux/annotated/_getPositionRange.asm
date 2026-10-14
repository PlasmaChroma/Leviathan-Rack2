00006868 <_getPositionRange>:
    6868: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    686c: e2804a02     	add	r4, r0, #8192
    6870: ed9f7ae6     	vldr	s14, [pc, #920]         @ 0x6c10 <_getPositionRange+0x3a8>  // f32=32
    6874: e1a06000     	mov	r6, r0
    6878: e5943894     	ldr	r3, [r4, #0x894]
    687c: e1a05001     	mov	r5, r1
    6880: eddf6ae3     	vldr	s13, [pc, #908]         @ 0x6c14 <_getPositionRange+0x3ac>  // f32=0
    6884: ed936ad9     	vldr	s12, [r3, #868]
    6888: edd37a0f     	vldr	s15, [r3, #60]
    688c: ed2d8b04     	vpush	{d8, d9}
    6890: ee567a07     	vnmls.f32	s15, s12, s14
    6894: eef47ae6     	vcmpe.f32	s15, s13
    6898: eef1fa10     	vmrs	APSR_nzcv, fpscr
    689c: 4a000079     	bmi	0x6a88 <_getPositionRange+0x220> @ imm = #0x1e4
    68a0: eeb70a00     	vmov.f32	s0, #1.000000e+00
    68a4: edd37a9f     	vldr	s15, [r3, #636]
    68a8: e2848e63     	add	r8, r4, #1584
    68ac: e2847e6a     	add	r7, r4, #1696
    68b0: e3a0c5fe     	mov	r12, #1065353216
    68b4: e59f2370     	ldr	r2, [pc, #0x370]        @ 0x6c2c <_getPositionRange+0x3c4>  // u32=0x44f0; f32?=2.47301153e-41
    68b8: eddf0ad6     	vldr	s1, [pc, #856]          @ 0x6c18 <_getPositionRange+0x3b0>  // f32=69
    68bc: e08f0002     	add	r0, pc, r2
    68c0: eeb66a00     	vmov.f32	s12, #5.000000e-01
    68c4: ee271a86     	vmul.f32	s2, s15, s12
    68c8: ee210a00     	vmul.f32	s0, s2, s0
    68cc: eebd2ac0     	vcvt.s32.f32	s4, s0
    68d0: ed880a01     	vstr	s0, [r8, #4]
    68d4: e587c004     	str	r12, [r7, #0x4]
    68d8: ee12ea10     	vmov	lr, s4
    68dc: e28e1050     	add	r1, lr, #80
    68e0: e284ee6a     	add	lr, r4, #1696
    68e4: e1a08081     	lsl	r8, r1, #1
    68e8: e1a0c00e     	mov	r12, lr
    68ec: e2881001     	add	r1, r8, #1
    68f0: ee018a90     	vmov	s3, r8
    68f4: e2848e6b     	add	r8, r4, #1712
    68f8: ee021a90     	vmov	s5, r1
    68fc: eeb83ae1     	vcvt.f32.s32	s6, s3
    6900: eebd4ac3     	vcvt.s32.f32	s8, s6
    6904: ee147a10     	vmov	r7, s8
    6908: eeb85ae2     	vcvt.f32.s32	s10, s5
    690c: eefd5ac5     	vcvt.s32.f32	s11, s10
    6910: e0832107     	add	r2, r3, r7, lsl #2
    6914: e3a07000     	mov	r7, #0
    6918: ed928a00     	vldr	s16, [r2]
    691c: ee152a90     	vmov	r2, s11
    6920: ee783a20     	vadd.f32	s7, s16, s1
    6924: eefd4ae3     	vcvt.s32.f32	s9, s7
    6928: ee141a90     	vmov	r1, s9
    692c: e0801101     	add	r1, r0, r1, lsl #2
    6930: e5911000     	ldr	r1, [r1]
    6934: e58e1008     	str	r1, [lr, #0x8]
    6938: e083e102     	add	lr, r3, r2, lsl #2
    693c: edde8a00     	vldr	s17, [lr]
    6940: ee389aa0     	vadd.f32	s18, s17, s1
    6944: eefd9ac9     	vcvt.s32.f32	s19, s18
    6948: ee192a90     	vmov	r2, s19
    694c: e0800102     	add	r0, r0, r2, lsl #2
    6950: e5901000     	ldr	r1, [r0]
    6954: e58c100c     	str	r1, [r12, #0xc]
    6958: e5887000     	str	r7, [r8]
    695c: edd37a67     	vldr	s15, [r3, #412]
    6960: edd30a24     	vldr	s1, [r3, #144]
    6964: eef77ae7     	vcvt.f64.f32	d23, s15
    6968: edd36a64     	vldr	s13, [r3, #400]
    696c: ed937a0a     	vldr	s14, [r3, #40]
    6970: eefc7ae0     	vcvt.u32.f32	s15, s1
    6974: eddf5b99     	vldr	d21, [pc, #612]         @ 0x6be0 <_getPositionRange+0x378>  // f64=4.8000000000000007
    6978: eddf6b9a     	vldr	d22, [pc, #616]         @ 0x6be8 <_getPositionRange+0x380>  // f64=0.024420029999999999
    697c: eef74ae6     	vcvt.f64.f32	d20, s13
    6980: ee17ca90     	vmov	r12, s15
    6984: eef78ac7     	vcvt.f64.f32	d24, s14
    6988: ee649ba5     	vmul.f64	d25, d20, d21
    698c: e31c00ff     	tst	r12, #255
    6990: ee67aba5     	vmul.f64	d26, d23, d21
    6994: ee68bba6     	vmul.f64	d27, d24, d22
    6998: eeb78be9     	vcvt.f32.f64	s16, d25
    699c: eef78bea     	vcvt.f32.f64	s17, d26
    69a0: eeb79beb     	vcvt.f32.f64	s18, d27
    69a4: 1a000077     	bne	0x6b88 <_getPositionRange+0x320> @ imm = #0x1dc
    69a8: ed9f0a9b     	vldr	s0, [pc, #620]          @ 0x6c1c <_getPositionRange+0x3b4>  // f32=480000
    69ac: ee382a28     	vadd.f32	s4, s16, s17
    69b0: eeb42ac0     	vcmpe.f32	s4, s0
    69b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    69b8: ce388a00     	vaddgt.f32	s16, s16, s0
    69bc: eef58ac0     	vcmpe.f32	s17, #0
    69c0: ce388a42     	vsubgt.f32	s16, s16, s4
    69c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    69c8: ca00005f     	bgt	0x6b4c <_getPositionRange+0x2e4> @ imm = #0x17c
    69cc: eefd1ac8     	vcvt.s32.f32	s3, s16
    69d0: ee11ca90     	vmov	r12, s3
    69d4: edc51a02     	vstr	s3, [r5, #8]
    69d8: ed933a65     	vldr	s6, [r3, #404]
    69dc: e2863da3     	add	r3, r6, #10432
    69e0: e2834004     	add	r4, r3, #4
    69e4: eddfcb81     	vldr	d28, [pc, #516]         @ 0x6bf0 <_getPositionRange+0x388>  // f64=1.4400000000000002
    69e8: eef7dac3     	vcvt.f64.f32	d29, s6
    69ec: ed9f4a8b     	vldr	s8, [pc, #556]          @ 0x6c20 <_getPositionRange+0x3b8>  // f32=128
    69f0: eddf2a8b     	vldr	s5, [pc, #556]          @ 0x6c24 <_getPositionRange+0x3bc>  // f32=144000
    69f4: ee6debac     	vmul.f64	d30, d29, d28
    69f8: eeb75bee     	vcvt.f32.f64	s10, d30
    69fc: eeb45ac4     	vcmpe.f32	s10, s8
    6a00: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6a04: beb05a44     	vmovlt.f32	s10, s8
    6a08: eeb45ae2     	vcmpe.f32	s10, s5
    6a0c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6a10: 8eb05a62     	vmovhi.f32	s10, s5
    6a14: eebd8ac5     	vcvt.s32.f32	s16, s10
    6a18: ee187a10     	vmov	r7, s16
    6a1c: e087800c     	add	r8, r7, r12
    6a20: e1580004     	cmp	r8, r4
    6a24: 95857004     	strls	r7, [r5, #0x4]
    6a28: 82868da3     	addhi	r8, r6, #10432
    6a2c: 828cc001     	addhi	r12, r12, #1
    6a30: 82888008     	addhi	r8, r8, #8
    6a34: 8048c10c     	subhi	r12, r8, r12, lsl #2
    6a38: 8585c004     	strhi	r12, [r5, #0x4]
    6a3c: ebffef20     	bl	0x26c4 <.plt+0x2e4>     @ imm = #-0x4380  // CALL random
    6a40: e3a0e064     	mov	lr, #100
    6a44: e1a06000     	mov	r6, r0
    6a48: e308051f     	movw	r0, #0x851f
    6a4c: e34501eb     	movt	r0, #0x51eb
    6a50: e1a02fc6     	asr	r2, r6, #31
    6a54: e0c13690     	smull	r3, r1, r0, r6
    6a58: e1a00005     	mov	r0, r5
    6a5c: e062c2c1     	rsb	r12, r2, r1, asr #5
    6a60: e0636c9e     	mls	r3, lr, r12, r6
    6a64: ee033a90     	vmov	s7, r3
    6a68: eef84ae3     	vcvt.f32.s32	s9, s7
    6a6c: eef44ac9     	vcmpe.f32	s9, s18
    6a70: ecbd8b04     	vpop	{d8, d9}
    6a74: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6a78: 43a03001     	movmi	r3, #1
    6a7c: 53e03000     	mvnpl	r3, #0
    6a80: e585303c     	str	r3, [r5, #0x3c]
    6a84: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    6a88: eddf0a66     	vldr	s1, [pc, #408]          @ 0x6c28 <_getPositionRange+0x3c0>  // f32=4095
    6a8c: eddf0b59     	vldr	d16, [pc, #356]         @ 0x6bf8 <_getPositionRange+0x390>  // f64=0.048840000000000001
    6a90: ee371aa0     	vadd.f32	s2, s15, s1
    6a94: eddf2b59     	vldr	d18, [pc, #356]         @ 0x6c00 <_getPositionRange+0x398>  // f64=100
    6a98: eddf3b5a     	vldr	d19, [pc, #360]         @ 0x6c08 <_getPositionRange+0x3a0>  // f64=0.01
    6a9c: eeb41ae6     	vcmpe.f32	s2, s13
    6aa0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6aa4: beb01a66     	vmovlt.f32	s2, s13
    6aa8: eef71ac1     	vcvt.f64.f32	d17, s2
    6aac: ee512ba0     	vnmls.f64	d18, d17, d16
    6ab0: eeb62b00     	vmov.f64	d2, #5.000000e-01
    6ab4: eef71be2     	vcvt.f32.f64	s3, d18
    6ab8: eeb73ae1     	vcvt.f64.f32	d3, s3
    6abc: ee334b02     	vadd.f64	d4, d3, d2
    6ac0: eefd2bc4     	vcvt.s32.f64	s5, d4
    6ac4: eeb85be2     	vcvt.f64.s32	d5, s5
    6ac8: ee258b23     	vmul.f64	d8, d5, d19
    6acc: eeb70bc8     	vcvt.f32.f64	s0, d8
    6ad0: eeb40ae6     	vcmpe.f32	s0, s13
    6ad4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6ad8: aaffff71     	bge	0x68a4 <_getPositionRange+0x3c> @ imm = #-0x23c
    6adc: e2840e63     	add	r0, r4, #1584
    6ae0: e2842e6a     	add	r2, r4, #1696
    6ae4: e3a0c5fe     	mov	r12, #1065353216
    6ae8: eddf3a4a     	vldr	s7, [pc, #296]          @ 0x6c18 <_getPositionRange+0x3b0>  // f32=69
    6aec: e59f113c     	ldr	r1, [pc, #0x13c]        @ 0x6c30 <_getPositionRange+0x3c8>  // u32=0x42b0; f32?=2.39229674e-41
    6af0: e1a0e002     	mov	lr, r2
    6af4: e1a0800e     	mov	r8, lr
    6af8: edc06a01     	vstr	s13, [r0, #4]
    6afc: e08f7001     	add	r7, pc, r1
    6b00: e582c004     	str	r12, [r2, #0x4]
    6b04: e2840e6b     	add	r0, r4, #1712
    6b08: eef14a40     	vneg.f32	s9, s0
    6b0c: edd35aa0     	vldr	s11, [r3, #640]
    6b10: ee758aa3     	vadd.f32	s17, s11, s7
    6b14: eebd9ae8     	vcvt.s32.f32	s18, s17
    6b18: ee19ca10     	vmov	r12, s18
    6b1c: e087210c     	add	r2, r7, r12, lsl #2
    6b20: e5921000     	ldr	r1, [r2]
    6b24: e58e1008     	str	r1, [lr, #0x8]
    6b28: edd39aa1     	vldr	s19, [r3, #644]
    6b2c: ee397aa3     	vadd.f32	s14, s19, s7
    6b30: eefd6ac7     	vcvt.s32.f32	s13, s14
    6b34: ee16ea90     	vmov	lr, s13
    6b38: e087710e     	add	r7, r7, lr, lsl #2
    6b3c: e597c000     	ldr	r12, [r7]
    6b40: e588c00c     	str	r12, [r8, #0xc]
    6b44: edc04a00     	vstr	s9, [r0]
    6b48: eaffff83     	b	0x695c <_getPositionRange+0xf4> @ imm = #-0x1f4
    6b4c: ebffeebe     	bl	0x264c <.plt+0x26c>     @ imm = #-0x4508  // CALL rand
    6b50: ee381aa8     	vadd.f32	s2, s17, s17
    6b54: eeb51ac0     	vcmpe.f32	s2, #0
    6b58: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6b5c: ceb71ac1     	vcvtgt.f64.f32	d1, s2
    6b60: deb71b00     	vmovle.f64	d1, #1.000000e+00
    6b64: ee070a90     	vmov	s15, r0
    6b68: eeb80be7     	vcvt.f64.s32	d0, s15
    6b6c: ebffee7d     	bl	0x2568 <.plt+0x188>     @ imm = #-0x460c  // CALL __fmod_finite
    6b70: e5943894     	ldr	r3, [r4, #0x894]
    6b74: eeb71bc0     	vcvt.f32.f64	s2, d0
    6b78: ee311a68     	vsub.f32	s2, s2, s17
    6b7c: eeb01ac1     	vabs.f32	s2, s2
    6b80: ee388a01     	vadd.f32	s16, s16, s2
    6b84: eaffff90     	b	0x69cc <_getPositionRange+0x164> @ imm = #-0x1c0
    6b88: eeb48ae8     	vcmpe.f32	s16, s17
    6b8c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6b90: 9ef08a48     	vmovls.f32	s17, s16
    6b94: eef58ac0     	vcmpe.f32	s17, #0
    6b98: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6b9c: da000007     	ble	0x6bc0 <_getPositionRange+0x358> @ imm = #0x1c
    6ba0: ebffeea9     	bl	0x264c <.plt+0x26c>     @ imm = #-0x455c  // CALL rand
    6ba4: eeb71ae8     	vcvt.f64.f32	d1, s17
    6ba8: ee060a10     	vmov	s12, r0
    6bac: eeb80bc6     	vcvt.f64.s32	d0, s12
    6bb0: ebffee6c     	bl	0x2568 <.plt+0x188>     @ imm = #-0x4650  // CALL __fmod_finite
    6bb4: e5943894     	ldr	r3, [r4, #0x894]
    6bb8: eeb71bc0     	vcvt.f32.f64	s2, d0
    6bbc: ee388a41     	vsub.f32	s16, s16, s2
    6bc0: eeb58ac0     	vcmpe.f32	s16, #0
    6bc4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    6bc8: aefd7ac8     	vcvtge.s32.f32	s15, s16
    6bcc: b3a0c000     	movlt	r12, #0
    6bd0: ae17ca90     	vmovge	r12, s15
    6bd4: e585c008     	str	r12, [r5, #0x8]
    6bd8: eaffff7e     	b	0x69d8 <_getPositionRange+0x170> @ imm = #-0x208
    6bdc: e320f000     	nop
    6be0: 34 33 33 33  	.word	0x33333334
    6be4: 33 33 13 40  	.word	0x40133333
    6be8: 15 8e de 78  	.word	0x78de8e15
    6bec: 90 01 99 3f  	.word	0x3f990190
    6bf0: 0b d7 a3 70  	.word	0x70a3d70b
    6bf4: 3d 0a f7 3f  	.word	0x3ff70a3d
    6bf8: e1 28 79 75  	.word	0x757928e1
    6bfc: 8e 01 a9 3f  	.word	0x3fa9018e
    6c00: 00 00 00 00  	.word	0x00000000
    6c04: 00 00 59 40  	.word	0x40590000
    6c08: 7b 14 ae 47  	.word	0x47ae147b
    6c0c: e1 7a 84 3f  	.word	0x3f847ae1
    6c10: 00 00 00 42  	.word	0x42000000
    6c14: 00 00 00 00  	.word	0x00000000
    6c18: 00 00 8a 42  	.word	0x428a0000
    6c1c: 00 60 ea 48  	.word	0x48ea6000
    6c20: 00 00 00 43  	.word	0x43000000
    6c24: 00 a0 0c 48  	.word	0x480ca000
    6c28: 00 f0 7f 45  	.word	0x457ff000
    6c2c: f0 44 00 00  	.word	0x000044f0
    6c30: b0 42 00 00  	.word	0x000042b0

