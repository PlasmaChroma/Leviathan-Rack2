; lubadh::Tap::Tap()
; VA 0x4bb88 size 172

   4bb88: e1a03000     	mov	r3, r0
   4bb8c: e92d4030     	push	{r4, r5, lr}
   4bb90: e3a02000     	mov	r2, #0
   4bb94: e3a0e000     	mov	lr, #0
   4bb98: e5802004     	str	r2, [r0, #0x4]
   4bb9c: e580e008     	str	lr, [r0, #0x8]
   4bba0: e280400c     	add	r4, r0, #12
   4bba4: e2805028     	add	r5, r0, #40
   4bba8: e3a0c5fe     	mov	r12, #1065353216
   4bbac: e1c000d4     	ldrd	r0, r1, [r0, #4]
   4bbb0: e5c32000     	strb	r2, [r3]
   4bbb4: e8840003     	stm	r4, {r0, r1}
   4bbb8: e2834040     	add	r4, r3, #64
   4bbbc: e583e024     	str	lr, [r3, #0x24]
   4bbc0: e5832020     	str	r2, [r3, #0x20]
   4bbc4: e5931024     	ldr	r1, [r3, #0x24]
   4bbc8: e5c32014     	strb	r2, [r3, #0x14]
   4bbcc: e5c3201c     	strb	r2, [r3, #0x1c]
   4bbd0: e583c018     	str	r12, [r3, #0x18]
   4bbd4: e8850003     	stm	r5, {r0, r1}
   4bbd8: e2835058     	add	r5, r3, #88
   4bbdc: e583e03c     	str	lr, [r3, #0x3c]
   4bbe0: e5832038     	str	r2, [r3, #0x38]
   4bbe4: e593103c     	ldr	r1, [r3, #0x3c]
   4bbe8: e583c030     	str	r12, [r3, #0x30]
   4bbec: e5c32034     	strb	r2, [r3, #0x34]
   4bbf0: e8840003     	stm	r4, {r0, r1}
   4bbf4: e2834070     	add	r4, r3, #112
   4bbf8: e583e054     	str	lr, [r3, #0x54]
   4bbfc: e5832050     	str	r2, [r3, #0x50]
   4bc00: e5931054     	ldr	r1, [r3, #0x54]
   4bc04: e583c048     	str	r12, [r3, #0x48]
   4bc08: e5c3204c     	strb	r2, [r3, #0x4c]
   4bc0c: e8850003     	stm	r5, {r0, r1}
   4bc10: e583c060     	str	r12, [r3, #0x60]
   4bc14: e5832068     	str	r2, [r3, #0x68]
   4bc18: e583e06c     	str	lr, [r3, #0x6c]
   4bc1c: e593106c     	ldr	r1, [r3, #0x6c]
   4bc20: e5c32064     	strb	r2, [r3, #0x64]
   4bc24: e8840003     	stm	r4, {r0, r1}
   4bc28: e1a00003     	mov	r0, r3
   4bc2c: e583c078     	str	r12, [r3, #0x78]
   4bc30: e8bd8030     	pop	{r4, r5, pc}
