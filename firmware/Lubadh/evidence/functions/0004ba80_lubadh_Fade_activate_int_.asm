; lubadh::Fade::activate(int)
; VA 0x4ba80 size 52

   4ba80: e92d4010     	push	{r4, lr}
   4ba84: e1a03000     	mov	r3, r0
   4ba88: e280200c     	add	r2, r0, #12
   4ba8c: e5801004     	str	r1, [r0, #0x4]
   4ba90: e3a04000     	mov	r4, #0
   4ba94: e5804008     	str	r4, [r0, #0x8]
   4ba98: e3a0c5fe     	mov	r12, #1065353216
   4ba9c: e3a0e001     	mov	lr, #1
   4baa0: e1c000d4     	ldrd	r0, r1, [r0, #4]
   4baa4: e5c3e000     	strb	lr, [r3]
   4baa8: e8820003     	stm	r2, {r0, r1}
   4baac: e583c014     	str	r12, [r3, #0x14]
   4bab0: e8bd8010     	pop	{r4, pc}
