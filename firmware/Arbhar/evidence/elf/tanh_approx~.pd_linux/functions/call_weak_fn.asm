00002140 <call_weak_fn>:
    2140: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x215c <call_weak_fn+0x1c>
    2144: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x2160 <call_weak_fn+0x20>
    2148: e08f3003     	add	r3, pc, r3
    214c: e7932002     	ldr	r2, [r3, r2]
    2150: e3520000     	cmp	r2, #0
    2154: 012fff1e     	bxeq	lr
    2158: eaffffb5     	b	0x2034 <.plt+0xec>      @ imm = #-0x12c
    215c: b0 5e 01 00  	.word	0x00015eb0
    2160: b8 00 00 00  	.word	0x000000b8

