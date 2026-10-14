00000934 <midi_fifo_free>:
     934: e92d4010     	push	{r4, lr}
     938: e1a04000     	mov	r4, r0
     93c: ebffff71     	bl	0x708 <.plt+0x44>       @ imm = #-0x23c
     940: e5940020     	ldr	r0, [r4, #0x20]
     944: e3500000     	cmp	r0, #0
     948: 08bd8010     	popeq	{r4, pc}
     94c: e8bd4010     	pop	{r4, lr}
     950: eaffff96     	b	0x7b0 <.plt+0xec>       @ imm = #-0x1a8

