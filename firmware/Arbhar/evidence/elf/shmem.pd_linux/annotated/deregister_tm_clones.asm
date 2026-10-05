000008bc <deregister_tm_clones>:
     8bc: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x8f0 <deregister_tm_clones+0x34>  // u32=0x117d8; f32?=1.00389022e-40
     8c0: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x8f4 <deregister_tm_clones+0x38>  // u32=0x117d4; f32?=1.00383417e-40
     8c4: e08f0000     	add	r0, pc, r0
     8c8: e08f3003     	add	r3, pc, r3
     8cc: e1530000     	cmp	r3, r0
     8d0: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x8f8 <deregister_tm_clones+0x3c>  // u32=0x11724; f32?=1.00136788e-40
     8d4: e08f3003     	add	r3, pc, r3
     8d8: 012fff1e     	bxeq	lr
     8dc: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x8fc <deregister_tm_clones+0x40>  // u32=0x6c; f32?=1.51340234e-43
     8e0: e7933002     	ldr	r3, [r3, r2]
     8e4: e3530000     	cmp	r3, #0
     8e8: 012fff1e     	bxeq	lr
     8ec: e12fff13     	bx	r3
     8f0: d8 17 01 00  	.word	0x000117d8
     8f4: d4 17 01 00  	.word	0x000117d4
     8f8: 24 17 01 00  	.word	0x00011724
     8fc: 6c 00 00 00  	.word	0x0000006c

