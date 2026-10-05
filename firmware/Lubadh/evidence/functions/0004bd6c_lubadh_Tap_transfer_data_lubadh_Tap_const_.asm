; lubadh::Tap::transfer_data(lubadh::Tap const&)
; VA 0x4bd6c size 48

   4bd6c: e1a0c001     	mov	r12, r1
   4bd70: e1a03000     	mov	r3, r0
   4bd74: e52de004     	str	lr, [sp, #-0x4]!
   4bd78: e281101c     	add	r1, r1, #28
   4bd7c: e280001c     	add	r0, r0, #28
   4bd80: e59ce018     	ldr	lr, [r12, #0x18]
   4bd84: e3a02060     	mov	r2, #96
   4bd88: e5dcc014     	ldrb	r12, [r12, #0x14]
   4bd8c: e5c3c014     	strb	r12, [r3, #0x14]
   4bd90: e583e018     	str	lr, [r3, #0x18]
   4bd94: e49de004     	ldr	lr, [sp], #4
   4bd98: eaff2711     	b	0x159e4    @ imm = #-0x363bc
