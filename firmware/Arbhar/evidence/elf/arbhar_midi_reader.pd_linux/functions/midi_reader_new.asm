00000534 <midi_reader_new>:
     534: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x55c <midi_reader_new+0x28>
     538: e92d4010     	push	{r4, lr}
     53c: e79f0003     	ldr	r0, [pc, r3]
     540: ebffffa5     	bl	0x3dc <.plt+0x2c>       @ imm = #-0x16c
     544: e1a04000     	mov	r4, r0
     548: e59f0010     	ldr	r0, [pc, #0x10]         @ 0x560 <midi_reader_new+0x2c>
     54c: e08f0000     	add	r0, pc, r0
     550: ebffffa7     	bl	0x3f4 <.plt+0x44>       @ imm = #-0x164
     554: e1a00004     	mov	r0, r4
     558: e8bd8010     	pop	{r4, pc}
     55c: 00 0b 01 00  	.word	0x00010b00
     560: 80 00 00 00  	.word	0x00000080

