; lubadh::TapManager::active(unsigned int) const
; VA 0x4d894 size 24

   4d894: e3a03faa     	mov	r3, #680
   4d898: e0210193     	mla	r1, r3, r1, r0
   4d89c: e5910294     	ldr	r0, [r1, #0x294]
   4d8a0: e3500000     	cmp	r0, #0
   4d8a4: 01a00001     	moveq	r0, r1
   4d8a8: e12fff1e     	bx	lr
