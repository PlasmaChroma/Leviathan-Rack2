; lubadh::LubadhHardware::readPin(unsigned int)
; VA 0x6c0c4 size 24

   6c0c4: e6ef0071     	uxtb	r0, r1
   6c0c8: e92d4010     	push	{r4, lr}
   6c0cc: eb000418     	bl	0x6d134
   6c0d0: e16f0f10     	clz	r0, r0
   6c0d4: e1a002a0     	lsr	r0, r0, #5
   6c0d8: e8bd8010     	pop	{r4, pc}
