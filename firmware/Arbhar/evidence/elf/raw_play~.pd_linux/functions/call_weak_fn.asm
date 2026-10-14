000021a8 <call_weak_fn>:
    21a8: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x21c4 <call_weak_fn+0x1c>
    21ac: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x21c8 <call_weak_fn+0x20>
    21b0: e08f3003     	add	r3, pc, r3
    21b4: e7932002     	ldr	r2, [r3, r2]
    21b8: e3520000     	cmp	r2, #0
    21bc: 012fff1e     	bxeq	lr
    21c0: eaffffb0     	b	0x2088 <.plt+0xf8>      @ imm = #-0x140
    21c4: 48 5e 01 00  	.word	0x00015e48
    21c8: c4 00 00 00  	.word	0x000000c4

