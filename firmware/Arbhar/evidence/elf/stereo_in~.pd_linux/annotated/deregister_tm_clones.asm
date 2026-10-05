00002294 <deregister_tm_clones>:
    2294: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x22c8 <deregister_tm_clones+0x34>  // u32=0x15e84; f32?=1.25741314e-40
    2298: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x22cc <deregister_tm_clones+0x38>  // u32=0x15e80; f32?=1.25735709e-40
    229c: e08f0000     	add	r0, pc, r0
    22a0: e08f3003     	add	r3, pc, r3
    22a4: e1530000     	cmp	r3, r0
    22a8: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x22d0 <deregister_tm_clones+0x3c>  // u32=0x15d4c; f32?=1.25304109e-40
    22ac: e08f3003     	add	r3, pc, r3
    22b0: 012fff1e     	bxeq	lr
    22b4: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x22d4 <deregister_tm_clones+0x40>  // u32=0xbc; f32?=2.63444111e-43
    22b8: e7933002     	ldr	r3, [r3, r2]
    22bc: e3530000     	cmp	r3, #0
    22c0: 012fff1e     	bxeq	lr
    22c4: e12fff13     	bx	r3
    22c8: 84 5e 01 00  	.word	0x00015e84
    22cc: 80 5e 01 00  	.word	0x00015e80
    22d0: 4c 5d 01 00  	.word	0x00015d4c
    22d4: bc 00 00 00  	.word	0x000000bc

