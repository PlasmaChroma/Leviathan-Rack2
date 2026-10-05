; lubadh::Fade::Fade()
; VA 0x4ba48 size 56

   4ba48: e52de004     	str	lr, [sp, #-0x4]!
   4ba4c: e1a03000     	mov	r3, r0
   4ba50: e3a0c000     	mov	r12, #0
   4ba54: e280200c     	add	r2, r0, #12
   4ba58: e580c004     	str	r12, [r0, #0x4]
   4ba5c: e3a01000     	mov	r1, #0
   4ba60: e5801008     	str	r1, [r0, #0x8]
   4ba64: e3a0e5fe     	mov	lr, #1065353216
   4ba68: e1c000d4     	ldrd	r0, r1, [r0, #4]
   4ba6c: e5c3c000     	strb	r12, [r3]
   4ba70: e8820003     	stm	r2, {r0, r1}
   4ba74: e1a00003     	mov	r0, r3
   4ba78: e583e014     	str	lr, [r3, #0x14]
   4ba7c: e49df004     	ldr	pc, [sp], #4
