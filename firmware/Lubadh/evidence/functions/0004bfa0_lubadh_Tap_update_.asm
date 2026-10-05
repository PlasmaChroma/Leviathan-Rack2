; lubadh::Tap::update()
; VA 0x4bfa0 size 476

   4bfa0: e5d0201c     	ldrb	r2, [r0, #0x1c]
   4bfa4: e1a03000     	mov	r3, r0
   4bfa8: e3520000     	cmp	r2, #0
   4bfac: 0a00000f     	beq	0x4bff0
   4bfb0: e5902020     	ldr	r2, [r0, #0x20]
   4bfb4: e3520000     	cmp	r2, #0
   4bfb8: ba000046     	blt	0x4c0d8
   4bfbc: e35200fe     	cmp	r2, #254
   4bfc0: da00000a     	ble	0x4bff0
   4bfc4: e3a00000     	mov	r0, #0
   4bfc8: e2832028     	add	r2, r3, #40
   4bfcc: e5830024     	str	r0, [r3, #0x24]
   4bfd0: e3a01000     	mov	r1, #0
   4bfd4: e1a00001     	mov	r0, r1
   4bfd8: e5831020     	str	r1, [r3, #0x20]
   4bfdc: e5c3101c     	strb	r1, [r3, #0x1c]
   4bfe0: e3a0c5fe     	mov	r12, #1065353216
   4bfe4: e5931024     	ldr	r1, [r3, #0x24]
   4bfe8: e8820003     	stm	r2, {r0, r1}
   4bfec: e583c030     	str	r12, [r3, #0x30]
   4bff0: e5d32034     	ldrb	r2, [r3, #0x34]
   4bff4: e3520000     	cmp	r2, #0
   4bff8: 0a00000f     	beq	0x4c03c
   4bffc: e5932038     	ldr	r2, [r3, #0x38]
   4c000: e3520000     	cmp	r2, #0
   4c004: ba000033     	blt	0x4c0d8
   4c008: e35200fe     	cmp	r2, #254
   4c00c: da00000a     	ble	0x4c03c
   4c010: e3a00000     	mov	r0, #0
   4c014: e2832040     	add	r2, r3, #64
   4c018: e583003c     	str	r0, [r3, #0x3c]
   4c01c: e3a01000     	mov	r1, #0
   4c020: e1a00001     	mov	r0, r1
   4c024: e5831038     	str	r1, [r3, #0x38]
   4c028: e5c31034     	strb	r1, [r3, #0x34]
   4c02c: e3a0c5fe     	mov	r12, #1065353216
   4c030: e593103c     	ldr	r1, [r3, #0x3c]
   4c034: e8820003     	stm	r2, {r0, r1}
   4c038: e583c048     	str	r12, [r3, #0x48]
   4c03c: e5d3204c     	ldrb	r2, [r3, #0x4c]
   4c040: e3520000     	cmp	r2, #0
   4c044: 0a00000f     	beq	0x4c088
   4c048: e5932050     	ldr	r2, [r3, #0x50]
   4c04c: e3520000     	cmp	r2, #0
   4c050: ba000020     	blt	0x4c0d8
   4c054: e35200fe     	cmp	r2, #254
   4c058: da00000a     	ble	0x4c088
   4c05c: e3a00000     	mov	r0, #0
   4c060: e2832058     	add	r2, r3, #88
   4c064: e5830054     	str	r0, [r3, #0x54]
   4c068: e3a01000     	mov	r1, #0
   4c06c: e1a00001     	mov	r0, r1
   4c070: e5831050     	str	r1, [r3, #0x50]
   4c074: e5c3104c     	strb	r1, [r3, #0x4c]
   4c078: e3a0c5fe     	mov	r12, #1065353216
   4c07c: e5931054     	ldr	r1, [r3, #0x54]
   4c080: e8820003     	stm	r2, {r0, r1}
   4c084: e583c060     	str	r12, [r3, #0x60]
   4c088: e5d32064     	ldrb	r2, [r3, #0x64]
   4c08c: e3520000     	cmp	r2, #0
   4c090: 012fff1e     	bxeq	lr
   4c094: e5932068     	ldr	r2, [r3, #0x68]
   4c098: e3520000     	cmp	r2, #0
   4c09c: ba00000d     	blt	0x4c0d8
   4c0a0: e35200fe     	cmp	r2, #254
   4c0a4: d12fff1e     	bxle	lr
   4c0a8: e3a00000     	mov	r0, #0
   4c0ac: e2832070     	add	r2, r3, #112
   4c0b0: e583006c     	str	r0, [r3, #0x6c]
   4c0b4: e3a01000     	mov	r1, #0
   4c0b8: e1a00001     	mov	r0, r1
   4c0bc: e5831068     	str	r1, [r3, #0x68]
   4c0c0: e5c31064     	strb	r1, [r3, #0x64]
   4c0c4: e3a0c5fe     	mov	r12, #1065353216
   4c0c8: e593106c     	ldr	r1, [r3, #0x6c]
   4c0cc: e8820003     	stm	r2, {r0, r1}
   4c0d0: e583c078     	str	r12, [r3, #0x78]
   4c0d4: e12fff1e     	bx	lr
   4c0d8: e92d4030     	push	{r4, r5, lr}
   4c0dc: e3a02000     	mov	r2, #0
   4c0e0: e3a0e000     	mov	lr, #0
   4c0e4: e5832004     	str	r2, [r3, #0x4]
   4c0e8: e283400c     	add	r4, r3, #12
   4c0ec: e583e008     	str	lr, [r3, #0x8]
   4c0f0: e2835028     	add	r5, r3, #40
   4c0f4: e5c32000     	strb	r2, [r3]
   4c0f8: e3a0c5fe     	mov	r12, #1065353216
   4c0fc: e9930003     	ldmib	r3, {r0, r1}
   4c100: e8840003     	stm	r4, {r0, r1}
   4c104: e583e024     	str	lr, [r3, #0x24]
   4c108: e2834040     	add	r4, r3, #64
   4c10c: e5832020     	str	r2, [r3, #0x20]
   4c110: e5931024     	ldr	r1, [r3, #0x24]
   4c114: e5c32014     	strb	r2, [r3, #0x14]
   4c118: e5c3201c     	strb	r2, [r3, #0x1c]
   4c11c: e583c018     	str	r12, [r3, #0x18]
   4c120: e8850003     	stm	r5, {r0, r1}
   4c124: e2835058     	add	r5, r3, #88
   4c128: e583e03c     	str	lr, [r3, #0x3c]
   4c12c: e5832038     	str	r2, [r3, #0x38]
   4c130: e593103c     	ldr	r1, [r3, #0x3c]
   4c134: e583c030     	str	r12, [r3, #0x30]
   4c138: e5c32034     	strb	r2, [r3, #0x34]
   4c13c: e8840003     	stm	r4, {r0, r1}
   4c140: e2834070     	add	r4, r3, #112
   4c144: e583e054     	str	lr, [r3, #0x54]
   4c148: e5832050     	str	r2, [r3, #0x50]
   4c14c: e5931054     	ldr	r1, [r3, #0x54]
   4c150: e583c048     	str	r12, [r3, #0x48]
   4c154: e5c3204c     	strb	r2, [r3, #0x4c]
   4c158: e8850003     	stm	r5, {r0, r1}
   4c15c: e583c060     	str	r12, [r3, #0x60]
   4c160: e5832068     	str	r2, [r3, #0x68]
   4c164: e583e06c     	str	lr, [r3, #0x6c]
   4c168: e593106c     	ldr	r1, [r3, #0x6c]
   4c16c: e5c32064     	strb	r2, [r3, #0x64]
   4c170: e8840003     	stm	r4, {r0, r1}
   4c174: e583c078     	str	r12, [r3, #0x78]
   4c178: e8bd8030     	pop	{r4, r5, pc}
