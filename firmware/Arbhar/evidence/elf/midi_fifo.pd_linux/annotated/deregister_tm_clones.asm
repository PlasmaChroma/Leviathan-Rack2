000007f8 <deregister_tm_clones>:
     7f8: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x82c <deregister_tm_clones+0x34>  // u32=0x1188c; f32?=1.00641256e-40
     7fc: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x830 <deregister_tm_clones+0x38>  // u32=0x11888; f32?=1.00635651e-40
     800: e08f0000     	add	r0, pc, r0
     804: e08f3003     	add	r3, pc, r3
     808: e1530000     	cmp	r3, r0
     80c: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x834 <deregister_tm_clones+0x3c>  // u32=0x117e8; f32?=1.00411443e-40
     810: e08f3003     	add	r3, pc, r3
     814: 012fff1e     	bxeq	lr
     818: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x838 <deregister_tm_clones+0x40>  // u32=0x68; f32?=1.4573504e-43
     81c: e7933002     	ldr	r3, [r3, r2]
     820: e3530000     	cmp	r3, #0
     824: 012fff1e     	bxeq	lr
     828: e12fff13     	bx	r3
     82c: 8c 18 01 00  	.word	0x0001188c
     830: 88 18 01 00  	.word	0x00011888
     834: e8 17 01 00  	.word	0x000117e8
     838: 68 00 00 00  	.word	0x00000068

