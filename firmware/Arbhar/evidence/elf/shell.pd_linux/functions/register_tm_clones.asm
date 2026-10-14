00000bec <register_tm_clones>:
     bec: e59f0038     	ldr	r0, [pc, #0x38]         @ 0xc2c <register_tm_clones+0x40>
     bf0: e59f3038     	ldr	r3, [pc, #0x38]         @ 0xc30 <register_tm_clones+0x44>
     bf4: e08f0000     	add	r0, pc, r0
     bf8: e08f3003     	add	r3, pc, r3
     bfc: e0431000     	sub	r1, r3, r0
     c00: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0xc34 <register_tm_clones+0x48>
     c04: e1a01141     	asr	r1, r1, #2
     c08: e08f3003     	add	r3, pc, r3
     c0c: e0811fa1     	add	r1, r1, r1, lsr #31
     c10: e1b010c1     	asrs	r1, r1, #1
     c14: 012fff1e     	bxeq	lr
     c18: e59f2018     	ldr	r2, [pc, #0x18]         @ 0xc38 <register_tm_clones+0x4c>
     c1c: e7933002     	ldr	r3, [r3, r2]
     c20: e3530000     	cmp	r3, #0
     c24: 012fff1e     	bxeq	lr
     c28: e12fff13     	bx	r3
     c2c: e8 14 01 00  	.word	0x000114e8
     c30: e4 14 01 00  	.word	0x000114e4
     c34: f0 13 01 00  	.word	0x000113f0
     c38: d8 00 00 00  	.word	0x000000d8

