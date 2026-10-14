000009b8 <midi_fifo_open>:
     9b8: e92d4010     	push	{r4, lr}
     9bc: e1a04000     	mov	r4, r0
     9c0: e59f0050     	ldr	r0, [pc, #0x50]         @ 0xa18 <midi_fifo_open+0x60>  // u32=0x634; f32?=2.22526196e-42
     9c4: e3a01b02     	mov	r1, #2048
     9c8: e08f0000     	add	r0, pc, r0
     9cc: ebffff5c     	bl	0x744 <.plt+0x80>       @ imm = #-0x290  // CALL open
     9d0: e3500000     	cmp	r0, #0
     9d4: e5840024     	str	r0, [r4, #0x24]
     9d8: ba000006     	blt	0x9f8 <midi_fifo_open+0x40> @ imm = #0x18
     9dc: e59f1038     	ldr	r1, [pc, #0x38]         @ 0xa1c <midi_fifo_open+0x64>  // u32=0x64c; f32?=2.25889312e-42
     9e0: e08f0001     	add	r0, pc, r1
     9e4: ebffff5f     	bl	0x768 <.plt+0xa4>       @ imm = #-0x284  // CALL post
     9e8: e5940020     	ldr	r0, [r4, #0x20]
     9ec: eeb20b04     	vmov.f64	d0, #1.000000e+01
     9f0: e8bd4010     	pop	{r4, lr}
     9f4: eaffff46     	b	0x714 <.plt+0x50>       @ imm = #-0x2e8  // CALL clock_delay
     9f8: ebffff57     	bl	0x75c <.plt+0x98>       @ imm = #-0x2a4  // CALL __errno_location
     9fc: e5900000     	ldr	r0, [r0]
     a00: ebffff46     	bl	0x720 <.plt+0x5c>       @ imm = #-0x2e8  // CALL strerror
     a04: e59f2014     	ldr	r2, [pc, #0x14]         @ 0xa20 <midi_fifo_open+0x68>  // u32=0x604; f32?=2.15799964e-42
     a08: e8bd4010     	pop	{r4, lr}
     a0c: e1a01000     	mov	r1, r0
     a10: e08f0002     	add	r0, pc, r2
     a14: eaffff53     	b	0x768 <.plt+0xa4>       @ imm = #-0x2b4  // CALL post
     a18: 34 06 00 00  	.word	0x00000634
     a1c: 4c 06 00 00  	.word	0x0000064c
     a20: 04 06 00 00  	.word	0x00000604

