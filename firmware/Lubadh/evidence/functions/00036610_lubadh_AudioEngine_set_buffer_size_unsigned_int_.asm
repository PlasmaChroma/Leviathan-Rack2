; lubadh::AudioEngine::set_buffer_size(unsigned int)
; VA 0x36610 size 44

   36610: e92d4070     	push	{r4, r5, r6, lr}
   36614: e1a04001     	mov	r4, r1
   36618: e5905084     	ldr	r5, [r0, #0x84]
   3661c: e580108c     	str	r1, [r0, #0x8c]
   36620: e2850048     	add	r0, r5, #72
   36624: eb001383     	bl	0x3b438
   36628: e2850ba9     	add	r0, r5, #173056
   3662c: e1a01004     	mov	r1, r4
   36630: e2800d06     	add	r0, r0, #384
   36634: e8bd4070     	pop	{r4, r5, r6, lr}
   36638: ea00137e     	b	0x3b438
