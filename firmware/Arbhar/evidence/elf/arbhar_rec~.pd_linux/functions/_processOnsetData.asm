00004044 <_processOnsetData>:
    4044: e3520001     	cmp	r2, #1
    4048: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    404c: e1a05000     	mov	r5, r0
    4050: ca000011     	bgt	0x409c <_processOnsetData+0x58> @ imm = #0x44
    4054: eeb20a00     	vmov.f32	s0, #8.000000e+00
    4058: e3a01000     	mov	r1, #0
    405c: ebfff944     	bl	0x2574 <.plt+0x74>      @ imm = #-0x1af0
    4060: eddf7a83     	vldr	s15, [pc, #524]         @ 0x4274 <_processOnsetData+0x230>
    4064: eeb40ae7     	vcmpe.f32	s0, s15
    4068: eef1fa10     	vmrs	APSR_nzcv, fpscr
    406c: b8bd81f0     	poplt	{r4, r5, r6, r7, r8, pc}
    4070: e1a00005     	mov	r0, r5
    4074: eeb70a00     	vmov.f32	s0, #1.000000e+00
    4078: e3a02069     	mov	r2, #105
    407c: e3a01000     	mov	r1, #0
    4080: ebfff99b     	bl	0x26f4 <.plt+0x1f4>     @ imm = #-0x1994
    4084: e1a00005     	mov	r0, r5
    4088: e3a0206a     	mov	r2, #106
    408c: e3a01000     	mov	r1, #0
    4090: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
    4094: eeb70a00     	vmov.f32	s0, #1.000000e+00
    4098: eafff995     	b	0x26f4 <.plt+0x1f4>     @ imm = #-0x19ac
    409c: e3a01000     	mov	r1, #0
    40a0: ed9f0a74     	vldr	s0, [pc, #464]          @ 0x4278 <_processOnsetData+0x234>
    40a4: e1a06002     	mov	r6, r2
    40a8: e1a04003     	mov	r4, r3
    40ac: ebfff930     	bl	0x2574 <.plt+0x74>      @ imm = #-0x1b40
    40b0: e59f31c8     	ldr	r3, [pc, #0x1c8]        @ 0x4280 <_processOnsetData+0x23c>
    40b4: e08f0003     	add	r0, pc, r3
    40b8: e59f31c4     	ldr	r3, [pc, #0x1c4]        @ 0x4284 <_processOnsetData+0x240>
    40bc: e5907010     	ldr	r7, [r0, #0x10]
    40c0: e1a00005     	mov	r0, r5
    40c4: e2871064     	add	r1, r7, #100
    40c8: eebd0ac0     	vcvt.s32.f32	s0, s0
    40cc: ee102a10     	vmov	r2, s0
    40d0: eeb20a00     	vmov.f32	s0, #8.000000e+00
    40d4: e1510002     	cmp	r1, r2
    40d8: b0427007     	sublt	r7, r2, r7
    40dc: b5951274     	ldrlt	r1, [r5, #0x274]
    40e0: b3a0c030     	movlt	r12, #48
    40e4: b027179c     	mlalt	r7, r12, r7, r1
    40e8: e3a01000     	mov	r1, #0
    40ec: b5857274     	strlt	r7, [r5, #0x274]
    40f0: e08f7003     	add	r7, pc, r3
    40f4: e5872010     	str	r2, [r7, #0x10]
    40f8: ebfff91d     	bl	0x2574 <.plt+0x74>      @ imm = #-0x1b8c
    40fc: eddf0a5c     	vldr	s1, [pc, #368]          @ 0x4274 <_processOnsetData+0x230>
    4100: eeb40ae0     	vcmpe.f32	s0, s1
    4104: eef1fa10     	vmrs	APSR_nzcv, fpscr
    4108: aa00004e     	bge	0x4248 <_processOnsetData+0x204> @ imm = #0x138
    410c: e5957274     	ldr	r7, [r5, #0x274]
    4110: e3570000     	cmp	r7, #0
    4114: 18bd81f0     	popne	{r4, r5, r6, r7, r8, pc}
    4118: e1a01007     	mov	r1, r7
    411c: e1a00005     	mov	r0, r5
    4120: ed9f0a54     	vldr	s0, [pc, #336]          @ 0x4278 <_processOnsetData+0x234>
    4124: e0846186     	add	r6, r4, r6, lsl #3
    4128: ebfff911     	bl	0x2574 <.plt+0x74>      @ imm = #-0x1bbc
    412c: e1a01007     	mov	r1, r7
    4130: e1a00005     	mov	r0, r5
    4134: e3a0206d     	mov	r2, #109
    4138: ed9f1a4f     	vldr	s2, [pc, #316]          @ 0x427c <_processOnsetData+0x238>
    413c: ee601a01     	vmul.f32	s3, s0, s2
    4140: eebd2ae1     	vcvt.s32.f32	s4, s3
    4144: eeb70a00     	vmov.f32	s0, #1.000000e+00
    4148: ed852a9d     	vstr	s4, [r5, #628]
    414c: ebfff968     	bl	0x26f4 <.plt+0x1f4>     @ imm = #-0x1a60
    4150: e0460004     	sub	r0, r6, r4
    4154: e2401008     	sub	r1, r0, #8
    4158: e1a021a1     	lsr	r2, r1, #3
    415c: e282c001     	add	r12, r2, #1
    4160: e21c3007     	ands	r3, r12, #7
    4164: 0a000022     	beq	0x41f4 <_processOnsetData+0x1b0> @ imm = #0x88
    4168: e3530001     	cmp	r3, #1
    416c: 0a00001b     	beq	0x41e0 <_processOnsetData+0x19c> @ imm = #0x6c
    4170: e3530002     	cmp	r3, #2
    4174: 0a000016     	beq	0x41d4 <_processOnsetData+0x190> @ imm = #0x58
    4178: e3530003     	cmp	r3, #3
    417c: 0a000011     	beq	0x41c8 <_processOnsetData+0x184> @ imm = #0x44
    4180: e3530004     	cmp	r3, #4
    4184: 0a00000c     	beq	0x41bc <_processOnsetData+0x178> @ imm = #0x30
    4188: e3530005     	cmp	r3, #5
    418c: 0a000007     	beq	0x41b0 <_processOnsetData+0x16c> @ imm = #0x1c
    4190: e3530006     	cmp	r3, #6
    4194: 0a000002     	beq	0x41a4 <_processOnsetData+0x160> @ imm = #0x8
    4198: e1a00004     	mov	r0, r4
    419c: e2844008     	add	r4, r4, #8
    41a0: ebfff929     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1b5c
    41a4: e1a00004     	mov	r0, r4
    41a8: e2844008     	add	r4, r4, #8
    41ac: ebfff926     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1b68
    41b0: e1a00004     	mov	r0, r4
    41b4: e2844008     	add	r4, r4, #8
    41b8: ebfff923     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1b74
    41bc: e1a00004     	mov	r0, r4
    41c0: e2844008     	add	r4, r4, #8
    41c4: ebfff920     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1b80
    41c8: e1a00004     	mov	r0, r4
    41cc: e2844008     	add	r4, r4, #8
    41d0: ebfff91d     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1b8c
    41d4: e1a00004     	mov	r0, r4
    41d8: e2844008     	add	r4, r4, #8
    41dc: ebfff91a     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1b98
    41e0: e1a00004     	mov	r0, r4
    41e4: e2844008     	add	r4, r4, #8
    41e8: ebfff917     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1ba4
    41ec: e1540006     	cmp	r4, r6
    41f0: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
    41f4: e2845008     	add	r5, r4, #8
    41f8: e1a00004     	mov	r0, r4
    41fc: ebfff912     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1bb8
    4200: e1a00005     	mov	r0, r5
    4204: ebfff910     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1bc0
    4208: e2840010     	add	r0, r4, #16
    420c: ebfff90e     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1bc8
    4210: e2840018     	add	r0, r4, #24
    4214: ebfff90c     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1bd0
    4218: e2840020     	add	r0, r4, #32
    421c: ebfff90a     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1bd8
    4220: e2840028     	add	r0, r4, #40
    4224: ebfff908     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1be0
    4228: e2840030     	add	r0, r4, #48
    422c: ebfff906     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1be8
    4230: e2840038     	add	r0, r4, #56
    4234: e2844040     	add	r4, r4, #64
    4238: ebfff903     	bl	0x264c <.plt+0x14c>     @ imm = #-0x1bf4
    423c: e1540006     	cmp	r4, r6
    4240: 1affffeb     	bne	0x41f4 <_processOnsetData+0x1b0> @ imm = #-0x54
    4244: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    4248: e1a00005     	mov	r0, r5
    424c: eeb70a00     	vmov.f32	s0, #1.000000e+00
    4250: e3a02069     	mov	r2, #105
    4254: e3a01000     	mov	r1, #0
    4258: ebfff925     	bl	0x26f4 <.plt+0x1f4>     @ imm = #-0x1b6c
    425c: e3a0206a     	mov	r2, #106
    4260: e3a01000     	mov	r1, #0
    4264: e1a00005     	mov	r0, r5
    4268: eeb70a00     	vmov.f32	s0, #1.000000e+00
    426c: ebfff920     	bl	0x26f4 <.plt+0x1f4>     @ imm = #-0x1b80
    4270: eaffffa5     	b	0x410c <_processOnsetData+0xc8> @ imm = #-0x16c
    4274: 00 20 7d 45  	.word	0x457d2000
    4278: 00 00 18 42  	.word	0x42180000
    427c: 00 00 40 42  	.word	0x42400000
    4280: f4 60 01 00  	.word	0x000160f4
    4284: b8 60 01 00  	.word	0x000160b8

