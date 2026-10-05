; lubadh::Channel::FilePreview::FilePreview(lubadh::Channel&)
; VA 0x3c320 size 624

   3c320: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   3c324: e1a04000     	mov	r4, r0
   3c328: e1a05000     	mov	r5, r0
   3c32c: e3047dc0     	movw	r7, #0x4dc0
   3c330: e3407009     	movt	r7, #0x9
   3c334: e3a02000     	mov	r2, #0
   3c338: e59fc230     	ldr	r12, [pc, #0x230]       @ 0x3c570
   3c33c: e4851004     	str	r1, [r5], #4
   3c340: e2806014     	add	r6, r0, #20
   3c344: e5973138     	ldr	r3, [r7, #0x138]
   3c348: e280000c     	add	r0, r0, #12
   3c34c: e5842008     	str	r2, [r4, #0x8]
   3c350: e24dd01c     	sub	sp, sp, #28
   3c354: e597213c     	ldr	r2, [r7, #0x13c]
   3c358: e1a01003     	mov	r1, r3
   3c35c: e584c004     	str	r12, [r4, #0x4]
   3c360: e0832002     	add	r2, r3, r2
   3c364: e584600c     	str	r6, [r4, #0xc]
   3c368: ebffeae2     	bl	0x36ef8
   3c36c: e3e03000     	mvn	r3, #0
   3c370: e1a00005     	mov	r0, r5
   3c374: e5843024     	str	r3, [r4, #0x24]
   3c378: eb002972     	bl	0x46948
   3c37c: e1a00005     	mov	r0, r5
   3c380: eb002aba     	bl	0x46e70
   3c384: e1a00005     	mov	r0, r5
   3c388: eb002b8d     	bl	0x471c4
   3c38c: e59f11e0     	ldr	r1, [pc, #0x1e0]        @ 0x3c574
   3c390: e2848034     	add	r8, r4, #52
   3c394: e3a02000     	mov	r2, #0
   3c398: e3a03000     	mov	r3, #0
   3c39c: e5841004     	str	r1, [r4, #0x4]
   3c3a0: e1a00008     	mov	r0, r8
   3c3a4: e2871e15     	add	r1, r7, #336
   3c3a8: e584302c     	str	r3, [r4, #0x2c]
   3c3ac: e5842028     	str	r2, [r4, #0x28]
   3c3b0: e1c423b0     	strh	r2, [r4, #48]
   3c3b4: eb00cf0b     	bl	0x6ffe8
   3c3b8: e5971168     	ldr	r1, [r7, #0x168]
   3c3bc: e3a03000     	mov	r3, #0
   3c3c0: e597216c     	ldr	r2, [r7, #0x16c]
   3c3c4: e2849060     	add	r9, r4, #96
   3c3c8: e59f01a8     	ldr	r0, [pc, #0x1a8]        @ 0x3c578
   3c3cc: e2846050     	add	r6, r4, #80
   3c3d0: e5840050     	str	r0, [r4, #0x50]
   3c3d4: e0812002     	add	r2, r1, r2
   3c3d8: e2840058     	add	r0, r4, #88
   3c3dc: e5843054     	str	r3, [r4, #0x54]
   3c3e0: e5849058     	str	r9, [r4, #0x58]
   3c3e4: ebffeac3     	bl	0x36ef8
   3c3e8: e3e03000     	mvn	r3, #0
   3c3ec: e1a00006     	mov	r0, r6
   3c3f0: e5843070     	str	r3, [r4, #0x70]
   3c3f4: eb0026a8     	bl	0x45e9c
   3c3f8: e1a00006     	mov	r0, r6
   3c3fc: eb0027ee     	bl	0x463bc
   3c400: e1a00006     	mov	r0, r6
   3c404: eb0028bf     	bl	0x46708
   3c408: e5942000     	ldr	r2, [r4]
   3c40c: e1a0000d     	mov	r0, sp
   3c410: e59f3164     	ldr	r3, [pc, #0x164]        @ 0x3c57c
   3c414: e59f1164     	ldr	r1, [pc, #0x164]        @ 0x3c580
   3c418: e2822004     	add	r2, r2, #4
   3c41c: e5843050     	str	r3, [r4, #0x50]
   3c420: ebffc9d5     	bl	0x2eb7c
   3c424: e2849074     	add	r9, r4, #116
   3c428: e1a0100d     	mov	r1, sp
   3c42c: e1a00009     	mov	r0, r9
   3c430: eb001675     	bl	0x41e0c
   3c434: e59d0000     	ldr	r0, [sp]
   3c438: e28d3008     	add	r3, sp, #8
   3c43c: e59fa140     	ldr	r10, [pc, #0x140]       @ 0x3c584
   3c440: e1500003     	cmp	r0, r3
   3c444: e584a074     	str	r10, [r4, #0x74]
   3c448: 0a000000     	beq	0x3c450
   3c44c: ebff667b     	bl	0x15e40    @ imm = #-0x26614 ; _ZdlPv
   3c450: e5971198     	ldr	r1, [r7, #0x198]
   3c454: e3a03000     	mov	r3, #0
   3c458: e597219c     	ldr	r2, [r7, #0x19c]
   3c45c: e284b0a8     	add	r11, r4, #168
   3c460: e59f0120     	ldr	r0, [pc, #0x120]        @ 0x3c588
   3c464: e2847098     	add	r7, r4, #152
   3c468: e5840098     	str	r0, [r4, #0x98]
   3c46c: e0812002     	add	r2, r1, r2
   3c470: e28400a0     	add	r0, r4, #160
   3c474: e584309c     	str	r3, [r4, #0x9c]
   3c478: e584b0a0     	str	r11, [r4, #0xa0]
   3c47c: ebffea9d     	bl	0x36ef8
   3c480: e3e03000     	mvn	r3, #0
   3c484: e1a00007     	mov	r0, r7
   3c488: e58430b8     	str	r3, [r4, #0xb8]
   3c48c: eb00212c     	bl	0x44944
   3c490: e1a00007     	mov	r0, r7
   3c494: eb002272     	bl	0x44e64
   3c498: e1a00007     	mov	r0, r7
   3c49c: eb002343     	bl	0x451b0
   3c4a0: e5940054     	ldr	r0, [r4, #0x54]
   3c4a4: f2c00010     	vmov.i32	d16, #0x0
   3c4a8: e5941078     	ldr	r1, [r4, #0x78]
   3c4ac: e3a03000     	mov	r3, #0
   3c4b0: e594209c     	ldr	r2, [r4, #0x9c]
   3c4b4: e59fc0d0     	ldr	r12, [pc, #0xd0]        @ 0x3c58c
   3c4b8: e584c098     	str	r12, [r4, #0x98]
   3c4bc: f440078f     	vst1.32	{d16}, [r0]
   3c4c0: e1a00004     	mov	r0, r4
   3c4c4: e5813000     	str	r3, [r1]
   3c4c8: e5c23000     	strb	r3, [r2]
   3c4cc: e5c23001     	strb	r3, [r2, #0x1]
   3c4d0: e28dd01c     	add	sp, sp, #28
   3c4d4: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   3c4d8: e594000c     	ldr	r0, [r4, #0xc]
   3c4dc: e1560000     	cmp	r6, r0
   3c4e0: 0a000000     	beq	0x3c4e8
   3c4e4: ebff6655     	bl	0x15e40    @ imm = #-0x266ac ; _ZdlPv
   3c4e8: ebff669c     	bl	0x15f60    @ imm = #-0x26590 ; __cxa_end_cleanup
   3c4ec: e59400a0     	ldr	r0, [r4, #0xa0]
   3c4f0: e15b0000     	cmp	r11, r0
   3c4f4: 0a000000     	beq	0x3c4fc
   3c4f8: ebff6650     	bl	0x15e40    @ imm = #-0x266c0 ; _ZdlPv
   3c4fc: e1a00009     	mov	r0, r9
   3c500: e584a074     	str	r10, [r4, #0x74]
   3c504: ebffcb2c     	bl	0x2f1bc
   3c508: e59f306c     	ldr	r3, [pc, #0x6c]         @ 0x3c57c
   3c50c: e1a00006     	mov	r0, r6
   3c510: e5843050     	str	r3, [r4, #0x50]
   3c514: ebffe089     	bl	0x34740
   3c518: e1a00008     	mov	r0, r8
   3c51c: eb00cfdb     	bl	0x70490
   3c520: e59f304c     	ldr	r3, [pc, #0x4c]         @ 0x3c574
   3c524: e1a00005     	mov	r0, r5
   3c528: e5843004     	str	r3, [r4, #0x4]
   3c52c: ebffe0ec     	bl	0x348e4
   3c530: eaffffec     	b	0x3c4e8
   3c534: eafffff3     	b	0x3c508
   3c538: e5940058     	ldr	r0, [r4, #0x58]
   3c53c: e1590000     	cmp	r9, r0
   3c540: 0afffff4     	beq	0x3c518
   3c544: ebff663d     	bl	0x15e40    @ imm = #-0x2670c ; _ZdlPv
   3c548: eafffff2     	b	0x3c518
   3c54c: eafffff1     	b	0x3c518
   3c550: eafffff2     	b	0x3c520
   3c554: eaffffe8     	b	0x3c4fc
   3c558: e59d0000     	ldr	r0, [sp]
   3c55c: e28d3008     	add	r3, sp, #8
   3c560: e1500003     	cmp	r0, r3
   3c564: 0affffe7     	beq	0x3c508
   3c568: ebff6634     	bl	0x15e40    @ imm = #-0x26730 ; _ZdlPv
   3c56c: eaffffe5     	b	0x3c508
   3c570: a0 1f 07 00  	.word	0x00071fa0
   3c574: 14 2f 07 00  	.word	0x00072f14
   3c578: 90 1f 07 00  	.word	0x00071f90
   3c57c: 04 2f 07 00  	.word	0x00072f04
   3c580: 40 4f 09 00  	.word	0x00094f40
   3c584: b0 1f 07 00  	.word	0x00071fb0
   3c588: 50 1f 07 00  	.word	0x00071f50
   3c58c: e4 2e 07 00  	.word	0x00072ee4
