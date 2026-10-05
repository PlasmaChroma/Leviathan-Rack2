00000818 <deregister_tm_clones>:
     818: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x84c <deregister_tm_clones+0x34>  // u32=0x10868; f32?=9.48510905e-41
     81c: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x850 <deregister_tm_clones+0x38>  // u32=0x10864; f32?=9.48454853e-41
     820: e08f0000     	add	r0, pc, r0
     824: e08f3003     	add	r3, pc, r3
     828: e1530000     	cmp	r3, r0
     82c: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x854 <deregister_tm_clones+0x3c>  // u32=0x107c8; f32?=9.46268827e-41
     830: e08f3003     	add	r3, pc, r3
     834: 012fff1e     	bxeq	lr
     838: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x858 <deregister_tm_clones+0x40>  // u32=0x6c; f32?=1.51340234e-43
     83c: e7933002     	ldr	r3, [r3, r2]
     840: e3530000     	cmp	r3, #0
     844: 012fff1e     	bxeq	lr
     848: e12fff13     	bx	r3
     84c: 68 08 01 00  	.word	0x00010868
     850: 64 08 01 00  	.word	0x00010864
     854: c8 07 01 00  	.word	0x000107c8
     858: 6c 00 00 00  	.word	0x0000006c

