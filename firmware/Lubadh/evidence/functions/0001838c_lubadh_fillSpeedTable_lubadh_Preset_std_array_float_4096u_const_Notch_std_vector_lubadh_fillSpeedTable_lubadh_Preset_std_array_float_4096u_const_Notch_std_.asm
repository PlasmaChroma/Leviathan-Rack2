; lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch& std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> >::emplace_back<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch>(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch&&)
; VA 0x1838c size 240

   1838c: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   18390: e1a07000     	mov	r7, r0
   18394: e5904004     	ldr	r4, [r0, #0x4]
   18398: e5903008     	ldr	r3, [r0, #0x8]
   1839c: e1a05001     	mov	r5, r1
   183a0: e1540003     	cmp	r4, r3
   183a4: 0a000005     	beq	0x183c0
   183a8: e891000f     	ldm	r1, {r0, r1, r2, r3}
   183ac: e884000f     	stm	r4, {r0, r1, r2, r3}
   183b0: e2843010     	add	r3, r4, #16
   183b4: e5873004     	str	r3, [r7, #0x4]
   183b8: e1a00004     	mov	r0, r4
   183bc: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   183c0: e5908000     	ldr	r8, [r0]
   183c4: e0449008     	sub	r9, r4, r8
   183c8: e1a06249     	asr	r6, r9, #4
   183cc: e376037e     	cmn	r6, #-134217727
   183d0: 0a000026     	beq	0x18470
   183d4: e3560000     	cmp	r6, #0
   183d8: 0a000022     	beq	0x18468
   183dc: e1560086     	cmp	r6, r6, lsl #1
   183e0: e1a06086     	lsl	r6, r6, #1
   183e4: 83e0633e     	mvnhi	r6, #-134217728
   183e8: 9a000017     	bls	0x1844c
   183ec: e1a0a206     	lsl	r10, r6, #4
   183f0: e1a0000a     	mov	r0, r10
   183f4: ebfff544     	bl	0x1590c     @ imm = #-0x2af0 ; _Znwj
   183f8: e1a06000     	mov	r6, r0
   183fc: e0864009     	add	r4, r6, r9
   18400: e289c010     	add	r12, r9, #16
   18404: e895000f     	ldm	r5, {r0, r1, r2, r3}
   18408: e3590000     	cmp	r9, #0
   1840c: e086500c     	add	r5, r6, r12
   18410: e884000f     	stm	r4, {r0, r1, r2, r3}
   18414: ca000005     	bgt	0x18430
   18418: e3580000     	cmp	r8, #0
   1841c: 1a000007     	bne	0x18440
   18420: e086a00a     	add	r10, r6, r10
   18424: e5876000     	str	r6, [r7]
   18428: e9870420     	stmib	r7, {r5, r10}
   1842c: eaffffe1     	b	0x183b8
   18430: e1a02009     	mov	r2, r9
   18434: e1a01008     	mov	r1, r8
   18438: e1a00006     	mov	r0, r6
   1843c: ebfff568     	bl	0x159e4    @ imm = #-0x2a60 ; memmove
   18440: e1a00008     	mov	r0, r8
   18444: ebfff67d     	bl	0x15e40    @ imm = #-0x260c ; _ZdlPv
   18448: eafffff4     	b	0x18420
   1844c: e3560000     	cmp	r6, #0
   18450: 01a0a006     	moveq	r10, r6
   18454: 0affffe8     	beq	0x183fc
   18458: e3e0333e     	mvn	r3, #-134217728
   1845c: e1560003     	cmp	r6, r3
   18460: 21a06003     	movhs	r6, r3
   18464: eaffffe0     	b	0x183ec
   18468: e3a06001     	mov	r6, #1
   1846c: eaffffde     	b	0x183ec
   18470: e3000c8c     	movw	r0, #0xc8c
   18474: e3400007     	movt	r0, #0x7
   18478: ebfff5ce     	bl	0x15bb8    @ imm = #-0x28c8 ; _ZSt20__throw_length_errorPKc
