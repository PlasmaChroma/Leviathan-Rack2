00000d40 <frame_dummy>:
     d40: e59f0038     	ldr	r0, [pc, #0x38]         @ 0xd80 <frame_dummy+0x40>  // u32=0x141cc; f32?=1.15438967e-40
     d44: e59f3038     	ldr	r3, [pc, #0x38]         @ 0xd84 <frame_dummy+0x44>  // u32=0x142a8; f32?=1.15747253e-40
     d48: e08f0000     	add	r0, pc, r0
     d4c: e5902000     	ldr	r2, [r0]
     d50: e08f3003     	add	r3, pc, r3
     d54: e3520000     	cmp	r2, #0
     d58: 1a000000     	bne	0xd60 <frame_dummy+0x20> @ imm = #0x0
     d5c: eaffffc9     	b	0xc88 <register_tm_clones> @ imm = #-0xdc
     d60: e59f2020     	ldr	r2, [pc, #0x20]         @ 0xd88 <frame_dummy+0x48>  // u32=0xcc; f32?=2.85864887e-43
     d64: e7933002     	ldr	r3, [r3, r2]
     d68: e3530000     	cmp	r3, #0
     d6c: 0afffffa     	beq	0xd5c <frame_dummy+0x1c> @ imm = #-0x18
     d70: e92d4010     	push	{r4, lr}
     d74: e12fff33     	blx	r3
     d78: e8bd4010     	pop	{r4, lr}
     d7c: eaffffc1     	b	0xc88 <register_tm_clones> @ imm = #-0xfc
     d80: cc 41 01 00  	.word	0x000141cc
     d84: a8 42 01 00  	.word	0x000142a8
     d88: cc 00 00 00  	.word	0x000000cc
     d8c: 00 00 00 00  	.word	0x00000000

