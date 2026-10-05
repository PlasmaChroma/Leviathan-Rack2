; lubadh::TapEngine::coactive()
; VA 0x4c984 size 32

   4c984: e1a03000     	mov	r3, r0
   4c988: e5900298     	ldr	r0, [r0, #0x298]
   4c98c: e3500000     	cmp	r0, #0
   4c990: 112fff1e     	bxne	lr
   4c994: e5930294     	ldr	r0, [r3, #0x294]
   4c998: e3500000     	cmp	r0, #0
   4c99c: 01a00003     	moveq	r0, r3
   4c9a0: e12fff1e     	bx	lr
