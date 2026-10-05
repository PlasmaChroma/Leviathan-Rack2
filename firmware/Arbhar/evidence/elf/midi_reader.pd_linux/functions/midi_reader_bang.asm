00000918 <midi_reader_bang>:
     918: e52de004     	str	lr, [sp, #-0x4]!
     91c: e24dd044     	sub	sp, sp, #68
     920: e3a02040     	mov	r2, #64
     924: e590001c     	ldr	r0, [r0, #0x1c]
     928: e1a0100d     	mov	r1, sp
     92c: ebffff7a     	bl	0x71c <.plt+0x38>       @ imm = #-0x218
     930: e1a01000     	mov	r1, r0
     934: e59f000c     	ldr	r0, [pc, #0xc]          @ 0x948 <midi_reader_bang+0x30>
     938: e08f0000     	add	r0, pc, r0
     93c: ebffff94     	bl	0x794 <.plt+0xb0>       @ imm = #-0x1b0
     940: e28dd044     	add	sp, sp, #68
     944: e49df004     	ldr	pc, [sp], #4
     948: f4 03 00 00  	.word	0x000003f4

