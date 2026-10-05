; lubadh::Tap::resetFade(lubadh::Tap::FadeType)
; VA 0x4bd9c size 68

   4bd9c: e3a03018     	mov	r3, #24
   4bda0: e280201c     	add	r2, r0, #28
   4bda4: e92d4030     	push	{r4, r5, lr}
   4bda8: e3a0e000     	mov	lr, #0
   4bdac: e3a05000     	mov	r5, #0
   4bdb0: e0030193     	mul	r3, r3, r1
   4bdb4: e3a045fe     	mov	r4, #1065353216
   4bdb8: e0822003     	add	r2, r2, r3
   4bdbc: e0803003     	add	r3, r0, r3
   4bdc0: e283c028     	add	r12, r3, #40
   4bdc4: e5c3e01c     	strb	lr, [r3, #0x1c]
   4bdc8: e582e004     	str	lr, [r2, #0x4]
   4bdcc: e5825008     	str	r5, [r2, #0x8]
   4bdd0: e1c302d0     	ldrd	r0, r1, [r3, #32]
   4bdd4: e88c0003     	stm	r12, {r0, r1}
   4bdd8: e5834030     	str	r4, [r3, #0x30]
   4bddc: e8bd8030     	pop	{r4, r5, pc}
