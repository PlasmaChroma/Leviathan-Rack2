; lubadh::Tap::is_fading() const
; VA 0x4bf40 size 44

   4bf40: e1a03000     	mov	r3, r0
   4bf44: e5d0001c     	ldrb	r0, [r0, #0x1c]
   4bf48: e3500000     	cmp	r0, #0
   4bf4c: 112fff1e     	bxne	lr
   4bf50: e5d30034     	ldrb	r0, [r3, #0x34]
   4bf54: e3500000     	cmp	r0, #0
   4bf58: 112fff1e     	bxne	lr
   4bf5c: e5d3004c     	ldrb	r0, [r3, #0x4c]
   4bf60: e3500000     	cmp	r0, #0
   4bf64: 05d30064     	ldrbeq	r0, [r3, #0x64]
   4bf68: e12fff1e     	bx	lr
