; lubadh::Channel::setState(lubadh::_StateLabels)
; VA 0x3a018 size 1112

   3a018: e59030e8     	ldr	r3, [r0, #0xe8]
   3a01c: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
   3a020: e1a04000     	mov	r4, r0
   3a024: e5932084     	ldr	r2, [r3, #0x84]
   3a028: ed2d8b02     	vpush	{d8}
   3a02c: e24dd054     	sub	sp, sp, #84
   3a030: e3520002     	cmp	r2, #2
   3a034: 05d0227c     	ldrbeq	r2, [r0, #0x27c]
   3a038: 03a00001     	moveq	r0, #1
   3a03c: 05c4027c     	strbeq	r0, [r4, #0x27c]
   3a040: 00222000     	eoreq	r2, r2, r0
   3a044: 05c4227d     	strbeq	r2, [r4, #0x27d]
   3a048: e3510003     	cmp	r1, #3
   3a04c: 979ff101     	ldrls	pc, [pc, r1, lsl #2]
   3a050: ea00004a     	b	0x3a180
   3a054: ec a1 03 00  	.word	0x0003a1ec
   3a058: 74 a2 03 00  	.word	0x0003a274
   3a05c: 64 a0 03 00  	.word	0x0003a064
   3a060: 8c a1 03 00  	.word	0x0003a18c
   3a064: e2847004     	add	r7, r4, #4
   3a068: e28d0038     	add	r0, sp, #56
   3a06c: e1a01007     	mov	r1, r7
   3a070: e30225fc     	movw	r2, #0x25fc
   3a074: e3402007     	movt	r2, #0x7
   3a078: ed938a00     	vldr	s16, [r3]
   3a07c: ebffd2d1     	bl	0x2ebc8
   3a080: e3090fec     	movw	r0, #0x9fec
   3a084: e3400009     	movt	r0, #0x9
   3a088: e28d1038     	add	r1, sp, #56
   3a08c: e3a02000     	mov	r2, #0
   3a090: eb00d7a2     	bl	0x6ff20
   3a094: e59d0038     	ldr	r0, [sp, #0x38]
   3a098: e28d8040     	add	r8, sp, #64
   3a09c: e1500008     	cmp	r0, r8
   3a0a0: 0a000000     	beq	0x3a0a8
   3a0a4: ebff6f65     	bl	0x15e40    @ imm = #-0x2426c ; _ZdlPv
   3a0a8: eeb58ac0     	vcmpe.f32	s16, #0
   3a0ac: e2846fda     	add	r6, r4, #872
   3a0b0: e2843e26     	add	r3, r4, #608
   3a0b4: e1a00006     	mov	r0, r6
   3a0b8: e2845ee6     	add	r5, r4, #3680
   3a0bc: e5843278     	str	r3, [r4, #0x278]
   3a0c0: e2855008     	add	r5, r5, #8
   3a0c4: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3a0c8: a3a09001     	movge	r9, #1
   3a0cc: b3a09000     	movlt	r9, #0
   3a0d0: eb004e8b     	bl	0x4db04
   3a0d4: e3500000     	cmp	r0, #0
   3a0d8: 1a000084     	bne	0x3a2f0
   3a0dc: e59420e8     	ldr	r2, [r4, #0xe8]
   3a0e0: e59230a0     	ldr	r3, [r2, #0xa0]
   3a0e4: e3530001     	cmp	r3, #1
   3a0e8: 0a000003     	beq	0x3a0fc
   3a0ec: eeb58ac0     	vcmpe.f32	s16, #0
   3a0f0: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3a0f4: b5921020     	ldrlt	r1, [r2, #0x20]
   3a0f8: ba000000     	blt	0x3a100
   3a0fc: e5921014     	ldr	r1, [r2, #0x14]
   3a100: e1a00005     	mov	r0, r5
   3a104: e5922064     	ldr	r2, [r2, #0x64]
   3a108: e1a03009     	mov	r3, r9
   3a10c: eb005365     	bl	0x4eea8
   3a110: e1a05000     	mov	r5, r0
   3a114: e3550000     	cmp	r5, #0
   3a118: 0a000014     	beq	0x3a170
   3a11c: e59430e8     	ldr	r3, [r4, #0xe8]
   3a120: e59330a0     	ldr	r3, [r3, #0xa0]
   3a124: e3530001     	cmp	r3, #1
   3a128: 0a0000b2     	beq	0x3a3f8
   3a12c: e2843a01     	add	r3, r4, #4096
   3a130: e2846d65     	add	r6, r4, #6464
   3a134: e2866028     	add	r6, r6, #40
   3a138: e5d31968     	ldrb	r1, [r3, #0x968]
   3a13c: e3510000     	cmp	r1, #0
   3a140: 0a0000a9     	beq	0x3a3ec
   3a144: e59430e8     	ldr	r3, [r4, #0xe8]
   3a148: e1a00006     	mov	r0, r6
   3a14c: ed9f0ac6     	vldr	s0, [pc, #792]          @ 0x3a46c ; float 256
   3a150: edd37a19     	vldr	s15, [r3, #100]
   3a154: eef87ae7     	vcvt.f32.s32	s15, s15
   3a158: ee800a27     	vdiv.f32	s0, s0, s15
   3a15c: eb004664     	bl	0x4baf4
   3a160: e59430e8     	ldr	r3, [r4, #0xe8]
   3a164: e5932098     	ldr	r2, [r3, #0x98]
   3a168: e3520001     	cmp	r2, #1
   3a16c: 0a00006a     	beq	0x3a31c
   3a170: e2844a29     	add	r4, r4, #167936
   3a174: e3a02000     	mov	r2, #0
   3a178: e5943f5c     	ldr	r3, [r4, #0xf5c]
   3a17c: e5c32000     	strb	r2, [r3]
   3a180: e28dd054     	add	sp, sp, #84
   3a184: ecbd8b02     	vpop	{d8}
   3a188: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   3a18c: e28d0038     	add	r0, sp, #56
   3a190: e2841004     	add	r1, r4, #4
   3a194: e3022644     	movw	r2, #0x2644
   3a198: e3402007     	movt	r2, #0x7
   3a19c: ebffd289     	bl	0x2ebc8
   3a1a0: e3090fec     	movw	r0, #0x9fec
   3a1a4: e3400009     	movt	r0, #0x9
   3a1a8: e28d1038     	add	r1, sp, #56
   3a1ac: e3a02000     	mov	r2, #0
   3a1b0: eb00d75a     	bl	0x6ff20
   3a1b4: e59d0038     	ldr	r0, [sp, #0x38]
   3a1b8: e28d3040     	add	r3, sp, #64
   3a1bc: e1500003     	cmp	r0, r3
   3a1c0: 0a000000     	beq	0x3a1c8
   3a1c4: ebff6f1d     	bl	0x15e40    @ imm = #-0x2438c ; _ZdlPv
   3a1c8: e2843a29     	add	r3, r4, #167936
   3a1cc: e2842f9b     	add	r2, r4, #620
   3a1d0: e5842278     	str	r2, [r4, #0x278]
   3a1d4: e3a02000     	mov	r2, #0
   3a1d8: e5933f5c     	ldr	r3, [r3, #0xf5c]
   3a1dc: e5c32000     	strb	r2, [r3]
   3a1e0: e28dd054     	add	sp, sp, #84
   3a1e4: ecbd8b02     	vpop	{d8}
   3a1e8: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   3a1ec: e28d0038     	add	r0, sp, #56
   3a1f0: e2841004     	add	r1, r4, #4
   3a1f4: e30225c0     	movw	r2, #0x25c0
   3a1f8: e3402007     	movt	r2, #0x7
   3a1fc: ebffd271     	bl	0x2ebc8
   3a200: e3090fec     	movw	r0, #0x9fec
   3a204: e3400009     	movt	r0, #0x9
   3a208: e28d1038     	add	r1, sp, #56
   3a20c: e3a02000     	mov	r2, #0
   3a210: eb00d742     	bl	0x6ff20
   3a214: e59d0038     	ldr	r0, [sp, #0x38]
   3a218: e28d3040     	add	r3, sp, #64
   3a21c: e1500003     	cmp	r0, r3
   3a220: 0a000000     	beq	0x3a228
   3a224: ebff6f05     	bl	0x15e40    @ imm = #-0x243ec ; _ZdlPv
   3a228: e2840ba7     	add	r0, r4, #171008
   3a22c: e2843f91     	add	r3, r4, #580
   3a230: e2800fdf     	add	r0, r0, #892
   3a234: e5843278     	str	r3, [r4, #0x278]
   3a238: ebfff9a4     	bl	0x388d0
   3a23c: e2840fb9     	add	r0, r4, #740
   3a240: eb004682     	bl	0x4bc50
   3a244: e2840fda     	add	r0, r4, #872
   3a248: eb004cad     	bl	0x4d504
   3a24c: e2840ee6     	add	r0, r4, #3680
   3a250: e2844a29     	add	r4, r4, #167936
   3a254: e2800008     	add	r0, r0, #8
   3a258: eb004ca9     	bl	0x4d504
   3a25c: e3a02001     	mov	r2, #1
   3a260: e5943f5c     	ldr	r3, [r4, #0xf5c]
   3a264: e5c32000     	strb	r2, [r3]
   3a268: e28dd054     	add	sp, sp, #84
   3a26c: ecbd8b02     	vpop	{d8}
   3a270: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   3a274: e28d0038     	add	r0, sp, #56
   3a278: e2841004     	add	r1, r4, #4
   3a27c: e30225dc     	movw	r2, #0x25dc
   3a280: e3402007     	movt	r2, #0x7
   3a284: ebffd24f     	bl	0x2ebc8
   3a288: e3090fec     	movw	r0, #0x9fec
   3a28c: e3400009     	movt	r0, #0x9
   3a290: e28d1038     	add	r1, sp, #56
   3a294: e3a02000     	mov	r2, #0
   3a298: eb00d720     	bl	0x6ff20
   3a29c: e59d0038     	ldr	r0, [sp, #0x38]
   3a2a0: e28d3040     	add	r3, sp, #64
   3a2a4: e1500003     	cmp	r0, r3
   3a2a8: 0a000000     	beq	0x3a2b0
   3a2ac: ebff6ee3     	bl	0x15e40    @ imm = #-0x24474 ; _ZdlPv
   3a2b0: e2843e25     	add	r3, r4, #592
   3a2b4: e2845fb9     	add	r5, r4, #740
   3a2b8: e5843278     	str	r3, [r4, #0x278]
   3a2bc: e2844a29     	add	r4, r4, #167936
   3a2c0: e1a00005     	mov	r0, r5
   3a2c4: e3a01000     	mov	r1, #0
   3a2c8: eb004f12     	bl	0x4df18
   3a2cc: e1a00005     	mov	r0, r5
   3a2d0: eeb70a00     	vmov.f32	s0, #1.000000e+00
   3a2d4: eb004710     	bl	0x4bf1c
   3a2d8: e5943f5c     	ldr	r3, [r4, #0xf5c]
   3a2dc: e3a02001     	mov	r2, #1
   3a2e0: e5c32000     	strb	r2, [r3]
   3a2e4: e28dd054     	add	sp, sp, #84
   3a2e8: ecbd8b02     	vpop	{d8}
   3a2ec: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
   3a2f0: e1a00006     	mov	r0, r6
   3a2f4: eb004d48     	bl	0x4d81c
   3a2f8: e59420e8     	ldr	r2, [r4, #0xe8]
   3a2fc: e1a01000     	mov	r1, r0
   3a300: e1a03009     	mov	r3, r9
   3a304: e1a00005     	mov	r0, r5
   3a308: e5922064     	ldr	r2, [r2, #0x64]
   3a30c: e5911004     	ldr	r1, [r1, #0x4]
   3a310: eb0052e4     	bl	0x4eea8
   3a314: e1a05000     	mov	r5, r0
   3a318: eaffff7d     	b	0x3a114
   3a31c: e5951004     	ldr	r1, [r5, #0x4]
   3a320: e5841078     	str	r1, [r4, #0x78]
   3a324: e5c4207c     	strb	r2, [r4, #0x7c]
   3a328: e59330a0     	ldr	r3, [r3, #0xa0]
   3a32c: e3530000     	cmp	r3, #0
   3a330: 1a000004     	bne	0x3a348
   3a334: eeb58ac0     	vcmpe.f32	s16, #0
   3a338: eef1fa10     	vmrs	APSR_nzcv, fpscr
   3a33c: b2811001     	addlt	r1, r1, #1
   3a340: b5841078     	strlt	r1, [r4, #0x78]
   3a344: ba000001     	blt	0x3a350
   3a348: e2411001     	sub	r1, r1, #1
   3a34c: e5841078     	str	r1, [r4, #0x78]
   3a350: e1a01007     	mov	r1, r7
   3a354: e28d0008     	add	r0, sp, #8
   3a358: e302261c     	movw	r2, #0x261c
   3a35c: e3402007     	movt	r2, #0x7
   3a360: ebffd218     	bl	0x2ebc8
   3a364: e59420e8     	ldr	r2, [r4, #0xe8]
   3a368: e3003d28     	movw	r3, #0xd28
   3a36c: e3403007     	movt	r3, #0x7
   3a370: e3061218     	movw	r1, #0x6218
   3a374: e3401001     	movt	r1, #0x1
   3a378: e5922054     	ldr	r2, [r2, #0x54]
   3a37c: e28d0020     	add	r0, sp, #32
   3a380: e58d2000     	str	r2, [sp]
   3a384: e3a02010     	mov	r2, #16
   3a388: ebfff306     	bl	0x36fa8
   3a38c: e28d2020     	add	r2, sp, #32
   3a390: e28d1008     	add	r1, sp, #8
   3a394: e28d0038     	add	r0, sp, #56
   3a398: ebffd153     	bl	0x2e8ec
   3a39c: e3090fec     	movw	r0, #0x9fec
   3a3a0: e3400009     	movt	r0, #0x9
   3a3a4: e28d1038     	add	r1, sp, #56
   3a3a8: e3a02000     	mov	r2, #0
   3a3ac: eb00d6db     	bl	0x6ff20
   3a3b0: e59d0038     	ldr	r0, [sp, #0x38]
   3a3b4: e1500008     	cmp	r0, r8
   3a3b8: 0a000000     	beq	0x3a3c0
   3a3bc: ebff6e9f     	bl	0x15e40    @ imm = #-0x24584 ; _ZdlPv
   3a3c0: e59d0020     	ldr	r0, [sp, #0x20]
   3a3c4: e28d3028     	add	r3, sp, #40
   3a3c8: e1500003     	cmp	r0, r3
   3a3cc: 0a000000     	beq	0x3a3d4
   3a3d0: ebff6e9a     	bl	0x15e40    @ imm = #-0x24598 ; _ZdlPv
   3a3d4: e59d0008     	ldr	r0, [sp, #0x8]
   3a3d8: e28d3010     	add	r3, sp, #16
   3a3dc: e1500003     	cmp	r0, r3
   3a3e0: 0affff62     	beq	0x3a170
   3a3e4: ebff6e95     	bl	0x15e40    @ imm = #-0x245ac ; _ZdlPv
   3a3e8: eaffff60     	b	0x3a170
   3a3ec: e1a00006     	mov	r0, r6
   3a3f0: eb0045a2     	bl	0x4ba80
   3a3f4: eaffff52     	b	0x3a144
   3a3f8: e1a00005     	mov	r0, r5
   3a3fc: eeb70a00     	vmov.f32	s0, #1.000000e+00
   3a400: eb0046c5     	bl	0x4bf1c
   3a404: eaffff48     	b	0x3a12c
   3a408: e59d0038     	ldr	r0, [sp, #0x38]
   3a40c: e1500008     	cmp	r0, r8
   3a410: 0a000000     	beq	0x3a418
   3a414: ebff6e89     	bl	0x15e40    @ imm = #-0x245dc ; _ZdlPv
   3a418: e59d0020     	ldr	r0, [sp, #0x20]
   3a41c: e28d3028     	add	r3, sp, #40
   3a420: e1500003     	cmp	r0, r3
   3a424: 0a000000     	beq	0x3a42c
   3a428: ebff6e84     	bl	0x15e40    @ imm = #-0x245f0 ; _ZdlPv
   3a42c: e59d0008     	ldr	r0, [sp, #0x8]
   3a430: e28d3010     	add	r3, sp, #16
   3a434: e1500003     	cmp	r0, r3
   3a438: 0a000000     	beq	0x3a440
   3a43c: ebff6e7f     	bl	0x15e40    @ imm = #-0x24604 ; _ZdlPv
   3a440: ebff6ec6     	bl	0x15f60    @ imm = #-0x244e8 ; __cxa_end_cleanup
   3a444: eafffff3     	b	0x3a418
   3a448: eafffff7     	b	0x3a42c
   3a44c: e59d0038     	ldr	r0, [sp, #0x38]
   3a450: e28d3040     	add	r3, sp, #64
   3a454: e1500003     	cmp	r0, r3
   3a458: 1afffff7     	bne	0x3a43c
   3a45c: eafffff7     	b	0x3a440
   3a460: eafffff9     	b	0x3a44c
   3a464: eafffff8     	b	0x3a44c
   3a468: eafffff7     	b	0x3a44c
   3a46c: 00 00 80 43  	.word	0x43800000
