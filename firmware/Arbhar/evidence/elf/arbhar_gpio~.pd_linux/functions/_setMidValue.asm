00004400 <_setMidValue>:
    4400: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    4404: e1a05000     	mov	r5, r0
    4408: ed2d8b02     	vpush	{d8}
    440c: e1a06003     	mov	r6, r3
    4410: e59f0228     	ldr	r0, [pc, #0x228]        @ 0x4640 <_setMidValue+0x240>
    4414: e1a01002     	mov	r1, r2
    4418: e1a08002     	mov	r8, r2
    441c: e08f0000     	add	r0, pc, r0
    4420: e24dd014     	sub	sp, sp, #20
    4424: ebfffdd3     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8b4
    4428: e1a00006     	mov	r0, r6
    442c: ebfffd53     	bl	0x3980 <.plt+0x284>     @ imm = #-0xab4
    4430: e59f120c     	ldr	r1, [pc, #0x20c]        @ 0x4644 <_setMidValue+0x244>
    4434: e08f0001     	add	r0, pc, r1
    4438: eefd7ac0     	vcvt.s32.f32	s15, s0
    443c: ee171a90     	vmov	r1, s15
    4440: edcd7a03     	vstr	s15, [sp, #12]
    4444: ebfffdcb     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x8d4
    4448: e3580001     	cmp	r8, #1
    444c: da000062     	ble	0x45dc <_setMidValue+0x1dc> @ imm = #0x188
    4450: e59f31f0     	ldr	r3, [pc, #0x1f0]        @ 0x4648 <_setMidValue+0x248>
    4454: e2482001     	sub	r2, r8, #1
    4458: e59f91ec     	ldr	r9, [pc, #0x1ec]        @ 0x464c <_setMidValue+0x24c>
    445c: e2866008     	add	r6, r6, #8
    4460: e08f4003     	add	r4, pc, r3
    4464: e58d4008     	str	r4, [sp, #0x8]
    4468: e59d400c     	ldr	r4, [sp, #0xc]
    446c: e08f9009     	add	r9, pc, r9
    4470: e285ba01     	add	r11, r5, #4096
    4474: e3a07000     	mov	r7, #0
    4478: e0828004     	add	r8, r2, r4
    447c: e1a00006     	mov	r0, r6
    4480: ebfffd3e     	bl	0x3980 <.plt+0x284>     @ imm = #-0xb08
    4484: e5d5311c     	ldrb	r3, [r5, #0x11c]
    4488: e5d5c2e0     	ldrb	r12, [r5, #0x2e0]
    448c: e1a00009     	mov	r0, r9
    4490: e1530004     	cmp	r3, r4
    4494: e5d524a4     	ldrb	r2, [r5, #0x4a4]
    4498: e5d51668     	ldrb	r1, [r5, #0x668]
    449c: 01a03004     	moveq	r3, r4
    44a0: 13a03000     	movne	r3, #0
    44a4: e15c0004     	cmp	r12, r4
    44a8: e5d5e82c     	ldrb	lr, [r5, #0x82c]
    44ac: 01a03004     	moveq	r3, r4
    44b0: 03a0c001     	moveq	r12, #1
    44b4: 13a0c000     	movne	r12, #0
    44b8: e1520004     	cmp	r2, r4
    44bc: 01a03004     	moveq	r3, r4
    44c0: 03a0c002     	moveq	r12, #2
    44c4: e1510004     	cmp	r1, r4
    44c8: e5d519f0     	ldrb	r1, [r5, #0x9f0]
    44cc: 01a03004     	moveq	r3, r4
    44d0: 03a02003     	moveq	r2, #3
    44d4: 11a0200c     	movne	r2, r12
    44d8: e154000e     	cmp	r4, lr
    44dc: e5d5cbb4     	ldrb	r12, [r5, #0xbb4]
    44e0: 01a03004     	moveq	r3, r4
    44e4: 03a02004     	moveq	r2, #4
    44e8: e1540001     	cmp	r4, r1
    44ec: e5d5ed78     	ldrb	lr, [r5, #0xd78]
    44f0: e5db1100     	ldrb	r1, [r11, #0x100]
    44f4: 01a03004     	moveq	r3, r4
    44f8: 03a02005     	moveq	r2, #5
    44fc: e154000c     	cmp	r4, r12
    4500: e5d5cf3c     	ldrb	r12, [r5, #0xf3c]
    4504: 01a03004     	moveq	r3, r4
    4508: 03a02006     	moveq	r2, #6
    450c: e154000e     	cmp	r4, lr
    4510: e5dbe2c4     	ldrb	lr, [r11, #0x2c4]
    4514: 01a03004     	moveq	r3, r4
    4518: 03a02007     	moveq	r2, #7
    451c: e154000c     	cmp	r4, r12
    4520: e5dbc64c     	ldrb	r12, [r11, #0x64c]
    4524: 01a03004     	moveq	r3, r4
    4528: 03a02008     	moveq	r2, #8
    452c: e1540001     	cmp	r4, r1
    4530: e5db1488     	ldrb	r1, [r11, #0x488]
    4534: 01a03004     	moveq	r3, r4
    4538: 03a02009     	moveq	r2, #9
    453c: e154000e     	cmp	r4, lr
    4540: e5dbe810     	ldrb	lr, [r11, #0x810]
    4544: eeb08a40     	vmov.f32	s16, s0
    4548: 01a03004     	moveq	r3, r4
    454c: 03a0200a     	moveq	r2, #10
    4550: e1540001     	cmp	r4, r1
    4554: e5db19d4     	ldrb	r1, [r11, #0x9d4]
    4558: 01a03004     	moveq	r3, r4
    455c: 03a0200b     	moveq	r2, #11
    4560: e154000c     	cmp	r4, r12
    4564: 01a0c004     	moveq	r12, r4
    4568: 11a0c003     	movne	r12, r3
    456c: 03a0200c     	moveq	r2, #12
    4570: e154000e     	cmp	r4, lr
    4574: e5dbeb98     	ldrb	lr, [r11, #0xb98]
    4578: 01a0c004     	moveq	r12, r4
    457c: 11a03002     	movne	r3, r2
    4580: 03a0300d     	moveq	r3, #13
    4584: e1540001     	cmp	r4, r1
    4588: 01a0c004     	moveq	r12, r4
    458c: 11a0a003     	movne	r10, r3
    4590: 03a0a00e     	moveq	r10, #14
    4594: e154000e     	cmp	r4, lr
    4598: 0a000012     	beq	0x45e8 <_setMidValue+0x1e8> @ imm = #0x48
    459c: eef70ac0     	vcvt.f64.f32	d16, s0
    45a0: e58da004     	str	r10, [sp, #0x4]
    45a4: e58dc000     	str	r12, [sp]
    45a8: e2844001     	add	r4, r4, #1
    45ac: e2866008     	add	r6, r6, #8
    45b0: ec532b30     	vmov	r2, r3, d16
    45b4: ebfffd6f     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xa44
    45b8: e3a00f71     	mov	r0, #452
    45bc: e0235a90     	mla	r3, r0, r10, r5
    45c0: e1580004     	cmp	r8, r4
    45c4: e3a02000     	mov	r2, #0
    45c8: ed838ab0     	vstr	s16, [r3, #704]
    45cc: e5c322d0     	strb	r2, [r3, #0x2d0]
    45d0: e58372d8     	str	r7, [r3, #0x2d8]
    45d4: e5837128     	str	r7, [r3, #0x128]
    45d8: 1affffa7     	bne	0x447c <_setMidValue+0x7c> @ imm = #-0x164
    45dc: e28dd014     	add	sp, sp, #20
    45e0: ecbd8b02     	vpop	{d8}
    45e4: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    45e8: eeb70ac0     	vcvt.f64.f32	d0, s0
    45ec: e58d4000     	str	r4, [sp]
    45f0: e3a0c00f     	mov	r12, #15
    45f4: e59d0008     	ldr	r0, [sp, #0x8]
    45f8: e58dc004     	str	r12, [sp, #0x4]
    45fc: e2844001     	add	r4, r4, #1
    4600: e2866008     	add	r6, r6, #8
    4604: ec532b10     	vmov	r2, r3, d0
    4608: ebfffd5a     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0xa98
    460c: e28b1ed3     	add	r1, r11, #3376
    4610: e28b0ed5     	add	r0, r11, #3408
    4614: e28b3eba     	add	r3, r11, #2976
    4618: e1580004     	cmp	r8, r4
    461c: ed818a03     	vstr	s16, [r1, #12]
    4620: e3a02000     	mov	r2, #0
    4624: e5cb2d4c     	strb	r2, [r11, #0xd4c]
    4628: e5807004     	str	r7, [r0, #0x4]
    462c: e5837004     	str	r7, [r3, #0x4]
    4630: 1affff91     	bne	0x447c <_setMidValue+0x7c> @ imm = #-0x1bc
    4634: e28dd014     	add	sp, sp, #20
    4638: ecbd8b02     	vpop	{d8}
    463c: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    4640: 44 06 01 00  	.word	0x00010644
    4644: 44 06 01 00  	.word	0x00010644
    4648: 2c 06 01 00  	.word	0x0001062c
    464c: 20 06 01 00  	.word	0x00010620

