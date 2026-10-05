00002174 <deregister_tm_clones>:
    2174: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x21a8 <deregister_tm_clones+0x34>
    2178: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x21ac <deregister_tm_clones+0x38>
    217c: e08f0000     	add	r0, pc, r0
    2180: e08f3003     	add	r3, pc, r3
    2184: e1530000     	cmp	r3, r0
    2188: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x21b0 <deregister_tm_clones+0x3c>
    218c: e08f3003     	add	r3, pc, r3
    2190: 012fff1e     	bxeq	lr
    2194: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x21b4 <deregister_tm_clones+0x40>
    2198: e7933002     	ldr	r3, [r3, r2]
    219c: e3530000     	cmp	r3, #0
    21a0: 012fff1e     	bxeq	lr
    21a4: e12fff13     	bx	r3
    21a8: 90 5f 01 00  	.word	0x00015f90
    21ac: 8c 5f 01 00  	.word	0x00015f8c
    21b0: 6c 5e 01 00  	.word	0x00015e6c
    21b4: b0 00 00 00  	.word	0x000000b0

