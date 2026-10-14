00000430 <deregister_tm_clones>:
     430: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x464 <deregister_tm_clones+0x34>
     434: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x468 <deregister_tm_clones+0x38>
     438: e08f0000     	add	r0, pc, r0
     43c: e08f3003     	add	r3, pc, r3
     440: e1530000     	cmp	r3, r0
     444: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x46c <deregister_tm_clones+0x3c>
     448: e08f3003     	add	r3, pc, r3
     44c: 012fff1e     	bxeq	lr
     450: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x470 <deregister_tm_clones+0x40>
     454: e7933002     	ldr	r3, [r3, r2]
     458: e3530000     	cmp	r3, #0
     45c: 012fff1e     	bxeq	lr
     460: e12fff13     	bx	r3
     464: 00 0c 01 00  	.word	0x00010c00
     468: fc 0b 01 00  	.word	0x00010bfc
     46c: b0 0b 01 00  	.word	0x00010bb0
     470: 28 00 00 00  	.word	0x00000028

