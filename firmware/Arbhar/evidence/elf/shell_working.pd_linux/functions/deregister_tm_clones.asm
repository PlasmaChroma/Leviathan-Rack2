00000d6c <deregister_tm_clones>:
     d6c: e59f3034     	ldr	r3, [pc, #0x34]         @ 0xda8 <deregister_tm_clones+0x3c>
     d70: e59f0034     	ldr	r0, [pc, #0x34]         @ 0xdac <deregister_tm_clones+0x40>
     d74: e08f3003     	add	r3, pc, r3
     d78: e08f0000     	add	r0, pc, r0
     d7c: e2833003     	add	r3, r3, #3
     d80: e0433000     	sub	r3, r3, r0
     d84: e3530006     	cmp	r3, #6
     d88: e59f3020     	ldr	r3, [pc, #0x20]         @ 0xdb0 <deregister_tm_clones+0x44>
     d8c: e08f3003     	add	r3, pc, r3
     d90: 912fff1e     	bxls	lr
     d94: e59f2018     	ldr	r2, [pc, #0x18]         @ 0xdb4 <deregister_tm_clones+0x48>
     d98: e7933002     	ldr	r3, [r3, r2]
     d9c: e3530000     	cmp	r3, #0
     da0: 012fff1e     	bxeq	lr
     da4: e12fff13     	bx	r3
     da8: 78 13 01 00  	.word	0x00011378
     dac: 74 13 01 00  	.word	0x00011374
     db0: 6c 12 01 00  	.word	0x0001126c
     db4: cc 00 00 00  	.word	0x000000cc

