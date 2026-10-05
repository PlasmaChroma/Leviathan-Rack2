; lubadh::Application::checkClkPulse(lubadh::Channel&)
; VA 0x29f44 size 52

   29f44: e281ca2a     	add	r12, r1, #172032
   29f48: e59c34dc     	ldr	r3, [r12, #0x4dc]
   29f4c: e3530000     	cmp	r3, #0
   29f50: d12fff1e     	bxle	lr
   29f54: e2433001     	sub	r3, r3, #1
   29f58: e58c34dc     	str	r3, [r12, #0x4dc]
   29f5c: e3530000     	cmp	r3, #0
   29f60: 112fff1e     	bxne	lr
   29f64: e5912000     	ldr	r2, [r1]
   29f68: e5dc148b     	ldrb	r1, [r12, #0x48b]
   29f6c: e16f2f12     	clz	r2, r2
   29f70: e1a022a2     	lsr	r2, r2, #5
   29f74: ea010858     	b	0x6c0dc
