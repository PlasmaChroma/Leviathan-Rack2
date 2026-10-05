; lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch& std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> >::emplace_back<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch>(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch&&)
; VA 0x1bacc size 240

   1bacc: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   1bad0: e1a07000     	mov	r7, r0
   1bad4: e5904004     	ldr	r4, [r0, #0x4]
   1bad8: e5903008     	ldr	r3, [r0, #0x8]
   1badc: e1a05001     	mov	r5, r1
   1bae0: e1540003     	cmp	r4, r3
   1bae4: 0a000005     	beq	0x1bb00
   1bae8: e891000f     	ldm	r1, {r0, r1, r2, r3}
   1baec: e884000f     	stm	r4, {r0, r1, r2, r3}
   1baf0: e2843010     	add	r3, r4, #16
   1baf4: e5873004     	str	r3, [r7, #0x4]
   1baf8: e1a00004     	mov	r0, r4
   1bafc: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   1bb00: e5908000     	ldr	r8, [r0]
   1bb04: e0449008     	sub	r9, r4, r8
   1bb08: e1a06249     	asr	r6, r9, #4
   1bb0c: e376037e     	cmn	r6, #-134217727
   1bb10: 0a000026     	beq	0x1bbb0
   1bb14: e3560000     	cmp	r6, #0
   1bb18: 0a000022     	beq	0x1bba8
   1bb1c: e1560086     	cmp	r6, r6, lsl #1
   1bb20: e1a06086     	lsl	r6, r6, #1
   1bb24: 83e0633e     	mvnhi	r6, #-134217728
   1bb28: 9a000017     	bls	0x1bb8c
   1bb2c: e1a0a206     	lsl	r10, r6, #4
   1bb30: e1a0000a     	mov	r0, r10
   1bb34: ebffe774     	bl	0x1590c     @ imm = #-0x6230 ; _Znwj
   1bb38: e1a06000     	mov	r6, r0
   1bb3c: e0864009     	add	r4, r6, r9
   1bb40: e289c010     	add	r12, r9, #16
   1bb44: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1bb48: e3590000     	cmp	r9, #0
   1bb4c: e086500c     	add	r5, r6, r12
   1bb50: e884000f     	stm	r4, {r0, r1, r2, r3}
   1bb54: ca000005     	bgt	0x1bb70
   1bb58: e3580000     	cmp	r8, #0
   1bb5c: 1a000007     	bne	0x1bb80
   1bb60: e086a00a     	add	r10, r6, r10
   1bb64: e5876000     	str	r6, [r7]
   1bb68: e9870420     	stmib	r7, {r5, r10}
   1bb6c: eaffffe1     	b	0x1baf8
   1bb70: e1a02009     	mov	r2, r9
   1bb74: e1a01008     	mov	r1, r8
   1bb78: e1a00006     	mov	r0, r6
   1bb7c: ebffe798     	bl	0x159e4    @ imm = #-0x61a0 ; memmove
   1bb80: e1a00008     	mov	r0, r8
   1bb84: ebffe8ad     	bl	0x15e40    @ imm = #-0x5d4c ; _ZdlPv
   1bb88: eafffff4     	b	0x1bb60
   1bb8c: e3560000     	cmp	r6, #0
   1bb90: 01a0a006     	moveq	r10, r6
   1bb94: 0affffe8     	beq	0x1bb3c
   1bb98: e3e0333e     	mvn	r3, #-134217728
   1bb9c: e1560003     	cmp	r6, r3
   1bba0: 21a06003     	movhs	r6, r3
   1bba4: eaffffe0     	b	0x1bb2c
   1bba8: e3a06001     	mov	r6, #1
   1bbac: eaffffde     	b	0x1bb2c
   1bbb0: e3000c8c     	movw	r0, #0xc8c
   1bbb4: e3400007     	movt	r0, #0x7
   1bbb8: ebffe7fe     	bl	0x15bb8    @ imm = #-0x6008 ; _ZSt20__throw_length_errorPKc
