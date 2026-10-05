00000bdc <deregister_tm_clones>:
     bdc: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0xc10 <deregister_tm_clones+0x34>  // u32=0x124f8; f32?=1.05097385e-40
     be0: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0xc14 <deregister_tm_clones+0x38>  // u32=0x124f4; f32?=1.0509178e-40
     be4: e08f0000     	add	r0, pc, r0
     be8: e08f3003     	add	r3, pc, r3
     bec: e1530000     	cmp	r3, r0
     bf0: e59f3020     	ldr	r3, [pc, #0x20]         @ 0xc18 <deregister_tm_clones+0x3c>  // u32=0x12404; f32?=1.04755468e-40
     bf4: e08f3003     	add	r3, pc, r3
     bf8: 012fff1e     	bxeq	lr
     bfc: e59f2018     	ldr	r2, [pc, #0x18]         @ 0xc1c <deregister_tm_clones+0x40>  // u32=0xa4; f32?=2.29812948e-43
     c00: e7933002     	ldr	r3, [r3, r2]
     c04: e3530000     	cmp	r3, #0
     c08: 012fff1e     	bxeq	lr
     c0c: e12fff13     	bx	r3
     c10: f8 24 01 00  	.word	0x000124f8
     c14: f4 24 01 00  	.word	0x000124f4
     c18: 04 24 01 00  	.word	0x00012404
     c1c: a4 00 00 00  	.word	0x000000a4

