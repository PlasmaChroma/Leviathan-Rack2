; lubadh::FirstRecMode::interpretButtonPress(int)
; VA 0x3a6ac size 1204

   3a6ac: e92d4070     	push	{r4, r5, r6, lr}
   3a6b0: e3510001     	cmp	r1, #1
   3a6b4: e1a04000     	mov	r4, r0
   3a6b8: e24dd098     	sub	sp, sp, #152
   3a6bc: da00005f     	ble	0x3a840
   3a6c0: e3510007     	cmp	r1, #7
   3a6c4: 1a00005f     	bne	0x3a848
   3a6c8: e5900008     	ldr	r0, [r0, #0x8]
   3a6cc: e59030e8     	ldr	r3, [r0, #0xe8]
   3a6d0: e59022e8     	ldr	r2, [r0, #0x2e8]
   3a6d4: e59330b0     	ldr	r3, [r3, #0xb0]
   3a6d8: e5933030     	ldr	r3, [r3, #0x30]
   3a6dc: e1a03103     	lsl	r3, r3, #2
   3a6e0: e1530002     	cmp	r3, r2
   3a6e4: c3a03001     	movgt	r3, #1
   3a6e8: d3a03000     	movle	r3, #0
   3a6ec: e5c4300c     	strb	r3, [r4, #0xc]
   3a6f0: da000069     	ble	0x3a89c
   3a6f4: e2801004     	add	r1, r0, #4
   3a6f8: e30226a4     	movw	r2, #0x26a4
   3a6fc: e3402007     	movt	r2, #0x7
   3a700: e28d0008     	add	r0, sp, #8
   3a704: ebffd12f     	bl	0x2ebc8
   3a708: e5942008     	ldr	r2, [r4, #0x8]
   3a70c: e3003d28     	movw	r3, #0xd28
   3a710: e3403007     	movt	r3, #0x7
   3a714: e3061218     	movw	r1, #0x6218
   3a718: e3401001     	movt	r1, #0x1
   3a71c: e592c0e8     	ldr	r12, [r2, #0xe8]
   3a720: e28d0020     	add	r0, sp, #32
   3a724: e3a02010     	mov	r2, #16
   3a728: e59cc0b0     	ldr	r12, [r12, #0xb0]
   3a72c: e59cc030     	ldr	r12, [r12, #0x30]
   3a730: e1a0c10c     	lsl	r12, r12, #2
   3a734: e58dc000     	str	r12, [sp]
   3a738: ebfff21a     	bl	0x36fa8
   3a73c: e28d2020     	add	r2, sp, #32
   3a740: e28d1008     	add	r1, sp, #8
   3a744: e28d0038     	add	r0, sp, #56
   3a748: ebffd067     	bl	0x2e8ec
   3a74c: e30216cc     	movw	r1, #0x26cc
   3a750: e3401007     	movt	r1, #0x7
   3a754: e28d0038     	add	r0, sp, #56
   3a758: ebff6f3b     	bl	0x1644c    @ imm = #-0x24314 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc
   3a75c: e28d5050     	add	r5, sp, #80
   3a760: e1a01000     	mov	r1, r0
   3a764: e1a00005     	mov	r0, r5
   3a768: e28d6068     	add	r6, sp, #104
   3a76c: ebff6d23     	bl	0x15c00    @ imm = #-0x24b74 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC1EOS4_
   3a770: e5942008     	ldr	r2, [r4, #0x8]
   3a774: e3003d28     	movw	r3, #0xd28
   3a778: e3403007     	movt	r3, #0x7
   3a77c: e3061218     	movw	r1, #0x6218
   3a780: e3401001     	movt	r1, #0x1
   3a784: e59222e8     	ldr	r2, [r2, #0x2e8]
   3a788: e1a00006     	mov	r0, r6
   3a78c: e58d2000     	str	r2, [sp]
   3a790: e3a02010     	mov	r2, #16
   3a794: ebfff203     	bl	0x36fa8
   3a798: e28d4080     	add	r4, sp, #128
   3a79c: e1a02006     	mov	r2, r6
   3a7a0: e1a01005     	mov	r1, r5
   3a7a4: e1a00004     	mov	r0, r4
   3a7a8: ebffd04f     	bl	0x2e8ec
   3a7ac: e3090fec     	movw	r0, #0x9fec
   3a7b0: e3400009     	movt	r0, #0x9
   3a7b4: e1a01004     	mov	r1, r4
   3a7b8: e3a02000     	mov	r2, #0
   3a7bc: eb00d5d7     	bl	0x6ff20
   3a7c0: e59d0080     	ldr	r0, [sp, #0x80]
   3a7c4: e28d3088     	add	r3, sp, #136
   3a7c8: e1500003     	cmp	r0, r3
   3a7cc: 0a000000     	beq	0x3a7d4
   3a7d0: ebff6d9a     	bl	0x15e40    @ imm = #-0x24998 ; _ZdlPv
   3a7d4: e59d0068     	ldr	r0, [sp, #0x68]
   3a7d8: e28d3070     	add	r3, sp, #112
   3a7dc: e1500003     	cmp	r0, r3
   3a7e0: 0a000000     	beq	0x3a7e8
   3a7e4: ebff6d95     	bl	0x15e40    @ imm = #-0x249ac ; _ZdlPv
   3a7e8: e59d0050     	ldr	r0, [sp, #0x50]
   3a7ec: e28d3058     	add	r3, sp, #88
   3a7f0: e1500003     	cmp	r0, r3
   3a7f4: 0a000000     	beq	0x3a7fc
   3a7f8: ebff6d90     	bl	0x15e40    @ imm = #-0x249c0 ; _ZdlPv
   3a7fc: e59d0038     	ldr	r0, [sp, #0x38]
   3a800: e28d3040     	add	r3, sp, #64
   3a804: e1500003     	cmp	r0, r3
   3a808: 0a000000     	beq	0x3a810
   3a80c: ebff6d8b     	bl	0x15e40    @ imm = #-0x249d4 ; _ZdlPv
   3a810: e59d0020     	ldr	r0, [sp, #0x20]
   3a814: e28d3028     	add	r3, sp, #40
   3a818: e1500003     	cmp	r0, r3
   3a81c: 0a000000     	beq	0x3a824
   3a820: ebff6d86     	bl	0x15e40    @ imm = #-0x249e8 ; _ZdlPv
   3a824: e59d0008     	ldr	r0, [sp, #0x8]
   3a828: e28d3010     	add	r3, sp, #16
   3a82c: e1500003     	cmp	r0, r3
   3a830: 0a000004     	beq	0x3a848
   3a834: ebff6d81     	bl	0x15e40    @ imm = #-0x249fc ; _ZdlPv
   3a838: e28dd098     	add	sp, sp, #152
   3a83c: e8bd8070     	pop	{r4, r5, r6, pc}
   3a840: e3510000     	cmp	r1, #0
   3a844: aa000001     	bge	0x3a850
   3a848: e28dd098     	add	sp, sp, #152
   3a84c: e8bd8070     	pop	{r4, r5, r6, pc}
   3a850: e5900008     	ldr	r0, [r0, #0x8]
   3a854: e59020e8     	ldr	r2, [r0, #0xe8]
   3a858: e59012e8     	ldr	r1, [r0, #0x2e8]
   3a85c: e59230b0     	ldr	r3, [r2, #0xb0]
   3a860: e5933030     	ldr	r3, [r3, #0x30]
   3a864: e1a03103     	lsl	r3, r3, #2
   3a868: e1530001     	cmp	r3, r1
   3a86c: c3a03001     	movgt	r3, #1
   3a870: d3a03000     	movle	r3, #0
   3a874: e5c4300c     	strb	r3, [r4, #0xc]
   3a878: caffff9d     	bgt	0x3a6f4
   3a87c: e5923084     	ldr	r3, [r2, #0x84]
   3a880: e3530001     	cmp	r3, #1
   3a884: 05d0227c     	ldrbeq	r2, [r0, #0x27c]
   3a888: 03a03000     	moveq	r3, #0
   3a88c: 05c0227d     	strbeq	r2, [r0, #0x27d]
   3a890: 05c0327c     	strbeq	r3, [r0, #0x27c]
   3a894: e3a03000     	mov	r3, #0
   3a898: e5c4300c     	strb	r3, [r4, #0xc]
   3a89c: e3a01003     	mov	r1, #3
   3a8a0: ebfffddc     	bl	0x3a018
   3a8a4: e5940008     	ldr	r0, [r4, #0x8]
   3a8a8: e303c004     	movw	r12, #0x3004
   3a8ac: e3a02001     	mov	r2, #1
   3a8b0: e59032e8     	ldr	r3, [r0, #0x2e8]
   3a8b4: e590e058     	ldr	lr, [r0, #0x58]
   3a8b8: e2831e99     	add	r1, r3, #2448
   3a8bc: e580304c     	str	r3, [r0, #0x4c]
   3a8c0: e281100a     	add	r1, r1, #10
   3a8c4: e5801050     	str	r1, [r0, #0x50]
   3a8c8: e2431001     	sub	r1, r3, #1
   3a8cc: e083300c     	add	r3, r3, r12
   3a8d0: e151000c     	cmp	r1, r12
   3a8d4: a1a0100c     	movge	r1, r12
   3a8d8: e5801084     	str	r1, [r0, #0x84]
   3a8dc: e3a01000     	mov	r1, #0
   3a8e0: e58e3000     	str	r3, [lr]
   3a8e4: ebfff370     	bl	0x376ac
   3a8e8: e5941008     	ldr	r1, [r4, #0x8]
   3a8ec: eddf0b99     	vldr	d16, [pc, #612]         @ 0x3ab58 ; float 5.26354424712e-315
   3a8f0: e3a025fe     	mov	r2, #1065353216
   3a8f4: e2810fb6     	add	r0, r1, #728
   3a8f8: e59130e8     	ldr	r3, [r1, #0xe8]
   3a8fc: e581202c     	str	r2, [r1, #0x2c]
   3a900: f440078f     	vst1.32	{d16}, [r0]
   3a904: e3a00000     	mov	r0, #0
   3a908: e593c0b0     	ldr	r12, [r3, #0xb0]
   3a90c: e5812024     	str	r2, [r1, #0x24]
   3a910: e58102e0     	str	r0, [r1, #0x2e0]
   3a914: e59c2040     	ldr	r2, [r12, #0x40]
   3a918: e3520003     	cmp	r2, #3
   3a91c: e593208c     	ldr	r2, [r3, #0x8c]
   3a920: 058100ec     	streq	r0, [r1, #0xec]
   3a924: e3520001     	cmp	r2, #1
   3a928: 0a00001f     	beq	0x3a9ac
   3a92c: edd37a00     	vldr	s15, [r3]
   3a930: e2810fda     	add	r0, r1, #872
   3a934: e5932064     	ldr	r2, [r3, #0x64]
   3a938: eef57ac0     	vcmpe.f32	s15, #0
   3a93c: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3a940: a5931014     	ldrge	r1, [r3, #0x14]
   3a944: b5931020     	ldrlt	r1, [r3, #0x20]
   3a948: e3a03001     	mov	r3, #1
   3a94c: eb005155     	bl	0x4eea8
   3a950: e2505000     	subs	r5, r0, #0
   3a954: 0a000013     	beq	0x3a9a8
   3a958: e5943008     	ldr	r3, [r4, #0x8]
   3a95c: e59310e8     	ldr	r1, [r3, #0xe8]
   3a960: e5913094     	ldr	r3, [r1, #0x94]
   3a964: e3530001     	cmp	r3, #1
   3a968: 0a00003f     	beq	0x3aa6c
   3a96c: e5d53014     	ldrb	r3, [r5, #0x14]
   3a970: e1a00005     	mov	r0, r5
   3a974: edd17a00     	vldr	s15, [r1]
   3a978: e3530000     	cmp	r3, #0
   3a97c: e5913064     	ldr	r3, [r1, #0x64]
   3a980: e3a01001     	mov	r1, #1
   3a984: e5952004     	ldr	r2, [r5, #0x4]
   3a988: 1dd57a06     	vldrne	s15, [r5, #24]
   3a98c: e58d1000     	str	r1, [sp]
   3a990: eef57ac0     	vcmpe.f32	s15, #0
   3a994: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3a998: a1a0c001     	movge	r12, r1
   3a99c: b3a0c000     	movlt	r12, #0
   3a9a0: e58dc004     	str	r12, [sp, #0x4]
   3a9a4: eb004eb0     	bl	0x4e46c
   3a9a8: e5941008     	ldr	r1, [r4, #0x8]
   3a9ac: e2813a29     	add	r3, r1, #167936
   3a9b0: e591c058     	ldr	r12, [r1, #0x58]
   3a9b4: e28d5050     	add	r5, sp, #80
   3a9b8: e2811004     	add	r1, r1, #4
   3a9bc: e1a00005     	mov	r0, r5
   3a9c0: e3022674     	movw	r2, #0x2674
   3a9c4: e3402007     	movt	r2, #0x7
   3a9c8: e5933f5c     	ldr	r3, [r3, #0xf5c]
   3a9cc: e59cc000     	ldr	r12, [r12]
   3a9d0: e28d6068     	add	r6, sp, #104
   3a9d4: e583c004     	str	r12, [r3, #0x4]
   3a9d8: ebffd07a     	bl	0x2ebc8
   3a9dc: e5942008     	ldr	r2, [r4, #0x8]
   3a9e0: e3003d28     	movw	r3, #0xd28
   3a9e4: e3403007     	movt	r3, #0x7
   3a9e8: e3061218     	movw	r1, #0x6218
   3a9ec: e3401001     	movt	r1, #0x1
   3a9f0: e592204c     	ldr	r2, [r2, #0x4c]
   3a9f4: e1a00006     	mov	r0, r6
   3a9f8: e58d2000     	str	r2, [sp]
   3a9fc: e3a02010     	mov	r2, #16
   3aa00: ebfff168     	bl	0x36fa8
   3aa04: e28d4080     	add	r4, sp, #128
   3aa08: e1a02006     	mov	r2, r6
   3aa0c: e1a01005     	mov	r1, r5
   3aa10: e1a00004     	mov	r0, r4
   3aa14: ebffcfb4     	bl	0x2e8ec
   3aa18: e3090fec     	movw	r0, #0x9fec
   3aa1c: e3400009     	movt	r0, #0x9
   3aa20: e1a01004     	mov	r1, r4
   3aa24: e3a02000     	mov	r2, #0
   3aa28: eb00d53c     	bl	0x6ff20
   3aa2c: e59d0080     	ldr	r0, [sp, #0x80]
   3aa30: e28d3088     	add	r3, sp, #136
   3aa34: e1500003     	cmp	r0, r3
   3aa38: 0a000000     	beq	0x3aa40
   3aa3c: ebff6cff     	bl	0x15e40    @ imm = #-0x24c04 ; _ZdlPv
   3aa40: e59d0068     	ldr	r0, [sp, #0x68]
   3aa44: e28d3070     	add	r3, sp, #112
   3aa48: e1500003     	cmp	r0, r3
   3aa4c: 0a000000     	beq	0x3aa54
   3aa50: ebff6cfa     	bl	0x15e40    @ imm = #-0x24c18 ; _ZdlPv
   3aa54: e59d0050     	ldr	r0, [sp, #0x50]
   3aa58: e28d3058     	add	r3, sp, #88
   3aa5c: e1500003     	cmp	r0, r3
   3aa60: 0affff78     	beq	0x3a848
   3aa64: ebff6cf5     	bl	0x15e40    @ imm = #-0x24c2c ; _ZdlPv
   3aa68: eaffff72     	b	0x3a838
   3aa6c: ed910a02     	vldr	s0, [r1, #8]
   3aa70: eb004529     	bl	0x4bf1c
   3aa74: e5943008     	ldr	r3, [r4, #0x8]
   3aa78: e59310e8     	ldr	r1, [r3, #0xe8]
   3aa7c: eaffffba     	b	0x3a96c
   3aa80: ea000024     	b	0x3ab18
   3aa84: ea00001e     	b	0x3ab04
   3aa88: e59d0080     	ldr	r0, [sp, #0x80]
   3aa8c: e28d3088     	add	r3, sp, #136
   3aa90: e1500003     	cmp	r0, r3
   3aa94: 0a000000     	beq	0x3aa9c
   3aa98: ebff6ce8     	bl	0x15e40    @ imm = #-0x24c60 ; _ZdlPv
   3aa9c: e59d0068     	ldr	r0, [sp, #0x68]
   3aaa0: e28d3070     	add	r3, sp, #112
   3aaa4: e1500003     	cmp	r0, r3
   3aaa8: 0a000000     	beq	0x3aab0
   3aaac: ebff6ce3     	bl	0x15e40    @ imm = #-0x24c74 ; _ZdlPv
   3aab0: e59d0050     	ldr	r0, [sp, #0x50]
   3aab4: e28d3058     	add	r3, sp, #88
   3aab8: e1500003     	cmp	r0, r3
   3aabc: 0a000000     	beq	0x3aac4
   3aac0: ebff6cde     	bl	0x15e40    @ imm = #-0x24c88 ; _ZdlPv
   3aac4: ebff6d25     	bl	0x15f60    @ imm = #-0x24b6c ; __cxa_end_cleanup
   3aac8: e59d0080     	ldr	r0, [sp, #0x80]
   3aacc: e28d3088     	add	r3, sp, #136
   3aad0: e1500003     	cmp	r0, r3
   3aad4: 0a000000     	beq	0x3aadc
   3aad8: ebff6cd8     	bl	0x15e40    @ imm = #-0x24ca0 ; _ZdlPv
   3aadc: e59d0068     	ldr	r0, [sp, #0x68]
   3aae0: e28d3070     	add	r3, sp, #112
   3aae4: e1500003     	cmp	r0, r3
   3aae8: 0a000000     	beq	0x3aaf0
   3aaec: ebff6cd3     	bl	0x15e40    @ imm = #-0x24cb4 ; _ZdlPv
   3aaf0: e59d0050     	ldr	r0, [sp, #0x50]
   3aaf4: e28d3058     	add	r3, sp, #88
   3aaf8: e1500003     	cmp	r0, r3
   3aafc: 0a000000     	beq	0x3ab04
   3ab00: ebff6cce     	bl	0x15e40    @ imm = #-0x24cc8 ; _ZdlPv
   3ab04: e59d0038     	ldr	r0, [sp, #0x38]
   3ab08: e28d3040     	add	r3, sp, #64
   3ab0c: e1500003     	cmp	r0, r3
   3ab10: 0a000000     	beq	0x3ab18
   3ab14: ebff6cc9     	bl	0x15e40    @ imm = #-0x24cdc ; _ZdlPv
   3ab18: e59d0020     	ldr	r0, [sp, #0x20]
   3ab1c: e28d3028     	add	r3, sp, #40
   3ab20: e1500003     	cmp	r0, r3
   3ab24: 0a000000     	beq	0x3ab2c
   3ab28: ebff6cc4     	bl	0x15e40    @ imm = #-0x24cf0 ; _ZdlPv
   3ab2c: e59d0008     	ldr	r0, [sp, #0x8]
   3ab30: e28d3010     	add	r3, sp, #16
   3ab34: e1500003     	cmp	r0, r3
   3ab38: 1affffe0     	bne	0x3aac0
   3ab3c: eaffffe0     	b	0x3aac4
   3ab40: eaffffd5     	b	0x3aa9c
   3ab44: eaffffe4     	b	0x3aadc
   3ab48: eaffffe8     	b	0x3aaf0
   3ab4c: eaffffd7     	b	0x3aab0
   3ab50: eafffff5     	b	0x3ab2c
   3ab54: e320f000     	nop
   3ab58: 00 00 80 3f  	.word	0x3f800000
   3ab5c: 00 00 00 00  	.word	0x00000000
