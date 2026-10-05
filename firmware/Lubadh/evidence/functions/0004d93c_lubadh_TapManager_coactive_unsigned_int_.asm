; lubadh::TapManager::coactive(unsigned int)
; VA 0x4d93c size 36

   4d93c: e3a03faa     	mov	r3, #680
   4d940: e0210193     	mla	r1, r3, r1, r0
   4d944: e5910298     	ldr	r0, [r1, #0x298]
   4d948: e3500000     	cmp	r0, #0
   4d94c: 112fff1e     	bxne	lr
   4d950: e5910294     	ldr	r0, [r1, #0x294]
   4d954: e3500000     	cmp	r0, #0
   4d958: 01a00001     	moveq	r0, r1
   4d95c: e12fff1e     	bx	lr
