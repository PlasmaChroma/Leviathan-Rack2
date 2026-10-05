; lubadh::Tap::activate(int)
; VA 0x4df18 size 1088

   4df18: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
   4df1c: e1a04000     	mov	r4, r0
   4df20: e5d03000     	ldrb	r3, [r0]
   4df24: e24dd09c     	sub	sp, sp, #156
   4df28: e1a05001     	mov	r5, r1
   4df2c: e3530000     	cmp	r3, #0
   4df30: 1a00002a     	bne	0x4dfe0
   4df34: e5845004     	str	r5, [r4, #0x4]
   4df38: e3a0c000     	mov	r12, #0
   4df3c: e284200c     	add	r2, r4, #12
   4df40: e584c008     	str	r12, [r4, #0x8]
   4df44: e3a01001     	mov	r1, #1
   4df48: e5c41000     	strb	r1, [r4]
   4df4c: e9940003     	ldmib	r4, {r0, r1}
   4df50: e8820003     	stm	r2, {r0, r1}
   4df54: e2845028     	add	r5, r4, #40
   4df58: e584c024     	str	r12, [r4, #0x24]
   4df5c: e3a03000     	mov	r3, #0
   4df60: e3a025fe     	mov	r2, #1065353216
   4df64: e5941024     	ldr	r1, [r4, #0x24]
   4df68: e1a00003     	mov	r0, r3
   4df6c: e5843020     	str	r3, [r4, #0x20]
   4df70: e284e040     	add	lr, r4, #64
   4df74: e5c43014     	strb	r3, [r4, #0x14]
   4df78: e5c4301c     	strb	r3, [r4, #0x1c]
   4df7c: e5842018     	str	r2, [r4, #0x18]
   4df80: e8850003     	stm	r5, {r0, r1}
   4df84: e2845058     	add	r5, r4, #88
   4df88: e584c03c     	str	r12, [r4, #0x3c]
   4df8c: e5843038     	str	r3, [r4, #0x38]
   4df90: e594103c     	ldr	r1, [r4, #0x3c]
   4df94: e5842030     	str	r2, [r4, #0x30]
   4df98: e5c43034     	strb	r3, [r4, #0x34]
   4df9c: e88e0003     	stm	lr, {r0, r1}
   4dfa0: e284e070     	add	lr, r4, #112
   4dfa4: e584c054     	str	r12, [r4, #0x54]
   4dfa8: e5843050     	str	r3, [r4, #0x50]
   4dfac: e5941054     	ldr	r1, [r4, #0x54]
   4dfb0: e5842048     	str	r2, [r4, #0x48]
   4dfb4: e5c4304c     	strb	r3, [r4, #0x4c]
   4dfb8: e8850003     	stm	r5, {r0, r1}
   4dfbc: e584c06c     	str	r12, [r4, #0x6c]
   4dfc0: e5842060     	str	r2, [r4, #0x60]
   4dfc4: e5843068     	str	r3, [r4, #0x68]
   4dfc8: e594106c     	ldr	r1, [r4, #0x6c]
   4dfcc: e5c43064     	strb	r3, [r4, #0x64]
   4dfd0: e88e0003     	stm	lr, {r0, r1}
   4dfd4: e5842078     	str	r2, [r4, #0x78]
   4dfd8: e28dd09c     	add	sp, sp, #156
   4dfdc: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
   4dfe0: e590307c     	ldr	r3, [r0, #0x7c]
   4dfe4: e3a02010     	mov	r2, #16
   4dfe8: e28d0008     	add	r0, sp, #8
   4dfec: e58d3000     	str	r3, [sp]
   4dff0: e3061218     	movw	r1, #0x6218
   4dff4: e3401001     	movt	r1, #0x1
   4dff8: e3023ab0     	movw	r3, #0x2ab0
   4dffc: e3403007     	movt	r3, #0x7
   4e000: ebfff0a0     	bl	0x4a288
   4e004: e3a02000     	mov	r2, #0
   4e008: e3a0c007     	mov	r12, #7
   4e00c: e3033698     	movw	r3, #0x3698
   4e010: e3403007     	movt	r3, #0x7
   4e014: e28d0008     	add	r0, sp, #8
   4e018: e1a01002     	mov	r1, r2
   4e01c: e58dc000     	str	r12, [sp]
   4e020: ebff1eb4     	bl	0x15af8    @ imm = #-0x38530 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_replaceEjjPKcj
   4e024: e1a0e000     	mov	lr, r0
   4e028: e28d7028     	add	r7, sp, #40
   4e02c: e58d7020     	str	r7, [sp, #0x20]
   4e030: e1a0c000     	mov	r12, r0
   4e034: e49e3008     	ldr	r3, [lr], #8
   4e038: e153000e     	cmp	r3, lr
   4e03c: 158d3020     	strne	r3, [sp, #0x20]
   4e040: 01a06007     	moveq	r6, r7
   4e044: 059e1004     	ldreq	r1, [lr, #0x4]
   4e048: 059e2008     	ldreq	r2, [lr, #0x8]
   4e04c: 059e300c     	ldreq	r3, [lr, #0xc]
   4e050: 059e0000     	ldreq	r0, [lr]
   4e054: 159c2008     	ldrne	r2, [r12, #0x8]
   4e058: 08a6000f     	stmeq	r6!, {r0, r1, r2, r3}
   4e05c: 158d2028     	strne	r2, [sp, #0x28]
   4e060: e3a02000     	mov	r2, #0
   4e064: e5cc2008     	strb	r2, [r12, #0x8]
   4e068: e59c3004     	ldr	r3, [r12, #0x4]
   4e06c: e58d3024     	str	r3, [sp, #0x24]
   4e070: e3e03103     	mvn	r3, #-1073741824
   4e074: e58c2004     	str	r2, [r12, #0x4]
   4e078: e59d1024     	ldr	r1, [sp, #0x24]
   4e07c: e58ce000     	str	lr, [r12]
   4e080: e0433001     	sub	r3, r3, r1
   4e084: e3530005     	cmp	r3, #5
   4e088: 9a000090     	bls	0x4e2d0
   4e08c: e30316a0     	movw	r1, #0x36a0
   4e090: e3401007     	movt	r1, #0x7
   4e094: e3a02006     	mov	r2, #6
   4e098: e28d0020     	add	r0, sp, #32
   4e09c: ebff1f79     	bl	0x15e88    @ imm = #-0x3821c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   4e0a0: e1a0e000     	mov	lr, r0
   4e0a4: e28d6040     	add	r6, sp, #64
   4e0a8: e58d6038     	str	r6, [sp, #0x38]
   4e0ac: e1a0c000     	mov	r12, r0
   4e0b0: e49e3008     	ldr	r3, [lr], #8
   4e0b4: e153000e     	cmp	r3, lr
   4e0b8: 158d3038     	strne	r3, [sp, #0x38]
   4e0bc: 01a08006     	moveq	r8, r6
   4e0c0: 059e0000     	ldreq	r0, [lr]
   4e0c4: 059e1004     	ldreq	r1, [lr, #0x4]
   4e0c8: 059e2008     	ldreq	r2, [lr, #0x8]
   4e0cc: 059e300c     	ldreq	r3, [lr, #0xc]
   4e0d0: 159c2008     	ldrne	r2, [r12, #0x8]
   4e0d4: 08a8000f     	stmeq	r8!, {r0, r1, r2, r3}
   4e0d8: e3a03000     	mov	r3, #0
   4e0dc: e3061218     	movw	r1, #0x6218
   4e0e0: e3401001     	movt	r1, #0x1
   4e0e4: 158d2040     	strne	r2, [sp, #0x40]
   4e0e8: e5cc3008     	strb	r3, [r12, #0x8]
   4e0ec: e28d0050     	add	r0, sp, #80
   4e0f0: e59c2004     	ldr	r2, [r12, #0x4]
   4e0f4: e58d203c     	str	r2, [sp, #0x3c]
   4e0f8: e58c3004     	str	r3, [r12, #0x4]
   4e0fc: e3023ab0     	movw	r3, #0x2ab0
   4e100: e3403007     	movt	r3, #0x7
   4e104: e58ce000     	str	lr, [r12]
   4e108: e5942080     	ldr	r2, [r4, #0x80]
   4e10c: e58d2000     	str	r2, [sp]
   4e110: e3a02010     	mov	r2, #16
   4e114: ebfff05b     	bl	0x4a288
   4e118: e59d3038     	ldr	r3, [sp, #0x38]
   4e11c: e28d9058     	add	r9, sp, #88
   4e120: e59dc03c     	ldr	r12, [sp, #0x3c]
   4e124: e1530006     	cmp	r3, r6
   4e128: e59d2054     	ldr	r2, [sp, #0x54]
   4e12c: 03a0100f     	moveq	r1, #15
   4e130: e08c0002     	add	r0, r12, r2
   4e134: 159d1040     	ldrne	r1, [sp, #0x40]
   4e138: e1500001     	cmp	r0, r1
   4e13c: e59d1050     	ldr	r1, [sp, #0x50]
   4e140: 9a000004     	bls	0x4e158
   4e144: e1510009     	cmp	r1, r9
   4e148: 03a0e00f     	moveq	lr, #15
   4e14c: 159de058     	ldrne	lr, [sp, #0x58]
   4e150: e150000e     	cmp	r0, lr
   4e154: 9a000054     	bls	0x4e2ac
   4e158: e28d0038     	add	r0, sp, #56
   4e15c: ebff1f49     	bl	0x15e88    @ imm = #-0x382dc ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   4e160: e1a0e000     	mov	lr, r0
   4e164: e28d8070     	add	r8, sp, #112
   4e168: e58d8068     	str	r8, [sp, #0x68]
   4e16c: e1a0c000     	mov	r12, r0
   4e170: e49e3008     	ldr	r3, [lr], #8
   4e174: e153000e     	cmp	r3, lr
   4e178: 158d3068     	strne	r3, [sp, #0x68]
   4e17c: 01a0a008     	moveq	r10, r8
   4e180: 059e1004     	ldreq	r1, [lr, #0x4]
   4e184: 059e2008     	ldreq	r2, [lr, #0x8]
   4e188: 059e300c     	ldreq	r3, [lr, #0xc]
   4e18c: 059e0000     	ldreq	r0, [lr]
   4e190: 159c2008     	ldrne	r2, [r12, #0x8]
   4e194: 08aa000f     	stmeq	r10!, {r0, r1, r2, r3}
   4e198: 158d2070     	strne	r2, [sp, #0x70]
   4e19c: e3a02000     	mov	r2, #0
   4e1a0: e5cc2008     	strb	r2, [r12, #0x8]
   4e1a4: e59c3004     	ldr	r3, [r12, #0x4]
   4e1a8: e58d306c     	str	r3, [sp, #0x6c]
   4e1ac: e3e03103     	mvn	r3, #-1073741824
   4e1b0: e58c2004     	str	r2, [r12, #0x4]
   4e1b4: e59d106c     	ldr	r1, [sp, #0x6c]
   4e1b8: e58ce000     	str	lr, [r12]
   4e1bc: e0433001     	sub	r3, r3, r1
   4e1c0: e353001f     	cmp	r3, #31
   4e1c4: 9a00003e     	bls	0x4e2c4
   4e1c8: e30316a8     	movw	r1, #0x36a8
   4e1cc: e3401007     	movt	r1, #0x7
   4e1d0: e3a02020     	mov	r2, #32
   4e1d4: e28d0068     	add	r0, sp, #104
   4e1d8: ebff1f2a     	bl	0x15e88    @ imm = #-0x38358 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   4e1dc: e1a0e000     	mov	lr, r0
   4e1e0: e28da088     	add	r10, sp, #136
   4e1e4: e58da080     	str	r10, [sp, #0x80]
   4e1e8: e1a0c000     	mov	r12, r0
   4e1ec: e49e3008     	ldr	r3, [lr], #8
   4e1f0: e153000e     	cmp	r3, lr
   4e1f4: 158d3080     	strne	r3, [sp, #0x80]
   4e1f8: 01a0b00a     	moveq	r11, r10
   4e1fc: 059e0000     	ldreq	r0, [lr]
   4e200: 059e1004     	ldreq	r1, [lr, #0x4]
   4e204: 059e2008     	ldreq	r2, [lr, #0x8]
   4e208: 059e300c     	ldreq	r3, [lr, #0xc]
   4e20c: 159c2008     	ldrne	r2, [r12, #0x8]
   4e210: 08ab000f     	stmeq	r11!, {r0, r1, r2, r3}
   4e214: e3a03000     	mov	r3, #0
   4e218: e3090fec     	movw	r0, #0x9fec
   4e21c: e3400009     	movt	r0, #0x9
   4e220: 158d2088     	strne	r2, [sp, #0x88]
   4e224: e28d1080     	add	r1, sp, #128
   4e228: e5cc3008     	strb	r3, [r12, #0x8]
   4e22c: e59c2004     	ldr	r2, [r12, #0x4]
   4e230: e58d2084     	str	r2, [sp, #0x84]
   4e234: e3a02001     	mov	r2, #1
   4e238: e58ce000     	str	lr, [r12]
   4e23c: e58c3004     	str	r3, [r12, #0x4]
   4e240: eb008736     	bl	0x6ff20
   4e244: e59d0080     	ldr	r0, [sp, #0x80]
   4e248: e150000a     	cmp	r0, r10
   4e24c: 0a000000     	beq	0x4e254
   4e250: ebff1efa     	bl	0x15e40    @ imm = #-0x38418 ; _ZdlPv
   4e254: e59d0068     	ldr	r0, [sp, #0x68]
   4e258: e1500008     	cmp	r0, r8
   4e25c: 0a000000     	beq	0x4e264
   4e260: ebff1ef6     	bl	0x15e40    @ imm = #-0x38428 ; _ZdlPv
   4e264: e59d0050     	ldr	r0, [sp, #0x50]
   4e268: e1500009     	cmp	r0, r9
   4e26c: 0a000000     	beq	0x4e274
   4e270: ebff1ef2     	bl	0x15e40    @ imm = #-0x38438 ; _ZdlPv
   4e274: e59d0038     	ldr	r0, [sp, #0x38]
   4e278: e1500006     	cmp	r0, r6
   4e27c: 0a000000     	beq	0x4e284
   4e280: ebff1eee     	bl	0x15e40    @ imm = #-0x38448 ; _ZdlPv
   4e284: e59d0020     	ldr	r0, [sp, #0x20]
   4e288: e1500007     	cmp	r0, r7
   4e28c: 0a000000     	beq	0x4e294
   4e290: ebff1eea     	bl	0x15e40    @ imm = #-0x38458 ; _ZdlPv
   4e294: e59d0008     	ldr	r0, [sp, #0x8]
   4e298: e28d3010     	add	r3, sp, #16
   4e29c: e1500003     	cmp	r0, r3
   4e2a0: 0affff23     	beq	0x4df34
   4e2a4: ebff1ee5     	bl	0x15e40    @ imm = #-0x3846c ; _ZdlPv
   4e2a8: eaffff21     	b	0x4df34
   4e2ac: e3a02000     	mov	r2, #0
   4e2b0: e28d0050     	add	r0, sp, #80
   4e2b4: e1a01002     	mov	r1, r2
   4e2b8: e58dc000     	str	r12, [sp]
   4e2bc: ebff1e0d     	bl	0x15af8    @ imm = #-0x387cc ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_replaceEjjPKcj
   4e2c0: eaffffa6     	b	0x4e160
   4e2c4: e3010b08     	movw	r0, #0x1b08
   4e2c8: e3400007     	movt	r0, #0x7
   4e2cc: ebff1e39     	bl	0x15bb8    @ imm = #-0x3871c ; _ZSt20__throw_length_errorPKc
   4e2d0: e3010b08     	movw	r0, #0x1b08
   4e2d4: e3400007     	movt	r0, #0x7
   4e2d8: ebff1e36     	bl	0x15bb8    @ imm = #-0x38728 ; _ZSt20__throw_length_errorPKc
   4e2dc: e59d0038     	ldr	r0, [sp, #0x38]
   4e2e0: e1500006     	cmp	r0, r6
   4e2e4: 0a000000     	beq	0x4e2ec
   4e2e8: ebff1ed4     	bl	0x15e40    @ imm = #-0x384b0 ; _ZdlPv
   4e2ec: e59d0020     	ldr	r0, [sp, #0x20]
   4e2f0: e1500007     	cmp	r0, r7
   4e2f4: 0a000000     	beq	0x4e2fc
   4e2f8: ebff1ed0     	bl	0x15e40    @ imm = #-0x384c0 ; _ZdlPv
   4e2fc: e59d0008     	ldr	r0, [sp, #0x8]
   4e300: e28d3010     	add	r3, sp, #16
   4e304: e1500003     	cmp	r0, r3
   4e308: 0a000000     	beq	0x4e310
   4e30c: ebff1ecb     	bl	0x15e40    @ imm = #-0x384d4 ; _ZdlPv
   4e310: ebff1f12     	bl	0x15f60    @ imm = #-0x383b8 ; __cxa_end_cleanup
   4e314: eafffff8     	b	0x4e2fc
   4e318: eafffff3     	b	0x4e2ec
   4e31c: e59d0080     	ldr	r0, [sp, #0x80]
   4e320: e150000a     	cmp	r0, r10
   4e324: 0a000000     	beq	0x4e32c
   4e328: ebff1ec4     	bl	0x15e40    @ imm = #-0x384f0 ; _ZdlPv
   4e32c: e59d0068     	ldr	r0, [sp, #0x68]
   4e330: e1500008     	cmp	r0, r8
   4e334: 0a000000     	beq	0x4e33c
   4e338: ebff1ec0     	bl	0x15e40    @ imm = #-0x38500 ; _ZdlPv
   4e33c: e59d0050     	ldr	r0, [sp, #0x50]
   4e340: e1500009     	cmp	r0, r9
   4e344: 0affffe4     	beq	0x4e2dc
   4e348: ebff1ebc     	bl	0x15e40    @ imm = #-0x38510 ; _ZdlPv
   4e34c: eaffffe2     	b	0x4e2dc
   4e350: eafffff5     	b	0x4e32c
   4e354: eafffff8     	b	0x4e33c
