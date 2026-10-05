; lubadh::TapEngine::activate(int)
; VA 0x4e358 size 264

   4e358: e92d4070     	push	{r4, r5, r6, lr}
   4e35c: e2803e21     	add	r3, r0, #528
   4e360: e1a05000     	mov	r5, r0
   4e364: e1a04000     	mov	r4, r0
   4e368: e2806fa5     	add	r6, r0, #660
   4e36c: ea00000b     	b	0x4e3a0
   4e370: e5d42084     	ldrb	r2, [r4, #0x84]
   4e374: e3520000     	cmp	r2, #0
   4e378: 0a000029     	beq	0x4e424
   4e37c: e5d42108     	ldrb	r2, [r4, #0x108]
   4e380: e3520000     	cmp	r2, #0
   4e384: 0a000028     	beq	0x4e42c
   4e388: e5d4218c     	ldrb	r2, [r4, #0x18c]
   4e38c: e3520000     	cmp	r2, #0
   4e390: 0a000027     	beq	0x4e434
   4e394: e2844e21     	add	r4, r4, #528
   4e398: e1540003     	cmp	r4, r3
   4e39c: 0a00000f     	beq	0x4e3e0
   4e3a0: e5d42000     	ldrb	r2, [r4]
   4e3a4: e3520000     	cmp	r2, #0
   4e3a8: 1afffff0     	bne	0x4e370
   4e3ac: e1560004     	cmp	r6, r4
   4e3b0: 0a000018     	beq	0x4e418
   4e3b4: e1a00004     	mov	r0, r4
   4e3b8: ebfffed6     	bl	0x4df18
   4e3bc: e2852fa9     	add	r2, r5, #676
   4e3c0: e0422006     	sub	r2, r2, r6
   4e3c4: e2850faa     	add	r0, r5, #680
   4e3c8: e1a01006     	mov	r1, r6
   4e3cc: e0400002     	sub	r0, r0, r2
   4e3d0: ebff1d83     	bl	0x159e4    @ imm = #-0x389f4 ; memmove
   4e3d4: e1a00004     	mov	r0, r4
   4e3d8: e5854294     	str	r4, [r5, #0x294]
   4e3dc: e8bd8070     	pop	{r4, r5, r6, pc}
   4e3e0: e0463004     	sub	r3, r6, r4
   4e3e4: e30823e1     	movw	r2, #0x83e1
   4e3e8: e3432e0f     	movt	r2, #0x3e0f
   4e3ec: e1a03143     	asr	r3, r3, #2
   4e3f0: e0030392     	mul	r3, r2, r3
   4e3f4: e3530002     	cmp	r3, #2
   4e3f8: 0a000013     	beq	0x4e44c
   4e3fc: e3530003     	cmp	r3, #3
   4e400: 0a00000d     	beq	0x4e43c
   4e404: e3530001     	cmp	r3, #1
   4e408: 1a000002     	bne	0x4e418
   4e40c: e5d43000     	ldrb	r3, [r4]
   4e410: e3530000     	cmp	r3, #0
   4e414: 0affffe4     	beq	0x4e3ac
   4e418: e3a04000     	mov	r4, #0
   4e41c: e1a00004     	mov	r0, r4
   4e420: e8bd8070     	pop	{r4, r5, r6, pc}
   4e424: e2844084     	add	r4, r4, #132
   4e428: eaffffdf     	b	0x4e3ac
   4e42c: e2844f42     	add	r4, r4, #264
   4e430: eaffffdd     	b	0x4e3ac
   4e434: e2844f63     	add	r4, r4, #396
   4e438: eaffffdb     	b	0x4e3ac
   4e43c: e5d43000     	ldrb	r3, [r4]
   4e440: e3530000     	cmp	r3, #0
   4e444: 0affffd8     	beq	0x4e3ac
   4e448: e2844084     	add	r4, r4, #132
   4e44c: e5d43000     	ldrb	r3, [r4]
   4e450: e3530000     	cmp	r3, #0
   4e454: 0affffd4     	beq	0x4e3ac
   4e458: e2844084     	add	r4, r4, #132
   4e45c: eaffffea     	b	0x4e40c
