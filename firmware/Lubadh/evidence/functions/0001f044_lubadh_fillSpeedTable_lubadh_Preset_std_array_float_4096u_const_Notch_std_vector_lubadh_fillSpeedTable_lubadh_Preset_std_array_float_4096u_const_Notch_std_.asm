; lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch& std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> >::emplace_back<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch>(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch&&)
; VA 0x1f044 size 240

   1f044: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   1f048: e1a07000     	mov	r7, r0
   1f04c: e5904004     	ldr	r4, [r0, #0x4]
   1f050: e5903008     	ldr	r3, [r0, #0x8]
   1f054: e1a05001     	mov	r5, r1
   1f058: e1540003     	cmp	r4, r3
   1f05c: 0a000005     	beq	0x1f078
   1f060: e891000f     	ldm	r1, {r0, r1, r2, r3}
   1f064: e884000f     	stm	r4, {r0, r1, r2, r3}
   1f068: e2843010     	add	r3, r4, #16
   1f06c: e5873004     	str	r3, [r7, #0x4]
   1f070: e1a00004     	mov	r0, r4
   1f074: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   1f078: e5908000     	ldr	r8, [r0]
   1f07c: e0449008     	sub	r9, r4, r8
   1f080: e1a06249     	asr	r6, r9, #4
   1f084: e376037e     	cmn	r6, #-134217727
   1f088: 0a000026     	beq	0x1f128
   1f08c: e3560000     	cmp	r6, #0
   1f090: 0a000022     	beq	0x1f120
   1f094: e1560086     	cmp	r6, r6, lsl #1
   1f098: e1a06086     	lsl	r6, r6, #1
   1f09c: 83e0633e     	mvnhi	r6, #-134217728
   1f0a0: 9a000017     	bls	0x1f104
   1f0a4: e1a0a206     	lsl	r10, r6, #4
   1f0a8: e1a0000a     	mov	r0, r10
   1f0ac: ebffda16     	bl	0x1590c     @ imm = #-0x97a8 ; _Znwj
   1f0b0: e1a06000     	mov	r6, r0
   1f0b4: e0864009     	add	r4, r6, r9
   1f0b8: e289c010     	add	r12, r9, #16
   1f0bc: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1f0c0: e3590000     	cmp	r9, #0
   1f0c4: e086500c     	add	r5, r6, r12
   1f0c8: e884000f     	stm	r4, {r0, r1, r2, r3}
   1f0cc: ca000005     	bgt	0x1f0e8
   1f0d0: e3580000     	cmp	r8, #0
   1f0d4: 1a000007     	bne	0x1f0f8
   1f0d8: e086a00a     	add	r10, r6, r10
   1f0dc: e5876000     	str	r6, [r7]
   1f0e0: e9870420     	stmib	r7, {r5, r10}
   1f0e4: eaffffe1     	b	0x1f070
   1f0e8: e1a02009     	mov	r2, r9
   1f0ec: e1a01008     	mov	r1, r8
   1f0f0: e1a00006     	mov	r0, r6
   1f0f4: ebffda3a     	bl	0x159e4    @ imm = #-0x9718 ; memmove
   1f0f8: e1a00008     	mov	r0, r8
   1f0fc: ebffdb4f     	bl	0x15e40    @ imm = #-0x92c4 ; _ZdlPv
   1f100: eafffff4     	b	0x1f0d8
   1f104: e3560000     	cmp	r6, #0
   1f108: 01a0a006     	moveq	r10, r6
   1f10c: 0affffe8     	beq	0x1f0b4
   1f110: e3e0333e     	mvn	r3, #-134217728
   1f114: e1560003     	cmp	r6, r3
   1f118: 21a06003     	movhs	r6, r3
   1f11c: eaffffe0     	b	0x1f0a4
   1f120: e3a06001     	mov	r6, #1
   1f124: eaffffde     	b	0x1f0a4
   1f128: e3000c8c     	movw	r0, #0xc8c
   1f12c: e3400007     	movt	r0, #0x7
   1f130: ebffdaa0     	bl	0x15bb8    @ imm = #-0x9580 ; _ZSt20__throw_length_errorPKc
