00000900 <register_tm_clones>:
     900: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x940 <register_tm_clones+0x40>
     904: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x944 <register_tm_clones+0x44>
     908: e08f0000     	add	r0, pc, r0
     90c: e08f3003     	add	r3, pc, r3
     910: e0431000     	sub	r1, r3, r0
     914: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x948 <register_tm_clones+0x48>
     918: e1a01141     	asr	r1, r1, #2
     91c: e08f3003     	add	r3, pc, r3
     920: e0811fa1     	add	r1, r1, r1, lsr #31
     924: e1b010c1     	asrs	r1, r1, #1
     928: 012fff1e     	bxeq	lr
     92c: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x94c <register_tm_clones+0x4c>
     930: e7933002     	ldr	r3, [r3, r2]
     934: e3530000     	cmp	r3, #0
     938: 012fff1e     	bxeq	lr
     93c: e12fff13     	bx	r3
     940: 94 17 01 00  	.word	0x00011794
     944: 90 17 01 00  	.word	0x00011790
     948: dc 16 01 00  	.word	0x000116dc
     94c: 9c 00 00 00  	.word	0x0000009c

