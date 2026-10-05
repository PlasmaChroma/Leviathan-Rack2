; lubadh::Button::interpretGPIO(bool)
; VA 0x36640 size 84

   36640: e5903008     	ldr	r3, [r0, #0x8]
   36644: e3510000     	cmp	r1, #0
   36648: 0a000009     	beq	0x36674
   3664c: e3530000     	cmp	r3, #0
   36650: e3a02001     	mov	r2, #1
   36654: 13a03000     	movne	r3, #0
   36658: e5802004     	str	r2, [r0, #0x4]
   3665c: 0a00000a     	beq	0x3668c
   36660: e3a01000     	mov	r1, #0
   36664: e5c0100d     	strb	r1, [r0, #0xd]
   36668: e5c0300e     	strb	r3, [r0, #0xe]
   3666c: e5802008     	str	r2, [r0, #0x8]
   36670: e12fff1e     	bx	lr
   36674: e2433001     	sub	r3, r3, #1
   36678: e1a02001     	mov	r2, r1
   3667c: e16f3f13     	clz	r3, r3
   36680: e5801004     	str	r1, [r0, #0x4]
   36684: e1a032a3     	lsr	r3, r3, #5
   36688: eafffff4     	b	0x36660
   3668c: e5c0200d     	strb	r2, [r0, #0xd]
   36690: eafffff4     	b	0x36668
