000022d8 <register_tm_clones>:
    22d8: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x2318 <register_tm_clones+0x40>  // u32=0x15e40; f32?=1.25646026e-40
    22dc: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x231c <register_tm_clones+0x44>  // u32=0x15e3c; f32?=1.2564042e-40
    22e0: e08f0000     	add	r0, pc, r0
    22e4: e08f3003     	add	r3, pc, r3
    22e8: e0431000     	sub	r1, r3, r0
    22ec: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x2320 <register_tm_clones+0x48>  // u32=0x15d04; f32?=1.25203215e-40
    22f0: e1a01141     	asr	r1, r1, #2
    22f4: e08f3003     	add	r3, pc, r3
    22f8: e0811fa1     	add	r1, r1, r1, lsr #31
    22fc: e1b010c1     	asrs	r1, r1, #1
    2300: 012fff1e     	bxeq	lr
    2304: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x2324 <register_tm_clones+0x4c>  // u32=0xe0; f32?=3.13890856e-43
    2308: e7933002     	ldr	r3, [r3, r2]
    230c: e3530000     	cmp	r3, #0
    2310: 012fff1e     	bxeq	lr
    2314: e12fff13     	bx	r3
    2318: 40 5e 01 00  	.word	0x00015e40
    231c: 3c 5e 01 00  	.word	0x00015e3c
    2320: 04 5d 01 00  	.word	0x00015d04
    2324: e0 00 00 00  	.word	0x000000e0

