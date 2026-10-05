; lubadh::Channel::AudioData::erase()
; VA 0x388d0 size 280

   388d0: e92d4070     	push	{r4, r5, r6, lr}
   388d4: e1a04000     	mov	r4, r0
   388d8: e24dd018     	sub	sp, sp, #24
   388dc: e5943000     	ldr	r3, [r4]
   388e0: e1a0000d     	mov	r0, sp
   388e4: e28d5008     	add	r5, sp, #8
   388e8: e58d5000     	str	r5, [sp]
   388ec: e9930006     	ldmib	r3, {r1, r2}
   388f0: e0812002     	add	r2, r1, r2
   388f4: ebfff97f     	bl	0x36ef8
   388f8: e59d2004     	ldr	r2, [sp, #0x4]
   388fc: e3e03103     	mvn	r3, #-1073741824
   38900: e0433002     	sub	r3, r3, r2
   38904: e3530013     	cmp	r3, #19
   38908: 9a000028     	bls	0x389b0
   3890c: e302137c     	movw	r1, #0x237c
   38910: e3401007     	movt	r1, #0x7
   38914: e3a02014     	mov	r2, #20
   38918: e1a0000d     	mov	r0, sp
   3891c: ebff7559     	bl	0x15e88    @ imm = #-0x22a9c ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   38920: e3090fec     	movw	r0, #0x9fec
   38924: e3400009     	movt	r0, #0x9
   38928: e1a0100d     	mov	r1, sp
   3892c: e3a02000     	mov	r2, #0
   38930: eb00dd7a     	bl	0x6ff20
   38934: e59d0000     	ldr	r0, [sp]
   38938: e1500005     	cmp	r0, r5
   3893c: 0a000000     	beq	0x38944
   38940: ebff753e     	bl	0x15e40    @ imm = #-0x22b08 ; _ZdlPv
   38944: e5942000     	ldr	r2, [r4]
   38948: e3a05001     	mov	r5, #1
   3894c: e5943008     	ldr	r3, [r4, #0x8]
   38950: e3a06000     	mov	r6, #0
   38954: e2840028     	add	r0, r4, #40
   38958: e5c26284     	strb	r6, [r2, #0x284]
   3895c: e5c35000     	strb	r5, [r3]
   38960: eb00dfae     	bl	0x70820
   38964: e5943000     	ldr	r3, [r4]
   38968: eddf0b1a     	vldr	d16, [pc, #104]         @ 0x389d8 ; float 2.12199579146e-314
   3896c: eddf1b1b     	vldr	d17, [pc, #108]         @ 0x389e0 ; float 3.39049992291e-300
   38970: e283c038     	add	r12, r3, #56
   38974: e5932058     	ldr	r2, [r3, #0x58]
   38978: e300099b     	movw	r0, #0x99b
   3897c: e3031004     	movw	r1, #0x3004
   38980: e5826000     	str	r6, [r2]
   38984: e3022a2e     	movw	r2, #0x2a2e
   38988: e34021c2     	movt	r2, #0x1c2
   3898c: f44c0a8f     	vst1.32	{d16, d17}, [r12]
   38990: e5835080     	str	r5, [r3, #0x80]
   38994: e5835088     	str	r5, [r3, #0x88]
   38998: e5830048     	str	r0, [r3, #0x48]
   3899c: e5831084     	str	r1, [r3, #0x84]
   389a0: e583204c     	str	r2, [r3, #0x4c]
   389a4: e5832050     	str	r2, [r3, #0x50]
   389a8: e28dd018     	add	sp, sp, #24
   389ac: e8bd8070     	pop	{r4, r5, r6, pc}
   389b0: e3010b08     	movw	r0, #0x1b08
   389b4: e3400007     	movt	r0, #0x7
   389b8: ebff747e     	bl	0x15bb8    @ imm = #-0x22e08 ; _ZSt20__throw_length_errorPKc
   389bc: e59d0000     	ldr	r0, [sp]
   389c0: e1500005     	cmp	r0, r5
   389c4: 0a000000     	beq	0x389cc
   389c8: ebff751c     	bl	0x15e40    @ imm = #-0x22b90 ; _ZdlPv
   389cc: ebff7563     	bl	0x15f60    @ imm = #-0x22a74 ; __cxa_end_cleanup
   389d0: eafffff9     	b	0x389bc
   389d4: e320f000     	nop
   389d8: 01 00 00 00  	.word	0x00000001
   389dc: 01 00 00 00  	.word	0x00000001
   389e0: 2d 2a c2 01  	.word	0x01c22a2d
   389e4: 2d 2a c2 01  	.word	0x01c22a2d
