00002210 <register_tm_clones>:
    2210: e59f0038     	ldr	r0, [pc, #0x38]         @ 0x2250 <register_tm_clones+0x40>
    2214: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x2254 <register_tm_clones+0x44>
    2218: e08f0000     	add	r0, pc, r0
    221c: e08f3003     	add	r3, pc, r3
    2220: e0431000     	sub	r1, r3, r0
    2224: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x2258 <register_tm_clones+0x48>
    2228: e1a01141     	asr	r1, r1, #2
    222c: e08f3003     	add	r3, pc, r3
    2230: e0811fa1     	add	r1, r1, r1, lsr #31
    2234: e1b010c1     	asrs	r1, r1, #1
    2238: 012fff1e     	bxeq	lr
    223c: e59f2018     	ldr	r2, [pc, #0x18]         @ 0x225c <register_tm_clones+0x4c>
    2240: e7933002     	ldr	r3, [r3, r2]
    2244: e3530000     	cmp	r3, #0
    2248: 012fff1e     	bxeq	lr
    224c: e12fff13     	bx	r3
    2250: 00 5f 01 00  	.word	0x00015f00
    2254: fc 5e 01 00  	.word	0x00015efc
    2258: cc 5d 01 00  	.word	0x00015dcc
    225c: d8 00 00 00  	.word	0x000000d8

