00008534 <_setMidValueByCh>:
    8534: e3520001     	cmp	r2, #1
    8538: d12fff1e     	bxle	lr
    853c: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    8540: e1a05000     	mov	r5, r0
    8544: e2830008     	add	r0, r3, #8
    8548: ebffed0c     	bl	0x3980 <.plt+0x284>     @ imm = #-0x4bd0
    854c: eefd7ac0     	vcvt.s32.f32	s15, s0
    8550: ee174a90     	vmov	r4, s15
    8554: e3540000     	cmp	r4, #0
    8558: ba000020     	blt	0x85e0 <_setMidValueByCh+0xac> @ imm = #0x80
    855c: e59f6090     	ldr	r6, [pc, #0x90]         @ 0x85f4 <_setMidValueByCh+0xc0>
    8560: e08f6006     	add	r6, pc, r6
    8564: e596204c     	ldr	r2, [r6, #0x4c]
    8568: e352000f     	cmp	r2, #15
    856c: ca000017     	bgt	0x85d0 <_setMidValueByCh+0x9c> @ imm = #0x5c
    8570: e59f7080     	ldr	r7, [pc, #0x80]         @ 0x85f8 <_setMidValueByCh+0xc4>
    8574: e1a01002     	mov	r1, r2
    8578: e59f007c     	ldr	r0, [pc, #0x7c]         @ 0x85fc <_setMidValueByCh+0xc8>
    857c: e08f7007     	add	r7, pc, r7
    8580: ee173a90     	vmov	r3, s15
    8584: e08f0000     	add	r0, pc, r0
    8588: e7d72002     	ldrb	r2, [r7, r2]
    858c: ebffed79     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x4a1c
    8590: ee004a10     	vmov	s0, r4
    8594: e596104c     	ldr	r1, [r6, #0x4c]
    8598: e3a03f71     	mov	r3, #452
    859c: eef80ac0     	vcvt.f32.s32	s1, s0
    85a0: e59fc058     	ldr	r12, [pc, #0x58]        @ 0x8600 <_setMidValueByCh+0xcc>
    85a4: e7d72001     	ldrb	r2, [r7, r1]
    85a8: e08f000c     	add	r0, pc, r12
    85ac: e0255293     	mla	r5, r3, r2, r5
    85b0: eebd1ae0     	vcvt.s32.f32	s2, s1
    85b4: edc50ab0     	vstr	s1, [r5, #704]
    85b8: ee113a10     	vmov	r3, s2
    85bc: ebffed6d     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x4a4c
    85c0: e596104c     	ldr	r1, [r6, #0x4c]
    85c4: e2812001     	add	r2, r1, #1
    85c8: e586204c     	str	r2, [r6, #0x4c]
    85cc: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    85d0: e1a00005     	mov	r0, r5
    85d4: ee171a90     	vmov	r1, s15
    85d8: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
    85dc: eaffed95     	b	0x3c38 <.plt+0x53c>     @ imm = #-0x49ac
    85e0: e59fe01c     	ldr	lr, [pc, #0x1c]         @ 0x8604 <_setMidValueByCh+0xd0>
    85e4: e3a03000     	mov	r3, #0
    85e8: e08f000e     	add	r0, pc, lr
    85ec: e580304c     	str	r3, [r0, #0x4c]
    85f0: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    85f4: 50 ee 01 00  	.word	0x0001ee50
    85f8: 98 aa 00 00  	.word	0x0000aa98
    85fc: e0 c9 00 00  	.word	0x0000c9e0
    8600: f0 c9 00 00  	.word	0x0000c9f0
    8604: c8 ed 01 00  	.word	0x0001edc8

