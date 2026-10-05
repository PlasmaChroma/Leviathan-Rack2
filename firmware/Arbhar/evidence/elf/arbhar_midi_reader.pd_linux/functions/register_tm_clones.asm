00000474 <register_tm_clones>:
     474: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x4b4 <register_tm_clones+0x40>
     478: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x4b8 <register_tm_clones+0x44>
     47c: e08f0000     	add	r0, pc, r0
     480: e08f3003     	add	r3, pc, r3
     484: e0431000     	sub	r1, r3, r0
     488: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x4bc <register_tm_clones+0x48>
     48c: e1a01141     	asr	r1, r1, #2
     490: e08f3003     	add	r3, pc, r3
     494: e0811fa1     	add	r1, r1, r1, lsr #31
     498: e1b010c1     	asrs	r1, r1, #1
     49c: 012fff1e     	bxeq	lr
     4a0: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x4c0 <register_tm_clones+0x4c>
     4a4: e7933002     	ldr	r3, [r3, r2]
     4a8: e3530000     	cmp	r3, #0
     4ac: 012fff1e     	bxeq	lr
     4b0: e12fff13     	bx	r3
     4b4: bc 0b 01 00  	.word	0x00010bbc
     4b8: b8 0b 01 00  	.word	0x00010bb8
     4bc: 68 0b 01 00  	.word	0x00010b68
     4c0: 38 00 00 00  	.word	0x00000038

