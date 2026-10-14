00000c3c <deregister_tm_clones>:
     c3c: e59f3034     	ldr	r3, [pc, #0x34]         @ 0xc78 <deregister_tm_clones+0x3c>  // u32=0x1448c; f32?=1.16425482e-40
     c40: e59f0034     	ldr	r0, [pc, #0x34]         @ 0xc7c <deregister_tm_clones+0x40>  // u32=0x14488; f32?=1.16419876e-40
     c44: e08f3003     	add	r3, pc, r3
     c48: e08f0000     	add	r0, pc, r0
     c4c: e2833003     	add	r3, r3, #3
     c50: e0433000     	sub	r3, r3, r0
     c54: e3530006     	cmp	r3, #6
     c58: e59f3020     	ldr	r3, [pc, #0x20]         @ 0xc80 <deregister_tm_clones+0x44>  // u32=0x1439c; f32?=1.1608917e-40
     c5c: e08f3003     	add	r3, pc, r3
     c60: 912fff1e     	bxls	lr
     c64: e59f2018     	ldr	r2, [pc, #0x18]         @ 0xc84 <deregister_tm_clones+0x48>  // u32=0xb8; f32?=2.57838917e-43
     c68: e7933002     	ldr	r3, [r3, r2]
     c6c: e3530000     	cmp	r3, #0
     c70: 012fff1e     	bxeq	lr
     c74: e12fff13     	bx	r3
     c78: 8c 44 01 00  	.word	0x0001448c
     c7c: 88 44 01 00  	.word	0x00014488
     c80: 9c 43 01 00  	.word	0x0001439c
     c84: b8 00 00 00  	.word	0x000000b8

