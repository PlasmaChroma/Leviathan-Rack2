00000b38 <midi_reader_connect>:
     b38: e92d4070     	push	{r4, r5, r6, lr}
     b3c: e1a05000     	mov	r5, r0
     b40: e59f4064     	ldr	r4, [pc, #0x64]         @ 0xbac <midi_reader_connect+0x74>  // u32=0x2b4; f32?=9.69698537e-43
     b44: e59f0064     	ldr	r0, [pc, #0x64]         @ 0xbb0 <midi_reader_connect+0x78>  // u32=0x1f4; f32?=7.00649232e-43
     b48: e08f6004     	add	r6, pc, r4
     b4c: e08f0000     	add	r0, pc, r0
     b50: e1a01006     	mov	r1, r6
     b54: ebffff0e     	bl	0x794 <.plt+0xb0>       @ imm = #-0x3c8  // CALL post
     b58: e1a00006     	mov	r0, r6
     b5c: e3a01000     	mov	r1, #0
     b60: ebffff02     	bl	0x770 <.plt+0x8c>       @ imm = #-0x3f8  // CALL open
     b64: e3500000     	cmp	r0, #0
     b68: e585001c     	str	r0, [r5, #0x1c]
     b6c: ba000004     	blt	0xb84 <midi_reader_connect+0x4c> @ imm = #0x10
     b70: e59f203c     	ldr	r2, [pc, #0x3c]         @ 0xbb4 <midi_reader_connect+0x7c>  // u32=0x214; f32?=7.45490783e-43
     b74: e1a01006     	mov	r1, r6
     b78: e8bd4070     	pop	{r4, r5, r6, lr}
     b7c: e08f0002     	add	r0, pc, r2
     b80: eaffff03     	b	0x794 <.plt+0xb0>       @ imm = #-0x3f4  // CALL post
     b84: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0xbb8 <midi_reader_connect+0x80>  // u32=0x1c8; f32?=6.389921e-43
     b88: e1a00005     	mov	r0, r5
     b8c: e1a02006     	mov	r2, r6
     b90: e08f1001     	add	r1, pc, r1
     b94: ebffff10     	bl	0x7dc <.plt+0xf8>       @ imm = #-0x3c0  // CALL pd_error
     b98: e59f301c     	ldr	r3, [pc, #0x1c]         @ 0xbbc <midi_reader_connect+0x84>  // u32=0x1d8; f32?=6.61412875e-43
     b9c: e1a01006     	mov	r1, r6
     ba0: e08f0003     	add	r0, pc, r3
     ba4: e8bd4070     	pop	{r4, r5, r6, lr}
     ba8: eafffef9     	b	0x794 <.plt+0xb0>       @ imm = #-0x41c  // CALL post
     bac: b4 02 00 00  	.word	0x000002b4
     bb0: f4 01 00 00  	.word	0x000001f4
     bb4: 14 02 00 00  	.word	0x00000214
     bb8: c8 01 00 00  	.word	0x000001c8
     bbc: d8 01 00 00  	.word	0x000001d8

