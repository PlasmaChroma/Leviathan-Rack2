; lubadh::TapEngine::coactive() const
; VA 0x4c9a4 size 32

   4c9a4: e1a03000     	mov	r3, r0
   4c9a8: e5900298     	ldr	r0, [r0, #0x298]
   4c9ac: e3500000     	cmp	r0, #0
   4c9b0: 112fff1e     	bxne	lr
   4c9b4: e5930294     	ldr	r0, [r3, #0x294]
   4c9b8: e3500000     	cmp	r0, #0
   4c9bc: 01a00003     	moveq	r0, r3
   4c9c0: e12fff1e     	bx	lr
