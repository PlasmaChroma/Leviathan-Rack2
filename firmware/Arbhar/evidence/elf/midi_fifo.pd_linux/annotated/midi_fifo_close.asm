000008f8 <midi_fifo_close>:
     8f8: e92d4010     	push	{r4, lr}
     8fc: e1a04000     	mov	r4, r0
     900: e5900020     	ldr	r0, [r0, #0x20]
     904: ebffff91     	bl	0x750 <.plt+0x8c>       @ imm = #-0x1bc  // CALL clock_unset
     908: e5940024     	ldr	r0, [r4, #0x24]
     90c: e3500000     	cmp	r0, #0
     910: b8bd8010     	poplt	{r4, pc}
     914: ebffffab     	bl	0x7c8 <.plt+0x104>      @ imm = #-0x154  // CALL close
     918: e59f0010     	ldr	r0, [pc, #0x10]         @ 0x930 <midi_fifo_close+0x38>  // u32=0x6c8; f32?=2.43265413e-42
     91c: e3e03000     	mvn	r3, #0
     920: e5843024     	str	r3, [r4, #0x24]
     924: e08f0000     	add	r0, pc, r0
     928: e8bd4010     	pop	{r4, lr}
     92c: eaffff8d     	b	0x768 <.plt+0xa4>       @ imm = #-0x1cc  // CALL post
     930: c8 06 00 00  	.word	0x000006c8

