; lubadh::Tap::transfer_data(lubadh::Tap const&, lubadh::Tap::FadeType)
; VA 0x4bcf8 size 116

   4bcf8: e92d4070     	push	{r4, r5, r6, lr}
   4bcfc: e3a04018     	mov	r4, #24
   4bd00: e5913018     	ldr	r3, [r1, #0x18]
   4bd04: e1a05000     	mov	r5, r0
   4bd08: e1a06001     	mov	r6, r1
   4bd0c: e0220294     	mla	r2, r4, r2, r0
   4bd10: e24dd018     	sub	sp, sp, #24
   4bd14: e1a0e00d     	mov	lr, sp
   4bd18: e282401c     	add	r4, r2, #28
   4bd1c: e5d12014     	ldrb	r2, [r1, #0x14]
   4bd20: e1a0c004     	mov	r12, r4
   4bd24: e5c02014     	strb	r2, [r0, #0x14]
   4bd28: e5803018     	str	r3, [r0, #0x18]
   4bd2c: e8bc000f     	ldm	r12!, {r0, r1, r2, r3}
   4bd30: e8ae000f     	stm	lr!, {r0, r1, r2, r3}
   4bd34: e3a02060     	mov	r2, #96
   4bd38: e89c0003     	ldm	r12, {r0, r1}
   4bd3c: e88e0003     	stm	lr, {r0, r1}
   4bd40: e286101c     	add	r1, r6, #28
   4bd44: e285001c     	add	r0, r5, #28
   4bd48: ebff2725     	bl	0x159e4    @ imm = #-0x3636c ; memmove
   4bd4c: e1a0e00d     	mov	lr, sp
   4bd50: e1a0c004     	mov	r12, r4
   4bd54: e8be000f     	ldm	lr!, {r0, r1, r2, r3}
   4bd58: e8ac000f     	stm	r12!, {r0, r1, r2, r3}
   4bd5c: e89e0003     	ldm	lr, {r0, r1}
   4bd60: e88c0003     	stm	r12, {r0, r1}
   4bd64: e28dd018     	add	sp, sp, #24
   4bd68: e8bd8070     	pop	{r4, r5, r6, pc}
