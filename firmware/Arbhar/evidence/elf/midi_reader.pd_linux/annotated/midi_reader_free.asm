00000b04 <midi_reader_free>:
     b04: e92d4010     	push	{r4, lr}
     b08: e1a04000     	mov	r4, r0
     b0c: e590001c     	ldr	r0, [r0, #0x1c]
     b10: e3500000     	cmp	r0, #0
     b14: ba000000     	blt	0xb1c <midi_reader_free+0x18> @ imm = #0x0
     b18: ebffff2c     	bl	0x7d0 <.plt+0xec>       @ imm = #-0x350  // CALL close
     b1c: e5940020     	ldr	r0, [r4, #0x20]
     b20: ebffff00     	bl	0x728 <.plt+0x44>       @ imm = #-0x400  // CALL outlet_free
     b24: e5940024     	ldr	r0, [r4, #0x24]
     b28: e3500000     	cmp	r0, #0
     b2c: 08bd8010     	popeq	{r4, pc}
     b30: e8bd4010     	pop	{r4, lr}
     b34: eaffff22     	b	0x7c4 <.plt+0xe0>       @ imm = #-0x378  // CALL clock_free

