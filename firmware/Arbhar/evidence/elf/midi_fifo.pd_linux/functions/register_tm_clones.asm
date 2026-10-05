0000083c <register_tm_clones>:
     83c: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x87c <register_tm_clones+0x40>
     840: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x880 <register_tm_clones+0x44>
     844: e08f0000     	add	r0, pc, r0
     848: e08f3003     	add	r3, pc, r3
     84c: e0431000     	sub	r1, r3, r0
     850: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x884 <register_tm_clones+0x48>
     854: e1a01141     	asr	r1, r1, #2
     858: e08f3003     	add	r3, pc, r3
     85c: e0811fa1     	add	r1, r1, r1, lsr #31
     860: e1b010c1     	asrs	r1, r1, #1
     864: 012fff1e     	bxeq	lr
     868: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x888 <register_tm_clones+0x4c>
     86c: e7933002     	ldr	r3, [r3, r2]
     870: e3530000     	cmp	r3, #0
     874: 012fff1e     	bxeq	lr
     878: e12fff13     	bx	r3
     87c: 48 18 01 00  	.word	0x00011848
     880: 44 18 01 00  	.word	0x00011844
     884: a0 17 01 00  	.word	0x000117a0
     888: 8c 00 00 00  	.word	0x0000008c

