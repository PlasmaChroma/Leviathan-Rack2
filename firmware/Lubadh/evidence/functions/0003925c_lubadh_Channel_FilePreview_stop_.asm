; lubadh::Channel::FilePreview::stop()
; VA 0x3925c size 200

   3925c: e92d4030     	push	{r4, r5, lr}
   39260: e1a04000     	mov	r4, r0
   39264: e24dd01c     	sub	sp, sp, #28
   39268: e5943000     	ldr	r3, [r4]
   3926c: e1a0000d     	mov	r0, sp
   39270: e28d5008     	add	r5, sp, #8
   39274: e58d5000     	str	r5, [sp]
   39278: e9930006     	ldmib	r3, {r1, r2}
   3927c: e0812002     	add	r2, r1, r2
   39280: ebfff71c     	bl	0x36ef8
   39284: e59d2004     	ldr	r2, [sp, #0x4]
   39288: e3e03103     	mvn	r3, #-1073741824
   3928c: e0433002     	sub	r3, r3, r2
   39290: e3530016     	cmp	r3, #22
   39294: 9a000019     	bls	0x39300
   39298: e3021458     	movw	r1, #0x2458
   3929c: e3401007     	movt	r1, #0x7
   392a0: e3a02017     	mov	r2, #23
   392a4: e1a0000d     	mov	r0, sp
   392a8: ebff72f6     	bl	0x15e88    @ imm = #-0x23428 ; _ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_appendEPKcj
   392ac: e3090fec     	movw	r0, #0x9fec
   392b0: e3400009     	movt	r0, #0x9
   392b4: e1a0100d     	mov	r1, sp
   392b8: e3a02000     	mov	r2, #0
   392bc: eb00db17     	bl	0x6ff20
   392c0: e59d0000     	ldr	r0, [sp]
   392c4: e1500005     	cmp	r0, r5
   392c8: 0a000000     	beq	0x392d0
   392cc: ebff72db     	bl	0x15e40    @ imm = #-0x23494 ; _ZdlPv
   392d0: e5943000     	ldr	r3, [r4]
   392d4: e3a02000     	mov	r2, #0
   392d8: e3a0c000     	mov	r12, #0
   392dc: e5842028     	str	r2, [r4, #0x28]
   392e0: e2831a2a     	add	r1, r3, #172032
   392e4: e1c423b0     	strh	r2, [r4, #48]
   392e8: e5930020     	ldr	r0, [r3, #0x20]
   392ec: e584c02c     	str	r12, [r4, #0x2c]
   392f0: e5d11496     	ldrb	r1, [r1, #0x496]
   392f4: eb00cda6     	bl	0x6c994
   392f8: e28dd01c     	add	sp, sp, #28
   392fc: e8bd8030     	pop	{r4, r5, pc}
   39300: e3010b08     	movw	r0, #0x1b08
   39304: e3400007     	movt	r0, #0x7
   39308: ebff722a     	bl	0x15bb8    @ imm = #-0x23758 ; _ZSt20__throw_length_errorPKc
   3930c: e59d0000     	ldr	r0, [sp]
   39310: e1500005     	cmp	r0, r5
   39314: 0a000000     	beq	0x3931c
   39318: ebff72c8     	bl	0x15e40    @ imm = #-0x234e0 ; _ZdlPv
   3931c: ebff730f     	bl	0x15f60    @ imm = #-0x233c4 ; __cxa_end_cleanup
   39320: eafffff9     	b	0x3930c
