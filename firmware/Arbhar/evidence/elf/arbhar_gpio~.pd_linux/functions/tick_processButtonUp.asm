0000b3d8 <tick_processButtonUp>:
    b3d8: e5d03030     	ldrb	r3, [r0, #0x30]
    b3dc: e3530062     	cmp	r3, #98
    b3e0: 8a00006b     	bhi	0xb594 <tick_processButtonUp+0x1bc> @ imm = #0x1ac
    b3e4: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
    b3e8: e2806a01     	add	r6, r0, #4096
    b3ec: e1a04000     	mov	r4, r0
    b3f0: e24dd010     	sub	sp, sp, #16
    b3f4: e5d60d5c     	ldrb	r0, [r6, #0xd5c]
    b3f8: e3500004     	cmp	r0, #4
    b3fc: 1a00000f     	bne	0xb440 <tick_processButtonUp+0x68> @ imm = #0x3c
    b400: e5d42026     	ldrb	r2, [r4, #0x26]
    b404: e5d67d76     	ldrb	r7, [r6, #0xd76]
    b408: e3520001     	cmp	r2, #1
    b40c: 0a000071     	beq	0xb5d8 <tick_processButtonUp+0x200> @ imm = #0x1c4
    b410: e3570001     	cmp	r7, #1
    b414: 0a000065     	beq	0xb5b0 <tick_processButtonUp+0x1d8> @ imm = #0x194
    b418: e5d4103c     	ldrb	r1, [r4, #0x3c]
    b41c: e1a00004     	mov	r0, r4
    b420: ebffe162     	bl	0x39b0 <.plt+0x2b4>     @ imm = #-0x7a78
    b424: e5d4803c     	ldrb	r8, [r4, #0x3c]
    b428: e3580000     	cmp	r8, #0
    b42c: 0a000071     	beq	0xb5f8 <tick_processButtonUp+0x220> @ imm = #0x1c4
    b430: e3580003     	cmp	r8, #3
    b434: 8a00008f     	bhi	0xb678 <tick_processButtonUp+0x2a0> @ imm = #0x23c
    b438: e28dd010     	add	sp, sp, #16
    b43c: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    b440: e2845d75     	add	r5, r4, #7488
    b444: e2850020     	add	r0, r5, #32
    b448: e285701c     	add	r7, r5, #28
    b44c: ebffe121     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x7b7c
    b450: e3500000     	cmp	r0, #0
    b454: 1affffe9     	bne	0xb400 <tick_processButtonUp+0x28> @ imm = #-0x5c
    b458: e2870014     	add	r0, r7, #20
    b45c: ebffe11d     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x7b8c
    b460: e3500000     	cmp	r0, #0
    b464: 1affffe5     	bne	0xb400 <tick_processButtonUp+0x28> @ imm = #-0x6c
    b468: e2840d76     	add	r0, r4, #7552
    b46c: ebffe119     	bl	0x38d8 <.plt+0x1dc>     @ imm = #-0x7b9c
    b470: e3500000     	cmp	r0, #0
    b474: 1affffe1     	bne	0xb400 <tick_processButtonUp+0x28> @ imm = #-0x7c
    b478: e5d4103c     	ldrb	r1, [r4, #0x3c]
    b47c: e3510002     	cmp	r1, #2
    b480: 0a000080     	beq	0xb688 <tick_processButtonUp+0x2b0> @ imm = #0x200
    b484: e5d45038     	ldrb	r5, [r4, #0x38]
    b488: e3550000     	cmp	r5, #0
    b48c: 1a00001a     	bne	0xb4fc <tick_processButtonUp+0x124> @ imm = #0x68
    b490: e59f2214     	ldr	r2, [pc, #0x214]        @ 0xb6ac <tick_processButtonUp+0x2d4>
    b494: e08f7002     	add	r7, pc, r2
    b498: e5d78003     	ldrb	r8, [r7, #0x3]
    b49c: e3580000     	cmp	r8, #0
    b4a0: 0a000015     	beq	0xb4fc <tick_processButtonUp+0x124> @ imm = #0x54
    b4a4: e59fc204     	ldr	r12, [pc, #0x204]       @ 0xb6b0 <tick_processButtonUp+0x2d8>
    b4a8: e1a01005     	mov	r1, r5
    b4ac: e08f000c     	add	r0, pc, r12
    b4b0: ebffe1b0     	bl	0x3b78 <.plt+0x47c>     @ imm = #-0x7940
    b4b4: e59f31f8     	ldr	r3, [pc, #0x1f8]        @ 0xb6b4 <tick_processButtonUp+0x2dc>
    b4b8: e3a0c001     	mov	r12, #1
    b4bc: e3a01000     	mov	r1, #0
    b4c0: e08f0003     	add	r0, pc, r3
    b4c4: e5948070     	ldr	r8, [r4, #0x70]
    b4c8: e58d1004     	str	r1, [sp, #0x4]
    b4cc: e3a02000     	mov	r2, #0
    b4d0: e58dc000     	str	r12, [sp]
    b4d4: e344231d     	movt	r2, #0x431d
    b4d8: e58dc008     	str	r12, [sp, #0x8]
    b4dc: e58d200c     	str	r2, [sp, #0xc]
    b4e0: ebffe090     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x7dc0
    b4e4: e1a0300d     	mov	r3, sp
    b4e8: e3a02002     	mov	r2, #2
    b4ec: e1a01000     	mov	r1, r0
    b4f0: e1a00008     	mov	r0, r8
    b4f4: ebffe1de     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x7888
    b4f8: e5c75003     	strb	r5, [r7, #0x3]
    b4fc: e3a05000     	mov	r5, #0
    b500: e3a010de     	mov	r1, #222
    b504: e1a02005     	mov	r2, r5
    b508: e1a00004     	mov	r0, r4
    b50c: e5c65d62     	strb	r5, [r6, #0xd62]
    b510: e5c65d72     	strb	r5, [r6, #0xd72]
    b514: e5c65d82     	strb	r5, [r6, #0xd82]
    b518: e5c65d5c     	strb	r5, [r6, #0xd5c]
    b51c: ebffe171     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x7a3c
    b520: e1a02005     	mov	r2, r5
    b524: e3a01079     	mov	r1, #121
    b528: e1a00004     	mov	r0, r4
    b52c: ebffe16d     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x7a4c
    b530: e3a0107a     	mov	r1, #122
    b534: e1a00004     	mov	r0, r4
    b538: e1a02005     	mov	r2, r5
    b53c: ebffe169     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x7a5c
    b540: e5d67d5c     	ldrb	r7, [r6, #0xd5c]
    b544: e59f016c     	ldr	r0, [pc, #0x16c]        @ 0xb6b8 <tick_processButtonUp+0x2e0>
    b548: ee077a90     	vmov	s15, r7
    b54c: e08f1000     	add	r1, pc, r0
    b550: eeb80a67     	vcvt.f32.u32	s0, s15
    b554: ed917a26     	vldr	s14, [r1, #152]
    b558: eeb40a47     	vcmp.f32	s0, s14
    b55c: eef1fa10     	vmrs	APSR_nzcv, fpscr
    b560: 0affffa6     	beq	0xb400 <tick_processButtonUp+0x28> @ imm = #-0x168
    b564: e5d4c03d     	ldrb	r12, [r4, #0x3d]
    b568: e596eddc     	ldr	lr, [r6, #0xddc]
    b56c: e5d4303c     	ldrb	r3, [r4, #0x3c]
    b570: e05c8005     	subs	r8, r12, r5
    b574: ed810a26     	vstr	s0, [r1, #152]
    b578: 13a08001     	movne	r8, #1
    b57c: e584e040     	str	lr, [r4, #0x40]
    b580: e584e048     	str	lr, [r4, #0x48]
    b584: e5c48045     	strb	r8, [r4, #0x45]
    b588: e5c43044     	strb	r3, [r4, #0x44]
    b58c: e5c4703c     	strb	r7, [r4, #0x3c]
    b590: eaffff9a     	b	0xb400 <tick_processButtonUp+0x28> @ imm = #-0x198
    b594: e3530064     	cmp	r3, #100
    b598: 02800a01     	addeq	r0, r0, #4096
    b59c: 03a03000     	moveq	r3, #0
    b5a0: 05c03d62     	strbeq	r3, [r0, #0xd62]
    b5a4: 05c03d72     	strbeq	r3, [r0, #0xd72]
    b5a8: 05c03d82     	strbeq	r3, [r0, #0xd82]
    b5ac: e12fff1e     	bx	lr
    b5b0: e5d40024     	ldrb	r0, [r4, #0x24]
    b5b4: e3500001     	cmp	r0, #1
    b5b8: 1affff96     	bne	0xb418 <tick_processButtonUp+0x40> @ imm = #-0x1a8
    b5bc: e5d41028     	ldrb	r1, [r4, #0x28]
    b5c0: e3510000     	cmp	r1, #0
    b5c4: 0affff93     	beq	0xb418 <tick_processButtonUp+0x40> @ imm = #-0x1b4
    b5c8: e3a01000     	mov	r1, #0
    b5cc: e1a00004     	mov	r0, r4
    b5d0: ebffe0c9     	bl	0x38fc <.plt+0x200>     @ imm = #-0x7cdc
    b5d4: eaffff8f     	b	0xb418 <tick_processButtonUp+0x40> @ imm = #-0x1c4
    b5d8: e3570001     	cmp	r7, #1
    b5dc: 1affff8d     	bne	0xb418 <tick_processButtonUp+0x40> @ imm = #-0x1cc
    b5e0: e1a00004     	mov	r0, r4
    b5e4: e3a05000     	mov	r5, #0
    b5e8: ebffe0cc     	bl	0x3920 <.plt+0x224>     @ imm = #-0x7cd0
    b5ec: e5c45026     	strb	r5, [r4, #0x26]
    b5f0: e5d67d76     	ldrb	r7, [r6, #0xd76]
    b5f4: eaffff85     	b	0xb410 <tick_processButtonUp+0x38> @ imm = #-0x1ec
    b5f8: e5c480b9     	strb	r8, [r4, #0xb9]
    b5fc: e3e0e000     	mvn	lr, #0
    b600: e1a00004     	mov	r0, r4
    b604: e1a02008     	mov	r2, r8
    b608: e5c6ede0     	strb	lr, [r6, #0xde0]
    b60c: e3a01079     	mov	r1, #121
    b610: ebffe134     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x7b30
    b614: e1a00004     	mov	r0, r4
    b618: e1a02008     	mov	r2, r8
    b61c: e3a0107a     	mov	r1, #122
    b620: ebffe130     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x7b40
    b624: e5d44024     	ldrb	r4, [r4, #0x24]
    b628: e3540000     	cmp	r4, #0
    b62c: 1affff81     	bne	0xb438 <tick_processButtonUp+0x60> @ imm = #-0x1fc
    b630: e59f2084     	ldr	r2, [pc, #0x84]         @ 0xb6bc <tick_processButtonUp+0x2e4>
    b634: e3a03001     	mov	r3, #1
    b638: e3a0c000     	mov	r12, #0
    b63c: e5966dac     	ldr	r6, [r6, #0xdac]
    b640: e08f0002     	add	r0, pc, r2
    b644: e58d3000     	str	r3, [sp]
    b648: e58d3008     	str	r3, [sp, #0x8]
    b64c: e3a05000     	mov	r5, #0
    b650: e58dc004     	str	r12, [sp, #0x4]
    b654: e344530e     	movt	r5, #0x430e
    b658: e58d500c     	str	r5, [sp, #0xc]
    b65c: ebffe031     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x7f3c
    b660: e1a0300d     	mov	r3, sp
    b664: e3a02002     	mov	r2, #2
    b668: e1a01000     	mov	r1, r0
    b66c: e1a00006     	mov	r0, r6
    b670: ebffe17f     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x7a04
    b674: eaffff6f     	b	0xb438 <tick_processButtonUp+0x60> @ imm = #-0x244
    b678: e1a00004     	mov	r0, r4
    b67c: ebffe116     	bl	0x3adc <.plt+0x3e0>     @ imm = #-0x7ba8
    b680: e28dd010     	add	sp, sp, #16
    b684: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
    b688: e1a00004     	mov	r0, r4
    b68c: ed940a30     	vldr	s0, [r4, #192]
    b690: e3a01066     	mov	r1, #102
    b694: ebffe0c2     	bl	0x39a4 <.plt+0x2a8>     @ imm = #-0x7cf8
    b698: e3a02001     	mov	r2, #1
    b69c: e3a0109e     	mov	r1, #158
    b6a0: e1a00004     	mov	r0, r4
    b6a4: ebffe10f     	bl	0x3ae8 <.plt+0x3ec>     @ imm = #-0x7bc4
    b6a8: eaffff75     	b	0xb484 <tick_processButtonUp+0xac> @ imm = #-0x22c
    b6ac: 50 be 01 00  	.word	0x0001be50
    b6b0: 08 99 00 00  	.word	0x00009908
    b6b4: f4 95 00 00  	.word	0x000095f4
    b6b8: 64 be 01 00  	.word	0x0001be64
    b6bc: 74 94 00 00  	.word	0x00009474

