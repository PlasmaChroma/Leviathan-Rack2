00000e70 <frame_dummy>:
     e70: e59f0038     	ldr	r0, [pc, #0x38]         @ 0xeb0 <frame_dummy+0x40>  // u32=0x1108c; f32?=9.77713965e-41
     e74: e59f3038     	ldr	r3, [pc, #0x38]         @ 0xeb4 <frame_dummy+0x44>  // u32=0x11178; f32?=9.81021029e-41
     e78: e08f0000     	add	r0, pc, r0
     e7c: e5902000     	ldr	r2, [r0]
     e80: e08f3003     	add	r3, pc, r3
     e84: e3520000     	cmp	r2, #0
     e88: 1a000000     	bne	0xe90 <frame_dummy+0x20> @ imm = #0x0
     e8c: eaffffc9     	b	0xdb8 <register_tm_clones> @ imm = #-0xdc
     e90: e59f2020     	ldr	r2, [pc, #0x20]         @ 0xeb8 <frame_dummy+0x48>  // u32=0xe0; f32?=3.13890856e-43
     e94: e7933002     	ldr	r3, [r3, r2]
     e98: e3530000     	cmp	r3, #0
     e9c: 0afffffa     	beq	0xe8c <frame_dummy+0x1c> @ imm = #-0x18
     ea0: e92d4010     	push	{r4, lr}
     ea4: e12fff33     	blx	r3
     ea8: e8bd4010     	pop	{r4, lr}
     eac: eaffffc1     	b	0xdb8 <register_tm_clones> @ imm = #-0xfc
     eb0: 8c 10 01 00  	.word	0x0001108c
     eb4: 78 11 01 00  	.word	0x00011178
     eb8: e0 00 00 00  	.word	0x000000e0

