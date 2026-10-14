000041cc <_learnMid>:
    41cc: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
    41d0: e1a08000     	mov	r8, r0
    41d4: e59f010c     	ldr	r0, [pc, #0x10c]        @ 0x42e8 <_learnMid+0x11c>  // u32=0x10800; f32?=9.47053554e-41
    41d8: e2886f71     	add	r6, r8, #452
    41dc: e59f5108     	ldr	r5, [pc, #0x108]        @ 0x42ec <_learnMid+0x120>  // u32=0xee20; f32?=8.54231544e-41
    41e0: e3a04001     	mov	r4, #1
    41e4: e08f0000     	add	r0, pc, r0
    41e8: e59f7100     	ldr	r7, [pc, #0x100]        @ 0x42f0 <_learnMid+0x124>  // u32=0x10810; f32?=9.47277762e-41
    41ec: ebfffe61     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x67c  // CALL post
    41f0: edd87ab1     	vldr	s15, [r8, #708]
    41f4: e08f5005     	add	r5, pc, r5
    41f8: e08f7007     	add	r7, pc, r7
    41fc: e3a01000     	mov	r1, #0
    4200: e1a00007     	mov	r0, r7
    4204: eebd0ae7     	vcvt.s32.f32	s0, s15
    4208: edc87ab0     	vstr	s15, [r8, #704]
    420c: e4d53001     	ldrb	r3, [r5], #1
    4210: ee102a10     	vmov	r2, s0
    4214: ebfffe57     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6a4  // CALL post
    4218: edd60ab1     	vldr	s1, [r6, #708]
    421c: e2868f71     	add	r8, r6, #452
    4220: e1a09005     	mov	r9, r5
    4224: e1a01004     	mov	r1, r4
    4228: e1a00007     	mov	r0, r7
    422c: e284a001     	add	r10, r4, #1
    4230: e2855005     	add	r5, r5, #5
    4234: eebd1ae0     	vcvt.s32.f32	s2, s1
    4238: edc60ab0     	vstr	s1, [r6, #704]
    423c: e4d93001     	ldrb	r3, [r9], #1
    4240: ee112a10     	vmov	r2, s2
    4244: ebfffe4b     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6d4  // CALL post
    4248: edd81ab1     	vldr	s3, [r8, #708]
    424c: e1a0100a     	mov	r1, r10
    4250: e1a00007     	mov	r0, r7
    4254: eebd2ae1     	vcvt.s32.f32	s4, s3
    4258: edc81ab0     	vstr	s3, [r8, #704]
    425c: e5553004     	ldrb	r3, [r5, #-0x4]
    4260: ee122a10     	vmov	r2, s4
    4264: ebfffe43     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x6f4  // CALL post
    4268: e2863fe2     	add	r3, r6, #904
    426c: e2841002     	add	r1, r4, #2
    4270: e1a00007     	mov	r0, r7
    4274: edd32ab1     	vldr	s5, [r3, #708]
    4278: eebd3ae2     	vcvt.s32.f32	s6, s5
    427c: edc32ab0     	vstr	s5, [r3, #704]
    4280: e5d93001     	ldrb	r3, [r9, #0x1]
    4284: ee132a10     	vmov	r2, s6
    4288: ebfffe3a     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x718  // CALL post
    428c: e2882fe2     	add	r2, r8, #904
    4290: e2841003     	add	r1, r4, #3
    4294: e1a00007     	mov	r0, r7
    4298: edd23ab1     	vldr	s7, [r2, #708]
    429c: eebd4ae3     	vcvt.s32.f32	s8, s7
    42a0: edc23ab0     	vstr	s7, [r2, #704]
    42a4: e5553002     	ldrb	r3, [r5, #-0x2]
    42a8: ee142a10     	vmov	r2, s8
    42ac: ebfffe31     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x73c  // CALL post
    42b0: e286ce71     	add	r12, r6, #1808
    42b4: e2841004     	add	r1, r4, #4
    42b8: e1a00007     	mov	r0, r7
    42bc: eddc4ab1     	vldr	s9, [r12, #708]
    42c0: e2844005     	add	r4, r4, #5
    42c4: e2886e71     	add	r6, r8, #1808
    42c8: eebd5ae4     	vcvt.s32.f32	s10, s9
    42cc: edcc4ab0     	vstr	s9, [r12, #704]
    42d0: e5553001     	ldrb	r3, [r5, #-0x1]
    42d4: ee152a10     	vmov	r2, s10
    42d8: ebfffe26     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x768  // CALL post
    42dc: e3540010     	cmp	r4, #16
    42e0: 1affffcc     	bne	0x4218 <_learnMid+0x4c> @ imm = #-0xd0
    42e4: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    42e8: 00 08 01 00  	.word	0x00010800
    42ec: 20 ee 00 00  	.word	0x0000ee20
    42f0: 10 08 01 00  	.word	0x00010810

