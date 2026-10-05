; lubadh::CapTouch::CapTouch(lubadh::Channel const&)
; VA 0x36b58 size 72

   36b58: eddf0b0c     	vldr	d16, [pc, #48]          @ 0x36b90 ; float 1.3647298012e-32
   36b5c: eddf1b0d     	vldr	d17, [pc, #52]          @ 0x36b98 ; float 5.33097580683e-35
   36b60: e280200c     	add	r2, r0, #12
   36b64: e52de004     	str	lr, [sp, #-0x4]!
   36b68: e309c99a     	movw	r12, #0x999a
   36b6c: e343ce99     	movt	r12, #0x3e99
   36b70: e5801000     	str	r1, [r0]
   36b74: e3a0e5fe     	mov	lr, #1065353216
   36b78: e3a01000     	mov	r1, #0
   36b7c: e580e004     	str	lr, [r0, #0x4]
   36b80: e580c01c     	str	r12, [r0, #0x1c]
   36b84: e5801008     	str	r1, [r0, #0x8]
   36b88: f4420a8f     	vst1.32	{d16, d17}, [r2]
   36b8c: e49df004     	ldr	pc, [sp], #4
   36b90: ac c5 a7 b7  	.word	0xb7a7c5ac
   36b94: 17 b7 51 39  	.word	0x3951b717
   36b98: 17 b7 d1 b8  	.word	0xb8d1b717
   36b9c: 17 b7 d1 38  	.word	0x38d1b717
