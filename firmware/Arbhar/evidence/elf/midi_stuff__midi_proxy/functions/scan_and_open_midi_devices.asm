00010864 <scan_and_open_midi_devices>:
   10864: e92d4800     	push	{r11, lr}
   10868: e28db004     	add	r11, sp, #4
   1086c: e24dde12     	sub	sp, sp, #288
   10870: e50b0118     	str	r0, [r11, #-0x118]
   10874: e50b111c     	str	r1, [r11, #-0x11c]
   10878: e3a03000     	mov	r3, #0
   1087c: e50b3008     	str	r3, [r11, #-0x8]
   10880: e59f0104     	ldr	r0, [pc, #0x104]        @ 0x1098c <scan_and_open_midi_devices+0x128>
   10884: ebffff6f     	bl	0x10648 <.plt+0x44>     @ imm = #-0x244
   10888: e50b000c     	str	r0, [r11, #-0xc]
   1088c: e51b300c     	ldr	r3, [r11, #-0xc]
   10890: e3530000     	cmp	r3, #0
   10894: 1a00002c     	bne	0x1094c <scan_and_open_midi_devices+0xe8> @ imm = #0xb0
   10898: e59f00f0     	ldr	r0, [pc, #0xf0]         @ 0x10990 <scan_and_open_midi_devices+0x12c>
   1089c: ebffff66     	bl	0x1063c <.plt+0x38>     @ imm = #-0x268
   108a0: e3e03000     	mvn	r3, #0
   108a4: ea000035     	b	0x10980 <scan_and_open_midi_devices+0x11c> @ imm = #0xd4
   108a8: e51b3010     	ldr	r3, [r11, #-0x10]
   108ac: e283300b     	add	r3, r3, #11
   108b0: e3a02005     	mov	r2, #5
   108b4: e59f10d8     	ldr	r1, [pc, #0xd8]         @ 0x10994 <scan_and_open_midi_devices+0x130>
   108b8: e1a00003     	mov	r0, r3
   108bc: ebffff8b     	bl	0x106f0 <.plt+0xec>     @ imm = #-0x1d4
   108c0: e1a03000     	mov	r3, r0
   108c4: e3530000     	cmp	r3, #0
   108c8: 1a00001f     	bne	0x1094c <scan_and_open_midi_devices+0xe8> @ imm = #0x7c
   108cc: e51b3010     	ldr	r3, [r11, #-0x10]
   108d0: e283300b     	add	r3, r3, #11
   108d4: e24b0f45     	sub	r0, r11, #276
   108d8: e58d3000     	str	r3, [sp]
   108dc: e59f30a8     	ldr	r3, [pc, #0xa8]         @ 0x1098c <scan_and_open_midi_devices+0x128>
   108e0: e59f20b0     	ldr	r2, [pc, #0xb0]         @ 0x10998 <scan_and_open_midi_devices+0x134>
   108e4: e3a01c01     	mov	r1, #256
   108e8: ebffff6e     	bl	0x106a8 <.plt+0xa4>     @ imm = #-0x248
   108ec: e24b3f45     	sub	r3, r11, #276
   108f0: e1a00003     	mov	r0, r3
   108f4: ebffffc8     	bl	0x1081c <open_midi_device> @ imm = #-0xe0
   108f8: e50b0014     	str	r0, [r11, #-0x14]
   108fc: e51b3014     	ldr	r3, [r11, #-0x14]
   10900: e3530000     	cmp	r3, #0
   10904: ba00000c     	blt	0x1093c <scan_and_open_midi_devices+0xd8> @ imm = #0x30
   10908: e51b3008     	ldr	r3, [r11, #-0x8]
   1090c: e2832001     	add	r2, r3, #1
   10910: e50b2008     	str	r2, [r11, #-0x8]
   10914: e1a03103     	lsl	r3, r3, #2
   10918: e51b2118     	ldr	r2, [r11, #-0x118]
   1091c: e0823003     	add	r3, r2, r3
   10920: e51b2014     	ldr	r2, [r11, #-0x14]
   10924: e5832000     	str	r2, [r3]
   10928: e24b3f45     	sub	r3, r11, #276
   1092c: e1a01003     	mov	r1, r3
   10930: e59f0064     	ldr	r0, [pc, #0x64]         @ 0x1099c <scan_and_open_midi_devices+0x138>
   10934: ebffff37     	bl	0x10618 <.plt+0x14>     @ imm = #-0x324
   10938: ea000003     	b	0x1094c <scan_and_open_midi_devices+0xe8> @ imm = #0xc
   1093c: e24b3f45     	sub	r3, r11, #276
   10940: e1a01003     	mov	r1, r3
   10944: e59f0054     	ldr	r0, [pc, #0x54]         @ 0x109a0 <scan_and_open_midi_devices+0x13c>
   10948: ebffff32     	bl	0x10618 <.plt+0x14>     @ imm = #-0x338
   1094c: e51b000c     	ldr	r0, [r11, #-0xc]
   10950: ebffff63     	bl	0x106e4 <.plt+0xe0>     @ imm = #-0x274
   10954: e50b0010     	str	r0, [r11, #-0x10]
   10958: e51b3010     	ldr	r3, [r11, #-0x10]
   1095c: e3530000     	cmp	r3, #0
   10960: 0a000003     	beq	0x10974 <scan_and_open_midi_devices+0x110> @ imm = #0xc
   10964: e51b2008     	ldr	r2, [r11, #-0x8]
   10968: e51b311c     	ldr	r3, [r11, #-0x11c]
   1096c: e1520003     	cmp	r2, r3
   10970: baffffcc     	blt	0x108a8 <scan_and_open_midi_devices+0x44> @ imm = #-0xd0
   10974: e51b000c     	ldr	r0, [r11, #-0xc]
   10978: ebffff68     	bl	0x10720 <.plt+0x11c>    @ imm = #-0x260
   1097c: e51b3008     	ldr	r3, [r11, #-0x8]
   10980: e1a00003     	mov	r0, r3
   10984: e24bd004     	sub	sp, r11, #4
   10988: e8bd8800     	pop	{r11, pc}
   1098c: 90 12 01 00  	.word	0x00011290
   10990: 9c 12 01 00  	.word	0x0001129c
   10994: c0 12 01 00  	.word	0x000112c0
   10998: c8 12 01 00  	.word	0x000112c8
   1099c: d0 12 01 00  	.word	0x000112d0
   109a0: e8 12 01 00  	.word	0x000112e8

