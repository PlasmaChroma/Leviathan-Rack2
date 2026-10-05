00000c88 <register_tm_clones>:
     c88: e59f1038     	ldr	r1, [pc, #0x38]         @ 0xcc8 <register_tm_clones+0x40>  // u32=0x14440; f32?=1.16318983e-40
     c8c: e59f0038     	ldr	r0, [pc, #0x38]         @ 0xccc <register_tm_clones+0x44>  // u32=0x1443c; f32?=1.16313378e-40
     c90: e08f1001     	add	r1, pc, r1
     c94: e08f0000     	add	r0, pc, r0
     c98: e0411000     	sub	r1, r1, r0
     c9c: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0xcd0 <register_tm_clones+0x48>  // u32=0x14354; f32?=1.15988276e-40
     ca0: e1a01141     	asr	r1, r1, #2
     ca4: e08f3003     	add	r3, pc, r3
     ca8: e0811fa1     	add	r1, r1, r1, lsr #31
     cac: e1b010c1     	asrs	r1, r1, #1
     cb0: 012fff1e     	bxeq	lr
     cb4: e59f2018     	ldr	r2, [pc, #0x18]         @ 0xcd4 <register_tm_clones+0x4c>  // u32=0xd0; f32?=2.91470081e-43
     cb8: e7933002     	ldr	r3, [r3, r2]
     cbc: e3530000     	cmp	r3, #0
     cc0: 012fff1e     	bxeq	lr
     cc4: e12fff13     	bx	r3
     cc8: 40 44 01 00  	.word	0x00014440
     ccc: 3c 44 01 00  	.word	0x0001443c
     cd0: 54 43 01 00  	.word	0x00014354
     cd4: d0 00 00 00  	.word	0x000000d0

