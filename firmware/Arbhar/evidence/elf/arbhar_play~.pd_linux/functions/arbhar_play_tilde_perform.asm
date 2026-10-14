00004248 <arbhar_play_tilde_perform>:
    4248: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    424c: e1a0e000     	mov	lr, r0
    4250: ed2d8b10     	vpush	{d8, d9, d10, d11, d12, d13, d14, d15}
    4254: e59f1568     	ldr	r1, [pc, #0x568]        @ 0x47c4 <arbhar_play_tilde_perform+0x57c>
    4258: e5907004     	ldr	r7, [r0, #0x4]
    425c: e08f4001     	add	r4, pc, r1
    4260: e5906018     	ldr	r6, [r0, #0x18]
    4264: e24dd044     	sub	sp, sp, #68
    4268: e2878a02     	add	r8, r7, #8192
    426c: e8941004     	ldm	r4, {r2, r12}
    4270: e58d003c     	str	r0, [sp, #0x3c]
    4274: e59806e8     	ldr	r0, [r8, #0x6e8]
    4278: e28cc001     	add	r12, r12, #1
    427c: e59e5008     	ldr	r5, [lr, #0x8]
    4280: e0862002     	add	r2, r6, r2
    4284: e59e900c     	ldr	r9, [lr, #0xc]
    4288: e3500000     	cmp	r0, #0
    428c: e59ea010     	ldr	r10, [lr, #0x10]
    4290: e59eb014     	ldr	r11, [lr, #0x14]
    4294: e8841004     	stm	r4, {r2, r12}
    4298: e59fc528     	ldr	r12, [pc, #0x528]       @ 0x47c8 <arbhar_play_tilde_perform+0x580>
    429c: e58d502c     	str	r5, [sp, #0x2c]
    42a0: e58d9030     	str	r9, [sp, #0x30]
    42a4: e08f100c     	add	r1, pc, r12
    42a8: e58da034     	str	r10, [sp, #0x34]
    42ac: e58db028     	str	r11, [sp, #0x28]
    42b0: e58d1038     	str	r1, [sp, #0x38]
    42b4: e5983894     	ldr	r3, [r8, #0x894]
    42b8: ca0002aa     	bgt	0x4d68 <arbhar_play_tilde_perform+0xb20> @ imm = #0xaa8
    42bc: e3500000     	cmp	r0, #0
    42c0: e5974030     	ldr	r4, [r7, #0x30]
    42c4: 1a0002c9     	bne	0x4df0 <arbhar_play_tilde_perform+0xba8> @ imm = #0xb24
    42c8: e3540000     	cmp	r4, #0
    42cc: da000252     	ble	0x4c1c <arbhar_play_tilde_perform+0x9d4> @ imm = #0x948
    42d0: ed930a8c     	vldr	s0, [r3, #560]
    42d4: eeb50ac0     	vcmpe.f32	s0, #0
    42d8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    42dc: da000261     	ble	0x4c68 <arbhar_play_tilde_perform+0xa20> @ imm = #0x984
    42e0: ed936a18     	vldr	s12, [r3, #96]
    42e4: eeb56ac0     	vcmpe.f32	s12, #0
    42e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    42ec: ca0004cf     	bgt	0x5630 <arbhar_play_tilde_perform+0x13e8> @ imm = #0x133c
    42f0: ed930a17     	vldr	s0, [r3, #92]
    42f4: eeb50a40     	vcmp.f32	s0, #0
    42f8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    42fc: 1a000008     	bne	0x4324 <arbhar_play_tilde_perform+0xdc> @ imm = #0x20
    4300: ed931a1c     	vldr	s2, [r3, #112]
    4304: e59fa4c0     	ldr	r10, [pc, #0x4c0]       @ 0x47cc <arbhar_play_tilde_perform+0x584>
    4308: e08fb00a     	add	r11, pc, r10
    430c: eefd1ac1     	vcvt.s32.f32	s3, s2
    4310: e59b100c     	ldr	r1, [r11, #0xc]
    4314: ee114a90     	vmov	r4, s3
    4318: e1510004     	cmp	r1, r4
    431c: 13540000     	cmpne	r4, #0
    4320: 1a0004eb     	bne	0x56d4 <arbhar_play_tilde_perform+0x148c> @ imm = #0x13ac
    4324: ed932a21     	vldr	s4, [r3, #132]
    4328: e2881e6e     	add	r1, r8, #1760
    432c: e598c8cc     	ldr	r12, [r8, #0x8cc]
    4330: eefd2ac2     	vcvt.s32.f32	s5, s4
    4334: ee124a90     	vmov	r4, s5
    4338: e3540000     	cmp	r4, #0
    433c: e58846cc     	str	r4, [r8, #0x6cc]
    4340: cdd32af9     	vldrgt	s5, [r3, #996]
    4344: d3a00000     	movle	r0, #0
    4348: e3a03000     	mov	r3, #0
    434c: cefd2ae2     	vcvtgt.s32.f32	s5, s5
    4350: ce120a90     	vmovgt	r0, s5
    4354: e3560000     	cmp	r6, #0
    4358: e58806d0     	str	r0, [r8, #0x6d0]
    435c: e24c0003     	sub	r0, r12, #3
    4360: e5813004     	str	r3, [r1, #0x4]
    4364: ee030a10     	vmov	s6, r0
    4368: da000093     	ble	0x45bc <arbhar_play_tilde_perform+0x374> @ imm = #0x24c
    436c: e59d5028     	ldr	r5, [sp, #0x28]
    4370: e59db034     	ldr	r11, [sp, #0x34]
    4374: e085e106     	add	lr, r5, r6, lsl #2
    4378: e58de004     	str	lr, [sp, #0x4]
    437c: e04e9005     	sub	r9, lr, r5
    4380: e59d4030     	ldr	r4, [sp, #0x30]
    4384: e249a004     	sub	r10, r9, #4
    4388: e59dc02c     	ldr	r12, [sp, #0x2c]
    438c: e04b1005     	sub	r1, r11, r5
    4390: e1a02005     	mov	r2, r5
    4394: e1a0012a     	lsr	r0, r10, #2
    4398: e044b005     	sub	r11, r4, r5
    439c: e280e001     	add	lr, r0, #1
    43a0: e04c9005     	sub	r9, r12, r5
    43a4: e21ea007     	ands	r10, lr, #7
    43a8: 0a00003e     	beq	0x44a8 <arbhar_play_tilde_perform+0x260> @ imm = #0xf8
    43ac: e35a0001     	cmp	r10, #1
    43b0: 0a000031     	beq	0x447c <arbhar_play_tilde_perform+0x234> @ imm = #0xc4
    43b4: e35a0002     	cmp	r10, #2
    43b8: 0a000027     	beq	0x445c <arbhar_play_tilde_perform+0x214> @ imm = #0x9c
    43bc: e35a0003     	cmp	r10, #3
    43c0: 0a00001d     	beq	0x443c <arbhar_play_tilde_perform+0x1f4> @ imm = #0x74
    43c4: e35a0004     	cmp	r10, #4
    43c8: 0a000013     	beq	0x441c <arbhar_play_tilde_perform+0x1d4> @ imm = #0x4c
    43cc: e35a0005     	cmp	r10, #5
    43d0: 0a000009     	beq	0x43fc <arbhar_play_tilde_perform+0x1b4> @ imm = #0x24
    43d4: e35a0006     	cmp	r10, #6
    43d8: 1a00048b     	bne	0x560c <arbhar_play_tilde_perform+0x13c4> @ imm = #0x122c
    43dc: e5823000     	str	r3, [r2]
    43e0: e081e002     	add	lr, r1, r2
    43e4: e08ba002     	add	r10, r11, r2
    43e8: e0890002     	add	r0, r9, r2
    43ec: e2822004     	add	r2, r2, #4
    43f0: e58e3000     	str	r3, [lr]
    43f4: e58a3000     	str	r3, [r10]
    43f8: e5803000     	str	r3, [r0]
    43fc: e5823000     	str	r3, [r2]
    4400: e0815002     	add	r5, r1, r2
    4404: e08b4002     	add	r4, r11, r2
    4408: e089c002     	add	r12, r9, r2
    440c: e2822004     	add	r2, r2, #4
    4410: e5853000     	str	r3, [r5]
    4414: e5843000     	str	r3, [r4]
    4418: e58c3000     	str	r3, [r12]
    441c: e5823000     	str	r3, [r2]
    4420: e081e002     	add	lr, r1, r2
    4424: e08ba002     	add	r10, r11, r2
    4428: e0890002     	add	r0, r9, r2
    442c: e2822004     	add	r2, r2, #4
    4430: e58e3000     	str	r3, [lr]
    4434: e58a3000     	str	r3, [r10]
    4438: e5803000     	str	r3, [r0]
    443c: e5823000     	str	r3, [r2]
    4440: e0815002     	add	r5, r1, r2
    4444: e08b4002     	add	r4, r11, r2
    4448: e089c002     	add	r12, r9, r2
    444c: e2822004     	add	r2, r2, #4
    4450: e5853000     	str	r3, [r5]
    4454: e5843000     	str	r3, [r4]
    4458: e58c3000     	str	r3, [r12]
    445c: e5823000     	str	r3, [r2]
    4460: e081e002     	add	lr, r1, r2
    4464: e08ba002     	add	r10, r11, r2
    4468: e0890002     	add	r0, r9, r2
    446c: e2822004     	add	r2, r2, #4
    4470: e58e3000     	str	r3, [lr]
    4474: e58a3000     	str	r3, [r10]
    4478: e5803000     	str	r3, [r0]
    447c: e59d4004     	ldr	r4, [sp, #0x4]
    4480: e081e002     	add	lr, r1, r2
    4484: e5823000     	str	r3, [r2]
    4488: e08b5002     	add	r5, r11, r2
    448c: e089c002     	add	r12, r9, r2
    4490: e2822004     	add	r2, r2, #4
    4494: e1540002     	cmp	r4, r2
    4498: e58e3000     	str	r3, [lr]
    449c: e5853000     	str	r3, [r5]
    44a0: e58c3000     	str	r3, [r12]
    44a4: 0a000044     	beq	0x45bc <arbhar_play_tilde_perform+0x374> @ imm = #0x110
    44a8: e58d7008     	str	r7, [sp, #0x8]
    44ac: e081a002     	add	r10, r1, r2
    44b0: e5823000     	str	r3, [r2]
    44b4: e08b0002     	add	r0, r11, r2
    44b8: e2827004     	add	r7, r2, #4
    44bc: e58a3000     	str	r3, [r10]
    44c0: e089a002     	add	r10, r9, r2
    44c4: e5803000     	str	r3, [r0]
    44c8: e2825008     	add	r5, r2, #8
    44cc: e58a3000     	str	r3, [r10]
    44d0: e081a007     	add	r10, r1, r7
    44d4: e5823004     	str	r3, [r2, #0x4]
    44d8: e282400c     	add	r4, r2, #12
    44dc: e58a3000     	str	r3, [r10]
    44e0: e08ba007     	add	r10, r11, r7
    44e4: e0897007     	add	r7, r9, r7
    44e8: e282e010     	add	lr, r2, #16
    44ec: e58a3000     	str	r3, [r10]
    44f0: e081a005     	add	r10, r1, r5
    44f4: e5873000     	str	r3, [r7]
    44f8: e08b7005     	add	r7, r11, r5
    44fc: e0895005     	add	r5, r9, r5
    4500: e5823008     	str	r3, [r2, #0x8]
    4504: e58a3000     	str	r3, [r10]
    4508: e081a004     	add	r10, r1, r4
    450c: e5873000     	str	r3, [r7]
    4510: e08b7004     	add	r7, r11, r4
    4514: e5853000     	str	r3, [r5]
    4518: e0894004     	add	r4, r9, r4
    451c: e081500e     	add	r5, r1, lr
    4520: e582300c     	str	r3, [r2, #0xc]
    4524: e282c014     	add	r12, r2, #20
    4528: e58a3000     	str	r3, [r10]
    452c: e2820018     	add	r0, r2, #24
    4530: e08ba00e     	add	r10, r11, lr
    4534: e5873000     	str	r3, [r7]
    4538: e089e00e     	add	lr, r9, lr
    453c: e5843000     	str	r3, [r4]
    4540: e0817000     	add	r7, r1, r0
    4544: e5823010     	str	r3, [r2, #0x10]
    4548: e081400c     	add	r4, r1, r12
    454c: e5853000     	str	r3, [r5]
    4550: e08b500c     	add	r5, r11, r12
    4554: e089c00c     	add	r12, r9, r12
    4558: e58a3000     	str	r3, [r10]
    455c: e08ba000     	add	r10, r11, r0
    4560: e0890000     	add	r0, r9, r0
    4564: e58e3000     	str	r3, [lr]
    4568: e5823014     	str	r3, [r2, #0x14]
    456c: e5843000     	str	r3, [r4]
    4570: e282401c     	add	r4, r2, #28
    4574: e5853000     	str	r3, [r5]
    4578: e2822020     	add	r2, r2, #32
    457c: e58c3000     	str	r3, [r12]
    4580: e081e004     	add	lr, r1, r4
    4584: e5023008     	str	r3, [r2, #-0x8]
    4588: e08b5004     	add	r5, r11, r4
    458c: e5873000     	str	r3, [r7]
    4590: e089c004     	add	r12, r9, r4
    4594: e59d7004     	ldr	r7, [sp, #0x4]
    4598: e58a3000     	str	r3, [r10]
    459c: e5803000     	str	r3, [r0]
    45a0: e5023004     	str	r3, [r2, #-0x4]
    45a4: e1570002     	cmp	r7, r2
    45a8: e58e3000     	str	r3, [lr]
    45ac: e5853000     	str	r3, [r5]
    45b0: e58c3000     	str	r3, [r12]
    45b4: 1affffbc     	bne	0x44ac <arbhar_play_tilde_perform+0x264> @ imm = #-0x110
    45b8: e59d7008     	ldr	r7, [sp, #0x8]
    45bc: e598162c     	ldr	r1, [r8, #0x62c]
    45c0: e59f3208     	ldr	r3, [pc, #0x208]        @ 0x47d0 <arbhar_play_tilde_perform+0x588>
    45c4: e3510018     	cmp	r1, #24
    45c8: e59f9204     	ldr	r9, [pc, #0x204]        @ 0x47d4 <arbhar_play_tilde_perform+0x58c>
    45cc: e08fb003     	add	r11, pc, r3
    45d0: a3a01018     	movge	r1, #24
    45d4: e08f2009     	add	r2, pc, r9
    45d8: e08ba101     	add	r10, r11, r1, lsl #2
    45dc: ed92ca00     	vldr	s24, [r2]
    45e0: edda3a90     	vldr	s7, [r10, #576]
    45e4: eef43a4c     	vcmp.f32	s7, s24
    45e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    45ec: 0a000006     	beq	0x460c <arbhar_play_tilde_perform+0x3c4> @ imm = #0x18
    45f0: ee334acc     	vsub.f32	s8, s7, s24
    45f4: eddf5b69     	vldr	d21, [pc, #420]         @ 0x47a0 <arbhar_play_tilde_perform+0x558>
    45f8: eef77acc     	vcvt.f64.f32	d23, s24
    45fc: eef76ac4     	vcvt.f64.f32	d22, s8
    4600: ee467ba5     	vmla.f64	d23, d22, d21
    4604: eeb7cbe7     	vcvt.f32.f64	s24, d23
    4608: ed82ca00     	vstr	s24, [r2]
    460c: e1d70af0     	ldrsh	r0, [r7, #160]
    4610: e3500000     	cmp	r0, #0
    4614: da000006     	ble	0x4634 <arbhar_play_tilde_perform+0x3ec> @ imm = #0x18
    4618: eef74a00     	vmov.f32	s9, #1.000000e+00
    461c: eddf8b61     	vldr	d24, [pc, #388]         @ 0x47a8 <arbhar_play_tilde_perform+0x560>
    4620: ee3c8a64     	vsub.f32	s16, s24, s9
    4624: eef79b00     	vmov.f64	d25, #1.000000e+00
    4628: eef7aac8     	vcvt.f64.f32	d26, s16
    462c: ee4a9ba8     	vmla.f64	d25, d26, d24
    4630: eeb7cbe9     	vcvt.f32.f64	s24, d25
    4634: e2874c25     	add	r4, r7, #9472
    4638: e1a09007     	mov	r9, r7
    463c: e1a0a006     	mov	r10, r6
    4640: e2845028     	add	r5, r4, #40
    4644: e58d5010     	str	r5, [sp, #0x10]
    4648: eef7da00     	vmov.f32	s27, #1.000000e+00
    464c: eeb1da08     	vmov.f32	s26, #6.000000e+00
    4650: eef0ea08     	vmov.f32	s29, #3.000000e+00
    4654: eef8cac3     	vcvt.f32.s32	s25, s6
    4658: e5996110     	ldr	r6, [r9, #0x110]
    465c: e3560000     	cmp	r6, #0
    4660: ca0001ed     	bgt	0x4e1c <arbhar_play_tilde_perform+0xbd4> @ imm = #0x7b4
    4664: e599c114     	ldr	r12, [r9, #0x114]
    4668: edd98a51     	vldr	s17, [r9, #324]
    466c: e35c0000     	cmp	r12, #0
    4670: ca00024c     	bgt	0x4fa8 <arbhar_play_tilde_perform+0xd60> @ imm = #0x930
    4674: e5990158     	ldr	r0, [r9, #0x158]
    4678: e3500000     	cmp	r0, #0
    467c: da00000a     	ble	0x46ac <arbhar_play_tilde_perform+0x464> @ imm = #0x28
    4680: edd99a55     	vldr	s19, [r9, #340]
    4684: eddfaa4d     	vldr	s21, [pc, #308]         @ 0x47c0 <arbhar_play_tilde_perform+0x578>
    4688: eef49aea     	vcmpe.f32	s19, s21
    468c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4690: 8e799aea     	vsubhi.f32	s19, s19, s21
    4694: 93a00000     	movls	r0, #0
    4698: 93a02000     	movls	r2, #0
    469c: 9589012c     	strls	r0, [r9, #0x12c]
    46a0: 95890158     	strls	r0, [r9, #0x158]
    46a4: 95892154     	strls	r2, [r9, #0x154]
    46a8: 8dc99a55     	vstrhi	s19, [r9, #340]
    46ac: e599515c     	ldr	r5, [r9, #0x15c]
    46b0: e3550000     	cmp	r5, #0
    46b4: da000008     	ble	0x46dc <arbhar_play_tilde_perform+0x494> @ imm = #0x20
    46b8: e5996160     	ldr	r6, [r9, #0x160]
    46bc: e246c001     	sub	r12, r6, #1
    46c0: e589c160     	str	r12, [r9, #0x160]
    46c4: e35c0000     	cmp	r12, #0
    46c8: d3a0c000     	movle	r12, #0
    46cc: d589c15c     	strle	r12, [r9, #0x15c]
    46d0: d598c6d8     	ldrle	r12, [r8, #0x6d8]
    46d4: d26cc001     	rsble	r12, r12, #1
    46d8: d588c6d8     	strle	r12, [r8, #0x6d8]
    46dc: e599112c     	ldr	r1, [r9, #0x12c]
    46e0: e3510000     	cmp	r1, #0
    46e4: 0a0001da     	beq	0x4e54 <arbhar_play_tilde_perform+0xc0c> @ imm = #0x768
    46e8: eef7bae8     	vcvt.f64.f32	d27, s17
    46ec: e599e140     	ldr	lr, [r9, #0x140]
    46f0: eddfcb2e     	vldr	d28, [pc, #184]         @ 0x47b0 <arbhar_play_tilde_perform+0x568>
    46f4: e1a00007     	mov	r0, r7
    46f8: e37e0001     	cmn	lr, #1
    46fc: e5972030     	ldr	r2, [r7, #0x30]
    4700: eddfdb2c     	vldr	d29, [pc, #176]         @ 0x47b8 <arbhar_play_tilde_perform+0x570>
    4704: eeb01a6d     	vmov.f32	s2, s27
    4708: ed99ba52     	vldr	s22, [r9, #328]
    470c: ee4bdbac     	vmla.f64	d29, d27, d28
    4710: edd9ba55     	vldr	s23, [r9, #340]
    4714: eeb7fbed     	vcvt.f32.f64	s30, d29
    4718: 0a00026e     	beq	0x50d8 <arbhar_play_tilde_perform+0xe90> @ imm = #0x9b8
    471c: e3520000     	cmp	r2, #0
    4720: ed9f0a2f     	vldr	s0, [pc, #188]          @ 0x47e4 <arbhar_play_tilde_perform+0x59c>
    4724: 1ddf0a2c     	vldrne	s1, [pc, #176]          @ 0x47dc <arbhar_play_tilde_perform+0x594>
    4728: 0ddf0a2c     	vldreq	s1, [pc, #176]          @ 0x47e0 <arbhar_play_tilde_perform+0x598>
    472c: ebfff766     	bl	0x24cc <.plt+0xec>      @ imm = #-0x2268
    4730: e599613c     	ldr	r6, [r9, #0x13c]
    4734: e35a0000     	cmp	r10, #0
    4738: ed99ea49     	vldr	s28, [r9, #292]
    473c: e58d6018     	str	r6, [sp, #0x18]
    4740: da00025e     	ble	0x50c0 <arbhar_play_tilde_perform+0xe78> @ imm = #0x978
    4744: e59d402c     	ldr	r4, [sp, #0x2c]
    4748: e300080c     	movw	r0, #0x80c
    474c: eebd9acf     	vcvt.s32.f32	s18, s30
    4750: e59d3030     	ldr	r3, [sp, #0x30]
    4754: e59fe07c     	ldr	lr, [pc, #0x7c]         @ 0x47d8 <arbhar_play_tilde_perform+0x590>
    4758: e043c004     	sub	r12, r3, r4
    475c: e59d6038     	ldr	r6, [sp, #0x38]
    4760: e59d5034     	ldr	r5, [sp, #0x34]
    4764: e58dc004     	str	r12, [sp, #0x4]
    4768: e045b004     	sub	r11, r5, r4
    476c: e58db008     	str	r11, [sp, #0x8]
    4770: e796e00e     	ldr	lr, [r6, lr]
    4774: e3a05000     	mov	r5, #0
    4778: ee193a10     	vmov	r3, s18
    477c: e59d1028     	ldr	r1, [sp, #0x28]
    4780: eef7aa00     	vmov.f32	s21, #1.000000e+00
    4784: e0412004     	sub	r2, r1, r4
    4788: e58d200c     	str	r2, [sp, #0xc]
    478c: eeb7fb08     	vmov.f64	d15, #1.500000e+00
    4790: e020e390     	mla	r0, r0, r3, lr
    4794: eeb68b00     	vmov.f64	d8, #5.000000e-01
    4798: e58d0014     	str	r0, [sp, #0x14]
    479c: ea000066     	b	0x493c <arbhar_play_tilde_perform+0x6f4> @ imm = #0x198
    47a0: c3 f5 28 5c  	.word	0x5c28f5c3
    47a4: 8f c2 c5 3f  	.word	0x3fc5c28f
    47a8: 9a 99 99 99  	.word	0x9999999a
    47ac: 99 99 f5 3f  	.word	0x3ff59999
    47b0: 00 00 00 00  	.word	0x00000000
    47b4: 00 40 49 40  	.word	0x40494000
    47b8: 00 00 00 00  	.word	0x00000000
    47bc: 00 00 49 40  	.word	0x40490000
    47c0: 00 00 80 3c  	.word	0x3c800000
    47c4: 38 9f 01 00  	.word	0x00019f38
    47c8: 54 9d 01 00  	.word	0x00019d54
    47cc: 8c 9e 01 00  	.word	0x00019e8c
    47d0: e0 67 00 00  	.word	0x000067e0
    47d4: 7c 9b 01 00  	.word	0x00019b7c
    47d8: 38 01 00 00  	.word	0x00000138
    47dc: 00 00 e0 42  	.word	0x42e00000
    47e0: 00 00 e4 42  	.word	0x42e40000
    47e4: 00 00 00 00  	.word	0x00000000
    47e8: ad aa 2a 3e  	.word	0x3e2aaaad
    47ec: e597e030     	ldr	lr, [r7, #0x30]
    47f0: e3a03000     	mov	r3, #0
    47f4: ed1f1a06     	vldr	s2, [pc, #-24]          @ 0x47e4 <arbhar_play_tilde_perform+0x59c>
    47f8: e1a00007     	mov	r0, r7
    47fc: e15e0003     	cmp	lr, r3
    4800: e589312c     	str	r3, [r9, #0x12c]
    4804: 1d5f0a0c     	vldrne	s1, [pc, #-48]          @ 0x47dc <arbhar_play_tilde_perform+0x594>
    4808: 0d5f0a0c     	vldreq	s1, [pc, #-48]          @ 0x47e0 <arbhar_play_tilde_perform+0x598>
    480c: eeb00a41     	vmov.f32	s0, s2
    4810: ebfff72d     	bl	0x24cc <.plt+0xec>      @ imm = #-0x234c
    4814: ed5f2a0e     	vldr	s5, [pc, #-56]          @ 0x47e4 <arbhar_play_tilde_perform+0x59c>
    4818: eef04a62     	vmov.f32	s9, s5
    481c: eeb04a62     	vmov.f32	s8, s5
    4820: eef09a62     	vmov.f32	s19, s5
    4824: edd46a00     	vldr	s13, [r4]
    4828: e288ce85     	add	r12, r8, #2128
    482c: e59d6004     	ldr	r6, [sp, #0x4]
    4830: e2855001     	add	r5, r5, #1
    4834: e59d1008     	ldr	r1, [sp, #0x8]
    4838: eef95a08     	vmov.f32	s11, #-6.000000e+00
    483c: e086b004     	add	r11, r6, r4
    4840: e0810004     	add	r0, r1, r4
    4844: e59d200c     	ldr	r2, [sp, #0xc]
    4848: e0822004     	add	r2, r2, r4
    484c: e2844004     	add	r4, r4, #4
    4850: ee761aa2     	vadd.f32	s3, s13, s5
    4854: ed441a01     	vstr	s3, [r4, #-4]
    4858: eeb32a0b     	vmov.f32	s4, #2.700000e+01
    485c: ed995a47     	vldr	s10, [r9, #284]
    4860: ed9c3a03     	vldr	s6, [r12, #12]
    4864: ed9b9a00     	vldr	s18, [r11]
    4868: ee296a85     	vmul.f32	s12, s19, s10
    486c: eeb46ae5     	vcmpe.f32	s12, s11
    4870: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4874: beb06a65     	vmovlt.f32	s12, s11
    4878: eeb46acd     	vcmpe.f32	s12, s26
    487c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4880: 8eb06a4d     	vmovhi.f32	s12, s26
    4884: ee667a06     	vmul.f32	s15, s12, s12
    4888: eeb20a02     	vmov.f32	s0, #9.000000e+00
    488c: eef03a42     	vmov.f32	s7, s4
    4890: ee377a82     	vadd.f32	s14, s15, s4
    4894: ee473a80     	vmla.f32	s7, s15, s0
    4898: ee672a06     	vmul.f32	s5, s14, s12
    489c: eec29aa3     	vdiv.f32	s19, s5, s7
    48a0: ee099a83     	vmla.f32	s18, s19, s6
    48a4: ed8b9a00     	vstr	s18, [r11]
    48a8: eeb09a42     	vmov.f32	s18, s4
    48ac: edd96a47     	vldr	s13, [r9, #284]
    48b0: edd01a00     	vldr	s3, [r0]
    48b4: ee245a26     	vmul.f32	s10, s8, s13
    48b8: eeb45ae5     	vcmpe.f32	s10, s11
    48bc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    48c0: beb05a65     	vmovlt.f32	s10, s11
    48c4: eeb45acd     	vcmpe.f32	s10, s26
    48c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    48cc: 8eb05a4d     	vmovhi.f32	s10, s26
    48d0: ee254a05     	vmul.f32	s8, s10, s10
    48d4: ee343a02     	vadd.f32	s6, s8, s4
    48d8: ee049a00     	vmla.f32	s18, s8, s0
    48dc: ee637a05     	vmul.f32	s15, s6, s10
    48e0: ee877a89     	vdiv.f32	s14, s15, s18
    48e4: ee713a87     	vadd.f32	s7, s3, s14
    48e8: edc03a00     	vstr	s7, [r0]
    48ec: ed991a47     	vldr	s2, [r9, #284]
    48f0: edd21a00     	vldr	s3, [r2]
    48f4: ee649a81     	vmul.f32	s19, s9, s2
    48f8: eddc2a03     	vldr	s5, [r12, #12]
    48fc: eef49ae5     	vcmpe.f32	s19, s11
    4900: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4904: bef09a65     	vmovlt.f32	s19, s11
    4908: eef49acd     	vcmpe.f32	s19, s26
    490c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4910: 8ef09a4d     	vmovhi.f32	s19, s26
    4914: e15a0005     	cmp	r10, r5
    4918: ee695aa9     	vmul.f32	s11, s19, s19
    491c: ee756a82     	vadd.f32	s13, s11, s4
    4920: ee052a80     	vmla.f32	s4, s11, s0
    4924: ee260aa9     	vmul.f32	s0, s13, s19
    4928: ee805a02     	vdiv.f32	s10, s0, s4
    492c: eeb02a61     	vmov.f32	s4, s3
    4930: ee052a22     	vmla.f32	s4, s10, s5
    4934: ed822a00     	vstr	s4, [r2]
    4938: 0a0001e0     	beq	0x50c0 <arbhar_play_tilde_perform+0xe78> @ imm = #0x780
    493c: edd99a46     	vldr	s19, [r9, #280]
    4940: ed997a42     	vldr	s14, [r9, #264]
    4944: ee3eea29     	vadd.f32	s28, s28, s19
    4948: eeb85ac7     	vcvt.f32.s32	s10, s14
    494c: eeb45ace     	vcmpe.f32	s10, s28
    4950: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4954: 9affffa4     	bls	0x47ec <arbhar_play_tilde_perform+0x5a4> @ imm = #-0x170
    4958: ed991a43     	vldr	s2, [r9, #268]
    495c: e598c6cc     	ldr	r12, [r8, #0x6cc]
    4960: e35c0000     	cmp	r12, #0
    4964: eef81ac1     	vcvt.f32.s32	s3, s2
    4968: ee312a8e     	vadd.f32	s4, s3, s28
    496c: da000015     	ble	0x49c8 <arbhar_play_tilde_perform+0x780> @ imm = #0x54
    4970: e598b6d0     	ldr	r11, [r8, #0x6d0]
    4974: ed1f6a66     	vldr	s12, [pc, #-408]        @ 0x47e4 <arbhar_play_tilde_perform+0x59c>
    4978: e085100b     	add	r1, r5, r11
    497c: ee0a1a90     	vmov	s21, r1
    4980: eeb83aea     	vcvt.f32.s32	s6, s21
    4984: ee737a42     	vsub.f32	s15, s6, s4
    4988: eefd3ae7     	vcvt.s32.f32	s7, s15
    498c: ee132a90     	vmov	r2, s7
    4990: eef00b48     	vmov.f64	d16, d8
    4994: e3520000     	cmp	r2, #0
    4998: b2622000     	rsblt	r2, r2, #0
    499c: ee042a10     	vmov	s8, r2
    49a0: eeba4aea     	vcvt.f32.s32	s8, s8, #11
    49a4: eef7eac4     	vcvt.f64.f32	d30, s8
    49a8: ee5e0b8f     	vnmls.f64	d16, d30, d15
    49ac: eef7abe0     	vcvt.f32.f64	s21, d16
    49b0: eef4aaed     	vcmpe.f32	s21, s27
    49b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    49b8: 8ef0aa6d     	vmovhi.f32	s21, s27
    49bc: eef4aac6     	vcmpe.f32	s21, s12
    49c0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    49c4: bef0aa46     	vmovlt.f32	s21, s12
    49c8: e1d72af0     	ldrsh	r2, [r7, #160]
    49cc: eeb42aed     	vcmpe.f32	s4, s27
    49d0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    49d4: eef49aed     	vcmpe.f32	s19, s27
    49d8: beb02a6d     	vmovlt.f32	s4, s27
    49dc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    49e0: 9a00013c     	bls	0x4ed8 <arbhar_play_tilde_perform+0xc90> @ imm = #0x4f0
    49e4: eef4cac2     	vcmpe.f32	s25, s4
    49e8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    49ec: da00018a     	ble	0x501c <arbhar_play_tilde_perform+0xdd4> @ imm = #0x628
    49f0: eefd5ac2     	vcvt.s32.f32	s11, s4
    49f4: e59d6018     	ldr	r6, [sp, #0x18]
    49f8: ed5f2a86     	vldr	s5, [pc, #-536]         @ 0x47e8 <arbhar_play_tilde_perform+0x5a0>
    49fc: e3520000     	cmp	r2, #0
    4a00: e087e106     	add	lr, r7, r6, lsl #2
    4a04: e28e0a02     	add	r0, lr, #8192
    4a08: e5901894     	ldr	r1, [r0, #0x894]
    4a0c: ee153a90     	vmov	r3, s11
    4a10: ee3d5ac2     	vsub.f32	s10, s27, s4
    4a14: eeb80ae5     	vcvt.f32.s32	s0, s11
    4a18: ee322a40     	vsub.f32	s4, s4, s0
    4a1c: e243c107     	sub	r12, r3, #-1073741823
    4a20: e1a0310c     	lsl	r3, r12, #2
    4a24: e283c004     	add	r12, r3, #4
    4a28: e2836008     	add	r6, r3, #8
    4a2c: e081b00c     	add	r11, r1, r12
    4a30: e58db020     	str	r11, [sp, #0x20]
    4a34: e081b006     	add	r11, r1, r6
    4a38: e58db024     	str	r11, [sp, #0x24]
    4a3c: e081b003     	add	r11, r1, r3
    4a40: e58db01c     	str	r11, [sp, #0x1c]
    4a44: e59db020     	ldr	r11, [sp, #0x20]
    4a48: e283e00c     	add	lr, r3, #12
    4a4c: e081100e     	add	r1, r1, lr
    4a50: ee356a00     	vadd.f32	s12, s10, s0
    4a54: eddb4a00     	vldr	s9, [r11]
    4a58: e59db024     	ldr	r11, [sp, #0x24]
    4a5c: edd19a00     	vldr	s19, [r1]
    4a60: eddb6a00     	vldr	s13, [r11]
    4a64: e59db01c     	ldr	r11, [sp, #0x1c]
    4a68: ed9b9a00     	vldr	s18, [r11]
    4a6c: ee361ae4     	vsub.f32	s2, s13, s9
    4a70: ee791a69     	vsub.f32	s3, s18, s19
    4a74: ee549aae     	vnmls.f32	s19, s9, s29
    4a78: ee411a2e     	vmla.f32	s3, s2, s29
    4a7c: ee397a09     	vadd.f32	s14, s18, s18
    4a80: ee393ac7     	vsub.f32	s6, s19, s14
    4a84: ee013a82     	vmla.f32	s6, s3, s4
    4a88: ee662a22     	vmul.f32	s5, s12, s5
    4a8c: ee031a22     	vmla.f32	s2, s6, s5
    4a90: ee414a02     	vmla.f32	s9, s2, s4
    4a94: ee644a8c     	vmul.f32	s9, s9, s24
    4a98: da000012     	ble	0x4ae8 <arbhar_play_tilde_perform+0x8a0> @ imm = #0x48
    4a9c: e5900898     	ldr	r0, [r0, #0x898]
    4aa0: e0803003     	add	r3, r0, r3
    4aa4: e080c00c     	add	r12, r0, r12
    4aa8: e080e00e     	add	lr, r0, lr
    4aac: e0806006     	add	r6, r0, r6
    4ab0: ed9caa00     	vldr	s20, [r12]
    4ab4: edd39a00     	vldr	s19, [r3]
    4ab8: ed9e1a00     	vldr	s2, [lr]
    4abc: edd61a00     	vldr	s3, [r6]
    4ac0: ee395ac1     	vsub.f32	s10, s19, s2
    4ac4: ee313aca     	vsub.f32	s6, s3, s20
    4ac8: ee1a1a2e     	vnmls.f32	s2, s20, s29
    4acc: ee035a2e     	vmla.f32	s10, s6, s29
    4ad0: ee397aa9     	vadd.f32	s14, s19, s19
    4ad4: ee316a47     	vsub.f32	s12, s2, s14
    4ad8: ee056a02     	vmla.f32	s12, s10, s4
    4adc: ee063a22     	vmla.f32	s6, s12, s5
    4ae0: ee03aa02     	vmla.f32	s20, s6, s4
    4ae4: ee2aaa0c     	vmul.f32	s20, s20, s24
    4ae8: e3001203     	movw	r1, #0x203
    4aec: ed994a5b     	vldr	s8, [r9, #364]
    4af0: ee2b2a0e     	vmul.f32	s4, s22, s28
    4af4: eefd7ac2     	vcvt.s32.f32	s15, s4
    4af8: ee17ba90     	vmov	r11, s15
    4afc: eef83ae7     	vcvt.f32.s32	s7, s15
    4b00: ee726a63     	vsub.f32	s13, s4, s7
    4b04: ee3d9ac2     	vsub.f32	s18, s27, s4
    4b08: e15b0001     	cmp	r11, r1
    4b0c: a1a0b001     	movge	r11, r1
    4b10: e3520000     	cmp	r2, #0
    4b14: e59d2014     	ldr	r2, [sp, #0x14]
    4b18: e082010b     	add	r0, r2, r11, lsl #2
    4b1c: ed900a01     	vldr	s0, [r0, #4]
    4b20: ee799a23     	vadd.f32	s19, s18, s7
    4b24: edd05a00     	vldr	s11, [r0]
    4b28: ee662a80     	vmul.f32	s5, s13, s0
    4b2c: ee492aa5     	vmla.f32	s5, s19, s11
    4b30: eeb44aed     	vcmpe.f32	s8, s27
    4b34: eeb01a62     	vmov.f32	s2, s5
    4b38: da0000ce     	ble	0x4e78 <arbhar_play_tilde_perform+0xc30> @ imm = #0x338
    4b3c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4b40: 5a00000c     	bpl	0x4b78 <arbhar_play_tilde_perform+0x930> @ imm = #0x30
    4b44: edd92a5c     	vldr	s5, [r9, #368]
    4b48: ee7d9ac4     	vsub.f32	s19, s27, s8
    4b4c: edd91a5a     	vldr	s3, [r9, #360]
    4b50: ed995a59     	vldr	s10, [r9, #356]
    4b54: ee3d3ae2     	vsub.f32	s6, s27, s5
    4b58: ee297a85     	vmul.f32	s14, s19, s10
    4b5c: ee232a21     	vmul.f32	s4, s6, s3
    4b60: ee047a84     	vmla.f32	s14, s9, s8
    4b64: ee022a8a     	vmla.f32	s4, s5, s20
    4b68: ed897a59     	vstr	s14, [r9, #356]
    4b6c: eef04a47     	vmov.f32	s9, s14
    4b70: eeb0aa42     	vmov.f32	s20, s4
    4b74: ed892a5a     	vstr	s4, [r9, #360]
    4b78: ed996a48     	vldr	s12, [r9, #288]
    4b7c: ed994a4d     	vldr	s8, [r9, #308]
    4b80: ee6b7a86     	vmul.f32	s15, s23, s12
    4b84: edd93a4e     	vldr	s7, [r9, #312]
    4b88: ed990a5d     	vldr	s0, [r9, #372]
    4b8c: ee271a81     	vmul.f32	s2, s15, s2
    4b90: ee616a2a     	vmul.f32	s13, s2, s21
    4b94: ee644aa6     	vmul.f32	s9, s9, s13
    4b98: ee7d2ac4     	vsub.f32	s5, s27, s8
    4b9c: ee6a5a26     	vmul.f32	s11, s20, s13
    4ba0: ee7d9ae3     	vsub.f32	s19, s27, s7
    4ba4: ee249a84     	vmul.f32	s18, s9, s8
    4ba8: ee624aa4     	vmul.f32	s9, s5, s9
    4bac: ee253aa3     	vmul.f32	s6, s11, s7
    4bb0: ee3d5ac0     	vsub.f32	s10, s27, s0
    4bb4: ee691aa5     	vmul.f32	s3, s19, s11
    4bb8: eef02a64     	vmov.f32	s5, s9
    4bbc: eeb04a49     	vmov.f32	s8, s18
    4bc0: ee452a03     	vmla.f32	s5, s10, s6
    4bc4: ee014a80     	vmla.f32	s8, s3, s0
    4bc8: ee019a85     	vmla.f32	s18, s3, s10
    4bcc: ee404a03     	vmla.f32	s9, s0, s6
    4bd0: eef09a62     	vmov.f32	s19, s5
    4bd4: ed992a47     	vldr	s4, [r9, #284]
    4bd8: ee299a02     	vmul.f32	s18, s18, s4
    4bdc: eeb49acd     	vcmpe.f32	s18, s26
    4be0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4be4: 8ddf2ae3     	vldrhi	s5, [pc, #908]          @ 0x4f78 <arbhar_play_tilde_perform+0xd30>
    4be8: 8affff0d     	bhi	0x4824 <arbhar_play_tilde_perform+0x5dc> @ imm = #-0x3cc
    4bec: eeb97a08     	vmov.f32	s14, #-6.000000e+00
    4bf0: eeb49ac7     	vcmpe.f32	s18, s14
    4bf4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4bf8: beb09a47     	vmovlt.f32	s18, s14
    4bfc: ee296a09     	vmul.f32	s12, s18, s18
    4c00: eef37a0b     	vmov.f32	s15, #2.700000e+01
    4c04: eef23a02     	vmov.f32	s7, #9.000000e+00
    4c08: ee360a27     	vadd.f32	s0, s12, s15
    4c0c: ee467a23     	vmla.f32	s15, s12, s7
    4c10: ee201a09     	vmul.f32	s2, s0, s18
    4c14: eec12a27     	vdiv.f32	s5, s2, s15
    4c18: eaffff01     	b	0x4824 <arbhar_play_tilde_perform+0x5dc> @ imm = #-0x3fc
    4c1c: 1afffdb3     	bne	0x42f0 <arbhar_play_tilde_perform+0xa8> @ imm = #-0x934
    4c20: ed936a8c     	vldr	s12, [r3, #560]
    4c24: eeb56a40     	vcmp.f32	s12, #0
    4c28: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4c2c: 1afffdaf     	bne	0x42f0 <arbhar_play_tilde_perform+0xa8> @ imm = #-0x944
    4c30: ed930a17     	vldr	s0, [r3, #92]
    4c34: eeb50ac0     	vcmpe.f32	s0, #0
    4c38: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4c3c: dafffdac     	ble	0x42f4 <arbhar_play_tilde_perform+0xac> @ imm = #-0x950
    4c40: ed9f1ad4     	vldr	s2, [pc, #848]          @ 0x4f98 <arbhar_play_tilde_perform+0xd50>
    4c44: e1a00007     	mov	r0, r7
    4c48: eef30a07     	vmov.f32	s1, #2.300000e+01
    4c4c: ea00027a     	b	0x563c <arbhar_play_tilde_perform+0x13f4> @ imm = #0x9e8
    4c50: e5974030     	ldr	r4, [r7, #0x30]
    4c54: e3a01001     	mov	r1, #1
    4c58: e58710ac     	str	r1, [r7, #0xac]
    4c5c: e3540000     	cmp	r4, #0
    4c60: 0a00027c     	beq	0x5658 <arbhar_play_tilde_perform+0x1410> @ imm = #0x9f0
    4c64: dafffda1     	ble	0x42f0 <arbhar_play_tilde_perform+0xa8> @ imm = #-0x97c
    4c68: eeb71a00     	vmov.f32	s2, #1.000000e+00
    4c6c: edd31a8c     	vldr	s3, [r3, #560]
    4c70: eef41ac1     	vcmpe.f32	s3, s2
    4c74: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4c78: 5afffd9c     	bpl	0x42f0 <arbhar_play_tilde_perform+0xa8> @ imm = #-0x990
    4c7c: ed932a65     	vldr	s4, [r3, #404]
    4c80: eddf2bba     	vldr	d18, [pc, #744]         @ 0x4f70 <arbhar_play_tilde_perform+0xd28>
    4c84: eef73ac2     	vcvt.f64.f32	d19, s4
    4c88: edd32ad7     	vldr	s5, [r3, #860]
    4c8c: ed9f3aba     	vldr	s6, [pc, #744]          @ 0x4f7c <arbhar_play_tilde_perform+0xd34>
    4c90: ee634ba2     	vmul.f64	d20, d19, d18
    4c94: edd33a0c     	vldr	s7, [r3, #48]
    4c98: ed9f4ab8     	vldr	s8, [pc, #736]          @ 0x4f80 <arbhar_play_tilde_perform+0xd38>
    4c9c: eddf4ab8     	vldr	s9, [pc, #736]          @ 0x4f84 <arbhar_play_tilde_perform+0xd3c>
    4ca0: ed9f8abc     	vldr	s16, [pc, #752]         @ 0x4f98 <arbhar_play_tilde_perform+0xd50>
    4ca4: ee052a10     	vmov	s10, r2
    4ca8: ee523a83     	vnmls.f32	s7, s5, s6
    4cac: eef78be4     	vcvt.f32.f64	s17, d20
    4cb0: eef48ac4     	vcmpe.f32	s17, s8
    4cb4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4cb8: bef08a44     	vmovlt.f32	s17, s8
    4cbc: eef48ae4     	vcmpe.f32	s17, s9
    4cc0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4cc4: eef43ac8     	vcmpe.f32	s7, s16
    4cc8: 8ef08a64     	vmovhi.f32	s17, s9
    4ccc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4cd0: eeb89a45     	vcvt.f32.u32	s18, s10
    4cd4: 5a00028b     	bpl	0x5708 <arbhar_play_tilde_perform+0x14c0> @ imm = #0xa2c
    4cd8: eddf9aaf     	vldr	s19, [pc, #700]         @ 0x4f9c <arbhar_play_tilde_perform+0xd54>
    4cdc: eddfaaaf     	vldr	s21, [pc, #700]         @ 0x4fa0 <arbhar_play_tilde_perform+0xd58>
    4ce0: ee33baa9     	vadd.f32	s22, s7, s19
    4ce4: eeb4bac8     	vcmpe.f32	s22, s16
    4ce8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4cec: beb0ba48     	vmovlt.f32	s22, s16
    4cf0: eeb4bae9     	vcmpe.f32	s22, s19
    4cf4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4cf8: 8eb0ba69     	vmovhi.f32	s22, s19
    4cfc: ee6bba2a     	vmul.f32	s23, s22, s21
    4d00: eeb6ca00     	vmov.f32	s24, #5.000000e-01
    4d04: eef4bacc     	vcmpe.f32	s23, s24
    4d08: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4d0c: 8a000293     	bhi	0x5760 <arbhar_play_tilde_perform+0x1518> @ imm = #0xa4c
    4d10: ee3bfaab     	vadd.f32	s30, s23, s23
    4d14: e3a02000     	mov	r2, #0
    4d18: eddffaa1     	vldr	s31, [pc, #644]         @ 0x4fa4 <arbhar_play_tilde_perform+0xd5c>
    4d1c: ee6f6a0f     	vmul.f32	s13, s30, s30
    4d20: ee667aaf     	vmul.f32	s15, s13, s31
    4d24: ee675a8f     	vmul.f32	s11, s15, s30
    4d28: e58720ac     	str	r2, [r7, #0xac]
    4d2c: ee887aa5     	vdiv.f32	s14, s17, s11
    4d30: eeb49ac7     	vcmpe.f32	s18, s14
    4d34: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4d38: dafffd6c     	ble	0x42f0 <arbhar_play_tilde_perform+0xa8> @ imm = #-0xa50
    4d3c: edd30a47     	vldr	s1, [r3, #284]
    4d40: e3a09000     	mov	r9, #0
    4d44: e5970028     	ldr	r0, [r7, #0x28]
    4d48: eeb70ae0     	vcvt.f64.f32	d0, s1
    4d4c: ebfff64a     	bl	0x267c <.plt+0x29c>     @ imm = #-0x26d8
    4d50: e59f3234     	ldr	r3, [pc, #0x234]        @ 0x4f8c <arbhar_play_tilde_perform+0xd44>
    4d54: e08f5003     	add	r5, pc, r3
    4d58: e5983894     	ldr	r3, [r8, #0x894]
    4d5c: e5859000     	str	r9, [r5]
    4d60: ed930a17     	vldr	s0, [r3, #92]
    4d64: eafffd62     	b	0x42f4 <arbhar_play_tilde_perform+0xac> @ imm = #-0xa78
    4d68: edd36ad7     	vldr	s13, [r3, #860]
    4d6c: ed9f7a82     	vldr	s14, [pc, #520]         @ 0x4f7c <arbhar_play_tilde_perform+0xd34>
    4d70: edd37a0c     	vldr	s15, [r3, #48]
    4d74: ed9f6a87     	vldr	s12, [pc, #540]         @ 0x4f98 <arbhar_play_tilde_perform+0xd50>
    4d78: ee567a87     	vnmls.f32	s15, s13, s14
    4d7c: eef47ac6     	vcmpe.f32	s15, s12
    4d80: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4d84: 5affffb1     	bpl	0x4c50 <arbhar_play_tilde_perform+0xa08> @ imm = #-0x13c
    4d88: ed9f0a83     	vldr	s0, [pc, #524]          @ 0x4f9c <arbhar_play_tilde_perform+0xd54>
    4d8c: ed9f1a83     	vldr	s2, [pc, #524]          @ 0x4fa0 <arbhar_play_tilde_perform+0xd58>
    4d90: ee771a80     	vadd.f32	s3, s15, s0
    4d94: eef41ac6     	vcmpe.f32	s3, s12
    4d98: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4d9c: bef01a46     	vmovlt.f32	s3, s12
    4da0: eef41ac0     	vcmpe.f32	s3, s0
    4da4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4da8: 8ef01a40     	vmovhi.f32	s3, s0
    4dac: ee212a81     	vmul.f32	s4, s3, s2
    4db0: eef62a00     	vmov.f32	s5, #5.000000e-01
    4db4: eeb42ae2     	vcmpe.f32	s4, s5
    4db8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4dbc: 8a00026e     	bhi	0x577c <arbhar_play_tilde_perform+0x1534> @ imm = #0x9b8
    4dc0: ee328a02     	vadd.f32	s16, s4, s4
    4dc4: e3a05000     	mov	r5, #0
    4dc8: eddf8a75     	vldr	s17, [pc, #468]         @ 0x4fa4 <arbhar_play_tilde_perform+0xd5c>
    4dcc: ee289a08     	vmul.f32	s18, s16, s16
    4dd0: ee699a28     	vmul.f32	s19, s18, s17
    4dd4: ee69aa88     	vmul.f32	s21, s19, s16
    4dd8: e58750ac     	str	r5, [r7, #0xac]
    4ddc: e5974030     	ldr	r4, [r7, #0x30]
    4de0: eeb7ba00     	vmov.f32	s22, #1.000000e+00
    4de4: eef4aacb     	vcmpe.f32	s21, s22
    4de8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4dec: 4affff9a     	bmi	0x4c5c <arbhar_play_tilde_perform+0xa14> @ imm = #-0x198
    4df0: e3540000     	cmp	r4, #0
    4df4: caffff9b     	bgt	0x4c68 <arbhar_play_tilde_perform+0xa20> @ imm = #-0x194
    4df8: e3500000     	cmp	r0, #0
    4dfc: c3a02001     	movgt	r2, #1
    4e00: d3a02000     	movle	r2, #0
    4e04: e3540000     	cmp	r4, #0
    4e08: 01a04002     	moveq	r4, r2
    4e0c: 13a04000     	movne	r4, #0
    4e10: e3540000     	cmp	r4, #0
    4e14: 1a000228     	bne	0x56bc <arbhar_play_tilde_perform+0x1474> @ imm = #0x8a0
    4e18: eafffd34     	b	0x42f0 <arbhar_play_tilde_perform+0xa8> @ imm = #-0xb30
    4e1c: e5982894     	ldr	r2, [r8, #0x894]
    4e20: ed9fba55     	vldr	s22, [pc, #340]         @ 0x4f7c <arbhar_play_tilde_perform+0xd34>
    4e24: edd2bad7     	vldr	s23, [r2, #860]
    4e28: edd28a0c     	vldr	s17, [r2, #48]
    4e2c: eddffa59     	vldr	s31, [pc, #356]         @ 0x4f98 <arbhar_play_tilde_perform+0xd50>
    4e30: ee5b8a8b     	vnmls.f32	s17, s23, s22
    4e34: eef48aef     	vcmpe.f32	s17, s31
    4e38: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4e3c: 4a00007b     	bmi	0x5030 <arbhar_play_tilde_perform+0xde8> @ imm = #0x1ec
    4e40: e3a03001     	mov	r3, #1
    4e44: e58730ac     	str	r3, [r7, #0xac]
    4e48: e3a0c000     	mov	r12, #0
    4e4c: e589c110     	str	r12, [r9, #0x110]
    4e50: e589c12c     	str	r12, [r9, #0x12c]
    4e54: e59d6010     	ldr	r6, [sp, #0x10]
    4e58: e2899074     	add	r9, r9, #116
    4e5c: e1560009     	cmp	r6, r9
    4e60: 1afffdfc     	bne	0x4658 <arbhar_play_tilde_perform+0x410> @ imm = #-0x810
    4e64: e59d803c     	ldr	r8, [sp, #0x3c]
    4e68: e288001c     	add	r0, r8, #28
    4e6c: e28dd044     	add	sp, sp, #68
    4e70: ecbd8b10     	vpop	{d8, d9, d10, d11, d12, d13, d14, d15}
    4e74: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    4e78: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4e7c: 5a000005     	bpl	0x4e98 <arbhar_play_tilde_perform+0xc50> @ imm = #0x14
    4e80: ee7d1ac4     	vsub.f32	s3, s27, s8
    4e84: ed995a59     	vldr	s10, [r9, #356]
    4e88: ee213a85     	vmul.f32	s6, s3, s10
    4e8c: ee043a84     	vmla.f32	s6, s9, s8
    4e90: eef04a43     	vmov.f32	s9, s6
    4e94: ed893a59     	vstr	s6, [r9, #356]
    4e98: ed997a48     	vldr	s14, [r9, #288]
    4e9c: ed992a4c     	vldr	s4, [r9, #304]
    4ea0: ee2b6a87     	vmul.f32	s12, s23, s14
    4ea4: edd92a5d     	vldr	s5, [r9, #372]
    4ea8: ee264a01     	vmul.f32	s8, s12, s2
    4eac: ee647a2a     	vmul.f32	s15, s8, s21
    4eb0: ee674aa4     	vmul.f32	s9, s15, s9
    4eb4: ee7d3ac2     	vsub.f32	s7, s27, s4
    4eb8: ee220a24     	vmul.f32	s0, s4, s9
    4ebc: ee636aa4     	vmul.f32	s13, s7, s9
    4ec0: ee7d5ae2     	vsub.f32	s11, s27, s5
    4ec4: ee604a22     	vmul.f32	s9, s0, s5
    4ec8: ee609a25     	vmul.f32	s19, s0, s11
    4ecc: ee259aa6     	vmul.f32	s18, s11, s13
    4ed0: ee224aa6     	vmul.f32	s8, s5, s13
    4ed4: eaffff3e     	b	0x4bd4 <arbhar_play_tilde_perform+0x98c> @ imm = #-0x308
    4ed8: eefd7ac2     	vcvt.s32.f32	s15, s4
    4edc: e59de018     	ldr	lr, [sp, #0x18]
    4ee0: eddf2a28     	vldr	s5, [pc, #160]          @ 0x4f88 <arbhar_play_tilde_perform+0xd40>
    4ee4: e3520000     	cmp	r2, #0
    4ee8: e087010e     	add	r0, r7, lr, lsl #2
    4eec: e2800a02     	add	r0, r0, #8192
    4ef0: e5901894     	ldr	r1, [r0, #0x894]
    4ef4: ee173a90     	vmov	r3, s15
    4ef8: ee3d9ac2     	vsub.f32	s18, s27, s4
    4efc: eef83ae7     	vcvt.f32.s32	s7, s15
    4f00: ee322a63     	vsub.f32	s4, s4, s7
    4f04: e243c107     	sub	r12, r3, #-1073741823
    4f08: e1a0310c     	lsl	r3, r12, #2
    4f0c: e283c004     	add	r12, r3, #4
    4f10: e2836008     	add	r6, r3, #8
    4f14: e081b00c     	add	r11, r1, r12
    4f18: e58db020     	str	r11, [sp, #0x20]
    4f1c: e081b006     	add	r11, r1, r6
    4f20: e58db024     	str	r11, [sp, #0x24]
    4f24: e081b003     	add	r11, r1, r3
    4f28: e58db01c     	str	r11, [sp, #0x1c]
    4f2c: e59db020     	ldr	r11, [sp, #0x20]
    4f30: e283e00c     	add	lr, r3, #12
    4f34: e081100e     	add	r1, r1, lr
    4f38: ee396a23     	vadd.f32	s12, s18, s7
    4f3c: eddb4a00     	vldr	s9, [r11]
    4f40: e59db024     	ldr	r11, [sp, #0x24]
    4f44: ed910a00     	vldr	s0, [r1]
    4f48: ed9b4a00     	vldr	s8, [r11]
    4f4c: e59db01c     	ldr	r11, [sp, #0x1c]
    4f50: eddb5a00     	vldr	s11, [r11]
    4f54: ee341a64     	vsub.f32	s2, s8, s9
    4f58: ee751ac0     	vsub.f32	s3, s11, s0
    4f5c: ee140aae     	vnmls.f32	s0, s9, s29
    4f60: ee756aa5     	vadd.f32	s13, s11, s11
    4f64: ee411a2e     	vmla.f32	s3, s2, s29
    4f68: ee303a66     	vsub.f32	s6, s0, s13
    4f6c: eafffec4     	b	0x4a84 <arbhar_play_tilde_perform+0x83c> @ imm = #-0x4f0
    4f70: 0b d7 a3 70  	.word	0x70a3d70b
    4f74: 3d 0a f7 3f  	.word	0x3ff70a3d
    4f78: 9e d8 89 3f  	.word	0x3f89d89e
    4f7c: 00 00 00 42  	.word	0x42000000
    4f80: 00 00 00 43  	.word	0x43000000
    4f84: 00 a0 0c 48  	.word	0x480ca000
    4f88: ad aa 2a 3e  	.word	0x3e2aaaad
    4f8c: 40 94 01 00  	.word	0x00019440
    4f90: 00 00 c6 42  	.word	0x42c60000
    4f94: 00 00 ea 42  	.word	0x42ea0000
    4f98: 00 00 00 00  	.word	0x00000000
    4f9c: 00 f0 7f 45  	.word	0x457ff000
    4fa0: 01 08 80 39  	.word	0x39800801
    4fa4: 00 00 28 42  	.word	0x42280000
    4fa8: e598e6d8     	ldr	lr, [r8, #0x6d8]
    4fac: e3a02000     	mov	r2, #0
    4fb0: ed5f0a0a     	vldr	s1, [pc, #-40]          @ 0x4f90 <arbhar_play_tilde_perform+0xd48>
    4fb4: e1a00007     	mov	r0, r7
    4fb8: e26e1001     	rsb	r1, lr, #1
    4fbc: e58816d8     	str	r1, [r8, #0x6d8]
    4fc0: e5892114     	str	r2, [r9, #0x114]
    4fc4: ed995a43     	vldr	s10, [r9, #268]
    4fc8: ed1f0a0e     	vldr	s0, [pc, #-56]          @ 0x4f98 <arbhar_play_tilde_perform+0xd50>
    4fcc: eeb81ac5     	vcvt.f32.s32	s2, s10
    4fd0: ebfff53d     	bl	0x24cc <.plt+0xec>      @ imm = #-0x2b0c
    4fd4: e5973030     	ldr	r3, [r7, #0x30]
    4fd8: e3530000     	cmp	r3, #0
    4fdc: dafffda4     	ble	0x4674 <arbhar_play_tilde_perform+0x42c> @ imm = #-0x970
    4fe0: e598b894     	ldr	r11, [r8, #0x894]
    4fe4: ed9b9a75     	vldr	s18, [r11, #468]
    4fe8: eefd5ac9     	vcvt.s32.f32	s11, s18
    4fec: ee154a90     	vmov	r4, s11
    4ff0: e3540000     	cmp	r4, #0
    4ff4: 1d1f1a19     	vldrne	s2, [pc, #-100]         @ 0x4f98 <arbhar_play_tilde_perform+0xd50>
    4ff8: 01a00007     	moveq	r0, r7
    4ffc: 11a00007     	movne	r0, r7
    5000: 0eb01a6d     	vmoveq.f32	s2, s27
    5004: 0d5f0a1e     	vldreq	s1, [pc, #-120]         @ 0x4f94 <arbhar_play_tilde_perform+0xd4c>
    5008: 0d1f0a1e     	vldreq	s0, [pc, #-120]         @ 0x4f98 <arbhar_play_tilde_perform+0xd50>
    500c: 1d5f0a20     	vldrne	s1, [pc, #-128]         @ 0x4f94 <arbhar_play_tilde_perform+0xd4c>
    5010: 1eb00a41     	vmovne.f32	s0, s2
    5014: ebfff52c     	bl	0x24cc <.plt+0xec>      @ imm = #-0x2b50
    5018: eafffd95     	b	0x4674 <arbhar_play_tilde_perform+0x42c> @ imm = #-0x9ac
    501c: e3520000     	cmp	r2, #0
    5020: dd5f4a24     	vldrle	s9, [pc, #-144]         @ 0x4f98 <arbhar_play_tilde_perform+0xd50>
    5024: cd1faa25     	vldrgt	s20, [pc, #-148]        @ 0x4f98 <arbhar_play_tilde_perform+0xd50>
    5028: cef04a4a     	vmovgt.f32	s9, s20
    502c: eafffead     	b	0x4ae8 <arbhar_play_tilde_perform+0x8a0> @ imm = #-0x54c
    5030: ed1ffa27     	vldr	s30, [pc, #-156]        @ 0x4f9c <arbhar_play_tilde_perform+0xd54>
    5034: ed1f8a27     	vldr	s16, [pc, #-156]        @ 0x4fa0 <arbhar_play_tilde_perform+0xd58>
    5038: ee38ea8f     	vadd.f32	s28, s17, s30
    503c: eeb4eaef     	vcmpe.f32	s28, s31
    5040: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5044: beb0ea6f     	vmovlt.f32	s28, s31
    5048: eeb4eacf     	vcmpe.f32	s28, s30
    504c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5050: 8eb0ea4f     	vmovhi.f32	s28, s30
    5054: ee6eaa08     	vmul.f32	s21, s28, s16
    5058: eef64a00     	vmov.f32	s9, #5.000000e-01
    505c: eef4aae4     	vcmpe.f32	s21, s9
    5060: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5064: 93a06000     	movls	r6, #0
    5068: 958760ac     	strls	r6, [r7, #0xac]
    506c: 9affff75     	bls	0x4e48 <arbhar_play_tilde_perform+0xc00> @ imm = #-0x22c
    5070: ee3d4aea     	vsub.f32	s8, s27, s21
    5074: e3a02001     	mov	r2, #1
    5078: ed1f3a37     	vldr	s6, [pc, #-220]         @ 0x4fa4 <arbhar_play_tilde_perform+0xd5c>
    507c: e58720ac     	str	r2, [r7, #0xac]
    5080: eddf1b3e     	vldr	d17, [pc, #248]         @ 0x5180 <arbhar_play_tilde_perform+0xf38>
    5084: ee349a04     	vadd.f32	s18, s8, s8
    5088: ee296a09     	vmul.f32	s12, s18, s18
    508c: ee667a03     	vmul.f32	s15, s12, s6
    5090: ee277a89     	vmul.f32	s14, s15, s18
    5094: eef72ac7     	vcvt.f64.f32	d18, s14
    5098: eef42be1     	vcmpe.f64	d18, d17
    509c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    50a0: 5046600a     	subpl	r6, r6, r10
    50a4: 55896110     	strpl	r6, [r9, #0x110]
    50a8: 4affff66     	bmi	0x4e48 <arbhar_play_tilde_perform+0xc00> @ imm = #-0x268
    50ac: e59d6010     	ldr	r6, [sp, #0x10]
    50b0: e2899074     	add	r9, r9, #116
    50b4: e1560009     	cmp	r6, r9
    50b8: 1afffd66     	bne	0x4658 <arbhar_play_tilde_perform+0x410> @ imm = #-0xa68
    50bc: eaffff68     	b	0x4e64 <arbhar_play_tilde_perform+0xc1c> @ imm = #-0x260
    50c0: e59d6010     	ldr	r6, [sp, #0x10]
    50c4: e2899074     	add	r9, r9, #116
    50c8: ed89ea2c     	vstr	s28, [r9, #176]
    50cc: e1560009     	cmp	r6, r9
    50d0: 1afffd60     	bne	0x4658 <arbhar_play_tilde_perform+0x410> @ imm = #-0xa80
    50d4: eaffff62     	b	0x4e64 <arbhar_play_tilde_perform+0xc1c> @ imm = #-0x278
    50d8: e3520000     	cmp	r2, #0
    50dc: ed9f0a2a     	vldr	s0, [pc, #168]          @ 0x518c <arbhar_play_tilde_perform+0xf44>
    50e0: 1ddf0a2a     	vldrne	s1, [pc, #168]          @ 0x5190 <arbhar_play_tilde_perform+0xf48>
    50e4: 0ddf0a2a     	vldreq	s1, [pc, #168]          @ 0x5194 <arbhar_play_tilde_perform+0xf4c>
    50e8: ebfff4f7     	bl	0x24cc <.plt+0xec>      @ imm = #-0x2c24
    50ec: e599313c     	ldr	r3, [r9, #0x13c]
    50f0: e35a0000     	cmp	r10, #0
    50f4: edd99a49     	vldr	s19, [r9, #292]
    50f8: e58d3018     	str	r3, [sp, #0x18]
    50fc: da00013c     	ble	0x55f4 <arbhar_play_tilde_perform+0x13ac> @ imm = #0x4f0
    5100: e59d002c     	ldr	r0, [sp, #0x2c]
    5104: e300580c     	movw	r5, #0x80c
    5108: eebdeacf     	vcvt.s32.f32	s28, s30
    510c: e59fb074     	ldr	r11, [pc, #0x74]        @ 0x5188 <arbhar_play_tilde_perform+0xf40>
    5110: e59d4030     	ldr	r4, [sp, #0x30]
    5114: e59dc034     	ldr	r12, [sp, #0x34]
    5118: e59d2038     	ldr	r2, [sp, #0x38]
    511c: e0446000     	sub	r6, r4, r0
    5120: e04ce000     	sub	lr, r12, r0
    5124: e58d6004     	str	r6, [sp, #0x4]
    5128: e58de008     	str	lr, [sp, #0x8]
    512c: e1a04000     	mov	r4, r0
    5130: e792300b     	ldr	r3, [r2, r11]
    5134: ee1e1a10     	vmov	r1, s28
    5138: e59db028     	ldr	r11, [sp, #0x28]
    513c: eeb78a00     	vmov.f32	s16, #1.000000e+00
    5140: ed9fea11     	vldr	s28, [pc, #68]          @ 0x518c <arbhar_play_tilde_perform+0xf44>
    5144: e04b6000     	sub	r6, r11, r0
    5148: e3a0b000     	mov	r11, #0
    514c: eef0fa4e     	vmov.f32	s31, s28
    5150: eeb0fa4e     	vmov.f32	s30, s28
    5154: eef0aa4e     	vmov.f32	s21, s28
    5158: e0253195     	mla	r5, r5, r1, r3
    515c: e1a01006     	mov	r1, r6
    5160: e1a06007     	mov	r6, r7
    5164: e58d100c     	str	r1, [sp, #0xc]
    5168: eeb99a08     	vmov.f32	s18, #-6.000000e+00
    516c: e1a0700a     	mov	r7, r10
    5170: e58d5014     	str	r5, [sp, #0x14]
    5174: eef38a0b     	vmov.f32	s17, #2.700000e+01
    5178: ea00009a     	b	0x53e8 <arbhar_play_tilde_perform+0x11a0> @ imm = #0x268
    517c: e320f000     	nop
    5180: 9a 99 99 99  	.word	0x9999999a
    5184: 99 99 a9 3f  	.word	0x3fa99999
    5188: 38 01 00 00  	.word	0x00000138
    518c: 00 00 00 00  	.word	0x00000000
    5190: 00 00 e2 42  	.word	0x42e20000
    5194: 00 00 e6 42  	.word	0x42e60000
    5198: ad aa 2a 3e  	.word	0x3e2aaaad
    519c: eef4cac0     	vcmpe.f32	s25, s0
    51a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    51a4: ca0000b9     	bgt	0x5490 <arbhar_play_tilde_perform+0x1248> @ imm = #0x2e4
    51a8: e3520000     	cmp	r2, #0
    51ac: dd1fea0a     	vldrle	s28, [pc, #-40]         @ 0x518c <arbhar_play_tilde_perform+0xf44>
    51b0: cd1faa0b     	vldrgt	s20, [pc, #-44]         @ 0x518c <arbhar_play_tilde_perform+0xf44>
    51b4: ceb0ea4a     	vmovgt.f32	s28, s20
    51b8: e3001202     	movw	r1, #0x202
    51bc: ed994a5b     	vldr	s8, [r9, #364]
    51c0: ee6b3a29     	vmul.f32	s7, s22, s19
    51c4: eebd5ae3     	vcvt.s32.f32	s10, s7
    51c8: ee15aa10     	vmov	r10, s10
    51cc: eef8aac5     	vcvt.f32.s32	s21, s10
    51d0: ee33faea     	vsub.f32	s30, s7, s21
    51d4: ee7d6ae3     	vsub.f32	s13, s27, s7
    51d8: e15a0001     	cmp	r10, r1
    51dc: a1a0a001     	movge	r10, r1
    51e0: e3520000     	cmp	r2, #0
    51e4: e59d2014     	ldr	r2, [sp, #0x14]
    51e8: e082010a     	add	r0, r2, r10, lsl #2
    51ec: edd04a01     	vldr	s9, [r0, #4]
    51f0: ee767aaa     	vadd.f32	s15, s13, s21
    51f4: edd05a00     	vldr	s11, [r0]
    51f8: ee6ffa24     	vmul.f32	s31, s30, s9
    51fc: ee47faa5     	vmla.f32	s31, s15, s11
    5200: eeb44aed     	vcmpe.f32	s8, s27
    5204: eeb00a6f     	vmov.f32	s0, s31
    5208: da0000df     	ble	0x558c <arbhar_play_tilde_perform+0x1344> @ imm = #0x37c
    520c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5210: 5a00000c     	bpl	0x5248 <arbhar_play_tilde_perform+0x1000> @ imm = #0x30
    5214: ed996a5c     	vldr	s12, [r9, #368]
    5218: ee7d6ac4     	vsub.f32	s13, s27, s8
    521c: ed997a5a     	vldr	s14, [r9, #360]
    5220: ed991a59     	vldr	s2, [r9, #356]
    5224: ee7d7ac6     	vsub.f32	s15, s27, s12
    5228: ee661a81     	vmul.f32	s3, s13, s2
    522c: ee272a87     	vmul.f32	s4, s15, s14
    5230: ee4e1a04     	vmla.f32	s3, s28, s8
    5234: ee062a0a     	vmla.f32	s4, s12, s20
    5238: edc91a59     	vstr	s3, [r9, #356]
    523c: eeb0ea61     	vmov.f32	s28, s3
    5240: eeb0aa42     	vmov.f32	s20, s4
    5244: ed892a5a     	vstr	s4, [r9, #360]
    5248: edd92a48     	vldr	s5, [r9, #288]
    524c: ed993a4d     	vldr	s6, [r9, #308]
    5250: ee6b3aa2     	vmul.f32	s7, s23, s5
    5254: ed994a4e     	vldr	s8, [r9, #312]
    5258: ed995a5d     	vldr	s10, [r9, #372]
    525c: ee230a80     	vmul.f32	s0, s7, s0
    5260: ee604a08     	vmul.f32	s9, s0, s16
    5264: ee6e5a24     	vmul.f32	s11, s28, s9
    5268: ee3deac3     	vsub.f32	s28, s27, s6
    526c: ee2afa24     	vmul.f32	s30, s20, s9
    5270: ee7dfac4     	vsub.f32	s31, s27, s8
    5274: ee65aa83     	vmul.f32	s21, s11, s6
    5278: ee2eea25     	vmul.f32	s28, s28, s11
    527c: ee6f6a8f     	vmul.f32	s13, s31, s30
    5280: ee2f7a04     	vmul.f32	s14, s30, s8
    5284: ee3d6ac5     	vsub.f32	s12, s27, s10
    5288: eef0fa6a     	vmov.f32	s31, s21
    528c: eeb0fa4e     	vmov.f32	s30, s28
    5290: ee46fa85     	vmla.f32	s31, s13, s10
    5294: ee46aa86     	vmla.f32	s21, s13, s12
    5298: ee06fa07     	vmla.f32	s30, s12, s14
    529c: ee05ea07     	vmla.f32	s28, s10, s14
    52a0: ed991a47     	vldr	s2, [r9, #284]
    52a4: e2883e85     	add	r3, r8, #2128
    52a8: e59dc004     	ldr	r12, [sp, #0x4]
    52ac: e28bb001     	add	r11, r11, #1
    52b0: e59d1008     	ldr	r1, [sp, #0x8]
    52b4: edd41a00     	vldr	s3, [r4]
    52b8: e08c5004     	add	r5, r12, r4
    52bc: e081a004     	add	r10, r1, r4
    52c0: e59d200c     	ldr	r2, [sp, #0xc]
    52c4: e0822004     	add	r2, r2, r4
    52c8: e2844004     	add	r4, r4, #4
    52cc: ee6a7a81     	vmul.f32	s15, s21, s2
    52d0: eef47ac9     	vcmpe.f32	s15, s18
    52d4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    52d8: bef07a49     	vmovlt.f32	s15, s18
    52dc: eef47acd     	vcmpe.f32	s15, s26
    52e0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    52e4: 8ef07a4d     	vmovhi.f32	s15, s26
    52e8: ee272aa7     	vmul.f32	s4, s15, s15
    52ec: eef22a02     	vmov.f32	s5, #9.000000e+00
    52f0: eef05a68     	vmov.f32	s11, s17
    52f4: ee323a28     	vadd.f32	s6, s4, s17
    52f8: ee425a22     	vmla.f32	s11, s4, s5
    52fc: ee234a27     	vmul.f32	s8, s6, s15
    5300: ee845a25     	vdiv.f32	s10, s8, s11
    5304: eef05a68     	vmov.f32	s11, s17
    5308: ee310a85     	vadd.f32	s0, s3, s10
    530c: ed040a01     	vstr	s0, [r4, #-4]
    5310: edd94a47     	vldr	s9, [r9, #284]
    5314: ed937a03     	vldr	s14, [r3, #12]
    5318: ee2f1a24     	vmul.f32	s2, s30, s9
    531c: edd56a00     	vldr	s13, [r5]
    5320: eeb41ac9     	vcmpe.f32	s2, s18
    5324: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5328: beb01a49     	vmovlt.f32	s2, s18
    532c: eeb41acd     	vcmpe.f32	s2, s26
    5330: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5334: 8eb01a4d     	vmovhi.f32	s2, s26
    5338: ee611a01     	vmul.f32	s3, s2, s2
    533c: ee316aa8     	vadd.f32	s12, s3, s17
    5340: ee415aa2     	vmla.f32	s11, s3, s5
    5344: ee667a01     	vmul.f32	s15, s12, s2
    5348: ee873aa5     	vdiv.f32	s6, s15, s11
    534c: eef05a68     	vmov.f32	s11, s17
    5350: ee436a07     	vmla.f32	s13, s6, s14
    5354: edc56a00     	vstr	s13, [r5]
    5358: edd93a47     	vldr	s7, [r9, #284]
    535c: ed9a4a00     	vldr	s8, [r10]
    5360: ee2f0aa3     	vmul.f32	s0, s31, s7
    5364: eeb40ac9     	vcmpe.f32	s0, s18
    5368: eef1fa10     	vmrs	APSR_nzcv, fpscr
    536c: beb00a49     	vmovlt.f32	s0, s18
    5370: eeb40acd     	vcmpe.f32	s0, s26
    5374: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5378: 8eb00a4d     	vmovhi.f32	s0, s26
    537c: ee604a00     	vmul.f32	s9, s0, s0
    5380: ee347aa8     	vadd.f32	s14, s9, s17
    5384: ee445aa2     	vmla.f32	s11, s9, s5
    5388: ee271a00     	vmul.f32	s2, s14, s0
    538c: eec11a25     	vdiv.f32	s3, s2, s11
    5390: eef05a68     	vmov.f32	s11, s17
    5394: ee346a21     	vadd.f32	s12, s8, s3
    5398: ed8a6a00     	vstr	s12, [r10]
    539c: ed992a47     	vldr	s4, [r9, #284]
    53a0: ed933a03     	vldr	s6, [r3, #12]
    53a4: ee6e7a02     	vmul.f32	s15, s28, s4
    53a8: edd26a00     	vldr	s13, [r2]
    53ac: eef47ac9     	vcmpe.f32	s15, s18
    53b0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    53b4: bef07a49     	vmovlt.f32	s15, s18
    53b8: eef47acd     	vcmpe.f32	s15, s26
    53bc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    53c0: 8ef07a4d     	vmovhi.f32	s15, s26
    53c4: e157000b     	cmp	r7, r11
    53c8: ee673aa7     	vmul.f32	s7, s15, s15
    53cc: ee334aa8     	vadd.f32	s8, s7, s17
    53d0: ee435aa2     	vmla.f32	s11, s7, s5
    53d4: ee240a27     	vmul.f32	s0, s8, s15
    53d8: eec04a25     	vdiv.f32	s9, s0, s11
    53dc: ee446a83     	vmla.f32	s13, s9, s6
    53e0: edc26a00     	vstr	s13, [r2]
    53e4: 0a000080     	beq	0x55ec <arbhar_play_tilde_perform+0x13a4> @ imm = #0x200
    53e8: eef49aee     	vcmpe.f32	s19, s29
    53ec: eef1fa10     	vmrs	APSR_nzcv, fpscr
    53f0: 8a000009     	bhi	0x541c <arbhar_play_tilde_perform+0x11d4> @ imm = #0x24
    53f4: e596a030     	ldr	r10, [r6, #0x30]
    53f8: e3a0c000     	mov	r12, #0
    53fc: ed1f1a9e     	vldr	s2, [pc, #-632]         @ 0x518c <arbhar_play_tilde_perform+0xf44>
    5400: e1a00006     	mov	r0, r6
    5404: e15a000c     	cmp	r10, r12
    5408: e589c12c     	str	r12, [r9, #0x12c]
    540c: 1d5f0aa1     	vldrne	s1, [pc, #-644]         @ 0x5190 <arbhar_play_tilde_perform+0xf48>
    5410: 0d5f0aa1     	vldreq	s1, [pc, #-644]         @ 0x5194 <arbhar_play_tilde_perform+0xf4c>
    5414: eeb00a41     	vmov.f32	s0, s2
    5418: ebfff42b     	bl	0x24cc <.plt+0xec>      @ imm = #-0x2f54
    541c: edd96a46     	vldr	s13, [r9, #280]
    5420: ee799ae6     	vsub.f32	s19, s19, s13
    5424: eef49aee     	vcmpe.f32	s19, s29
    5428: eef1fa10     	vmrs	APSR_nzcv, fpscr
    542c: daffff9b     	ble	0x52a0 <arbhar_play_tilde_perform+0x1058> @ imm = #-0x194
    5430: edd9fa43     	vldr	s31, [r9, #268]
    5434: e59806cc     	ldr	r0, [r8, #0x6cc]
    5438: e3500000     	cmp	r0, #0
    543c: eef87aef     	vcvt.f32.s32	s15, s31
    5440: ee370aa9     	vadd.f32	s0, s15, s19
    5444: da00000d     	ble	0x5480 <arbhar_play_tilde_perform+0x1238> @ imm = #0x34
    5448: e59826d0     	ldr	r2, [r8, #0x6d0]
    544c: e08b3002     	add	r3, r11, r2
    5450: ee073a10     	vmov	s14, r3
    5454: eeb86ac7     	vcvt.f32.s32	s12, s14
    5458: ee361a40     	vsub.f32	s2, s12, s0
    545c: eefd1ac1     	vcvt.s32.f32	s3, s2
    5460: ee115a90     	vmov	r5, s3
    5464: e3550000     	cmp	r5, #0
    5468: b2655000     	rsblt	r5, r5, #0
    546c: ee085a10     	vmov	s16, r5
    5470: eeba8aca     	vcvt.f32.s32	s16, s16, #12
    5474: eeb48aed     	vcmpe.f32	s16, s27
    5478: eef1fa10     	vmrs	APSR_nzcv, fpscr
    547c: 8eb08a6d     	vmovhi.f32	s16, s27
    5480: e1d62af0     	ldrsh	r2, [r6, #160]
    5484: eef46aed     	vcmpe.f32	s13, s27
    5488: eef1fa10     	vmrs	APSR_nzcv, fpscr
    548c: 8affff42     	bhi	0x519c <arbhar_play_tilde_perform+0xf54> @ imm = #-0x2f8
    5490: eebd2ac0     	vcvt.s32.f32	s4, s0
    5494: e59de018     	ldr	lr, [sp, #0x18]
    5498: ed5f2ac2     	vldr	s5, [pc, #-776]         @ 0x5198 <arbhar_play_tilde_perform+0xf50>
    549c: e3520000     	cmp	r2, #0
    54a0: e086110e     	add	r1, r6, lr, lsl #2
    54a4: e2810a02     	add	r0, r1, #8192
    54a8: e5901894     	ldr	r1, [r0, #0x894]
    54ac: ee12aa10     	vmov	r10, s4
    54b0: eeb83ac2     	vcvt.f32.s32	s6, s4
    54b4: ee3deac0     	vsub.f32	s28, s27, s0
    54b8: ee70fa43     	vsub.f32	s31, s0, s6
    54bc: e24ac10b     	sub	r12, r10, #-1073741822
    54c0: e1a0310c     	lsl	r3, r12, #2
    54c4: e283c004     	add	r12, r3, #4
    54c8: e2835008     	add	r5, r3, #8
    54cc: e081a00c     	add	r10, r1, r12
    54d0: e58da020     	str	r10, [sp, #0x20]
    54d4: e081a005     	add	r10, r1, r5
    54d8: e58da024     	str	r10, [sp, #0x24]
    54dc: e081a003     	add	r10, r1, r3
    54e0: e58da01c     	str	r10, [sp, #0x1c]
    54e4: e59da020     	ldr	r10, [sp, #0x20]
    54e8: e283e00c     	add	lr, r3, #12
    54ec: e081100e     	add	r1, r1, lr
    54f0: ee7e7a03     	vadd.f32	s15, s28, s6
    54f4: edda3a00     	vldr	s7, [r10]
    54f8: e59da024     	ldr	r10, [sp, #0x24]
    54fc: edd15a00     	vldr	s11, [r1]
    5500: ed9a4a00     	vldr	s8, [r10]
    5504: e59da01c     	ldr	r10, [sp, #0x1c]
    5508: ee270aa2     	vmul.f32	s0, s15, s5
    550c: ed9a5a00     	vldr	s10, [r10]
    5510: ee74aa63     	vsub.f32	s21, s8, s7
    5514: ee754a65     	vsub.f32	s9, s10, s11
    5518: ee535aae     	vnmls.f32	s11, s7, s29
    551c: ee4a4aae     	vmla.f32	s9, s21, s29
    5520: ee35fa05     	vadd.f32	s30, s10, s10
    5524: ee756acf     	vsub.f32	s13, s11, s30
    5528: ee446aaf     	vmla.f32	s13, s9, s31
    552c: ee46aa80     	vmla.f32	s21, s13, s0
    5530: ee4a3aaf     	vmla.f32	s7, s21, s31
    5534: ee23ea8c     	vmul.f32	s28, s7, s24
    5538: daffff1e     	ble	0x51b8 <arbhar_play_tilde_perform+0xf70> @ imm = #-0x388
    553c: e5900898     	ldr	r0, [r0, #0x898]
    5540: e0803003     	add	r3, r0, r3
    5544: e080c00c     	add	r12, r0, r12
    5548: e080e00e     	add	lr, r0, lr
    554c: e0805005     	add	r5, r0, r5
    5550: ed9caa00     	vldr	s20, [r12]
    5554: ed936a00     	vldr	s12, [r3]
    5558: ed9e7a00     	vldr	s14, [lr]
    555c: ed951a00     	vldr	s2, [r5]
    5560: ee761a47     	vsub.f32	s3, s12, s14
    5564: ee312a4a     	vsub.f32	s4, s2, s20
    5568: ee1a7a2e     	vnmls.f32	s14, s20, s29
    556c: ee421a2e     	vmla.f32	s3, s4, s29
    5570: ee762a06     	vadd.f32	s5, s12, s12
    5574: ee373a62     	vsub.f32	s6, s14, s5
    5578: ee013aaf     	vmla.f32	s6, s3, s31
    557c: ee032a00     	vmla.f32	s4, s6, s0
    5580: ee02aa2f     	vmla.f32	s20, s4, s31
    5584: ee2aaa0c     	vmul.f32	s20, s20, s24
    5588: eaffff0a     	b	0x51b8 <arbhar_play_tilde_perform+0xf70> @ imm = #-0x3d8
    558c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5590: 5a000005     	bpl	0x55ac <arbhar_play_tilde_perform+0x1364> @ imm = #0x14
    5594: ee3d6ac4     	vsub.f32	s12, s27, s8
    5598: ed997a59     	vldr	s14, [r9, #356]
    559c: ee261a07     	vmul.f32	s2, s12, s14
    55a0: ee0e1a04     	vmla.f32	s2, s28, s8
    55a4: eeb0ea41     	vmov.f32	s28, s2
    55a8: ed891a59     	vstr	s2, [r9, #356]
    55ac: edd91a48     	vldr	s3, [r9, #288]
    55b0: ed992a4c     	vldr	s4, [r9, #304]
    55b4: ee6b2aa1     	vmul.f32	s5, s23, s3
    55b8: ed993a5d     	vldr	s6, [r9, #372]
    55bc: ee623a80     	vmul.f32	s7, s5, s0
    55c0: ee234a88     	vmul.f32	s8, s7, s16
    55c4: ee24ea0e     	vmul.f32	s28, s8, s28
    55c8: ee3d5ac2     	vsub.f32	s10, s27, s4
    55cc: ee624a0e     	vmul.f32	s9, s4, s28
    55d0: ee655a0e     	vmul.f32	s11, s10, s28
    55d4: ee7daac3     	vsub.f32	s21, s27, s6
    55d8: ee24ea83     	vmul.f32	s28, s9, s6
    55dc: ee24faaa     	vmul.f32	s30, s9, s21
    55e0: ee63fa25     	vmul.f32	s31, s6, s11
    55e4: ee6aaaa5     	vmul.f32	s21, s21, s11
    55e8: eaffff2c     	b	0x52a0 <arbhar_play_tilde_perform+0x1058> @ imm = #-0x350
    55ec: e1a0a007     	mov	r10, r7
    55f0: e1a07006     	mov	r7, r6
    55f4: e59d6010     	ldr	r6, [sp, #0x10]
    55f8: e2899074     	add	r9, r9, #116
    55fc: edc99a2c     	vstr	s19, [r9, #176]
    5600: e1560009     	cmp	r6, r9
    5604: 1afffc13     	bne	0x4658 <arbhar_play_tilde_perform+0x410> @ imm = #-0xfb4
    5608: eafffe15     	b	0x4e64 <arbhar_play_tilde_perform+0xc1c> @ imm = #-0x7ac
    560c: e08b2005     	add	r2, r11, r5
    5610: e0814005     	add	r4, r1, r5
    5614: e089c005     	add	r12, r9, r5
    5618: e5853000     	str	r3, [r5]
    561c: e5843000     	str	r3, [r4]
    5620: e5823000     	str	r3, [r2]
    5624: e2852004     	add	r2, r5, #4
    5628: e58c3000     	str	r3, [r12]
    562c: eafffb6a     	b	0x43dc <arbhar_play_tilde_perform+0x194> @ imm = #-0x1258
    5630: eef30a08     	vmov.f32	s1, #2.400000e+01
    5634: e1a00007     	mov	r0, r7
    5638: ed9f1a5c     	vldr	s2, [pc, #368]          @ 0x57b0 <arbhar_play_tilde_perform+0x1568>
    563c: eeb00a41     	vmov.f32	s0, s2
    5640: ebfff3a1     	bl	0x24cc <.plt+0xec>      @ imm = #-0x317c
    5644: ed9f0b55     	vldr	d0, [pc, #340]          @ 0x57a0 <arbhar_play_tilde_perform+0x1558>
    5648: e5970020     	ldr	r0, [r7, #0x20]
    564c: ebfff3aa     	bl	0x24fc <.plt+0x11c>     @ imm = #-0x3158
    5650: e5983894     	ldr	r3, [r8, #0x894]
    5654: eafffb25     	b	0x42f0 <arbhar_play_tilde_perform+0xa8> @ imm = #-0x136c
    5658: edd3ba65     	vldr	s23, [r3, #404]
    565c: e59806ec     	ldr	r0, [r8, #0x6ec]
    5660: e59f9158     	ldr	r9, [pc, #0x158]        @ 0x57c0 <arbhar_play_tilde_perform+0x1578>
    5664: eddf1b4f     	vldr	d17, [pc, #316]         @ 0x57a8 <arbhar_play_tilde_perform+0x1560>
    5668: e08fa009     	add	r10, pc, r9
    566c: e08ab100     	add	r11, r10, r0, lsl #2
    5670: eef70aeb     	vcvt.f64.f32	d16, s23
    5674: eddfca4e     	vldr	s25, [pc, #312]         @ 0x57b4 <arbhar_play_tilde_perform+0x156c>
    5678: ed9fda4e     	vldr	s26, [pc, #312]         @ 0x57b8 <arbhar_play_tilde_perform+0x1570>
    567c: ee20eba1     	vmul.f64	d14, d16, d17
    5680: ed9bca88     	vldr	s24, [r11, #544]
    5684: ee0f2a90     	vmov	s31, r2
    5688: eef8dacc     	vcvt.f32.s32	s27, s24
    568c: eef7ebce     	vcvt.f32.f64	s29, d14
    5690: eef4eaec     	vcmpe.f32	s29, s25
    5694: eef1fa10     	vmrs	APSR_nzcv, fpscr
    5698: bef0ea6c     	vmovlt.f32	s29, s25
    569c: eef4eacd     	vcmpe.f32	s29, s26
    56a0: eef1fa10     	vmrs	APSR_nzcv, fpscr
    56a4: 8ef0ea4d     	vmovhi.f32	s29, s26
    56a8: ee8efaad     	vdiv.f32	s30, s29, s27
    56ac: eef86a6f     	vcvt.f32.u32	s13, s31
    56b0: eef46acf     	vcmpe.f32	s13, s30
    56b4: eef1fa10     	vmrs	APSR_nzcv, fpscr
    56b8: ca00001b     	bgt	0x572c <arbhar_play_tilde_perform+0x14e4> @ imm = #0x6c
    56bc: eef77a00     	vmov.f32	s15, #1.000000e+00
    56c0: ed937a8c     	vldr	s14, [r3, #560]
    56c4: eeb47a67     	vcmp.f32	s14, s15
    56c8: eef1fa10     	vmrs	APSR_nzcv, fpscr
    56cc: 0afffb03     	beq	0x42e0 <arbhar_play_tilde_perform+0x98> @ imm = #-0x13f4
    56d0: eafffb06     	b	0x42f0 <arbhar_play_tilde_perform+0xa8> @ imm = #-0x13e8
    56d4: e597c030     	ldr	r12, [r7, #0x30]
    56d8: e35c0000     	cmp	r12, #0
    56dc: 1afffb10     	bne	0x4324 <arbhar_play_tilde_perform+0xdc> @ imm = #-0x13c0
    56e0: e59b2004     	ldr	r2, [r11, #0x4]
    56e4: e59b5010     	ldr	r5, [r11, #0x10]
    56e8: e0429005     	sub	r9, r2, r5
    56ec: e3590006     	cmp	r9, #6
    56f0: 9a000007     	bls	0x5714 <arbhar_play_tilde_perform+0x14cc> @ imm = #0x1c
    56f4: e59fa0c8     	ldr	r10, [pc, #0xc8]        @ 0x57c4 <arbhar_play_tilde_perform+0x157c>
    56f8: e08fb00a     	add	r11, pc, r10
    56fc: e58b400c     	str	r4, [r11, #0xc]
    5700: e58b2010     	str	r2, [r11, #0x10]
    5704: eafffb06     	b	0x4324 <arbhar_play_tilde_perform+0xdc> @ imm = #-0x13e8
    5708: eef05a48     	vmov.f32	s11, s16
    570c: e3a02001     	mov	r2, #1
    5710: eafffd84     	b	0x4d28 <arbhar_play_tilde_perform+0xae0> @ imm = #-0x9f0
    5714: eeb00b00     	vmov.f64	d0, #2.000000e+00
    5718: e5970020     	ldr	r0, [r7, #0x20]
    571c: ebfff376     	bl	0x24fc <.plt+0x11c>     @ imm = #-0x3228
    5720: e59b2004     	ldr	r2, [r11, #0x4]
    5724: e5983894     	ldr	r3, [r8, #0x894]
    5728: eafffff1     	b	0x56f4 <arbhar_play_tilde_perform+0x14ac> @ imm = #-0x3c
    572c: e597002c     	ldr	r0, [r7, #0x2c]
    5730: ed9f0b1a     	vldr	d0, [pc, #104]          @ 0x57a0 <arbhar_play_tilde_perform+0x1558>
    5734: ebfff3d0     	bl	0x267c <.plt+0x29c>     @ imm = #-0x30c0
    5738: e59f0088     	ldr	r0, [pc, #0x88]         @ 0x57c8 <arbhar_play_tilde_perform+0x1580>
    573c: e1a02004     	mov	r2, r4
    5740: e5983894     	ldr	r3, [r8, #0x894]
    5744: e08f1000     	add	r1, pc, r0
    5748: e59806e8     	ldr	r0, [r8, #0x6e8]
    574c: e591c008     	ldr	r12, [r1, #0x8]
    5750: e5814000     	str	r4, [r1]
    5754: e28c4001     	add	r4, r12, #1
    5758: e5814008     	str	r4, [r1, #0x8]
    575c: eafffad6     	b	0x42bc <arbhar_play_tilde_perform+0x74> @ imm = #-0x14a8
    5760: ee71ca6b     	vsub.f32	s25, s2, s23
    5764: ed9fda14     	vldr	s26, [pc, #80]          @ 0x57bc <arbhar_play_tilde_perform+0x1574>
    5768: ee3ceaac     	vadd.f32	s28, s25, s25
    576c: ee6eda0e     	vmul.f32	s27, s28, s28
    5770: ee6dea8d     	vmul.f32	s29, s27, s26
    5774: ee6e5a8e     	vmul.f32	s11, s29, s28
    5778: eaffffe3     	b	0x570c <arbhar_play_tilde_perform+0x14c4> @ imm = #-0x74
    577c: eeb73a00     	vmov.f32	s6, #1.000000e+00
    5780: e3a05001     	mov	r5, #1
    5784: eddf3a0c     	vldr	s7, [pc, #48]           @ 0x57bc <arbhar_play_tilde_perform+0x1574>
    5788: ee334a42     	vsub.f32	s8, s6, s4
    578c: ee744a04     	vadd.f32	s9, s8, s8
    5790: ee245aa4     	vmul.f32	s10, s9, s9
    5794: ee655a23     	vmul.f32	s11, s10, s7
    5798: ee65aaa4     	vmul.f32	s21, s11, s9
    579c: eafffd8d     	b	0x4dd8 <arbhar_play_tilde_perform+0xb90> @ imm = #-0x9cc
    57a0: 00 00 00 00  	.word	0x00000000
    57a4: 00 00 00 00  	.word	0x00000000
    57a8: 0b d7 a3 70  	.word	0x70a3d70b
    57ac: 3d 0a f7 3f  	.word	0x3ff70a3d
    57b0: 00 00 00 00  	.word	0x00000000
    57b4: 00 00 00 43  	.word	0x43000000
    57b8: 00 a0 0c 48  	.word	0x480ca000
    57bc: 00 00 28 42  	.word	0x42280000
    57c0: 44 57 00 00  	.word	0x00005744
    57c4: 9c 8a 01 00  	.word	0x00018a9c
    57c8: 50 8a 01 00  	.word	0x00018a50

