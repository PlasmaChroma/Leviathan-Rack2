00002164 <deregister_tm_clones>:
    2164: e59f002c     	ldr	r0, [pc, #0x2c]         @ 0x2198 <deregister_tm_clones+0x34>
    2168: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x219c <deregister_tm_clones+0x38>
    216c: e08f0000     	add	r0, pc, r0
    2170: e08f3003     	add	r3, pc, r3
    2174: e1530000     	cmp	r3, r0
    2178: e59f3020     	ldr	r3, [pc, #0x20]         @ 0x21a0 <deregister_tm_clones+0x3c>
    217c: e08f3003     	add	r3, pc, r3
    2180: 012fff1e     	bxeq	lr
    2184: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x21a4 <deregister_tm_clones+0x40>
    2188: e7933002     	ldr	r3, [r3, r2]
    218c: e3530000     	cmp	r3, #0
    2190: 012fff1e     	bxeq	lr
    2194: e12fff13     	bx	r3
    2198: a0 5f 01 00  	.word	0x00015fa0
    219c: 9c 5f 01 00  	.word	0x00015f9c
    21a0: 7c 5e 01 00  	.word	0x00015e7c
    21a4: b0 00 00 00  	.word	0x000000b0

