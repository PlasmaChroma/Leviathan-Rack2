; lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch& std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> >::emplace_back<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch>(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch&&)
; VA 0x1a014 size 240

   1a014: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   1a018: e1a07000     	mov	r7, r0
   1a01c: e5904004     	ldr	r4, [r0, #0x4]
   1a020: e5903008     	ldr	r3, [r0, #0x8]
   1a024: e1a05001     	mov	r5, r1
   1a028: e1540003     	cmp	r4, r3
   1a02c: 0a000005     	beq	0x1a048
   1a030: e891000f     	ldm	r1, {r0, r1, r2, r3}
   1a034: e884000f     	stm	r4, {r0, r1, r2, r3}
   1a038: e2843010     	add	r3, r4, #16
   1a03c: e5873004     	str	r3, [r7, #0x4]
   1a040: e1a00004     	mov	r0, r4
   1a044: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   1a048: e5908000     	ldr	r8, [r0]
   1a04c: e0449008     	sub	r9, r4, r8
   1a050: e1a06249     	asr	r6, r9, #4
   1a054: e376037e     	cmn	r6, #-134217727
   1a058: 0a000026     	beq	0x1a0f8
   1a05c: e3560000     	cmp	r6, #0
   1a060: 0a000022     	beq	0x1a0f0
   1a064: e1560086     	cmp	r6, r6, lsl #1
   1a068: e1a06086     	lsl	r6, r6, #1
   1a06c: 83e0633e     	mvnhi	r6, #-134217728
   1a070: 9a000017     	bls	0x1a0d4
   1a074: e1a0a206     	lsl	r10, r6, #4
   1a078: e1a0000a     	mov	r0, r10
   1a07c: ebffee22     	bl	0x1590c     @ imm = #-0x4778 ; _Znwj
   1a080: e1a06000     	mov	r6, r0
   1a084: e0864009     	add	r4, r6, r9
   1a088: e289c010     	add	r12, r9, #16
   1a08c: e895000f     	ldm	r5, {r0, r1, r2, r3}
   1a090: e3590000     	cmp	r9, #0
   1a094: e086500c     	add	r5, r6, r12
   1a098: e884000f     	stm	r4, {r0, r1, r2, r3}
   1a09c: ca000005     	bgt	0x1a0b8
   1a0a0: e3580000     	cmp	r8, #0
   1a0a4: 1a000007     	bne	0x1a0c8
   1a0a8: e086a00a     	add	r10, r6, r10
   1a0ac: e5876000     	str	r6, [r7]
   1a0b0: e9870420     	stmib	r7, {r5, r10}
   1a0b4: eaffffe1     	b	0x1a040
   1a0b8: e1a02009     	mov	r2, r9
   1a0bc: e1a01008     	mov	r1, r8
   1a0c0: e1a00006     	mov	r0, r6
   1a0c4: ebffee46     	bl	0x159e4    @ imm = #-0x46e8 ; memmove
   1a0c8: e1a00008     	mov	r0, r8
   1a0cc: ebffef5b     	bl	0x15e40    @ imm = #-0x4294 ; _ZdlPv
   1a0d0: eafffff4     	b	0x1a0a8
   1a0d4: e3560000     	cmp	r6, #0
   1a0d8: 01a0a006     	moveq	r10, r6
   1a0dc: 0affffe8     	beq	0x1a084
   1a0e0: e3e0333e     	mvn	r3, #-134217728
   1a0e4: e1560003     	cmp	r6, r3
   1a0e8: 21a06003     	movhs	r6, r3
   1a0ec: eaffffe0     	b	0x1a074
   1a0f0: e3a06001     	mov	r6, #1
   1a0f4: eaffffde     	b	0x1a074
   1a0f8: e3000c8c     	movw	r0, #0xc8c
   1a0fc: e3400007     	movt	r0, #0x7
   1a100: ebffeeac     	bl	0x15bb8    @ imm = #-0x4550 ; _ZSt20__throw_length_errorPKc
