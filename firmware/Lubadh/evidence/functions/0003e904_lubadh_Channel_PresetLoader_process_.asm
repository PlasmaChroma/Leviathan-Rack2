; lubadh::Channel::PresetLoader::process()
; VA 0x3e904 size 572

   3e904: e5903144     	ldr	r3, [r0, #0x144]
   3e908: e92d4070     	push	{r4, r5, r6, lr}
   3e90c: e1a04000     	mov	r4, r0
   3e910: e5933000     	ldr	r3, [r3]
   3e914: e24dd018     	sub	sp, sp, #24
   3e918: e2432002     	sub	r2, r3, #2
   3e91c: e3520004     	cmp	r2, #4
   3e920: 979ff102     	ldrls	pc, [pc, r2, lsl #2]
   3e924: ea00002d     	b	0x3e9e0
   3e928: ec e9 03 00  	.word	0x0003e9ec
   3e92c: 4c ea 03 00  	.word	0x0003ea4c
   3e930: e0 e9 03 00  	.word	0x0003e9e0
   3e934: 88 ea 03 00  	.word	0x0003ea88
   3e938: 3c e9 03 00  	.word	0x0003e93c
   3e93c: e5902008     	ldr	r2, [r0, #0x8]
   3e940: e304129c     	movw	r1, #0x429c
   3e944: e590302c     	ldr	r3, [r0, #0x2c]
   3e948: e1a0500d     	mov	r5, sp
   3e94c: e5900000     	ldr	r0, [r0]
   3e950: e5922000     	ldr	r2, [r2]
   3e954: e0213291     	mla	r1, r1, r2, r3
   3e958: ebfffd0c     	bl	0x3dd90
   3e95c: e1a00005     	mov	r0, r5
   3e960: e302182c     	movw	r1, #0x282c
   3e964: e3401007     	movt	r1, #0x7
   3e968: ebffe136     	bl	0x36e48
   3e96c: e3090fec     	movw	r0, #0x9fec
   3e970: e3400009     	movt	r0, #0x9
   3e974: e1a01005     	mov	r1, r5
   3e978: e3a02000     	mov	r2, #0
   3e97c: eb00c567     	bl	0x6ff20
   3e980: e59d0000     	ldr	r0, [sp]
   3e984: e28d6008     	add	r6, sp, #8
   3e988: e1500006     	cmp	r0, r6
   3e98c: 0a000000     	beq	0x3e994
   3e990: ebff5d2a     	bl	0x15e40    @ imm = #-0x28b58 ; _ZdlPv
   3e994: e5941000     	ldr	r1, [r4]
   3e998: e1a00005     	mov	r0, r5
   3e99c: e3022840     	movw	r2, #0x2840
   3e9a0: e3402007     	movt	r2, #0x7
   3e9a4: e2811004     	add	r1, r1, #4
   3e9a8: ebffc086     	bl	0x2ebc8
   3e9ac: e3090fec     	movw	r0, #0x9fec
   3e9b0: e3400009     	movt	r0, #0x9
   3e9b4: e1a01005     	mov	r1, r5
   3e9b8: e3a02000     	mov	r2, #0
   3e9bc: eb00c557     	bl	0x6ff20
   3e9c0: e59d0000     	ldr	r0, [sp]
   3e9c4: e1500006     	cmp	r0, r6
   3e9c8: 0a000000     	beq	0x3e9d0
   3e9cc: ebff5d1b     	bl	0x15e40    @ imm = #-0x28b94 ; _ZdlPv
   3e9d0: e1a00004     	mov	r0, r4
   3e9d4: ebffe91c     	bl	0x38e4c
   3e9d8: e5943144     	ldr	r3, [r4, #0x144]
   3e9dc: e5933000     	ldr	r3, [r3]
   3e9e0: e5843164     	str	r3, [r4, #0x164]
   3e9e4: e28dd018     	add	sp, sp, #24
   3e9e8: e8bd8070     	pop	{r4, r5, r6, pc}
   3e9ec: e1a0000d     	mov	r0, sp
   3e9f0: e30217e0     	movw	r1, #0x27e0
   3e9f4: e3401007     	movt	r1, #0x7
   3e9f8: ebffe112     	bl	0x36e48
   3e9fc: e3090fec     	movw	r0, #0x9fec
   3ea00: e3400009     	movt	r0, #0x9
   3ea04: e1a0100d     	mov	r1, sp
   3ea08: e3a02000     	mov	r2, #0
   3ea0c: eb00c543     	bl	0x6ff20
   3ea10: e59d0000     	ldr	r0, [sp]
   3ea14: e28d3008     	add	r3, sp, #8
   3ea18: e1500003     	cmp	r0, r3
   3ea1c: 0a000000     	beq	0x3ea24
   3ea20: ebff5d06     	bl	0x15e40    @ imm = #-0x28be8 ; _ZdlPv
   3ea24: e5943000     	ldr	r3, [r4]
   3ea28: e1a00004     	mov	r0, r4
   3ea2c: eddf0b41     	vldr	d16, [pc, #260]         @ 0x3eb38 ; float 6.36598738204e-313
   3ea30: edc30ba4     	vstr	d16, [r3, #656]
   3ea34: ebffe904     	bl	0x38e4c
   3ea38: e5943144     	ldr	r3, [r4, #0x144]
   3ea3c: e5933000     	ldr	r3, [r3]
   3ea40: e5843164     	str	r3, [r4, #0x164]
   3ea44: e28dd018     	add	sp, sp, #24
   3ea48: e8bd8070     	pop	{r4, r5, r6, pc}
   3ea4c: e5902000     	ldr	r2, [r0]
   3ea50: eef77a00     	vmov.f32	s15, #1.000000e+00
   3ea54: e5901050     	ldr	r1, [r0, #0x50]
   3ea58: e2822ba9     	add	r2, r2, #173056
   3ea5c: e28220cc     	add	r2, r2, #204
   3ea60: edd16a00     	vldr	s13, [r1]
   3ea64: ed927a00     	vldr	s14, [r2]
   3ea68: eef86ae6     	vcvt.f32.s32	s13, s13
   3ea6c: eeba7aca     	vcvt.f32.s32	s14, s14, #12
   3ea70: eee67a87     	vfma.f32	s15, s13, s14
   3ea74: eefd7ae7     	vcvt.s32.f32	s15, s15
   3ea78: edc07a37     	vstr	s15, [r0, #220]
   3ea7c: e5843164     	str	r3, [r4, #0x164]
   3ea80: e28dd018     	add	sp, sp, #24
   3ea84: e8bd8070     	pop	{r4, r5, r6, pc}
   3ea88: e1a0500d     	mov	r5, sp
   3ea8c: e3021800     	movw	r1, #0x2800
   3ea90: e3401007     	movt	r1, #0x7
   3ea94: e1a00005     	mov	r0, r5
   3ea98: ebffe0ea     	bl	0x36e48
   3ea9c: e3090fec     	movw	r0, #0x9fec
   3eaa0: e3400009     	movt	r0, #0x9
   3eaa4: e1a01005     	mov	r1, r5
   3eaa8: e3a02000     	mov	r2, #0
   3eaac: eb00c51b     	bl	0x6ff20
   3eab0: e59d0000     	ldr	r0, [sp]
   3eab4: e28d6008     	add	r6, sp, #8
   3eab8: e1500006     	cmp	r0, r6
   3eabc: 0a000000     	beq	0x3eac4
   3eac0: ebff5cde     	bl	0x15e40    @ imm = #-0x28c88 ; _ZdlPv
   3eac4: e5941000     	ldr	r1, [r4]
   3eac8: e1a00005     	mov	r0, r5
   3eacc: e3022814     	movw	r2, #0x2814
   3ead0: e3402007     	movt	r2, #0x7
   3ead4: e2811004     	add	r1, r1, #4
   3ead8: ebffc03a     	bl	0x2ebc8
   3eadc: e3090fec     	movw	r0, #0x9fec
   3eae0: e3400009     	movt	r0, #0x9
   3eae4: e1a01005     	mov	r1, r5
   3eae8: e3a02002     	mov	r2, #2
   3eaec: eb00c50b     	bl	0x6ff20
   3eaf0: e59d0000     	ldr	r0, [sp]
   3eaf4: e1500006     	cmp	r0, r6
   3eaf8: 1affffc8     	bne	0x3ea20
   3eafc: eaffffc8     	b	0x3ea24
   3eb00: e59d0000     	ldr	r0, [sp]
   3eb04: e28d3008     	add	r3, sp, #8
   3eb08: e1500003     	cmp	r0, r3
   3eb0c: 0a000000     	beq	0x3eb14
   3eb10: ebff5cca     	bl	0x15e40    @ imm = #-0x28cd8 ; _ZdlPv
   3eb14: ebff5d11     	bl	0x15f60    @ imm = #-0x28bbc ; __cxa_end_cleanup
   3eb18: e59d0000     	ldr	r0, [sp]
   3eb1c: e1500006     	cmp	r0, r6
   3eb20: 1afffffa     	bne	0x3eb10
   3eb24: eafffffa     	b	0x3eb14
   3eb28: eafffff4     	b	0x3eb00
   3eb2c: eafffff3     	b	0x3eb00
   3eb30: eafffff8     	b	0x3eb18
   3eb34: e320f000     	nop
   3eb38: b9 00 00 00  	.word	0x000000b9
   3eb3c: 1e 00 00 00  	.word	0x0000001e
