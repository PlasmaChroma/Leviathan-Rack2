000021b8 <register_tm_clones>:
    21b8: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x21f8 <register_tm_clones+0x40>  // u32=0x15f4c; f32?=1.26021573e-40
    21bc: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x21fc <register_tm_clones+0x44>  // u32=0x15f48; f32?=1.26015968e-40
    21c0: e08f0000     	add	r0, pc, r0
    21c4: e08f3003     	add	r3, pc, r3
    21c8: e0431000     	sub	r1, r3, r0
    21cc: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x2200 <register_tm_clones+0x48>  // u32=0x15e24; f32?=1.25606789e-40
    21d0: e1a01141     	asr	r1, r1, #2
    21d4: e08f3003     	add	r3, pc, r3
    21d8: e0811fa1     	add	r1, r1, r1, lsr #31
    21dc: e1b010c1     	asrs	r1, r1, #1
    21e0: 012fff1e     	bxeq	lr
    21e4: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x2204 <register_tm_clones+0x4c>  // u32=0xd0; f32?=2.91470081e-43
    21e8: e7933002     	ldr	r3, [r3, r2]
    21ec: e3530000     	cmp	r3, #0
    21f0: 012fff1e     	bxeq	lr
    21f4: e12fff13     	bx	r3
    21f8: 4c 5f 01 00  	.word	0x00015f4c
    21fc: 48 5f 01 00  	.word	0x00015f48
    2200: 24 5e 01 00  	.word	0x00015e24
    2204: d0 00 00 00  	.word	0x000000d0

