; lubadh::ADC::hasChanged()
; VA 0x6c13c size 40

   6c13c: e1c020d4     	ldrd	r2, r3, [r0, #4]
   6c140: e5802008     	str	r2, [r0, #0x8]
   6c144: e590100c     	ldr	r1, [r0, #0xc]
   6c148: e0423003     	sub	r3, r2, r3
   6c14c: e3530000     	cmp	r3, #0
   6c150: b2633000     	rsblt	r3, r3, #0
   6c154: e1530001     	cmp	r3, r1
   6c158: d3a00000     	movle	r0, #0
   6c15c: c3a00001     	movgt	r0, #1
   6c160: e12fff1e     	bx	lr
