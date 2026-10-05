; lubadh::TapEngine::is_active() const
; VA 0x4c9c4 size 56

   4c9c4: e1a03000     	mov	r3, r0
   4c9c8: e5d00000     	ldrb	r0, [r0]
   4c9cc: e3500000     	cmp	r0, #0
   4c9d0: 112fff1e     	bxne	lr
   4c9d4: e5d30084     	ldrb	r0, [r3, #0x84]
   4c9d8: e3500000     	cmp	r0, #0
   4c9dc: 112fff1e     	bxne	lr
   4c9e0: e5d30108     	ldrb	r0, [r3, #0x108]
   4c9e4: e3500000     	cmp	r0, #0
   4c9e8: 112fff1e     	bxne	lr
   4c9ec: e5d3018c     	ldrb	r0, [r3, #0x18c]
   4c9f0: e3500000     	cmp	r0, #0
   4c9f4: 05d30210     	ldrbeq	r0, [r3, #0x210]
   4c9f8: e12fff1e     	bx	lr
