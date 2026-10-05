; lubadh::Tap::reset()
; VA 0x4bc50 size 168

   4bc50: e1a03000     	mov	r3, r0
   4bc54: e92d4030     	push	{r4, r5, lr}
   4bc58: e3a02000     	mov	r2, #0
   4bc5c: e3a0e000     	mov	lr, #0
   4bc60: e5802004     	str	r2, [r0, #0x4]
   4bc64: e580e008     	str	lr, [r0, #0x8]
   4bc68: e280400c     	add	r4, r0, #12
   4bc6c: e2805028     	add	r5, r0, #40
   4bc70: e3a0c5fe     	mov	r12, #1065353216
   4bc74: e1c000d4     	ldrd	r0, r1, [r0, #4]
   4bc78: e5c32000     	strb	r2, [r3]
   4bc7c: e8840003     	stm	r4, {r0, r1}
   4bc80: e2834040     	add	r4, r3, #64
   4bc84: e583e024     	str	lr, [r3, #0x24]
   4bc88: e5832020     	str	r2, [r3, #0x20]
   4bc8c: e5931024     	ldr	r1, [r3, #0x24]
   4bc90: e5c32014     	strb	r2, [r3, #0x14]
   4bc94: e5c3201c     	strb	r2, [r3, #0x1c]
   4bc98: e583c018     	str	r12, [r3, #0x18]
   4bc9c: e8850003     	stm	r5, {r0, r1}
   4bca0: e2835058     	add	r5, r3, #88
   4bca4: e583e03c     	str	lr, [r3, #0x3c]
   4bca8: e5832038     	str	r2, [r3, #0x38]
   4bcac: e593103c     	ldr	r1, [r3, #0x3c]
   4bcb0: e583c030     	str	r12, [r3, #0x30]
   4bcb4: e5c32034     	strb	r2, [r3, #0x34]
   4bcb8: e8840003     	stm	r4, {r0, r1}
   4bcbc: e2834070     	add	r4, r3, #112
   4bcc0: e583e054     	str	lr, [r3, #0x54]
   4bcc4: e5832050     	str	r2, [r3, #0x50]
   4bcc8: e5931054     	ldr	r1, [r3, #0x54]
   4bccc: e583c048     	str	r12, [r3, #0x48]
   4bcd0: e5c3204c     	strb	r2, [r3, #0x4c]
   4bcd4: e8850003     	stm	r5, {r0, r1}
   4bcd8: e583c060     	str	r12, [r3, #0x60]
   4bcdc: e5832068     	str	r2, [r3, #0x68]
   4bce0: e583e06c     	str	lr, [r3, #0x6c]
   4bce4: e593106c     	ldr	r1, [r3, #0x6c]
   4bce8: e5c32064     	strb	r2, [r3, #0x64]
   4bcec: e8840003     	stm	r4, {r0, r1}
   4bcf0: e583c078     	str	r12, [r3, #0x78]
   4bcf4: e8bd8030     	pop	{r4, r5, pc}
