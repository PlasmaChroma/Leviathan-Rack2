; lubadh::Channel::FileSaver::save()
; VA 0x3932c size 92

   3932c: e5903000     	ldr	r3, [r0]
   39330: e3a0c001     	mov	r12, #1
   39334: e92d4010     	push	{r4, lr}
   39338: e3a0105c     	mov	r1, #92
   3933c: e5932020     	ldr	r2, [r3, #0x20]
   39340: e5c3c240     	strb	r12, [r3, #0x240]
   39344: e2822915     	add	r2, r2, #344064
   39348: e5831294     	str	r1, [r3, #0x294]
   3934c: e5d22abc     	ldrb	r2, [r2, #0xabc]
   39350: e3520000     	cmp	r2, #0
   39354: e59320e8     	ldr	r2, [r3, #0xe8]
   39358: 12804068     	addne	r4, r0, #104
   3935c: 02804004     	addeq	r4, r0, #4
   39360: e1a00004     	mov	r0, r4
   39364: e5922034     	ldr	r2, [r2, #0x34]
   39368: e5943020     	ldr	r3, [r4, #0x20]
   3936c: e5922000     	ldr	r2, [r2]
   39370: e5832000     	str	r2, [r3]
   39374: eb00dd29     	bl	0x70820
   39378: e5943044     	ldr	r3, [r4, #0x44]
   3937c: e3a02001     	mov	r2, #1
   39380: e5832000     	str	r2, [r3]
   39384: e8bd8010     	pop	{r4, pc}
