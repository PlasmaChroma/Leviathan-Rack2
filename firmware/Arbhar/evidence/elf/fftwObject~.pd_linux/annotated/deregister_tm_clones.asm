0000075c <deregister_tm_clones>:
     75c: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x790 <deregister_tm_clones+0x34>  // u32=0x14910; f32?=1.18045383e-40
     760: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x794 <deregister_tm_clones+0x38>  // u32=0x1490c; f32?=1.18039777e-40
     764: e08f0000     	add	r0, pc, r0
     768: e08f3003     	add	r3, pc, r3
     76c: e1530000     	cmp	r3, r0
     770: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x798 <deregister_tm_clones+0x3c>  // u32=0x14884; f32?=1.17849201e-40
     774: e08f3003     	add	r3, pc, r3
     778: 012fff1e     	bxeq	lr
     77c: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x79c <deregister_tm_clones+0x40>  // u32=0x68; f32?=1.4573504e-43
     780: e7933002     	ldr	r3, [r3, r2]
     784: e3530000     	cmp	r3, #0
     788: 012fff1e     	bxeq	lr
     78c: e12fff13     	bx	r3
     790: 10 49 01 00  	.word	0x00014910
     794: 0c 49 01 00  	.word	0x0001490c
     798: 84 48 01 00  	.word	0x00014884
     79c: 68 00 00 00  	.word	0x00000068

