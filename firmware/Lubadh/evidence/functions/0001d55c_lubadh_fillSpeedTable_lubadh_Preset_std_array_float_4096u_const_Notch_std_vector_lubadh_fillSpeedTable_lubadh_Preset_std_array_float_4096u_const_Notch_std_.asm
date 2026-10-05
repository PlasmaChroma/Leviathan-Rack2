; lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch& std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> >::emplace_back<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch>(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch&&)
; VA 0x1d55c size 240

   1d55c: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   1d560: e1a07000     	mov	r7, r0
   1d564: e5904004     	ldr	r4, [r0, #0x4]
   1d568: e5903008     	ldr	r3, [r0, #0x8]
   1d56c: e1a05001     	mov	r5, r1
   1d570: e1540003     	cmp	r4, r3
   1d574: 0a000005     	beq	0x1d590
   1d578: e891000f     	ldm	r1, {r0, r1, r2, r3}
   1d57c: e884000f     	stm	r4, {r0, r1, r2, r3}
   1d580: e2843010     	add	r3, r4, #16
   1d584: e5873004     	str	r3, [r7, #0x4]
   1d588: e1a00004     	mov	r0, r4
   1d58c: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   1d590: e5908000     	ldr	r8, [r0]
   1d594: e0449008     	sub	r9, r4, r8
   1d598: e1a06249     	asr	r6, r9, #4
   1d59c: e376037e     	cmn	r6, #-134217727
   1d5a0: 0a000026     	beq	0x1d640
   1d5a4: e3560000     	cmp	r6, #0
   1d5a8: 0a000022     	beq	0x1d638
   1d5ac: e1560086     	cmp	r6, r6, lsl #1
   1d5b0: e1a06086     	lsl	r6, r6, #1
   1d5b4: 83e0633e     	mvnhi	r6, #-134217728
   1d5b8: 9a000017     	bls	0x1d61c
   1d5bc: e1a0a206     	lsl	r10, r6, #4
   1d5c0: e1a0000a     	mov	r0, r10
   1d5c4: ebffe0d0     	bl	0x1590c     @ imm = #-0x7cc0 ; _Znwj
   1d5c8: e1a06000     	mov	r6, r0
   1d5cc: e0864009     	add	r4, r6, r9
   1d5d0: e289c010     	add	r12, r9, #16
   1d5d4: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1d5d8: e3590000     	cmp	r9, #0
   1d5dc: e086500c     	add	r5, r6, r12
   1d5e0: e884000f     	stm	r4, {r0, r1, r2, r3}
   1d5e4: ca000005     	bgt	0x1d600
   1d5e8: e3580000     	cmp	r8, #0
   1d5ec: 1a000007     	bne	0x1d610
   1d5f0: e086a00a     	add	r10, r6, r10
   1d5f4: e5876000     	str	r6, [r7]
   1d5f8: e9870420     	stmib	r7, {r5, r10}
   1d5fc: eaffffe1     	b	0x1d588
   1d600: e1a02009     	mov	r2, r9
   1d604: e1a01008     	mov	r1, r8
   1d608: e1a00006     	mov	r0, r6
   1d60c: ebffe0f4     	bl	0x159e4    @ imm = #-0x7c30 ; memmove
   1d610: e1a00008     	mov	r0, r8
   1d614: ebffe209     	bl	0x15e40    @ imm = #-0x77dc ; _ZdlPv
   1d618: eafffff4     	b	0x1d5f0
   1d61c: e3560000     	cmp	r6, #0
   1d620: 01a0a006     	moveq	r10, r6
   1d624: 0affffe8     	beq	0x1d5cc
   1d628: e3e0333e     	mvn	r3, #-134217728
   1d62c: e1560003     	cmp	r6, r3
   1d630: 21a06003     	movhs	r6, r3
   1d634: eaffffe0     	b	0x1d5bc
   1d638: e3a06001     	mov	r6, #1
   1d63c: eaffffde     	b	0x1d5bc
   1d640: e3000c8c     	movw	r0, #0xc8c
   1d644: e3400007     	movt	r0, #0x7
   1d648: ebffe15a     	bl	0x15bb8    @ imm = #-0x7a98 ; _ZSt20__throw_length_errorPKc
