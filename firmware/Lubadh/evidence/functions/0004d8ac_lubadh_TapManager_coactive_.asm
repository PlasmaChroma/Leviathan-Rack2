; lubadh::TapManager::coactive()
; VA 0x4d8ac size 72

   4d8ac: e5902aa0     	ldr	r2, [r0, #0xaa0]
   4d8b0: e1a03000     	mov	r3, r0
   4d8b4: e3520000     	cmp	r2, #0
   4d8b8: 0a000006     	beq	0x4d8d8
   4d8bc: e5920298     	ldr	r0, [r2, #0x298]
   4d8c0: e3500000     	cmp	r0, #0
   4d8c4: 112fff1e     	bxne	lr
   4d8c8: e5920294     	ldr	r0, [r2, #0x294]
   4d8cc: e3500000     	cmp	r0, #0
   4d8d0: 01a00002     	moveq	r0, r2
   4d8d4: e12fff1e     	bx	lr
   4d8d8: e5900298     	ldr	r0, [r0, #0x298]
   4d8dc: e3500000     	cmp	r0, #0
   4d8e0: 112fff1e     	bxne	lr
   4d8e4: e5930294     	ldr	r0, [r3, #0x294]
   4d8e8: e3500000     	cmp	r0, #0
   4d8ec: 01a00003     	moveq	r0, r3
   4d8f0: e12fff1e     	bx	lr
