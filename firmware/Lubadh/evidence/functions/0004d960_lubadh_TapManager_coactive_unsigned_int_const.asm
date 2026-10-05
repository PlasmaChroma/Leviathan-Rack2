; lubadh::TapManager::coactive(unsigned int) const
; VA 0x4d960 size 36

   4d960: e3a03faa     	mov	r3, #680
   4d964: e0210193     	mla	r1, r3, r1, r0
   4d968: e5910298     	ldr	r0, [r1, #0x298]
   4d96c: e3500000     	cmp	r0, #0
   4d970: 112fff1e     	bxne	lr
   4d974: e5910294     	ldr	r0, [r1, #0x294]
   4d978: e3500000     	cmp	r0, #0
   4d97c: 01a00001     	moveq	r0, r1
   4d980: e12fff1e     	bx	lr
