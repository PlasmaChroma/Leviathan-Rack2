000109a4 <close_midi_device>:
   109a4: e92d4800     	push	{r11, lr}
   109a8: e28db004     	add	r11, sp, #4
   109ac: e24dd018     	sub	sp, sp, #24
   109b0: e50b0010     	str	r0, [r11, #-0x10]
   109b4: e50b1014     	str	r1, [r11, #-0x14]
   109b8: e50b2018     	str	r2, [r11, #-0x18]
   109bc: e51b3014     	ldr	r3, [r11, #-0x14]
   109c0: e1a03103     	lsl	r3, r3, #2
   109c4: e51b2010     	ldr	r2, [r11, #-0x10]
   109c8: e0823003     	add	r3, r2, r3
   109cc: e5933000     	ldr	r3, [r3]
   109d0: e1a01003     	mov	r1, r3
   109d4: e59f0098     	ldr	r0, [pc, #0x98]         @ 0x10a74 <close_midi_device+0xd0>
   109d8: ebffff0e     	bl	0x10618 <.plt+0x14>     @ imm = #-0x3c8
   109dc: e51b3014     	ldr	r3, [r11, #-0x14]
   109e0: e1a03103     	lsl	r3, r3, #2
   109e4: e51b2010     	ldr	r2, [r11, #-0x10]
   109e8: e0823003     	add	r3, r2, r3
   109ec: e5933000     	ldr	r3, [r3]
   109f0: e1a00003     	mov	r0, r3
   109f4: ebffff43     	bl	0x10708 <.plt+0x104>    @ imm = #-0x2f4
   109f8: e51b3014     	ldr	r3, [r11, #-0x14]
   109fc: e50b3008     	str	r3, [r11, #-0x8]
   10a00: ea00000d     	b	0x10a3c <close_midi_device+0x98> @ imm = #0x34
   10a04: e51b3008     	ldr	r3, [r11, #-0x8]
   10a08: e2833001     	add	r3, r3, #1
   10a0c: e1a03103     	lsl	r3, r3, #2
   10a10: e51b2010     	ldr	r2, [r11, #-0x10]
   10a14: e0822003     	add	r2, r2, r3
   10a18: e51b3008     	ldr	r3, [r11, #-0x8]
   10a1c: e1a03103     	lsl	r3, r3, #2
   10a20: e51b1010     	ldr	r1, [r11, #-0x10]
   10a24: e0813003     	add	r3, r1, r3
   10a28: e5922000     	ldr	r2, [r2]
   10a2c: e5832000     	str	r2, [r3]
   10a30: e51b3008     	ldr	r3, [r11, #-0x8]
   10a34: e2833001     	add	r3, r3, #1
   10a38: e50b3008     	str	r3, [r11, #-0x8]
   10a3c: e51b3018     	ldr	r3, [r11, #-0x18]
   10a40: e5933000     	ldr	r3, [r3]
   10a44: e2433001     	sub	r3, r3, #1
   10a48: e51b2008     	ldr	r2, [r11, #-0x8]
   10a4c: e1520003     	cmp	r2, r3
   10a50: baffffeb     	blt	0x10a04 <close_midi_device+0x60> @ imm = #-0x54
   10a54: e51b3018     	ldr	r3, [r11, #-0x18]
   10a58: e5933000     	ldr	r3, [r3]
   10a5c: e2432001     	sub	r2, r3, #1
   10a60: e51b3018     	ldr	r3, [r11, #-0x18]
   10a64: e5832000     	str	r2, [r3]
   10a68: e1a00000     	mov	r0, r0
   10a6c: e24bd004     	sub	sp, r11, #4
   10a70: e8bd8800     	pop	{r11, pc}
   10a74: 08 13 01 00  	.word	0x00011308

