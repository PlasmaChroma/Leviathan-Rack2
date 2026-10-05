000007a0 <register_tm_clones>:
     7a0: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x7e0 <register_tm_clones+0x40>
     7a4: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x7e4 <register_tm_clones+0x44>
     7a8: e08f0000     	add	r0, pc, r0
     7ac: e08f3003     	add	r3, pc, r3
     7b0: e0431000     	sub	r1, r3, r0
     7b4: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x7e8 <register_tm_clones+0x48>
     7b8: e1a01141     	asr	r1, r1, #2
     7bc: e08f3003     	add	r3, pc, r3
     7c0: e0811fa1     	add	r1, r1, r1, lsr #31
     7c4: e1b010c1     	asrs	r1, r1, #1
     7c8: 012fff1e     	bxeq	lr
     7cc: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x7ec <register_tm_clones+0x4c>
     7d0: e7933002     	ldr	r3, [r3, r2]
     7d4: e3530000     	cmp	r3, #0
     7d8: 012fff1e     	bxeq	lr
     7dc: e12fff13     	bx	r3
     7e0: cc 48 01 00  	.word	0x000148cc
     7e4: c8 48 01 00  	.word	0x000148c8
     7e8: 3c 48 01 00  	.word	0x0001483c
     7ec: 74 00 00 00  	.word	0x00000074

