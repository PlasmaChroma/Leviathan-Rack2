; lubadh::Channel::AudioData::AudioData(lubadh::Channel&)
; VA 0x3cb40 size 516

   3cb40: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   3cb44: e1a05000     	mov	r5, r0
   3cb48: e1a04000     	mov	r4, r0
   3cb4c: e24dd018     	sub	sp, sp, #24
   3cb50: e1a03001     	mov	r3, r1
   3cb54: e2812004     	add	r2, r1, #4
   3cb58: e1a0000d     	mov	r0, sp
   3cb5c: e59f11c4     	ldr	r1, [pc, #0x1c4]        @ 0x3cd28
   3cb60: e2846014     	add	r6, r4, #20
   3cb64: e4853004     	str	r3, [r5], #4
   3cb68: ebffc803     	bl	0x2eb7c
   3cb6c: e59f01b8     	ldr	r0, [pc, #0x1b8]        @ 0x3cd2c
   3cb70: e89d0006     	ldm	sp, {r1, r2}
   3cb74: e3a03000     	mov	r3, #0
   3cb78: e5840004     	str	r0, [r4, #0x4]
   3cb7c: e284000c     	add	r0, r4, #12
   3cb80: e0812002     	add	r2, r1, r2
   3cb84: e5843008     	str	r3, [r4, #0x8]
   3cb88: e584600c     	str	r6, [r4, #0xc]
   3cb8c: ebffe8d9     	bl	0x36ef8
   3cb90: e3e03000     	mvn	r3, #0
   3cb94: e1a00005     	mov	r0, r5
   3cb98: e5843024     	str	r3, [r4, #0x24]
   3cb9c: eb0014b5     	bl	0x41e78
   3cba0: e1a00005     	mov	r0, r5
   3cba4: eb0015fb     	bl	0x42398
   3cba8: e1a00005     	mov	r0, r5
   3cbac: eb0016cc     	bl	0x426e4
   3cbb0: e59d0000     	ldr	r0, [sp]
   3cbb4: e28d6008     	add	r6, sp, #8
   3cbb8: e59f9170     	ldr	r9, [pc, #0x170]        @ 0x3cd30
   3cbbc: e1500006     	cmp	r0, r6
   3cbc0: e5849004     	str	r9, [r4, #0x4]
   3cbc4: 0a000000     	beq	0x3cbcc
   3cbc8: ebff649c     	bl	0x15e40    @ imm = #-0x26d90 ; _ZdlPv
   3cbcc: e1a07004     	mov	r7, r4
   3cbd0: e59f115c     	ldr	r1, [pc, #0x15c]        @ 0x3cd34
   3cbd4: e1a0000d     	mov	r0, sp
   3cbd8: e4972028     	ldr	r2, [r7], #40
   3cbdc: e2822004     	add	r2, r2, #4
   3cbe0: ebffc7e5     	bl	0x2eb7c
   3cbe4: e3a02000     	mov	r2, #0
   3cbe8: e1a0100d     	mov	r1, sp
   3cbec: e1a00007     	mov	r0, r7
   3cbf0: eb00ccfc     	bl	0x6ffe8
   3cbf4: e59d0000     	ldr	r0, [sp]
   3cbf8: e1500006     	cmp	r0, r6
   3cbfc: 0a000000     	beq	0x3cc04
   3cc00: ebff648e     	bl	0x15e40    @ imm = #-0x26dc8 ; _ZdlPv
   3cc04: e5942000     	ldr	r2, [r4]
   3cc08: e1a0000d     	mov	r0, sp
   3cc0c: e59f1124     	ldr	r1, [pc, #0x124]        @ 0x3cd38
   3cc10: e2822004     	add	r2, r2, #4
   3cc14: ebffc7d8     	bl	0x2eb7c
   3cc18: e59d1000     	ldr	r1, [sp]
   3cc1c: e3a03000     	mov	r3, #0
   3cc20: e59d2004     	ldr	r2, [sp, #0x4]
   3cc24: e284a054     	add	r10, r4, #84
   3cc28: e59f010c     	ldr	r0, [pc, #0x10c]        @ 0x3cd3c
   3cc2c: e2848044     	add	r8, r4, #68
   3cc30: e5840044     	str	r0, [r4, #0x44]
   3cc34: e0812002     	add	r2, r1, r2
   3cc38: e284004c     	add	r0, r4, #76
   3cc3c: e5843048     	str	r3, [r4, #0x48]
   3cc40: e584a04c     	str	r10, [r4, #0x4c]
   3cc44: ebffe8ab     	bl	0x36ef8
   3cc48: e3e03000     	mvn	r3, #0
   3cc4c: e1a00008     	mov	r0, r8
   3cc50: e5843064     	str	r3, [r4, #0x64]
   3cc54: eb001733     	bl	0x42928
   3cc58: e1a00008     	mov	r0, r8
   3cc5c: eb00187d     	bl	0x42e58
   3cc60: e1a00008     	mov	r0, r8
   3cc64: eb001950     	bl	0x431ac
   3cc68: e59d0000     	ldr	r0, [sp]
   3cc6c: e59fa0cc     	ldr	r10, [pc, #0xcc]        @ 0x3cd40
   3cc70: e1500006     	cmp	r0, r6
   3cc74: e584a044     	str	r10, [r4, #0x44]
   3cc78: 0a000000     	beq	0x3cc80
   3cc7c: ebff646f     	bl	0x15e40    @ imm = #-0x26e44 ; _ZdlPv
   3cc80: e1a00004     	mov	r0, r4
   3cc84: ebffef11     	bl	0x388d0
   3cc88: e1a00004     	mov	r0, r4
   3cc8c: e28dd018     	add	sp, sp, #24
   3cc90: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   3cc94: e59d0000     	ldr	r0, [sp]
   3cc98: e28d3008     	add	r3, sp, #8
   3cc9c: e1500003     	cmp	r0, r3
   3cca0: 0a000009     	beq	0x3cccc
   3cca4: ebff6465     	bl	0x15e40    @ imm = #-0x26e6c ; _ZdlPv
   3cca8: ea000007     	b	0x3cccc
   3ccac: e1a00008     	mov	r0, r8
   3ccb0: e584a044     	str	r10, [r4, #0x44]
   3ccb4: ebffdf74     	bl	0x34a8c
   3ccb8: e1a00007     	mov	r0, r7
   3ccbc: eb00cdf3     	bl	0x70490
   3ccc0: e1a00005     	mov	r0, r5
   3ccc4: e5849004     	str	r9, [r4, #0x4]
   3ccc8: ebffddca     	bl	0x343f8
   3cccc: ebff64a3     	bl	0x15f60    @ imm = #-0x26d74 ; __cxa_end_cleanup
   3ccd0: e594004c     	ldr	r0, [r4, #0x4c]
   3ccd4: e15a0000     	cmp	r10, r0
   3ccd8: 0a000000     	beq	0x3cce0
   3ccdc: ebff6457     	bl	0x15e40    @ imm = #-0x26ea4 ; _ZdlPv
   3cce0: e59d0000     	ldr	r0, [sp]
   3cce4: e1500006     	cmp	r0, r6
   3cce8: 0afffff2     	beq	0x3ccb8
   3ccec: ebff6453     	bl	0x15e40    @ imm = #-0x26eb4 ; _ZdlPv
   3ccf0: eafffff0     	b	0x3ccb8
   3ccf4: eafffff9     	b	0x3cce0
   3ccf8: eaffffee     	b	0x3ccb8
   3ccfc: e59d0000     	ldr	r0, [sp]
   3cd00: e1500006     	cmp	r0, r6
   3cd04: 0affffed     	beq	0x3ccc0
   3cd08: ebff644c     	bl	0x15e40    @ imm = #-0x26ed0 ; _ZdlPv
   3cd0c: eaffffeb     	b	0x3ccc0
   3cd10: eaffffea     	b	0x3ccc0
   3cd14: e594000c     	ldr	r0, [r4, #0xc]
   3cd18: e1560000     	cmp	r6, r0
   3cd1c: 0affffdc     	beq	0x3cc94
   3cd20: ebff6446     	bl	0x15e40    @ imm = #-0x26ee8 ; _ZdlPv
   3cd24: eaffffda     	b	0x3cc94
   3cd28: 60 50 09 00  	.word	0x00095060
   3cd2c: 00 1f 07 00  	.word	0x00071f00
   3cd30: a4 2e 07 00  	.word	0x00072ea4
   3cd34: 78 50 09 00  	.word	0x00095078
   3cd38: 90 50 09 00  	.word	0x00095090
   3cd3c: 10 1f 07 00  	.word	0x00071f10
   3cd40: b4 2e 07 00  	.word	0x00072eb4
