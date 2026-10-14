000028b4 <deregister_tm_clones>:
    28b4: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x28e8 <deregister_tm_clones+0x34>  // u32=0x178e8; f32?=1.35208486e-40
    28b8: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x28ec <deregister_tm_clones+0x38>  // u32=0x178e4; f32?=1.35202881e-40
    28bc: e08f0000     	add	r0, pc, r0
    28c0: e08f3003     	add	r3, pc, r3
    28c4: e1530000     	cmp	r3, r0
    28c8: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x28f0 <deregister_tm_clones+0x3c>  // u32=0x1772c; f32?=1.3458631e-40
    28cc: e08f3003     	add	r3, pc, r3
    28d0: 012fff1e     	bxeq	lr
    28d4: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x28f4 <deregister_tm_clones+0x40>  // u32=0x13c; f32?=4.42810315e-43
    28d8: e7933002     	ldr	r3, [r3, r2]
    28dc: e3530000     	cmp	r3, #0
    28e0: 012fff1e     	bxeq	lr
    28e4: e12fff13     	bx	r3
    28e8: e8 78 01 00  	.word	0x000178e8
    28ec: e4 78 01 00  	.word	0x000178e4
    28f0: 2c 77 01 00  	.word	0x0001772c
    28f4: 3c 01 00 00  	.word	0x0000013c

