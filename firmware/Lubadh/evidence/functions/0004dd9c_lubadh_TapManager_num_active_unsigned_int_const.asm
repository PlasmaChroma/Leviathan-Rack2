; lubadh::TapManager::num_active(unsigned int) const
; VA 0x4dd9c size 52

   4dd9c: e3a03faa     	mov	r3, #680
   4dda0: e0010193     	mul	r1, r3, r1
   4dda4: e0803001     	add	r3, r0, r1
   4dda8: e7d00001     	ldrb	r0, [r0, r1]
   4ddac: e5d32084     	ldrb	r2, [r3, #0x84]
   4ddb0: e5d31108     	ldrb	r1, [r3, #0x108]
   4ddb4: e0800002     	add	r0, r0, r2
   4ddb8: e5d3218c     	ldrb	r2, [r3, #0x18c]
   4ddbc: e0811000     	add	r1, r1, r0
   4ddc0: e5d30210     	ldrb	r0, [r3, #0x210]
   4ddc4: e0823001     	add	r3, r2, r1
   4ddc8: e0800003     	add	r0, r0, r3
   4ddcc: e12fff1e     	bx	lr
