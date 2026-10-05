0000085c <register_tm_clones>:
     85c: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x89c <register_tm_clones+0x40>
     860: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x8a0 <register_tm_clones+0x44>
     864: e08f0000     	add	r0, pc, r0
     868: e08f3003     	add	r3, pc, r3
     86c: e0431000     	sub	r1, r3, r0
     870: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x8a4 <register_tm_clones+0x48>
     874: e1a01141     	asr	r1, r1, #2
     878: e08f3003     	add	r3, pc, r3
     87c: e0811fa1     	add	r1, r1, r1, lsr #31
     880: e1b010c1     	asrs	r1, r1, #1
     884: 012fff1e     	bxeq	lr
     888: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x8a8 <register_tm_clones+0x4c>
     88c: e7933002     	ldr	r3, [r3, r2]
     890: e3530000     	cmp	r3, #0
     894: 012fff1e     	bxeq	lr
     898: e12fff13     	bx	r3
     89c: 24 08 01 00  	.word	0x00010824
     8a0: 20 08 01 00  	.word	0x00010820
     8a4: 80 07 01 00  	.word	0x00010780
     8a8: 88 00 00 00  	.word	0x00000088

