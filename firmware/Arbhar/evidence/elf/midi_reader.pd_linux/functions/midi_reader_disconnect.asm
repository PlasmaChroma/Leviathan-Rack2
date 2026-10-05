00000a0c <midi_reader_disconnect>:
     a0c: e92d4010     	push	{r4, lr}
     a10: e1a04000     	mov	r4, r0
     a14: e5900024     	ldr	r0, [r0, #0x24]
     a18: ebffff57     	bl	0x77c <.plt+0x98>       @ imm = #-0x2a4
     a1c: e594001c     	ldr	r0, [r4, #0x1c]
     a20: e3500000     	cmp	r0, #0
     a24: b8bd8010     	poplt	{r4, pc}
     a28: ebffff68     	bl	0x7d0 <.plt+0xec>       @ imm = #-0x260
     a2c: e59f0014     	ldr	r0, [pc, #0x14]         @ 0xa48 <midi_reader_disconnect+0x3c>
     a30: e3e03000     	mvn	r3, #0
     a34: e2841028     	add	r1, r4, #40
     a38: e584301c     	str	r3, [r4, #0x1c]
     a3c: e08f0000     	add	r0, pc, r0
     a40: e8bd4010     	pop	{r4, lr}
     a44: eaffff52     	b	0x794 <.plt+0xb0>       @ imm = #-0x2b8
     a48: 64 03 00 00  	.word	0x00000364

