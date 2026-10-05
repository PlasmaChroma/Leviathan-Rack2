00000c20 <register_tm_clones>:
     c20: e59f0038     	ldr	r0, [pc, #0x38]         @ 0xc60 <register_tm_clones+0x40>
     c24: e59f3038     	ldr	r3, [pc, #0x38]         @ 0xc64 <register_tm_clones+0x44>
     c28: e08f0000     	add	r0, pc, r0
     c2c: e08f3003     	add	r3, pc, r3
     c30: e0431000     	sub	r1, r3, r0
     c34: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0xc68 <register_tm_clones+0x48>
     c38: e1a01141     	asr	r1, r1, #2
     c3c: e08f3003     	add	r3, pc, r3
     c40: e0811fa1     	add	r1, r1, r1, lsr #31
     c44: e1b010c1     	asrs	r1, r1, #1
     c48: 012fff1e     	bxeq	lr
     c4c: e59f2018     	ldr	r2, [pc, #0x18]         @ 0xc6c <register_tm_clones+0x4c>
     c50: e7933002     	ldr	r3, [r3, r2]
     c54: e3530000     	cmp	r3, #0
     c58: 012fff1e     	bxeq	lr
     c5c: e12fff13     	bx	r3
     c60: b4 24 01 00  	.word	0x000124b4
     c64: b0 24 01 00  	.word	0x000124b0
     c68: bc 23 01 00  	.word	0x000123bc
     c6c: d4 00 00 00  	.word	0x000000d4

