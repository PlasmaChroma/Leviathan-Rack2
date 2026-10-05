; lubadh::Channel::resetLoop()
; VA 0x386a4 size 100

   386a4: eddf0b13     	vldr	d16, [pc, #76]          @ 0x386f8 ; float 2.12199579146e-314
   386a8: eddf1b14     	vldr	d17, [pc, #80]          @ 0x38700 ; float 3.39049992291e-300
   386ac: e92d4010     	push	{r4, lr}
   386b0: e5902058     	ldr	r2, [r0, #0x58]
   386b4: e280e038     	add	lr, r0, #56
   386b8: e3a04000     	mov	r4, #0
   386bc: e300c99b     	movw	r12, #0x99b
   386c0: e3031004     	movw	r1, #0x3004
   386c4: e5824000     	str	r4, [r2]
   386c8: e3023a2e     	movw	r3, #0x2a2e
   386cc: e34031c2     	movt	r3, #0x1c2
   386d0: f44e0a8f     	vst1.32	{d16, d17}, [lr]
   386d4: e3a02001     	mov	r2, #1
   386d8: e580c048     	str	r12, [r0, #0x48]
   386dc: e5801084     	str	r1, [r0, #0x84]
   386e0: e580304c     	str	r3, [r0, #0x4c]
   386e4: e5803050     	str	r3, [r0, #0x50]
   386e8: e5802080     	str	r2, [r0, #0x80]
   386ec: e5802088     	str	r2, [r0, #0x88]
   386f0: e8bd8010     	pop	{r4, pc}
   386f4: e320f000     	nop
   386f8: 01 00 00 00  	.word	0x00000001
   386fc: 01 00 00 00  	.word	0x00000001
   38700: 2d 2a c2 01  	.word	0x01c22a2d
   38704: 2d 2a c2 01  	.word	0x01c22a2d
