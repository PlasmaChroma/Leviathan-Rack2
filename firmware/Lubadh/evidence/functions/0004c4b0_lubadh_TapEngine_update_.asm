; lubadh::TapEngine::update()
; VA 0x4c4b0 size 440

   4c4b0: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   4c4b4: e2804fa5     	add	r4, r0, #660
   4c4b8: e1a07000     	mov	r7, r0
   4c4bc: e2805faa     	add	r5, r0, #680
   4c4c0: e1a06004     	mov	r6, r4
   4c4c4: e4960004     	ldr	r0, [r6], #4
   4c4c8: e3500000     	cmp	r0, #0
   4c4cc: 0a000000     	beq	0x4c4d4
   4c4d0: ebfffeb2     	bl	0x4bfa0
   4c4d4: e1560005     	cmp	r6, r5
   4c4d8: 1afffff9     	bne	0x4c4c4
   4c4dc: e0452004     	sub	r2, r5, r4
   4c4e0: e1a03242     	asr	r3, r2, #4
   4c4e4: e1a02142     	asr	r2, r2, #2
   4c4e8: e3530000     	cmp	r3, #0
   4c4ec: da000021     	ble	0x4c578
   4c4f0: e0873203     	add	r3, r7, r3, lsl #4
   4c4f4: e2833fa5     	add	r3, r3, #660
   4c4f8: e5942000     	ldr	r2, [r4]
   4c4fc: e2841004     	add	r1, r4, #4
   4c500: e2840008     	add	r0, r4, #8
   4c504: e284c00c     	add	r12, r4, #12
   4c508: e3520000     	cmp	r2, #0
   4c50c: 0a000025     	beq	0x4c5a8
   4c510: e5d22000     	ldrb	r2, [r2]
   4c514: e3520000     	cmp	r2, #0
   4c518: 0a000022     	beq	0x4c5a8
   4c51c: e5942004     	ldr	r2, [r4, #0x4]
   4c520: e3520000     	cmp	r2, #0
   4c524: 0a000034     	beq	0x4c5fc
   4c528: e5d22000     	ldrb	r2, [r2]
   4c52c: e3520000     	cmp	r2, #0
   4c530: 0a000031     	beq	0x4c5fc
   4c534: e5942008     	ldr	r2, [r4, #0x8]
   4c538: e3520000     	cmp	r2, #0
   4c53c: 0a000032     	beq	0x4c60c
   4c540: e5d22000     	ldrb	r2, [r2]
   4c544: e3520000     	cmp	r2, #0
   4c548: 0a00002f     	beq	0x4c60c
   4c54c: e594200c     	ldr	r2, [r4, #0xc]
   4c550: e2844010     	add	r4, r4, #16
   4c554: e3520000     	cmp	r2, #0
   4c558: 0a00002f     	beq	0x4c61c
   4c55c: e5d22000     	ldrb	r2, [r2]
   4c560: e3520000     	cmp	r2, #0
   4c564: 0a00002c     	beq	0x4c61c
   4c568: e1540003     	cmp	r4, r3
   4c56c: 1affffe1     	bne	0x4c4f8
   4c570: e0452004     	sub	r2, r5, r4
   4c574: e1a02142     	asr	r2, r2, #2
   4c578: e3520002     	cmp	r2, #2
   4c57c: 0a000031     	beq	0x4c648
   4c580: e3520003     	cmp	r2, #3
   4c584: 0a000028     	beq	0x4c62c
   4c588: e3520001     	cmp	r2, #1
   4c58c: 18bd81f0     	popne	{r4, r5, r6, r7, r8, pc}
   4c590: e5943000     	ldr	r3, [r4]
   4c594: e3530000     	cmp	r3, #0
   4c598: 0a000002     	beq	0x4c5a8
   4c59c: e5d33000     	ldrb	r3, [r3]
   4c5a0: e3530000     	cmp	r3, #0
   4c5a4: 18bd81f0     	popne	{r4, r5, r6, r7, r8, pc}
   4c5a8: e1550004     	cmp	r5, r4
   4c5ac: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
   4c5b0: e2843004     	add	r3, r4, #4
   4c5b4: e1550003     	cmp	r5, r3
   4c5b8: 0a000009     	beq	0x4c5e4
   4c5bc: e4932004     	ldr	r2, [r3], #4
   4c5c0: e3520000     	cmp	r2, #0
   4c5c4: 0a000002     	beq	0x4c5d4
   4c5c8: e5d21000     	ldrb	r1, [r2]
   4c5cc: e3510000     	cmp	r1, #0
   4c5d0: 14842004     	strne	r2, [r4], #4
   4c5d4: e1550003     	cmp	r5, r3
   4c5d8: 1afffff7     	bne	0x4c5bc
   4c5dc: e1550004     	cmp	r5, r4
   4c5e0: 08bd81f0     	popeq	{r4, r5, r6, r7, r8, pc}
   4c5e4: e0472004     	sub	r2, r7, r4
   4c5e8: e1a00004     	mov	r0, r4
   4c5ec: e2822faa     	add	r2, r2, #680
   4c5f0: e3a01000     	mov	r1, #0
   4c5f4: e8bd41f0     	pop	{r4, r5, r6, r7, r8, lr}
   4c5f8: eaff25dd     	b	0x15d74    @ imm = #-0x3688c
   4c5fc: e1a04001     	mov	r4, r1
   4c600: e1550004     	cmp	r5, r4
   4c604: 1affffe9     	bne	0x4c5b0
   4c608: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   4c60c: e1a04000     	mov	r4, r0
   4c610: e1550004     	cmp	r5, r4
   4c614: 1affffe5     	bne	0x4c5b0
   4c618: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   4c61c: e1a0400c     	mov	r4, r12
   4c620: e1550004     	cmp	r5, r4
   4c624: 1affffe1     	bne	0x4c5b0
   4c628: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
   4c62c: e5943000     	ldr	r3, [r4]
   4c630: e3530000     	cmp	r3, #0
   4c634: 0affffdb     	beq	0x4c5a8
   4c638: e5d33000     	ldrb	r3, [r3]
   4c63c: e3530000     	cmp	r3, #0
   4c640: 0affffd8     	beq	0x4c5a8
   4c644: e2844004     	add	r4, r4, #4
   4c648: e5943000     	ldr	r3, [r4]
   4c64c: e3530000     	cmp	r3, #0
   4c650: 0affffd4     	beq	0x4c5a8
   4c654: e5d33000     	ldrb	r3, [r3]
   4c658: e3530000     	cmp	r3, #0
   4c65c: 0affffd1     	beq	0x4c5a8
   4c660: e2844004     	add	r4, r4, #4
   4c664: eaffffc9     	b	0x4c590
