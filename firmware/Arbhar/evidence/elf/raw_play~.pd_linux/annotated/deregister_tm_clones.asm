000021cc <deregister_tm_clones>:
    21cc: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x2200 <deregister_tm_clones+0x34>  // u32=0x15f44; f32?=1.26010363e-40
    21d0: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x2204 <deregister_tm_clones+0x38>  // u32=0x15f40; f32?=1.26004758e-40
    21d4: e08f0000     	add	r0, pc, r0
    21d8: e08f3003     	add	r3, pc, r3
    21dc: e1530000     	cmp	r3, r0
    21e0: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x2208 <deregister_tm_clones+0x3c>  // u32=0x15e14; f32?=1.25584368e-40
    21e4: e08f3003     	add	r3, pc, r3
    21e8: 012fff1e     	bxeq	lr
    21ec: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x220c <deregister_tm_clones+0x40>  // u32=0xbc; f32?=2.63444111e-43
    21f0: e7933002     	ldr	r3, [r3, r2]
    21f4: e3530000     	cmp	r3, #0
    21f8: 012fff1e     	bxeq	lr
    21fc: e12fff13     	bx	r3
    2200: 44 5f 01 00  	.word	0x00015f44
    2204: 40 5f 01 00  	.word	0x00015f40
    2208: 14 5e 01 00  	.word	0x00015e14
    220c: bc 00 00 00  	.word	0x000000bc

