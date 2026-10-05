; lubadh::Application::runCalibration()::{lambda(lubadh::Channel&)#1}::operator()(lubadh::Channel&) const [clone .isra.0]
; VA 0x27a84 size 292

   27a84: e92d4030     	push	{r4, r5, lr}
   27a88: e3a02001     	mov	r2, #1
   27a8c: e1a04000     	mov	r4, r0
   27a90: e24dd00c     	sub	sp, sp, #12
   27a94: e5900000     	ldr	r0, [r0]
   27a98: e1a05001     	mov	r5, r1
   27a9c: e5d11000     	ldrb	r1, [r1]
   27aa0: eb0113bb     	bl	0x6c994
   27aa4: eddf0b3d     	vldr	d16, [pc, #244]         @ 0x27ba0 ; float 6.97119066971e-237
   27aa8: edcd0b00     	vstr	d16, [sp]
   27aac: ea000003     	b	0x27ac0
   27ab0: ebffb8d6     	bl	0x15e10    @ imm = #-0x11ca8 ; __errno_location
   27ab4: e5903000     	ldr	r3, [r0]
   27ab8: e3530004     	cmp	r3, #4
   27abc: 1a000004     	bne	0x27ad4
   27ac0: e1a0100d     	mov	r1, sp
   27ac4: e1a0000d     	mov	r0, sp
   27ac8: ebffb93c     	bl	0x15fc0    @ imm = #-0x11b10 ; nanosleep
   27acc: e3700001     	cmn	r0, #1
   27ad0: 0afffff6     	beq	0x27ab0
   27ad4: e5d51000     	ldrb	r1, [r5]
   27ad8: e3a02000     	mov	r2, #0
   27adc: e5940000     	ldr	r0, [r4]
   27ae0: eb0113ab     	bl	0x6c994
   27ae4: eddf0b2d     	vldr	d16, [pc, #180]         @ 0x27ba0 ; float 6.97119066971e-237
   27ae8: edcd0b00     	vstr	d16, [sp]
   27aec: ea000003     	b	0x27b00
   27af0: ebffb8c6     	bl	0x15e10    @ imm = #-0x11ce8 ; __errno_location
   27af4: e5903000     	ldr	r3, [r0]
   27af8: e3530004     	cmp	r3, #4
   27afc: 1a000004     	bne	0x27b14
   27b00: e1a0100d     	mov	r1, sp
   27b04: e1a0000d     	mov	r0, sp
   27b08: ebffb92c     	bl	0x15fc0    @ imm = #-0x11b50 ; nanosleep
   27b0c: e3700001     	cmn	r0, #1
   27b10: 0afffff6     	beq	0x27af0
   27b14: e5d51000     	ldrb	r1, [r5]
   27b18: e3a02001     	mov	r2, #1
   27b1c: e5940000     	ldr	r0, [r4]
   27b20: eb01139b     	bl	0x6c994
   27b24: eddf0b1d     	vldr	d16, [pc, #116]         @ 0x27ba0 ; float 6.97119066971e-237
   27b28: edcd0b00     	vstr	d16, [sp]
   27b2c: ea000003     	b	0x27b40
   27b30: ebffb8b6     	bl	0x15e10    @ imm = #-0x11d28 ; __errno_location
   27b34: e5903000     	ldr	r3, [r0]
   27b38: e3530004     	cmp	r3, #4
   27b3c: 1a000004     	bne	0x27b54
   27b40: e1a0100d     	mov	r1, sp
   27b44: e1a0000d     	mov	r0, sp
   27b48: ebffb91c     	bl	0x15fc0    @ imm = #-0x11b90 ; nanosleep
   27b4c: e3700001     	cmn	r0, #1
   27b50: 0afffff6     	beq	0x27b30
   27b54: e5d51000     	ldrb	r1, [r5]
   27b58: e3a02000     	mov	r2, #0
   27b5c: e5940000     	ldr	r0, [r4]
   27b60: eb01138b     	bl	0x6c994
   27b64: eddf0b0d     	vldr	d16, [pc, #52]          @ 0x27ba0 ; float 6.97119066971e-237
   27b68: edcd0b00     	vstr	d16, [sp]
   27b6c: ea000003     	b	0x27b80
   27b70: ebffb8a6     	bl	0x15e10    @ imm = #-0x11d68 ; __errno_location
   27b74: e5903000     	ldr	r3, [r0]
   27b78: e3530004     	cmp	r3, #4
   27b7c: 1a000004     	bne	0x27b94
   27b80: e1a0100d     	mov	r1, sp
   27b84: e1a0000d     	mov	r0, sp
   27b88: ebffb90c     	bl	0x15fc0    @ imm = #-0x11bd0 ; nanosleep
   27b8c: e3700001     	cmn	r0, #1
   27b90: 0afffff6     	beq	0x27b70
   27b94: e28dd00c     	add	sp, sp, #12
   27b98: e8bd8030     	pop	{r4, r5, pc}
   27b9c: e320f000     	nop
   27ba0: 00 00 00 00  	.word	0x00000000
   27ba4: 80 b2 e6 0e  	.word	0x0ee6b280
