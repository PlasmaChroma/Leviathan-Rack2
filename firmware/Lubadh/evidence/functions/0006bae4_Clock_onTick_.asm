; Clock::onTick()
; VA 0x6bae4 size 116

   6bae4: e59f3064     	ldr	r3, [pc, #0x64]         @ 0x6bb50
   6bae8: e59f2064     	ldr	r2, [pc, #0x64]         @ 0x6bb54
   6baec: e08f3003     	add	r3, pc, r3
   6baf0: e92d4070     	push	{r4, r5, r6, lr}
   6baf4: e1a04000     	mov	r4, r0
   6baf8: e24dd008     	sub	sp, sp, #8
   6bafc: e280601c     	add	r6, r0, #28
   6bb00: e7935002     	ldr	r5, [r3, r2]
   6bb04: e3550000     	cmp	r5, #0
   6bb08: 0a000003     	beq	0x6bb1c
   6bb0c: e1a00006     	mov	r0, r6
   6bb10: ebfea8d9     	bl	0x15e7c    @ imm = #-0x55c9c ; pthread_mutex_lock
   6bb14: e3500000     	cmp	r0, #0
   6bb18: 1a00000b     	bne	0x6bb4c
   6bb1c: e1a0000d     	mov	r0, sp
   6bb20: ebfea953     	bl	0x16074    @ imm = #-0x55ab4 ; _ZNSt6chrono3_V212steady_clock3nowEv
   6bb24: e3550000     	cmp	r5, #0
   6bb28: e1cd20d0     	ldrd	r2, r3, [sp]
   6bb2c: e1c421f0     	strd	r2, r3, [r4, #16]
   6bb30: 0a000003     	beq	0x6bb44
   6bb34: e1a00006     	mov	r0, r6
   6bb38: e28dd008     	add	sp, sp, #8
   6bb3c: e8bd4070     	pop	{r4, r5, r6, lr}
   6bb40: eafea86d     	b	0x15cfc    @ imm = #-0x55e4c
   6bb44: e28dd008     	add	sp, sp, #8
   6bb48: e8bd8070     	pop	{r4, r5, r6, pc}
   6bb4c: ebfea84c     	bl	0x15c84    @ imm = #-0x55ed0 ; _ZSt20__throw_system_errori
   6bb50: 0c 35 02 00  	.word	0x0002350c
   6bb54: 28 04 00 00  	.word	0x00000428
