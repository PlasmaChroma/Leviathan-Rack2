; lubadh::TapManager::any_active(unsigned int) const
; VA 0x4dbf0 size 144

   4dbf0: e3a03faa     	mov	r3, #680
   4dbf4: e0010193     	mul	r1, r3, r1
   4dbf8: e080c001     	add	r12, r0, r1
   4dbfc: e28c2fa5     	add	r2, r12, #660
   4dc00: e28c3084     	add	r3, r12, #132
   4dc04: e7d01001     	ldrb	r1, [r0, r1]
   4dc08: e3510000     	cmp	r1, #0
   4dc0c: 1a000003     	bne	0x4dc20
   4dc10: e5dc1084     	ldrb	r1, [r12, #0x84]
   4dc14: e28c3f42     	add	r3, r12, #264
   4dc18: e3510000     	cmp	r1, #0
   4dc1c: 0a00000a     	beq	0x4dc4c
   4dc20: e1520003     	cmp	r2, r3
   4dc24: 12833084     	addne	r3, r3, #132
   4dc28: 0a000005     	beq	0x4dc44
   4dc2c: e2831084     	add	r1, r3, #132
   4dc30: e1520003     	cmp	r2, r3
   4dc34: 0a000002     	beq	0x4dc44
   4dc38: e2833f42     	add	r3, r3, #264
   4dc3c: e1520001     	cmp	r2, r1
   4dc40: 1afffff9     	bne	0x4dc2c
   4dc44: e3a00001     	mov	r0, #1
   4dc48: e12fff1e     	bx	lr
   4dc4c: e5dc1108     	ldrb	r1, [r12, #0x108]
   4dc50: e28c3f63     	add	r3, r12, #396
   4dc54: e3510000     	cmp	r1, #0
   4dc58: 1afffff0     	bne	0x4dc20
   4dc5c: e5dc118c     	ldrb	r1, [r12, #0x18c]
   4dc60: e28c3e21     	add	r3, r12, #528
   4dc64: e3510000     	cmp	r1, #0
   4dc68: 1affffec     	bne	0x4dc20
   4dc6c: e5dc0210     	ldrb	r0, [r12, #0x210]
   4dc70: e1a03002     	mov	r3, r2
   4dc74: e3500000     	cmp	r0, #0
   4dc78: 012fff1e     	bxeq	lr
   4dc7c: eaffffe7     	b	0x4dc20
