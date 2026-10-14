0001081c <open_midi_device>:
   1081c: e92d4800     	push	{r11, lr}
   10820: e28db004     	add	r11, sp, #4
   10824: e24dd010     	sub	sp, sp, #16
   10828: e50b0010     	str	r0, [r11, #-0x10]
   1082c: e3a01b02     	mov	r1, #2048
   10830: e51b0010     	ldr	r0, [r11, #-0x10]
   10834: ebffff8f     	bl	0x10678 <.plt+0x74>     @ imm = #-0x1c4
   10838: e50b0008     	str	r0, [r11, #-0x8]
   1083c: e51b3008     	ldr	r3, [r11, #-0x8]
   10840: e3530000     	cmp	r3, #0
   10844: aa000001     	bge	0x10850 <open_midi_device+0x34> @ imm = #0x4
   10848: e59f0010     	ldr	r0, [pc, #0x10]         @ 0x10860 <open_midi_device+0x44>
   1084c: ebffff7a     	bl	0x1063c <.plt+0x38>     @ imm = #-0x218
   10850: e51b3008     	ldr	r3, [r11, #-0x8]
   10854: e1a00003     	mov	r0, r3
   10858: e24bd004     	sub	sp, r11, #4
   1085c: e8bd8800     	pop	{r11, pc}
   10860: 74 12 01 00  	.word	0x00011274

