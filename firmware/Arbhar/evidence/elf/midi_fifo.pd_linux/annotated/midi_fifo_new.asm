00000954 <midi_fifo_new>:
     954: e59f304c     	ldr	r3, [pc, #0x4c]         @ 0x9a8 <midi_fifo_new+0x54>  // u32=0x11734; f32?=1.00159209e-40
     958: e92d4070     	push	{r4, r5, r6, lr}
     95c: e08f0003     	add	r0, pc, r3
     960: e59f5044     	ldr	r5, [pc, #0x44]         @ 0x9ac <midi_fifo_new+0x58>  // u32=0x11688; f32?=9.99181857e-41
     964: e5900000     	ldr	r0, [r0]
     968: ebffff63     	bl	0x6fc <.plt+0x38>       @ imm = #-0x274  // CALL pd_new
     96c: e59f103c     	ldr	r1, [pc, #0x3c]         @ 0x9b0 <midi_fifo_new+0x5c>  // u32=0x70; f32?=1.56945428e-43
     970: e08f5005     	add	r5, pc, r5
     974: e3e02000     	mvn	r2, #0
     978: e5802024     	str	r2, [r0, #0x24]
     97c: e1a04000     	mov	r4, r0
     980: e7951001     	ldr	r1, [r5, r1]
     984: ebffff7a     	bl	0x774 <.plt+0xb0>       @ imm = #-0x218  // CALL outlet_new
     988: e59fc024     	ldr	r12, [pc, #0x24]        @ 0x9b4 <midi_fifo_new+0x60>  // u32=0x7c; f32?=1.7376101e-43
     98c: e584001c     	str	r0, [r4, #0x1c]
     990: e1a00004     	mov	r0, r4
     994: e795100c     	ldr	r1, [r5, r12]
     998: ebffff63     	bl	0x72c <.plt+0x68>       @ imm = #-0x274  // CALL clock_new
     99c: e5840020     	str	r0, [r4, #0x20]
     9a0: e1a00004     	mov	r0, r4
     9a4: e8bd8070     	pop	{r4, r5, r6, pc}
     9a8: 34 17 01 00  	.word	0x00011734
     9ac: 88 16 01 00  	.word	0x00011688
     9b0: 70 00 00 00  	.word	0x00000070
     9b4: 7c 00 00 00  	.word	0x0000007c

