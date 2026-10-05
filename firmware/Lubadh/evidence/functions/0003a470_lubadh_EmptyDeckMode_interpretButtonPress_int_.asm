; lubadh::EmptyDeckMode::interpretButtonPress(int)
; VA 0x3a470 size 124

   3a470: e3510001     	cmp	r1, #1
   3a474: 0a000011     	beq	0x3a4c0
   3a478: e3510006     	cmp	r1, #6
   3a47c: 0a00000c     	beq	0x3a4b4
   3a480: e3510000     	cmp	r1, #0
   3a484: 112fff1e     	bxne	lr
   3a488: e5900008     	ldr	r0, [r0, #0x8]
   3a48c: e59030e8     	ldr	r3, [r0, #0xe8]
   3a490: e5933084     	ldr	r3, [r3, #0x84]
   3a494: e3530001     	cmp	r3, #1
   3a498: 1a000006     	bne	0x3a4b8
   3a49c: e5d0227c     	ldrb	r2, [r0, #0x27c]
   3a4a0: e3520000     	cmp	r2, #0
   3a4a4: 1a000003     	bne	0x3a4b8
   3a4a8: e5c0327d     	strb	r3, [r0, #0x27d]
   3a4ac: e5c0327c     	strb	r3, [r0, #0x27c]
   3a4b0: e12fff1e     	bx	lr
   3a4b4: e5900008     	ldr	r0, [r0, #0x8]
   3a4b8: e3a01001     	mov	r1, #1
   3a4bc: eafffed5     	b	0x3a018
   3a4c0: e5903008     	ldr	r3, [r0, #0x8]
   3a4c4: e59320e8     	ldr	r2, [r3, #0xe8]
   3a4c8: e5922084     	ldr	r2, [r2, #0x84]
   3a4cc: e3520001     	cmp	r2, #1
   3a4d0: 112fff1e     	bxne	lr
   3a4d4: e5d3127c     	ldrb	r1, [r3, #0x27c]
   3a4d8: e3510000     	cmp	r1, #0
   3a4dc: 15c3227d     	strbne	r2, [r3, #0x27d]
   3a4e0: 13a02000     	movne	r2, #0
   3a4e4: 15c3227c     	strbne	r2, [r3, #0x27c]
   3a4e8: e12fff1e     	bx	lr
