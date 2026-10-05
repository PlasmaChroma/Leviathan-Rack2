; lubadh::Button::Button()
; VA 0x369f8 size 40

   369f8: e1a02000     	mov	r2, r0
   369fc: f2c00010     	vmov.i32	d16, #0x0
   36a00: e59fc014     	ldr	r12, [pc, #0x14]        @ 0x36a1c
   36a04: e3a01000     	mov	r1, #0
   36a08: e482c004     	str	r12, [r2], #4
   36a0c: f442078f     	vst1.32	{d16}, [r2]
   36a10: e1c010bc     	strh	r1, [r0, #12]
   36a14: e5c0100e     	strb	r1, [r0, #0xe]
   36a18: e12fff1e     	bx	lr
   36a1c: 98 22 07 00  	.word	0x00072298
