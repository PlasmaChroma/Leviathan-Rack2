; lubadh::LubadhHardware::serialWrite(unsigned char, unsigned char)
; VA 0x6c994 size 384

   6c994: e92d4070     	push	{r4, r5, r6, lr}
   6c998: e1a0c001     	mov	r12, r1
   6c99c: e1a03002     	mov	r3, r2
   6c9a0: e24dd010     	sub	sp, sp, #16
   6c9a4: e590003c     	ldr	r0, [r0, #0x3c]
   6c9a8: e28d1004     	add	r1, sp, #4
   6c9ac: e3a02002     	mov	r2, #2
   6c9b0: e5cdc005     	strb	r12, [sp, #0x5]
   6c9b4: e5cd3004     	strb	r3, [sp, #0x4]
   6c9b8: ebfea523     	bl	0x15e4c    @ imm = #-0x56b74 ; write
   6c9bc: e3700001     	cmn	r0, #1
   6c9c0: 13a04000     	movne	r4, #0
   6c9c4: 0a000002     	beq	0x6c9d4
   6c9c8: e1a00004     	mov	r0, r4
   6c9cc: e28dd010     	add	sp, sp, #16
   6c9d0: e8bd8070     	pop	{r4, r5, r6, pc}
   6c9d4: e1a04000     	mov	r4, r0
   6c9d8: e3090fbc     	movw	r0, #0x9fbc
   6c9dc: e3400009     	movt	r0, #0x9
   6c9e0: ebfea5a9     	bl	0x1608c    @ imm = #-0x5695c ; localtime
   6c9e4: e3053264     	movw	r3, #0x5264
   6c9e8: e3403007     	movt	r3, #0x7
   6c9ec: e58d300c     	str	r3, [sp, #0xc]
   6c9f0: e28d3010     	add	r3, sp, #16
   6c9f4: e58d0008     	str	r0, [sp, #0x8]
   6c9f8: e30f06d0     	movw	r0, #0xf6d0
   6c9fc: e3400008     	movt	r0, #0x8
   6ca00: e9130006     	ldmdb	r3, {r1, r2}
   6ca04: eb000044     	bl	0x6cb1c
   6ca08: e3a02009     	mov	r2, #9
   6ca0c: e1a05000     	mov	r5, r0
   6ca10: e305127c     	movw	r1, #0x527c
   6ca14: e3401007     	movt	r1, #0x7
   6ca18: ebfea592     	bl	0x16068    @ imm = #-0x569b8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6ca1c: e59f10ec     	ldr	r1, [pc, #0xec]         @ 0x6cb10
   6ca20: e1a00005     	mov	r0, r5
   6ca24: e3a0200c     	mov	r2, #12
   6ca28: ebfea58e     	bl	0x16068    @ imm = #-0x569c8 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6ca2c: e1a00005     	mov	r0, r5
   6ca30: e3a02001     	mov	r2, #1
   6ca34: e30512c4     	movw	r1, #0x52c4
   6ca38: e3401007     	movt	r1, #0x7
   6ca3c: ebfea589     	bl	0x16068    @ imm = #-0x569dc ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6ca40: e1a00005     	mov	r0, r5
   6ca44: e3a0200b     	mov	r2, #11
   6ca48: e3051394     	movw	r1, #0x5394
   6ca4c: e3401007     	movt	r1, #0x7
   6ca50: ebfea584     	bl	0x16068    @ imm = #-0x569f0 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6ca54: e3a02001     	mov	r2, #1
   6ca58: e1a00005     	mov	r0, r5
   6ca5c: e30512d0     	movw	r1, #0x52d0
   6ca60: e3401007     	movt	r1, #0x7
   6ca64: ebfea57f     	bl	0x16068    @ imm = #-0x56a04 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6ca68: e1a00005     	mov	r0, r5
   6ca6c: e3a0109c     	mov	r1, #156
   6ca70: ebfea633     	bl	0x16344    @ imm = #-0x56734 ; _ZNSolsEi
   6ca74: e1a05000     	mov	r5, r0
   6ca78: e3a02003     	mov	r2, #3
   6ca7c: e30512d4     	movw	r1, #0x52d4
   6ca80: e3401007     	movt	r1, #0x7
   6ca84: ebfea577     	bl	0x16068    @ imm = #-0x56a24 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6ca88: e1a00005     	mov	r0, r5
   6ca8c: e3a0201b     	mov	r2, #27
   6ca90: e30513a0     	movw	r1, #0x53a0
   6ca94: e3401007     	movt	r1, #0x7
   6ca98: ebfea572     	bl	0x16068    @ imm = #-0x56a38 ; _ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_i
   6ca9c: e5953000     	ldr	r3, [r5]
   6caa0: e513300c     	ldr	r3, [r3, #-0xc]
   6caa4: e0853003     	add	r3, r5, r3
   6caa8: e593607c     	ldr	r6, [r3, #0x7c]
   6caac: e3560000     	cmp	r6, #0
   6cab0: 0a000015     	beq	0x6cb0c
   6cab4: e5d6301c     	ldrb	r3, [r6, #0x1c]
   6cab8: e3530000     	cmp	r3, #0
   6cabc: 15d61027     	ldrbne	r1, [r6, #0x27]
   6cac0: 1a000008     	bne	0x6cae8
   6cac4: e1a00006     	mov	r0, r6
   6cac8: ebfea52d     	bl	0x15f84    @ imm = #-0x56b4c ; _ZNKSt5ctypeIcE13_M_widen_initEv
   6cacc: e5962000     	ldr	r2, [r6]
   6cad0: e30c3b14     	movw	r3, #0xcb14
   6cad4: e3403006     	movt	r3, #0x6
   6cad8: e5922018     	ldr	r2, [r2, #0x18]
   6cadc: e1520003     	cmp	r2, r3
   6cae0: 03a0100a     	moveq	r1, #10
   6cae4: 1a000003     	bne	0x6caf8
   6cae8: e1a00005     	mov	r0, r5
   6caec: ebfea37d     	bl	0x158e8     @ imm = #-0x5720c ; _ZNSo3putEc
   6caf0: ebfea46f     	bl	0x15cb4    @ imm = #-0x56e44 ; _ZNSo5flushEv
   6caf4: eaffffb3     	b	0x6c9c8
   6caf8: e3a0100a     	mov	r1, #10
   6cafc: e1a00006     	mov	r0, r6
   6cb00: e12fff32     	blx	r2
   6cb04: e1a01000     	mov	r1, r0
   6cb08: eafffff6     	b	0x6cae8
   6cb0c: ebfea549     	bl	0x16038    @ imm = #-0x56adc ; _ZSt16__throw_bad_castv
   6cb10: b6 52 07 00  	.word	0x000752b6
