; lubadh::Channel::FileLoader::active() const
; VA 0x39204 size 28

   39204: e5903008     	ldr	r3, [r0, #0x8]
   39208: e5930000     	ldr	r0, [r3]
   3920c: e3500000     	cmp	r0, #0
   39210: 012fff1e     	bxeq	lr
   39214: e2500006     	subs	r0, r0, #6
   39218: 13a00001     	movne	r0, #1
   3921c: e12fff1e     	bx	lr
