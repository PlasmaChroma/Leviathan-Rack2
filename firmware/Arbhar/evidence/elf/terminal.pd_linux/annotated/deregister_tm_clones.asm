000006dc <deregister_tm_clones>:
     6dc: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x710 <deregister_tm_clones+0x34>  // u32=0x10990; f32?=9.52658748e-41
     6e0: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x714 <deregister_tm_clones+0x38>  // u32=0x1098c; f32?=9.52602696e-41
     6e4: e08f0000     	add	r0, pc, r0
     6e8: e08f3003     	add	r3, pc, r3
     6ec: e1530000     	cmp	r3, r0
     6f0: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x718 <deregister_tm_clones+0x3c>  // u32=0x10904; f32?=9.5069693e-41
     6f4: e08f3003     	add	r3, pc, r3
     6f8: 012fff1e     	bxeq	lr
     6fc: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x71c <deregister_tm_clones+0x40>  // u32=0x5c; f32?=1.28919459e-43
     700: e7933002     	ldr	r3, [r3, r2]
     704: e3530000     	cmp	r3, #0
     708: 012fff1e     	bxeq	lr
     70c: e12fff13     	bx	r3
     710: 90 09 01 00  	.word	0x00010990
     714: 8c 09 01 00  	.word	0x0001098c
     718: 04 09 01 00  	.word	0x00010904
     71c: 5c 00 00 00  	.word	0x0000005c

