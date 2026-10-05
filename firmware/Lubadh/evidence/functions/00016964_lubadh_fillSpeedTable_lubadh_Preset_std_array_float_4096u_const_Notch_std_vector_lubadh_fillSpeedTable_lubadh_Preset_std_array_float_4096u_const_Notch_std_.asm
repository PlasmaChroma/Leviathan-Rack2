; lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch& std::vector<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch, std::allocator<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch> >::emplace_back<lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch>(lubadh::fillSpeedTable(lubadh::Preset&, std::array<float, 4096u> const&)::Notch&&)
; VA 0x16964 size 240

   16964: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
   16968: e1a07000     	mov	r7, r0
   1696c: e5904004     	ldr	r4, [r0, #0x4]
   16970: e5903008     	ldr	r3, [r0, #0x8]
   16974: e1a05001     	mov	r5, r1
   16978: e1540003     	cmp	r4, r3
   1697c: 0a000005     	beq	0x16998
   16980: e891000f     	ldm	r1, {r0, r1, r2, r3}
   16984: e884000f     	stm	r4, {r0, r1, r2, r3}
   16988: e2843010     	add	r3, r4, #16
   1698c: e5873004     	str	r3, [r7, #0x4]
   16990: e1a00004     	mov	r0, r4
   16994: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
   16998: e5908000     	ldr	r8, [r0]
   1699c: e0449008     	sub	r9, r4, r8
   169a0: e1a06249     	asr	r6, r9, #4
   169a4: e376037e     	cmn	r6, #-134217727
   169a8: 0a000026     	beq	0x16a48
   169ac: e3560000     	cmp	r6, #0
   169b0: 0a000022     	beq	0x16a40
   169b4: e1560086     	cmp	r6, r6, lsl #1
   169b8: e1a06086     	lsl	r6, r6, #1
   169bc: 83e0633e     	mvnhi	r6, #-134217728
   169c0: 9a000017     	bls	0x16a24
   169c4: e1a0a206     	lsl	r10, r6, #4
   169c8: e1a0000a     	mov	r0, r10
   169cc: ebfffbce     	bl	0x1590c     @ imm = #-0x10c8 ; _Znwj
   169d0: e1a06000     	mov	r6, r0
   169d4: e0864009     	add	r4, r6, r9
   169d8: e289c010     	add	r12, r9, #16
   169dc: e895000f     	ldm	r5, {r0, r1, r2, r3}
   169e0: e3590000     	cmp	r9, #0
   169e4: e086500c     	add	r5, r6, r12
   169e8: e884000f     	stm	r4, {r0, r1, r2, r3}
   169ec: ca000005     	bgt	0x16a08
   169f0: e3580000     	cmp	r8, #0
   169f4: 1a000007     	bne	0x16a18
   169f8: e086a00a     	add	r10, r6, r10
   169fc: e5876000     	str	r6, [r7]
   16a00: e9870420     	stmib	r7, {r5, r10}
   16a04: eaffffe1     	b	0x16990
   16a08: e1a02009     	mov	r2, r9
   16a0c: e1a01008     	mov	r1, r8
   16a10: e1a00006     	mov	r0, r6
   16a14: ebfffbf2     	bl	0x159e4    @ imm = #-0x1038 ; memmove
   16a18: e1a00008     	mov	r0, r8
   16a1c: ebfffd07     	bl	0x15e40    @ imm = #-0xbe4 ; _ZdlPv
   16a20: eafffff4     	b	0x169f8
   16a24: e3560000     	cmp	r6, #0
   16a28: 01a0a006     	moveq	r10, r6
   16a2c: 0affffe8     	beq	0x169d4
   16a30: e3e0333e     	mvn	r3, #-134217728
   16a34: e1560003     	cmp	r6, r3
   16a38: 21a06003     	movhs	r6, r3
   16a3c: eaffffe0     	b	0x169c4
   16a40: e3a06001     	mov	r6, #1
   16a44: eaffffde     	b	0x169c4
   16a48: e3000c8c     	movw	r0, #0xc8c
   16a4c: e3400007     	movt	r0, #0x7
   16a50: ebfffc58     	bl	0x15bb8    @ imm = #-0xea0 ; _ZSt20__throw_length_errorPKc
