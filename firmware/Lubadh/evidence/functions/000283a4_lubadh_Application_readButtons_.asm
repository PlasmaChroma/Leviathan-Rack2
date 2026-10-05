; lubadh::Application::readButtons()
; VA 0x283a4 size 148

   283a4: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   283a8: e2804ba9     	add	r4, r0, #173056
   283ac: e2807a7e     	add	r7, r0, #516096
   283b0: e1a05000     	mov	r5, r0
   283b4: e2844d06     	add	r4, r4, #384
   283b8: e2877eff     	add	r7, r7, #4080
   283bc: e2446ba9     	sub	r6, r4, #173056
   283c0: e55410b4     	ldrb	r1, [r4, #-0xb4]
   283c4: e1a00005     	mov	r0, r5
   283c8: eb010f3d     	bl	0x6c0c4
   283cc: e1a01000     	mov	r1, r0
   283d0: e2460038     	sub	r0, r6, #56
   283d4: eb003899     	bl	0x36640
   283d8: e55410b0     	ldrb	r1, [r4, #-0xb0]
   283dc: e1a00005     	mov	r0, r5
   283e0: eb010f37     	bl	0x6c0c4
   283e4: e1a01000     	mov	r1, r0
   283e8: e2460020     	sub	r0, r6, #32
   283ec: eb0038be     	bl	0x366ec
   283f0: e55410b2     	ldrb	r1, [r4, #-0xb2]
   283f4: e1a00005     	mov	r0, r5
   283f8: eb010f31     	bl	0x6c0c4
   283fc: e1a01000     	mov	r1, r0
   28400: e2460048     	sub	r0, r6, #72
   28404: eb00388d     	bl	0x36640
   28408: e55410ae     	ldrb	r1, [r4, #-0xae]
   2840c: e1a00005     	mov	r0, r5
   28410: eb010f2b     	bl	0x6c0c4
   28414: e1a01000     	mov	r1, r0
   28418: e2440a2a     	sub	r0, r4, #172032
   2841c: e2844ba9     	add	r4, r4, #173056
   28420: e2400fde     	sub	r0, r0, #888
   28424: e2844f4e     	add	r4, r4, #312
   28428: eb003884     	bl	0x36640
   2842c: e1540007     	cmp	r4, r7
   28430: 1affffe1     	bne	0x283bc
   28434: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
