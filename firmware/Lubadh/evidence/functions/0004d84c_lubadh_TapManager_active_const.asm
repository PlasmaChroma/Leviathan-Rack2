; lubadh::TapManager::active() const
; VA 0x4d84c size 48

   4d84c: e5902aa0     	ldr	r2, [r0, #0xaa0]
   4d850: e1a03000     	mov	r3, r0
   4d854: e3520000     	cmp	r2, #0
   4d858: 0a000003     	beq	0x4d86c
   4d85c: e5920294     	ldr	r0, [r2, #0x294]
   4d860: e3500000     	cmp	r0, #0
   4d864: 01a00002     	moveq	r0, r2
   4d868: e12fff1e     	bx	lr
   4d86c: e5900294     	ldr	r0, [r0, #0x294]
   4d870: e3500000     	cmp	r0, #0
   4d874: 01a00003     	moveq	r0, r3
   4d878: e12fff1e     	bx	lr
