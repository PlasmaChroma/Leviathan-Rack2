00002294 <deregister_tm_clones>:
    2294: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x22c8 <deregister_tm_clones+0x34>
    2298: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x22cc <deregister_tm_clones+0x38>
    229c: e08f0000     	add	r0, pc, r0
    22a0: e08f3003     	add	r3, pc, r3
    22a4: e1530000     	cmp	r3, r0
    22a8: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x22d0 <deregister_tm_clones+0x3c>
    22ac: e08f3003     	add	r3, pc, r3
    22b0: 012fff1e     	bxeq	lr
    22b4: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x22d4 <deregister_tm_clones+0x40>
    22b8: e7933002     	ldr	r3, [r3, r2]
    22bc: e3530000     	cmp	r3, #0
    22c0: 012fff1e     	bxeq	lr
    22c4: e12fff13     	bx	r3
    22c8: 84 5e 01 00  	.word	0x00015e84
    22cc: 80 5e 01 00  	.word	0x00015e80
    22d0: 4c 5d 01 00  	.word	0x00015d4c
    22d4: bc 00 00 00  	.word	0x000000bc

