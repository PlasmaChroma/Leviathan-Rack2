000021a8 <register_tm_clones>:
    21a8: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x21e8 <register_tm_clones+0x40>  // u32=0x15f5c; f32?=1.26043994e-40
    21ac: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x21ec <register_tm_clones+0x44>  // u32=0x15f58; f32?=1.26038389e-40
    21b0: e08f0000     	add	r0, pc, r0
    21b4: e08f3003     	add	r3, pc, r3
    21b8: e0431000     	sub	r1, r3, r0
    21bc: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x21f0 <register_tm_clones+0x48>  // u32=0x15e34; f32?=1.2562921e-40
    21c0: e1a01141     	asr	r1, r1, #2
    21c4: e08f3003     	add	r3, pc, r3
    21c8: e0811fa1     	add	r1, r1, r1, lsr #31
    21cc: e1b010c1     	asrs	r1, r1, #1
    21d0: 012fff1e     	bxeq	lr
    21d4: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x21f4 <register_tm_clones+0x4c>  // u32=0xc4; f32?=2.74654499e-43
    21d8: e7933002     	ldr	r3, [r3, r2]
    21dc: e3530000     	cmp	r3, #0
    21e0: 012fff1e     	bxeq	lr
    21e4: e12fff13     	bx	r3
    21e8: 5c 5f 01 00  	.word	0x00015f5c
    21ec: 58 5f 01 00  	.word	0x00015f58
    21f0: 34 5e 01 00  	.word	0x00015e34
    21f4: c4 00 00 00  	.word	0x000000c4

