; lubadh::TapManager::active()
; VA 0x4d81c size 48

   4d81c: e5902aa0     	ldr	r2, [r0, #0xaa0]
   4d820: e1a03000     	mov	r3, r0
   4d824: e3520000     	cmp	r2, #0
   4d828: 0a000003     	beq	0x4d83c
   4d82c: e5920294     	ldr	r0, [r2, #0x294]
   4d830: e3500000     	cmp	r0, #0
   4d834: 01a00002     	moveq	r0, r2
   4d838: e12fff1e     	bx	lr
   4d83c: e5900294     	ldr	r0, [r0, #0x294]
   4d840: e3500000     	cmp	r0, #0
   4d844: 01a00003     	moveq	r0, r3
   4d848: e12fff1e     	bx	lr
