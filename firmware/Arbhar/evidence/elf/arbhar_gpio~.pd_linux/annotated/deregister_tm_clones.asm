00003e0c <deregister_tm_clones>:
    3e0c: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x3e40 <deregister_tm_clones+0x34>  // u32=0x23598; f32?=2.02896807e-40
    3e10: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x3e44 <deregister_tm_clones+0x38>  // u32=0x23594; f32?=2.02891202e-40
    3e14: e08f0000     	add	r0, pc, r0
    3e18: e08f3003     	add	r3, pc, r3
    3e1c: e1530000     	cmp	r3, r0
    3e20: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x3e48 <deregister_tm_clones+0x3c>  // u32=0x231d4; f32?=2.01545956e-40
    3e24: e08f3003     	add	r3, pc, r3
    3e28: 012fff1e     	bxeq	lr
    3e2c: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x3e4c <deregister_tm_clones+0x40>  // u32=0x25c; f32?=8.46384272e-43
    3e30: e7933002     	ldr	r3, [r3, r2]
    3e34: e3530000     	cmp	r3, #0
    3e38: 012fff1e     	bxeq	lr
    3e3c: e12fff13     	bx	r3
    3e40: 98 35 02 00  	.word	0x00023598
    3e44: 94 35 02 00  	.word	0x00023594
    3e48: d4 31 02 00  	.word	0x000231d4
    3e4c: 5c 02 00 00  	.word	0x0000025c

