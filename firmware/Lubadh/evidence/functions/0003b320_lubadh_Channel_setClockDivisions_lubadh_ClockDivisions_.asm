; lubadh::Channel::setClockDivisions(lubadh::ClockDivisions)
; VA 0x3b320 size 280

   3b320: e92d4010     	push	{r4, lr}
   3b324: e1a04000     	mov	r4, r0
   3b328: e24dd048     	sub	sp, sp, #72
   3b32c: e3510003     	cmp	r1, #3
   3b330: 979ff101     	ldrls	pc, [pc, r1, lsl #2]
   3b334: ea00000f     	b	0x3b378
   3b338: 80 b3 03 00  	.word	0x0003b380
   3b33c: b0 b3 03 00  	.word	0x0003b3b0
   3b340: f0 b3 03 00  	.word	0x0003b3f0
   3b344: 48 b3 03 00  	.word	0x0003b348
   3b348: e59fe0d8     	ldr	lr, [pc, #0xd8]         @ 0x3b428
   3b34c: e28dc004     	add	r12, sp, #4
   3b350: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3b354: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3b358: e89e000f     	ldm	lr, {r0, r1, r2, r3}
   3b35c: e88c000f     	stm	r12, {r0, r1, r2, r3}
   3b360: e2840ba9     	add	r0, r4, #173056
   3b364: e28000e8     	add	r0, r0, #232
   3b368: e28d2024     	add	r2, sp, #36
   3b36c: e28d1004     	add	r1, sp, #4
   3b370: e3a03000     	mov	r3, #0
   3b374: eb001383     	bl	0x40188
   3b378: e28dd048     	add	sp, sp, #72
   3b37c: e8bd8010     	pop	{r4, pc}
   3b380: e59f10a4     	ldr	r1, [pc, #0xa4]         @ 0x3b42c
   3b384: e3a02044     	mov	r2, #68
   3b388: e28d0004     	add	r0, sp, #4
   3b38c: ebff6b2c     	bl	0x16044    @ imm = #-0x25350 ; memcpy
   3b390: e2840ba9     	add	r0, r4, #173056
   3b394: e28d1004     	add	r1, sp, #4
   3b398: e28000e8     	add	r0, r0, #232
   3b39c: e28d2048     	add	r2, sp, #72
   3b3a0: e3a03000     	mov	r3, #0
   3b3a4: eb001377     	bl	0x40188
   3b3a8: e28dd048     	add	sp, sp, #72
   3b3ac: e8bd8010     	pop	{r4, pc}
   3b3b0: e59fe078     	ldr	lr, [pc, #0x78]         @ 0x3b430
   3b3b4: e28dc004     	add	r12, sp, #4
   3b3b8: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3b3bc: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3b3c0: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3b3c4: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3b3c8: e3a03000     	mov	r3, #0
   3b3cc: e89e0007     	ldm	lr, {r0, r1, r2}
   3b3d0: e88c0007     	stm	r12, {r0, r1, r2}
   3b3d4: e2840ba9     	add	r0, r4, #173056
   3b3d8: e28000e8     	add	r0, r0, #232
   3b3dc: e28d2030     	add	r2, sp, #48
   3b3e0: e28d1004     	add	r1, sp, #4
   3b3e4: eb001367     	bl	0x40188
   3b3e8: e28dd048     	add	sp, sp, #72
   3b3ec: e8bd8010     	pop	{r4, pc}
   3b3f0: e59fe03c     	ldr	lr, [pc, #0x3c]         @ 0x3b434
   3b3f4: e28dc004     	add	r12, sp, #4
   3b3f8: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   3b3fc: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   3b400: e3a03000     	mov	r3, #0
   3b404: e89e0007     	ldm	lr, {r0, r1, r2}
   3b408: e88c0007     	stm	r12, {r0, r1, r2}
   3b40c: e2840ba9     	add	r0, r4, #173056
   3b410: e28000e8     	add	r0, r0, #232
   3b414: e28d2020     	add	r2, sp, #32
   3b418: e28d1004     	add	r1, sp, #4
   3b41c: eb001359     	bl	0x40188
   3b420: e28dd048     	add	sp, sp, #72
   3b424: e8bd8010     	pop	{r4, pc}
   3b428: d8 2f 07 00  	.word	0x00072fd8
   3b42c: 4c 2f 07 00  	.word	0x00072f4c
   3b430: 90 2f 07 00  	.word	0x00072f90
   3b434: bc 2f 07 00  	.word	0x00072fbc
