; lubadh::Channel::FileSaver::active() const
; VA 0x39388 size 40

   39388: e5903048     	ldr	r3, [r0, #0x48]
   3938c: e5933000     	ldr	r3, [r3]
   39390: e3530001     	cmp	r3, #1
   39394: 159030ac     	ldrne	r3, [r0, #0xac]
   39398: 15930000     	ldrne	r0, [r3]
   3939c: 12400001     	subne	r0, r0, #1
   393a0: 116f0f10     	clzne	r0, r0
   393a4: 11a002a0     	lsrne	r0, r0, #5
   393a8: 01a00003     	moveq	r0, r3
   393ac: e12fff1e     	bx	lr
