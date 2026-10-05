; lubadh::TapManager::coactive() const
; VA 0x4d8f4 size 72

   4d8f4: e5902aa0     	ldr	r2, [r0, #0xaa0]
   4d8f8: e1a03000     	mov	r3, r0
   4d8fc: e3520000     	cmp	r2, #0
   4d900: 0a000006     	beq	0x4d920
   4d904: e5920298     	ldr	r0, [r2, #0x298]
   4d908: e3500000     	cmp	r0, #0
   4d90c: 112fff1e     	bxne	lr
   4d910: e5920294     	ldr	r0, [r2, #0x294]
   4d914: e3500000     	cmp	r0, #0
   4d918: 01a00002     	moveq	r0, r2
   4d91c: e12fff1e     	bx	lr
   4d920: e5900298     	ldr	r0, [r0, #0x298]
   4d924: e3500000     	cmp	r0, #0
   4d928: 112fff1e     	bxne	lr
   4d92c: e5930294     	ldr	r0, [r3, #0x294]
   4d930: e3500000     	cmp	r0, #0
   4d934: 01a00003     	moveq	r0, r3
   4d938: e12fff1e     	bx	lr
