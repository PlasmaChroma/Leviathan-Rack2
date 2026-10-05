; lubadh::LubadhHardware::readMCP(unsigned int)
; VA 0x6c064 size 96

   6c064: e3510007     	cmp	r1, #7
   6c068: 8a000013     	bhi	0x6c0bc
   6c06c: e52de004     	str	lr, [sp, #-0x4]!
   6c070: e1a03001     	mov	r3, r1
   6c074: e1a02121     	lsr	r2, r1, #2
   6c078: e24dd00c     	sub	sp, sp, #12
   6c07c: e3a01003     	mov	r1, #3
   6c080: e28d0004     	add	r0, sp, #4
   6c084: e3822006     	orr	r2, r2, #6
   6c088: e1a03303     	lsl	r3, r3, #6
   6c08c: e5cd2004     	strb	r2, [sp, #0x4]
   6c090: e5cd3005     	strb	r3, [sp, #0x5]
   6c094: e3a03000     	mov	r3, #0
   6c098: e5cd3006     	strb	r3, [sp, #0x6]
   6c09c: eb0006cb     	bl	0x6dbd0
   6c0a0: e5dd0005     	ldrb	r0, [sp, #0x5]
   6c0a4: e5dd3006     	ldrb	r3, [sp, #0x6]
   6c0a8: e1a00400     	lsl	r0, r0, #8
   6c0ac: e2000c0f     	and	r0, r0, #3840
   6c0b0: e1830000     	orr	r0, r3, r0
   6c0b4: e28dd00c     	add	sp, sp, #12
   6c0b8: e49df004     	ldr	pc, [sp], #4
   6c0bc: e30f0fff     	movw	r0, #0xffff
   6c0c0: e12fff1e     	bx	lr
