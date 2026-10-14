00000950 <midi_reader_connect_device>:
     950: e92d4070     	push	{r4, r5, r6, lr}
     954: e24dd068     	sub	sp, sp, #104
     958: e28d4004     	add	r4, sp, #4
     95c: e1a06003     	mov	r6, r3
     960: e2801028     	add	r1, r0, #40
     964: e1a05000     	mov	r5, r0
     968: e3a02064     	mov	r2, #100
     96c: e1a00003     	mov	r0, r3
     970: ebffff9c     	bl	0x7e8 <.plt+0x104>      @ imm = #-0x190
     974: e3a02064     	mov	r2, #100
     978: e1a00006     	mov	r0, r6
     97c: e1a01004     	mov	r1, r4
     980: ebffff98     	bl	0x7e8 <.plt+0x104>      @ imm = #-0x1a0
     984: e59f0070     	ldr	r0, [pc, #0x70]         @ 0x9fc <midi_reader_connect_device+0xac>
     988: e1a01004     	mov	r1, r4
     98c: e08f0000     	add	r0, pc, r0
     990: ebffff7f     	bl	0x794 <.plt+0xb0>       @ imm = #-0x204
     994: e1a00004     	mov	r0, r4
     998: e3a01b02     	mov	r1, #2048
     99c: ebffff73     	bl	0x770 <.plt+0x8c>       @ imm = #-0x234
     9a0: e3500000     	cmp	r0, #0
     9a4: e585001c     	str	r0, [r5, #0x1c]
     9a8: ba000008     	blt	0x9d0 <midi_reader_connect_device+0x80> @ imm = #0x20
     9ac: e59f204c     	ldr	r2, [pc, #0x4c]         @ 0xa00 <midi_reader_connect_device+0xb0>
     9b0: e1a01004     	mov	r1, r4
     9b4: e08f0002     	add	r0, pc, r2
     9b8: ebffff75     	bl	0x794 <.plt+0xb0>       @ imm = #-0x22c
     9bc: e5950024     	ldr	r0, [r5, #0x24]
     9c0: eeb20b04     	vmov.f64	d0, #1.000000e+01
     9c4: ebffff60     	bl	0x74c <.plt+0x68>       @ imm = #-0x280
     9c8: e28dd068     	add	sp, sp, #104
     9cc: e8bd8070     	pop	{r4, r5, r6, pc}
     9d0: e59f102c     	ldr	r1, [pc, #0x2c]         @ 0xa04 <midi_reader_connect_device+0xb4>
     9d4: e1a00005     	mov	r0, r5
     9d8: e1a02004     	mov	r2, r4
     9dc: e08f1001     	add	r1, pc, r1
     9e0: ebffff7d     	bl	0x7dc <.plt+0xf8>       @ imm = #-0x20c
     9e4: e59f301c     	ldr	r3, [pc, #0x1c]         @ 0xa08 <midi_reader_connect_device+0xb8>
     9e8: e1a01004     	mov	r1, r4
     9ec: e08f0003     	add	r0, pc, r3
     9f0: ebffff67     	bl	0x794 <.plt+0xb0>       @ imm = #-0x264
     9f4: e28dd068     	add	sp, sp, #104
     9f8: e8bd8070     	pop	{r4, r5, r6, pc}
     9fc: b4 03 00 00  	.word	0x000003b4
     a00: dc 03 00 00  	.word	0x000003dc
     a04: 7c 03 00 00  	.word	0x0000037c
     a08: 8c 03 00 00  	.word	0x0000038c

