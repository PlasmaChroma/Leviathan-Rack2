; main::{lambda(bool)#2}::operator()(bool) const [clone .isra.0]
; VA 0x4b5b8 size 112

   4b5b8: e1a03000     	mov	r3, r0
   4b5bc: e3510000     	cmp	r1, #0
   4b5c0: e2802008     	add	r2, r0, #8
   4b5c4: 0a00000a     	beq	0x4b5f4
   4b5c8: e3031348     	movw	r1, #0x3348
   4b5cc: e3401007     	movt	r1, #0x7
   4b5d0: e5802000     	str	r2, [r0]
   4b5d4: e3a00004     	mov	r0, #4
   4b5d8: e5830004     	str	r0, [r3, #0x4]
   4b5dc: e3a02000     	mov	r2, #0
   4b5e0: e5910000     	ldr	r0, [r1]
   4b5e4: e5830008     	str	r0, [r3, #0x8]
   4b5e8: e1a00003     	mov	r0, r3
   4b5ec: e5c3200c     	strb	r2, [r3, #0xc]
   4b5f0: e12fff1e     	bx	lr
   4b5f4: e303c350     	movw	r12, #0x3350
   4b5f8: e340c007     	movt	r12, #0x7
   4b5fc: e52de004     	str	lr, [sp, #-0x4]!
   4b600: e3a0e005     	mov	lr, #5
   4b604: e5802000     	str	r2, [r0]
   4b608: e59c0000     	ldr	r0, [r12]
   4b60c: e5dcc004     	ldrb	r12, [r12, #0x4]
   4b610: e5830008     	str	r0, [r3, #0x8]
   4b614: e1a00003     	mov	r0, r3
   4b618: e5c2c004     	strb	r12, [r2, #0x4]
   4b61c: e5c3100d     	strb	r1, [r3, #0xd]
   4b620: e583e004     	str	lr, [r3, #0x4]
   4b624: e49df004     	ldr	pc, [sp], #4
