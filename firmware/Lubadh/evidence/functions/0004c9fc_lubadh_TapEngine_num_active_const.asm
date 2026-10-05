; lubadh::TapEngine::num_active() const
; VA 0x4c9fc size 40

   4c9fc: e5d03084     	ldrb	r3, [r0, #0x84]
   4ca00: e5d01000     	ldrb	r1, [r0]
   4ca04: e5d02108     	ldrb	r2, [r0, #0x108]
   4ca08: e0811003     	add	r1, r1, r3
   4ca0c: e5d0318c     	ldrb	r3, [r0, #0x18c]
   4ca10: e0822001     	add	r2, r2, r1
   4ca14: e5d00210     	ldrb	r0, [r0, #0x210]
   4ca18: e0833002     	add	r3, r3, r2
   4ca1c: e0800003     	add	r0, r0, r3
   4ca20: e12fff1e     	bx	lr
