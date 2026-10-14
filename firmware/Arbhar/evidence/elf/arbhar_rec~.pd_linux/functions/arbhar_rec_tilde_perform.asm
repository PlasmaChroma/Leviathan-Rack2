00004288 <arbhar_rec_tilde_perform>:
    4288: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    428c: e1a07000     	mov	r7, r0
    4290: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
    4294: e28db060     	add	r11, sp, #96
    4298: e24dd05c     	sub	sp, sp, #92
    429c: e5903004     	ldr	r3, [r0, #0x4]
    42a0: e5902008     	ldr	r2, [r0, #0x8]
    42a4: e2831e29     	add	r1, r3, #656
    42a8: e590400c     	ldr	r4, [r0, #0xc]
    42ac: e5935284     	ldr	r5, [r3, #0x284]
    42b0: e1a09003     	mov	r9, r3
    42b4: e5908010     	ldr	r8, [r0, #0x10]
    42b8: e1d160f0     	ldrsh	r6, [r1]
    42bc: e3550000     	cmp	r5, #0
    42c0: e5900014     	ldr	r0, [r0, #0x14]
    42c4: e50b20a4     	str	r2, [r11, #-0xa4]
    42c8: e50b40b4     	str	r4, [r11, #-0xb4]
    42cc: e50b607c     	str	r6, [r11, #-0x7c]
    42d0: e50b80a8     	str	r8, [r11, #-0xa8]
    42d4: e50b008c     	str	r0, [r11, #-0x8c]
    42d8: da000002     	ble	0x42e8 <arbhar_rec_tilde_perform+0x60> @ imm = #0x8
    42dc: e593a048     	ldr	r10, [r3, #0x48]
    42e0: e35a0000     	cmp	r10, #0
    42e4: 0a000037     	beq	0x43c8 <arbhar_rec_tilde_perform+0x140> @ imm = #0xdc
    42e8: e1a08009     	mov	r8, r9
    42ec: e50b709c     	str	r7, [r11, #-0x9c]
    42f0: e5987048     	ldr	r7, [r8, #0x48]
    42f4: e1a06009     	mov	r6, r9
    42f8: e51b908c     	ldr	r9, [r11, #-0x8c]
    42fc: e3a05000     	mov	r5, #0
    4300: e3570000     	cmp	r7, #0
    4304: ed9f9bfb     	vldr	d9, [pc, #1004]         @ 0x46f8 <arbhar_rec_tilde_perform+0x470>
    4308: e1a04109     	lsl	r4, r9, #2
    430c: e50b40b8     	str	r4, [r11, #-0xb8]
    4310: eef78a00     	vmov.f32	s17, #1.000000e+00
    4314: eddfaafb     	vldr	s21, [pc, #1004]        @ 0x4708 <arbhar_rec_tilde_perform+0x480>
    4318: eebfba00     	vmov.f32	s22, #-1.000000e+00
    431c: da00001e     	ble	0x439c <arbhar_rec_tilde_perform+0x114> @ imm = #0x78
    4320: e598e020     	ldr	lr, [r8, #0x20]
    4324: e598401c     	ldr	r4, [r8, #0x1c]
    4328: e50bd090     	str	sp, [r11, #-0x90]
    432c: e086010e     	add	r0, r6, lr, lsl #2
    4330: ed96aa97     	vldr	s20, [r6, #604]
    4334: e50b0088     	str	r0, [r11, #-0x88]
    4338: e5901094     	ldr	r1, [r0, #0x94]
    433c: e1540001     	cmp	r4, r1
    4340: e50b10a0     	str	r1, [r11, #-0xa0]
    4344: ba000051     	blt	0x4490 <arbhar_rec_tilde_perform+0x208> @ imm = #0x144
    4348: e3e02102     	mvn	r2, #-2147483648
    434c: e3a09000     	mov	r9, #0
    4350: e588201c     	str	r2, [r8, #0x1c]
    4354: e586928c     	str	r9, [r6, #0x28c]
    4358: ed88aa0d     	vstr	s20, [r8, #52]
    435c: e51bd090     	ldr	sp, [r11, #-0x90]
    4360: e3550001     	cmp	r5, #1
    4364: e2888030     	add	r8, r8, #48
    4368: 1a000007     	bne	0x438c <arbhar_rec_tilde_perform+0x104> @ imm = #0x1c
    436c: e596007c     	ldr	r0, [r6, #0x7c]
    4370: ed9f0be2     	vldr	d0, [pc, #904]          @ 0x4700 <arbhar_rec_tilde_perform+0x478>
    4374: e51b509c     	ldr	r5, [r11, #-0x9c]
    4378: ebfff90a     	bl	0x27a8 <.plt+0x2a8>     @ imm = #-0x1bd8
    437c: e2850018     	add	r0, r5, #24
    4380: e24bd060     	sub	sp, r11, #96
    4384: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
    4388: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    438c: e5987048     	ldr	r7, [r8, #0x48]
    4390: e3a05001     	mov	r5, #1
    4394: e3570000     	cmp	r7, #0
    4398: caffffe0     	bgt	0x4320 <arbhar_rec_tilde_perform+0x98> @ imm = #-0x80
    439c: e596a280     	ldr	r10, [r6, #0x280]
    43a0: e15a0005     	cmp	r10, r5
    43a4: 1affffed     	bne	0x4360 <arbhar_rec_tilde_perform+0xd8> @ imm = #-0x4c
    43a8: e51b308c     	ldr	r3, [r11, #-0x8c]
    43ac: e3530000     	cmp	r3, #0
    43b0: daffffea     	ble	0x4360 <arbhar_rec_tilde_perform+0xd8> @ imm = #-0x58
    43b4: e51b20b8     	ldr	r2, [r11, #-0xb8]
    43b8: e3a01000     	mov	r1, #0
    43bc: e51b00a8     	ldr	r0, [r11, #-0xa8]
    43c0: ebfff8dd     	bl	0x273c <.plt+0x23c>     @ imm = #-0x1c8c
    43c4: eaffffe5     	b	0x4360 <arbhar_rec_tilde_perform+0xd8> @ imm = #-0x6c
    43c8: e5934078     	ldr	r4, [r3, #0x78]
    43cc: e3540000     	cmp	r4, #0
    43d0: 1affffc4     	bne	0x42e8 <arbhar_rec_tilde_perform+0x60> @ imm = #-0xf0
    43d4: e59fc330     	ldr	r12, [pc, #0x330]       @ 0x470c <arbhar_rec_tilde_perform+0x484>
    43d8: e3a01030     	mov	r1, #48
    43dc: e5834088     	str	r4, [r3, #0x88]
    43e0: e3a00c01     	mov	r0, #256
    43e4: e08f500c     	add	r5, pc, r12
    43e8: e5830084     	str	r0, [r3, #0x84]
    43ec: e583008c     	str	r0, [r3, #0x8c]
    43f0: e3e0e102     	mvn	lr, #-2147483648
    43f4: e595a014     	ldr	r10, [r5, #0x14]
    43f8: e3a08001     	mov	r8, #1
    43fc: e5830090     	str	r0, [r3, #0x90]
    4400: e5996080     	ldr	r6, [r9, #0x80]
    4404: e0223a91     	mla	r2, r1, r10, r3
    4408: e593327c     	ldr	r3, [r3, #0x27c]
    440c: e589a280     	str	r10, [r9, #0x280]
    4410: e5890268     	str	r0, [r9, #0x268]
    4414: e589e264     	str	lr, [r9, #0x264]
    4418: e021119a     	mla	r1, r10, r1, r1
    441c: e5828048     	str	r8, [r2, #0x48]
    4420: e5823020     	str	r3, [r2, #0x20]
    4424: e1a0a009     	mov	r10, r9
    4428: e582601c     	str	r6, [r2, #0x1c]
    442c: e5820024     	str	r0, [r2, #0x24]
    4430: e0893001     	add	r3, r9, r1
    4434: e5824028     	str	r4, [r2, #0x28]
    4438: e5820030     	str	r0, [r2, #0x30]
    443c: e5820044     	str	r0, [r2, #0x44]
    4440: e582e040     	str	lr, [r2, #0x40]
    4444: e5990078     	ldr	r0, [r9, #0x78]
    4448: e599e048     	ldr	lr, [r9, #0x48]
    444c: e599125c     	ldr	r1, [r9, #0x25c]
    4450: e08e2000     	add	r2, lr, r0
    4454: e59902a0     	ldr	r0, [r9, #0x2a0]
    4458: e3520000     	cmp	r2, #0
    445c: e5831004     	str	r1, [r3, #0x4]
    4460: c1a0a008     	movgt	r10, r8
    4464: d3a0a000     	movle	r10, #0
    4468: ee00aa10     	vmov	s0, r10
    446c: eeb80ac0     	vcvt.f32.s32	s0, s0
    4470: ebfff8db     	bl	0x27e4 <.plt+0x2e4>     @ imm = #-0x1c94
    4474: e5958014     	ldr	r8, [r5, #0x14]
    4478: e5894284     	str	r4, [r9, #0x284]
    447c: e3580000     	cmp	r8, #0
    4480: c3a08000     	movgt	r8, #0
    4484: d3a08001     	movle	r8, #1
    4488: e5858014     	str	r8, [r5, #0x14]
    448c: eaffff95     	b	0x42e8 <arbhar_rec_tilde_perform+0x60> @ imm = #-0x1ac
    4490: e0417004     	sub	r7, r1, r4
    4494: e51b108c     	ldr	r1, [r11, #-0x8c]
    4498: e28ea001     	add	r10, lr, #1
    449c: e598e040     	ldr	lr, [r8, #0x40]
    44a0: e1510007     	cmp	r1, r7
    44a4: e5989028     	ldr	r9, [r8, #0x28]
    44a8: ed988a0f     	vldr	s16, [r8, #60]
    44ac: b1a0c001     	movlt	r12, r1
    44b0: a1a0c007     	movge	r12, r7
    44b4: e1a07001     	mov	r7, r1
    44b8: e3510000     	cmp	r1, #0
    44bc: e5981030     	ldr	r1, [r8, #0x30]
    44c0: e084300c     	add	r3, r4, r12
    44c4: e2430040     	sub	r0, r3, #64
    44c8: e50bc098     	str	r12, [r11, #-0x98]
    44cc: e04e2000     	sub	r2, lr, r0
    44d0: e1a0c08a     	lsl	r12, r10, #1
    44d4: e50b3078     	str	r3, [r11, #-0x78]
    44d8: e50bc074     	str	r12, [r11, #-0x74]
    44dc: e50b20b0     	str	r2, [r11, #-0xb0]
    44e0: e598a024     	ldr	r10, [r8, #0x24]
    44e4: e50b1094     	str	r1, [r11, #-0x94]
    44e8: e598e02c     	ldr	lr, [r8, #0x2c]
    44ec: e5983044     	ldr	r3, [r8, #0x44]
    44f0: da0000d2     	ble	0x4840 <arbhar_rec_tilde_perform+0x5b8> @ imm = #0x348
    44f4: ee073a90     	vmov	s15, r3
    44f8: e0847007     	add	r7, r4, r7
    44fc: ee00ea90     	vmov	s1, lr
    4500: e50b7084     	str	r7, [r11, #-0x84]
    4504: ee06aa90     	vmov	s13, r10
    4508: e51b00b4     	ldr	r0, [r11, #-0xb4]
    450c: eeb87ae7     	vcvt.f32.s32	s14, s15
    4510: e51b70a4     	ldr	r7, [r11, #-0xa4]
    4514: ed98da0d     	vldr	s26, [r8, #52]
    4518: e24cc001     	sub	r12, r12, #1
    451c: e0402007     	sub	r2, r0, r7
    4520: e3a01000     	mov	r1, #0
    4524: e50b2080     	str	r2, [r11, #-0x80]
    4528: e50b10ac     	str	r1, [r11, #-0xac]
    452c: e50bc068     	str	r12, [r11, #-0x68]
    4530: eeb81ae0     	vcvt.f32.s32	s2, s1
    4534: eef81ae6     	vcvt.f32.s32	s3, s13
    4538: eec8ca87     	vdiv.f32	s25, s17, s14
    453c: ee88ca81     	vdiv.f32	s24, s17, s2
    4540: eec8baa1     	vdiv.f32	s23, s17, s3
    4544: eeb4aacd     	vcmpe.f32	s20, s26
    4548: eef1fa10     	vmrs	APSR_nzcv, fpscr
    454c: da00007e     	ble	0x474c <arbhar_rec_tilde_perform+0x4c4> @ imm = #0x1f8
    4550: eeb73acd     	vcvt.f64.f32	d3, s26
    4554: ee334b09     	vadd.f64	d4, d3, d9
    4558: eeb7dbc4     	vcvt.f32.f64	s26, d4
    455c: eeb4dae8     	vcmpe.f32	s26, s17
    4560: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4564: 8eb0da68     	vmovhi.f32	s26, s17
    4568: 8a000002     	bhi	0x4578 <arbhar_rec_tilde_perform+0x2f0> @ imm = #0x8
    456c: eeb4daca     	vcmpe.f32	s26, s20
    4570: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4574: 8eb0da4a     	vmovhi.f32	s26, s20
    4578: ee044a90     	vmov	s9, r4
    457c: e51be080     	ldr	lr, [r11, #-0x80]
    4580: edd73a00     	vldr	s7, [r7]
    4584: e1a00006     	mov	r0, r6
    4588: e08e3007     	add	r3, lr, r7
    458c: e51b1068     	ldr	r1, [r11, #-0x68]
    4590: eef8dae4     	vcvt.f32.s32	s27, s9
    4594: edd32a00     	vldr	s5, [r3]
    4598: ed4b3a1b     	vstr	s7, [r11, #-108]
    459c: eeb00a6d     	vmov.f32	s0, s27
    45a0: ed4b2a1c     	vstr	s5, [r11, #-112]
    45a4: ebfff7f2     	bl	0x2574 <.plt+0x74>      @ imm = #-0x2038
    45a8: e51b1074     	ldr	r1, [r11, #-0x74]
    45ac: e1a00006     	mov	r0, r6
    45b0: eeb0fa40     	vmov.f32	s30, s0
    45b4: eeb00a6d     	vmov.f32	s0, s27
    45b8: ebfff7ed     	bl	0x2574 <.plt+0x74>      @ imm = #-0x204c
    45bc: e15a0009     	cmp	r10, r9
    45c0: eeb0ea40     	vmov.f32	s28, s0
    45c4: ba000053     	blt	0x4718 <arbhar_rec_tilde_perform+0x490> @ imm = #0x14c
    45c8: ee079a90     	vmov	s15, r9
    45cc: e2899001     	add	r9, r9, #1
    45d0: eeb85ae7     	vcvt.f32.s32	s10, s15
    45d4: ee258a2b     	vmul.f32	s16, s10, s23
    45d8: eeb58ac0     	vcmpe.f32	s16, #0
    45dc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    45e0: beb08a6a     	vmovlt.f32	s16, s21
    45e4: ba000002     	blt	0x45f4 <arbhar_rec_tilde_perform+0x36c> @ imm = #0x8
    45e8: eeb48ae8     	vcmpe.f32	s16, s17
    45ec: eef1fa10     	vmrs	APSR_nzcv, fpscr
    45f0: 8eb08a68     	vmovhi.f32	s16, s17
    45f4: e51be06c     	ldr	lr, [r11, #-0x6c]
    45f8: e51bc070     	ldr	r12, [r11, #-0x70]
    45fc: e02e10ae     	eor	r1, lr, lr, lsr #1
    4600: e3110202     	tst	r1, #536870912
    4604: e02c20ac     	eor	r2, r12, r12, lsr #1
    4608: 1e07ea90     	vmovne	s15, lr
    460c: e51be078     	ldr	lr, [r11, #-0x78]
    4610: 0eb00a6a     	vmoveq.f32	s0, s21
    4614: 1e280a27     	vmulne.f32	s0, s16, s15
    4618: e3120202     	tst	r2, #536870912
    461c: 1e07ca90     	vmovne	s15, r12
    4620: 0ef0ea6a     	vmoveq.f32	s29, s21
    4624: 1e68ea27     	vmulne.f32	s29, s16, s15
    4628: e15a000e     	cmp	r10, lr
    462c: aa000037     	bge	0x4710 <arbhar_rec_tilde_perform+0x488> @ imm = #0xdc
    4630: ee7d5a68     	vsub.f32	s11, s26, s17
    4634: eeb06a4d     	vmov.f32	s12, s26
    4638: ee156a88     	vnmls.f32	s12, s11, s16
    463c: eeb46acb     	vcmpe.f32	s12, s22
    4640: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4644: ba000031     	blt	0x4710 <arbhar_rec_tilde_perform+0x488> @ imm = #0xc4
    4648: ee767a28     	vadd.f32	s15, s12, s17
    464c: eef47ae8     	vcmpe.f32	s15, s17
    4650: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4654: 8ef07a68     	vmovhi.f32	s15, s17
    4658: ee77fa8d     	vadd.f32	s31, s15, s26
    465c: e1a02004     	mov	r2, r4
    4660: e51b1068     	ldr	r1, [r11, #-0x68]
    4664: e1a00006     	mov	r0, r6
    4668: ee0f0a2f     	vmla.f32	s0, s30, s31
    466c: ebfff820     	bl	0x26f4 <.plt+0x1f4>     @ imm = #-0x1f80
    4670: e51b307c     	ldr	r3, [r11, #-0x7c]
    4674: e3530000     	cmp	r3, #0
    4678: da000005     	ble	0x4694 <arbhar_rec_tilde_perform+0x40c> @ imm = #0x14
    467c: e1a02004     	mov	r2, r4
    4680: e51b1074     	ldr	r1, [r11, #-0x74]
    4684: e1a00006     	mov	r0, r6
    4688: eeb00a6e     	vmov.f32	s0, s29
    468c: ee0e0a2f     	vmla.f32	s0, s28, s31
    4690: ebfff817     	bl	0x26f4 <.plt+0x1f4>     @ imm = #-0x1fa4
    4694: e5961280     	ldr	r1, [r6, #0x280]
    4698: e1510005     	cmp	r1, r5
    469c: 0a000042     	beq	0x47ac <arbhar_rec_tilde_perform+0x524> @ imm = #0x108
    46a0: e51bc084     	ldr	r12, [r11, #-0x84]
    46a4: e2844001     	add	r4, r4, #1
    46a8: ed88da0d     	vstr	s26, [r8, #52]
    46ac: e2877004     	add	r7, r7, #4
    46b0: e154000c     	cmp	r4, r12
    46b4: 1affffa2     	bne	0x4544 <arbhar_rec_tilde_perform+0x2bc> @ imm = #-0x178
    46b8: e51ba0ac     	ldr	r10, [r11, #-0xac]
    46bc: e20a4001     	and	r4, r10, #1
    46c0: e51b70a0     	ldr	r7, [r11, #-0xa0]
    46c4: e51b2078     	ldr	r2, [r11, #-0x78]
    46c8: e1570002     	cmp	r7, r2
    46cc: d3844001     	orrle	r4, r4, #1
    46d0: e3540000     	cmp	r4, #0
    46d4: 1a00004a     	bne	0x4804 <arbhar_rec_tilde_perform+0x57c> @ imm = #0x128
    46d8: e51b0078     	ldr	r0, [r11, #-0x78]
    46dc: e5889028     	str	r9, [r8, #0x28]
    46e0: e51b9094     	ldr	r9, [r11, #-0x94]
    46e4: e588001c     	str	r0, [r8, #0x1c]
    46e8: ed888a0f     	vstr	s16, [r8, #60]
    46ec: e5889030     	str	r9, [r8, #0x30]
    46f0: eaffff19     	b	0x435c <arbhar_rec_tilde_perform+0xd4> @ imm = #-0x39c
    46f4: e320f000     	nop
    46f8: de 71 8a 8e  	.word	0x8e8a71de
    46fc: e4 f2 7f 3f  	.word	0x3f7ff2e4
    4700: 00 00 00 00  	.word	0x00000000
    4704: 00 00 00 00  	.word	0x00000000
    4708: 00 00 00 00  	.word	0x00000000
    470c: c4 5d 01 00  	.word	0x00015dc4
    4710: eef0fa4d     	vmov.f32	s31, s26
    4714: eaffffd0     	b	0x465c <arbhar_rec_tilde_perform+0x3d4> @ imm = #-0xc0
    4718: e51bc088     	ldr	r12, [r11, #-0x88]
    471c: e51b2098     	ldr	r2, [r11, #-0x98]
    4720: e51b3094     	ldr	r3, [r11, #-0x94]
    4724: e59c0094     	ldr	r0, [r12, #0x94]
    4728: e082e004     	add	lr, r2, r4
    472c: e0401003     	sub	r1, r0, r3
    4730: e151000e     	cmp	r1, lr
    4734: aa000010     	bge	0x477c <arbhar_rec_tilde_perform+0x4f4> @ imm = #0x40
    4738: e3530000     	cmp	r3, #0
    473c: ee388a4c     	vsub.f32	s16, s16, s24
    4740: c2433001     	subgt	r3, r3, #1
    4744: c50b3094     	strgt	r3, [r11, #-0x94]
    4748: eaffffa2     	b	0x45d8 <arbhar_rec_tilde_perform+0x350> @ imm = #-0x178
    474c: 5affff89     	bpl	0x4578 <arbhar_rec_tilde_perform+0x2f0> @ imm = #-0x1dc
    4750: eef70acd     	vcvt.f64.f32	d16, s26
    4754: ee302bc9     	vsub.f64	d2, d16, d9
    4758: eeb7dbc2     	vcvt.f32.f64	s26, d2
    475c: eeb5dac0     	vcmpe.f32	s26, #0
    4760: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4764: beb0da6a     	vmovlt.f32	s26, s21
    4768: baffff82     	blt	0x4578 <arbhar_rec_tilde_perform+0x2f0> @ imm = #-0x1f8
    476c: eeb4daca     	vcmpe.f32	s26, s20
    4770: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4774: beb0da4a     	vmovlt.f32	s26, s20
    4778: eaffff7e     	b	0x4578 <arbhar_rec_tilde_perform+0x2f0> @ imm = #-0x208
    477c: e598c044     	ldr	r12, [r8, #0x44]
    4780: e51b00b0     	ldr	r0, [r11, #-0xb0]
    4784: e15c0000     	cmp	r12, r0
    4788: a3500000     	cmpge	r0, #0
    478c: ca00001a     	bgt	0x47fc <arbhar_rec_tilde_perform+0x574> @ imm = #0x68
    4790: e51b20ac     	ldr	r2, [r11, #-0xac]
    4794: e3500000     	cmp	r0, #0
    4798: d3a02001     	movle	r2, #1
    479c: ceb08a68     	vmovgt.f32	s16, s17
    47a0: deb08a6a     	vmovle.f32	s16, s21
    47a4: e50b20ac     	str	r2, [r11, #-0xac]
    47a8: eaffff91     	b	0x45f4 <arbhar_rec_tilde_perform+0x36c> @ imm = #-0x1bc
    47ac: e51b0098     	ldr	r0, [r11, #-0x98]
    47b0: e51be0a8     	ldr	lr, [r11, #-0xa8]
    47b4: e0802004     	add	r2, r0, r4
    47b8: e51b30a4     	ldr	r3, [r11, #-0xa4]
    47bc: e1a00006     	mov	r0, r6
    47c0: e2844001     	add	r4, r4, #1
    47c4: ee002a10     	vmov	s0, r2
    47c8: e04e1003     	sub	r1, lr, r3
    47cc: e081c007     	add	r12, r1, r7
    47d0: e3a020f9     	mov	r2, #249
    47d4: eeb80ac0     	vcvt.f32.s32	s0, s0
    47d8: e3a01000     	mov	r1, #0
    47dc: e2877004     	add	r7, r7, #4
    47e0: ed8c0a00     	vstr	s0, [r12]
    47e4: ebfff7c2     	bl	0x26f4 <.plt+0x1f4>     @ imm = #-0x20f8
    47e8: e51b0084     	ldr	r0, [r11, #-0x84]
    47ec: e1540000     	cmp	r4, r0
    47f0: ed88da0d     	vstr	s26, [r8, #52]
    47f4: 1affff52     	bne	0x4544 <arbhar_rec_tilde_perform+0x2bc> @ imm = #-0x2b8
    47f8: eaffffae     	b	0x46b8 <arbhar_rec_tilde_perform+0x430> @ imm = #-0x148
    47fc: ee388a6c     	vsub.f32	s16, s16, s25
    4800: eaffff74     	b	0x45d8 <arbhar_rec_tilde_perform+0x350> @ imm = #-0x230
    4804: e3a04000     	mov	r4, #0
    4808: e5884048     	str	r4, [r8, #0x48]
    480c: e596e048     	ldr	lr, [r6, #0x48]
    4810: e3e03102     	mvn	r3, #-2147483648
    4814: e5961078     	ldr	r1, [r6, #0x78]
    4818: e50b3078     	str	r3, [r11, #-0x78]
    481c: e08ec001     	add	r12, lr, r1
    4820: e59602a0     	ldr	r0, [r6, #0x2a0]
    4824: e35c0000     	cmp	r12, #0
    4828: c3a03001     	movgt	r3, #1
    482c: d3a03000     	movle	r3, #0
    4830: ee0a3a10     	vmov	s20, r3
    4834: eeb80aca     	vcvt.f32.s32	s0, s20
    4838: ebfff7e9     	bl	0x27e4 <.plt+0x2e4>     @ imm = #-0x205c
    483c: eaffffa5     	b	0x46d8 <arbhar_rec_tilde_perform+0x450> @ imm = #-0x16c
    4840: e3a04000     	mov	r4, #0
    4844: eaffff9d     	b	0x46c0 <arbhar_rec_tilde_perform+0x438> @ imm = #-0x18c

