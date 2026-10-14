00000ba8 <deregister_tm_clones>:
     ba8: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0xbdc <deregister_tm_clones+0x34>  // u32=0x1152c; f32?=9.94305338e-41
     bac: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0xbe0 <deregister_tm_clones+0x38>  // u32=0x11528; f32?=9.94249286e-41
     bb0: e08f0000     	add	r0, pc, r0
     bb4: e08f3003     	add	r3, pc, r3
     bb8: e1530000     	cmp	r3, r0
     bbc: e59f3020     	ldr	r3, [pc, #0x20]         @ 0xbe4 <deregister_tm_clones+0x3c>  // u32=0x11438; f32?=9.9088617e-41
     bc0: e08f3003     	add	r3, pc, r3
     bc4: 012fff1e     	bxeq	lr
     bc8: e59f2018     	ldr	r2, [pc, #0x18]         @ 0xbe8 <deregister_tm_clones+0x40>  // u32=0xc0; f32?=2.69049305e-43
     bcc: e7933002     	ldr	r3, [r3, r2]
     bd0: e3530000     	cmp	r3, #0
     bd4: 012fff1e     	bxeq	lr
     bd8: e12fff13     	bx	r3
     bdc: 2c 15 01 00  	.word	0x0001152c
     be0: 28 15 01 00  	.word	0x00011528
     be4: 38 14 01 00  	.word	0x00011438
     be8: c0 00 00 00  	.word	0x000000c0

