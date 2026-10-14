00002780 <register_tm_clones>:
    2780: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x27c0 <register_tm_clones+0x40>  // u32=0x1ba08; f32?=1.58570934e-40
    2784: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x27c4 <register_tm_clones+0x44>  // u32=0x1ba04; f32?=1.58565329e-40
    2788: e08f0000     	add	r0, pc, r0
    278c: e08f3003     	add	r3, pc, r3
    2790: e0431000     	sub	r1, r3, r0
    2794: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x27c8 <register_tm_clones+0x48>  // u32=0x1b85c; f32?=1.57971178e-40
    2798: e1a01141     	asr	r1, r1, #2
    279c: e08f3003     	add	r3, pc, r3
    27a0: e0811fa1     	add	r1, r1, r1, lsr #31
    27a4: e1b010c1     	asrs	r1, r1, #1
    27a8: 012fff1e     	bxeq	lr
    27ac: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x27cc <register_tm_clones+0x4c>  // u32=0x150; f32?=4.70836284e-43
    27b0: e7933002     	ldr	r3, [r3, r2]
    27b4: e3530000     	cmp	r3, #0
    27b8: 012fff1e     	bxeq	lr
    27bc: e12fff13     	bx	r3
    27c0: 08 ba 01 00  	.word	0x0001ba08
    27c4: 04 ba 01 00  	.word	0x0001ba04
    27c8: 5c b8 01 00  	.word	0x0001b85c
    27cc: 50 01 00 00  	.word	0x00000150

