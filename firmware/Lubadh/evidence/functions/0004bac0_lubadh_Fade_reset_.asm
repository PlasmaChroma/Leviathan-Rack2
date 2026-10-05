; lubadh::Fade::reset()
; VA 0x4bac0 size 52

   4bac0: e52de004     	str	lr, [sp, #-0x4]!
   4bac4: e1a03000     	mov	r3, r0
   4bac8: e3a01000     	mov	r1, #0
   4bacc: e3a0c000     	mov	r12, #0
   4bad0: e5801008     	str	r1, [r0, #0x8]
   4bad4: e580c004     	str	r12, [r0, #0x4]
   4bad8: e280200c     	add	r2, r0, #12
   4badc: e3a0e5fe     	mov	lr, #1065353216
   4bae0: e1c000d4     	ldrd	r0, r1, [r0, #4]
   4bae4: e5c3c000     	strb	r12, [r3]
   4bae8: e8820003     	stm	r2, {r0, r1}
   4baec: e583e014     	str	lr, [r3, #0x14]
   4baf0: e49df004     	ldr	pc, [sp], #4
