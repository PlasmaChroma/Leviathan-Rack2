; lubadh::Application::triggerClkPulse(lubadh::Channel&, int)
; VA 0x29f18 size 44

   29f18: e5912000     	ldr	r2, [r1]
   29f1c: e92d4010     	push	{r4, lr}
   29f20: e2814a2a     	add	r4, r1, #172032
   29f24: e2422001     	sub	r2, r2, #1
   29f28: e16f2f12     	clz	r2, r2
   29f2c: e5d4148b     	ldrb	r1, [r4, #0x48b]
   29f30: e1a022a2     	lsr	r2, r2, #5
   29f34: eb010868     	bl	0x6c0dc
   29f38: e59434d4     	ldr	r3, [r4, #0x4d4]
   29f3c: e58434dc     	str	r3, [r4, #0x4dc]
   29f40: e8bd8010     	pop	{r4, pc}
