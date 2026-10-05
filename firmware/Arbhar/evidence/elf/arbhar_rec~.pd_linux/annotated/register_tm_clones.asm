000028f8 <register_tm_clones>:
    28f8: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x2938 <register_tm_clones+0x40>  // u32=0x178a4; f32?=1.35113198e-40
    28fc: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x293c <register_tm_clones+0x44>  // u32=0x178a0; f32?=1.35107593e-40
    2900: e08f0000     	add	r0, pc, r0
    2904: e08f3003     	add	r3, pc, r3
    2908: e0431000     	sub	r1, r3, r0
    290c: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x2940 <register_tm_clones+0x48>  // u32=0x176e4; f32?=1.34485416e-40
    2910: e1a01141     	asr	r1, r1, #2
    2914: e08f3003     	add	r3, pc, r3
    2918: e0811fa1     	add	r1, r1, r1, lsr #31
    291c: e1b010c1     	asrs	r1, r1, #1
    2920: 012fff1e     	bxeq	lr
    2924: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x2944 <register_tm_clones+0x4c>  // u32=0x168; f32?=5.04467447e-43
    2928: e7933002     	ldr	r3, [r3, r2]
    292c: e3530000     	cmp	r3, #0
    2930: 012fff1e     	bxeq	lr
    2934: e12fff13     	bx	r3
    2938: a4 78 01 00  	.word	0x000178a4
    293c: a0 78 01 00  	.word	0x000178a0
    2940: e4 76 01 00  	.word	0x000176e4
    2944: 68 01 00 00  	.word	0x00000168

