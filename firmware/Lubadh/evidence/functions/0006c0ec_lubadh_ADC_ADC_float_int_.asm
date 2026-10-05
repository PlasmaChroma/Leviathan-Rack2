; lubadh::ADC::ADC(float, int)
; VA 0x6c0ec size 24

   6c0ec: e3a02000     	mov	r2, #0
   6c0f0: e580100c     	str	r1, [r0, #0xc]
   6c0f4: e5802004     	str	r2, [r0, #0x4]
   6c0f8: e5802008     	str	r2, [r0, #0x8]
   6c0fc: ed800a00     	vstr	s0, [r0]
   6c100: e12fff1e     	bx	lr
