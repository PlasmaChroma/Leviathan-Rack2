000025fc <deregister_tm_clones>:
    25fc: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x2630 <deregister_tm_clones+0x34>  // u32=0x16b5c; f32?=1.30348783e-40
    2600: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x2634 <deregister_tm_clones+0x38>  // u32=0x16b58; f32?=1.30343178e-40
    2604: e08f0000     	add	r0, pc, r0
    2608: e08f3003     	add	r3, pc, r3
    260c: e1530000     	cmp	r3, r0
    2610: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x2638 <deregister_tm_clones+0x3c>  // u32=0x169e4; f32?=1.29821895e-40
    2614: e08f3003     	add	r3, pc, r3
    2618: 012fff1e     	bxeq	lr
    261c: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x263c <deregister_tm_clones+0x40>  // u32=0x104; f32?=3.64337601e-43
    2620: e7933002     	ldr	r3, [r3, r2]
    2624: e3530000     	cmp	r3, #0
    2628: 012fff1e     	bxeq	lr
    262c: e12fff13     	bx	r3
    2630: 5c 6b 01 00  	.word	0x00016b5c
    2634: 58 6b 01 00  	.word	0x00016b58
    2638: e4 69 01 00  	.word	0x000169e4
    263c: 04 01 00 00  	.word	0x00000104

