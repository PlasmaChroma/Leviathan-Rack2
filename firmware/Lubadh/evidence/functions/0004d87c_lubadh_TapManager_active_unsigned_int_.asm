; lubadh::TapManager::active(unsigned int)
; VA 0x4d87c size 24

   4d87c: e3a03faa     	mov	r3, #680
   4d880: e0210193     	mla	r1, r3, r1, r0
   4d884: e5910294     	ldr	r0, [r1, #0x294]
   4d888: e3500000     	cmp	r0, #0
   4d88c: 01a00001     	moveq	r0, r1
   4d890: e12fff1e     	bx	lr
