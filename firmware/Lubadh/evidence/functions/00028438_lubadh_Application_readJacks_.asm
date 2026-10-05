; lubadh::Application::readJacks()
; VA 0x28438 size 120

   28438: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
   2843c: e2804ba9     	add	r4, r0, #173056
   28440: e2807a7e     	add	r7, r0, #516096
   28444: e1a05000     	mov	r5, r0
   28448: e2844d06     	add	r4, r4, #384
   2844c: e2877eff     	add	r7, r7, #4080
   28450: e2446a2a     	sub	r6, r4, #172032
   28454: e55410b3     	ldrb	r1, [r4, #-0xb3]
   28458: e1a00005     	mov	r0, r5
   2845c: eb010f18     	bl	0x6c0c4
   28460: e1a01000     	mov	r1, r0
   28464: e2460fe2     	sub	r0, r6, #904
   28468: eb003874     	bl	0x36640
   2846c: e55410af     	ldrb	r1, [r4, #-0xaf]
   28470: e1a00005     	mov	r0, r5
   28474: eb010f12     	bl	0x6c0c4
   28478: e1a01000     	mov	r1, r0
   2847c: e2460fe6     	sub	r0, r6, #920
   28480: eb00386e     	bl	0x36640
   28484: e55410b1     	ldrb	r1, [r4, #-0xb1]
   28488: e1a00005     	mov	r0, r5
   2848c: e2844ba9     	add	r4, r4, #173056
   28490: eb010f0b     	bl	0x6c0c4
   28494: e2844f4e     	add	r4, r4, #312
   28498: e1a01000     	mov	r1, r0
   2849c: e2460fea     	sub	r0, r6, #936
   284a0: eb003866     	bl	0x36640
   284a4: e1540007     	cmp	r4, r7
   284a8: 1affffe8     	bne	0x28450
   284ac: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
