; lubadh::Application::stop()
; VA 0x27c58 size 28

   27c58: e92d4010     	push	{r4, lr}
   27c5c: e2804915     	add	r4, r0, #344064
   27c60: e5940ab8     	ldr	r0, [r4, #0xab8]
   27c64: eb003a3c     	bl	0x3655c
   27c68: e3a03000     	mov	r3, #0
   27c6c: e5c43abd     	strb	r3, [r4, #0xabd]
   27c70: e8bd8010     	pop	{r4, pc}
