; lubadh::TapManager::newest() const
; VA 0x4ddf8 size 52

   4ddf8: e5902aa0     	ldr	r2, [r0, #0xaa0]
   4ddfc: e3520000     	cmp	r2, #0
   4de00: 0a000004     	beq	0x4de18
   4de04: e5923294     	ldr	r3, [r2, #0x294]
   4de08: e3530000     	cmp	r3, #0
   4de0c: 01a03002     	moveq	r3, r2
   4de10: e593007c     	ldr	r0, [r3, #0x7c]
   4de14: e12fff1e     	bx	lr
   4de18: e5903294     	ldr	r3, [r0, #0x294]
   4de1c: e3530000     	cmp	r3, #0
   4de20: 01a03000     	moveq	r3, r0
   4de24: e593007c     	ldr	r0, [r3, #0x7c]
   4de28: e12fff1e     	bx	lr
