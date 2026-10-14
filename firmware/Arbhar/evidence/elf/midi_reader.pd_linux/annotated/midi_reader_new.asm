00000a4c <midi_reader_new>:
     a4c: e59f3094     	ldr	r3, [pc, #0x94]         @ 0xae8 <midi_reader_new+0x9c>  // u32=0x10638; f32?=9.40663633e-41
     a50: e92d4070     	push	{r4, r5, r6, lr}
     a54: e08f0003     	add	r0, pc, r3
     a58: e59f508c     	ldr	r5, [pc, #0x8c]         @ 0xaec <midi_reader_new+0xa0>  // u32=0x10590; f32?=9.38309452e-41
     a5c: e5900000     	ldr	r0, [r0]
     a60: ebffff33     	bl	0x734 <.plt+0x50>       @ imm = #-0x334  // CALL pd_new
     a64: e59f1084     	ldr	r1, [pc, #0x84]         @ 0xaf0 <midi_reader_new+0xa4>  // u32=0x33c; f32?=1.16027513e-42
     a68: e08f5005     	add	r5, pc, r5
     a6c: e1a04000     	mov	r4, r0
     a70: e2806028     	add	r6, r0, #40
     a74: e08f0001     	add	r0, pc, r1
     a78: ebffff45     	bl	0x794 <.plt+0xb0>       @ imm = #-0x2ec  // CALL post
     a7c: e59fc070     	ldr	r12, [pc, #0x70]        @ 0xaf4 <midi_reader_new+0xa8>  // u32=0x33c; f32?=1.16027513e-42
     a80: e3a020ff     	mov	r2, #255
     a84: e1a00006     	mov	r0, r6
     a88: e08f100c     	add	r1, pc, r12
     a8c: ebffff3d     	bl	0x788 <.plt+0xa4>       @ imm = #-0x30c  // CALL strncpy
     a90: e59f3060     	ldr	r3, [pc, #0x60]         @ 0xaf8 <midi_reader_new+0xac>  // u32=0x33c; f32?=1.16027513e-42
     a94: e3a02000     	mov	r2, #0
     a98: e1a01006     	mov	r1, r6
     a9c: e08f0003     	add	r0, pc, r3
     aa0: e5c42127     	strb	r2, [r4, #0x127]
     aa4: ebffff3a     	bl	0x794 <.plt+0xb0>       @ imm = #-0x318  // CALL post
     aa8: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0xafc <midi_reader_new+0xb0>  // u32=0x74; f32?=1.62550622e-43
     aac: e1a00004     	mov	r0, r4
     ab0: e7951001     	ldr	r1, [r5, r1]
     ab4: ebffff27     	bl	0x758 <.plt+0x74>       @ imm = #-0x364  // CALL clock_new
     ab8: e3e0c000     	mvn	r12, #0
     abc: e584c01c     	str	r12, [r4, #0x1c]
     ac0: e5840024     	str	r0, [r4, #0x24]
     ac4: e59f0034     	ldr	r0, [pc, #0x34]         @ 0xb00 <midi_reader_new+0xb4>  // u32=0x32c; f32?=1.13785435e-42
     ac8: e08f0000     	add	r0, pc, r0
     acc: ebffff09     	bl	0x6f8 <.plt+0x14>       @ imm = #-0x3dc  // CALL gensym
     ad0: e1a01000     	mov	r1, r0
     ad4: e1a00004     	mov	r0, r4
     ad8: ebffff30     	bl	0x7a0 <.plt+0xbc>       @ imm = #-0x340  // CALL outlet_new
     adc: e5840020     	str	r0, [r4, #0x20]
     ae0: e1a00004     	mov	r0, r4
     ae4: e8bd8070     	pop	{r4, r5, r6, pc}
     ae8: 38 06 01 00  	.word	0x00010638
     aec: 90 05 01 00  	.word	0x00010590
     af0: 3c 03 00 00  	.word	0x0000033c
     af4: 3c 03 00 00  	.word	0x0000033c
     af8: 3c 03 00 00  	.word	0x0000033c
     afc: 74 00 00 00  	.word	0x00000074
     b00: 2c 03 00 00  	.word	0x0000032c

