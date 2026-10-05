; lubadh::TapManager::oldest() const
; VA 0x4de2c size 236

   4de2c: e2803eab     	add	r3, r0, #2736
   4de30: e280ceaa     	add	r12, r0, #2720
   4de34: e043100c     	sub	r1, r3, r12
   4de38: e1a02241     	asr	r2, r1, #4
   4de3c: e1a01141     	asr	r1, r1, #2
   4de40: e3520000     	cmp	r2, #0
   4de44: da000019     	ble	0x4deb0
   4de48: e1a02202     	lsl	r2, r2, #4
   4de4c: e2622eab     	rsb	r2, r2, #2736
   4de50: e0800002     	add	r0, r0, r2
   4de54: ea00000b     	b	0x4de88
   4de58: e5132008     	ldr	r2, [r3, #-0x8]
   4de5c: e3520000     	cmp	r2, #0
   4de60: 1a000026     	bne	0x4df00
   4de64: e513200c     	ldr	r2, [r3, #-0xc]
   4de68: e3520000     	cmp	r2, #0
   4de6c: 1a000025     	bne	0x4df08
   4de70: e5132010     	ldr	r2, [r3, #-0x10]
   4de74: e3520000     	cmp	r2, #0
   4de78: 1a000024     	bne	0x4df10
   4de7c: e2433010     	sub	r3, r3, #16
   4de80: e1500003     	cmp	r0, r3
   4de84: 0a000007     	beq	0x4dea8
   4de88: e5132004     	ldr	r2, [r3, #-0x4]
   4de8c: e3520000     	cmp	r2, #0
   4de90: 0afffff0     	beq	0x4de58
   4de94: e15c0003     	cmp	r12, r3
   4de98: 0a00000a     	beq	0x4dec8
   4de9c: e5133004     	ldr	r3, [r3, #-0x4]
   4dea0: e593007c     	ldr	r0, [r3, #0x7c]
   4dea4: e12fff1e     	bx	lr
   4dea8: e043100c     	sub	r1, r3, r12
   4deac: e1a01141     	asr	r1, r1, #2
   4deb0: e3510002     	cmp	r1, #2
   4deb4: 0a000009     	beq	0x4dee0
   4deb8: e3510003     	cmp	r1, #3
   4debc: 0a000003     	beq	0x4ded0
   4dec0: e3510001     	cmp	r1, #1
   4dec4: 0a000009     	beq	0x4def0
   4dec8: e3a00000     	mov	r0, #0
   4decc: e12fff1e     	bx	lr
   4ded0: e5132004     	ldr	r2, [r3, #-0x4]
   4ded4: e3520000     	cmp	r2, #0
   4ded8: 1affffed     	bne	0x4de94
   4dedc: e2433004     	sub	r3, r3, #4
   4dee0: e5132004     	ldr	r2, [r3, #-0x4]
   4dee4: e3520000     	cmp	r2, #0
   4dee8: 1affffe9     	bne	0x4de94
   4deec: e2433004     	sub	r3, r3, #4
   4def0: e5132004     	ldr	r2, [r3, #-0x4]
   4def4: e3520000     	cmp	r2, #0
   4def8: 1affffe5     	bne	0x4de94
   4defc: eafffff1     	b	0x4dec8
   4df00: e2433004     	sub	r3, r3, #4
   4df04: eaffffe2     	b	0x4de94
   4df08: e2433008     	sub	r3, r3, #8
   4df0c: eaffffe0     	b	0x4de94
   4df10: e243300c     	sub	r3, r3, #12
   4df14: eaffffde     	b	0x4de94
