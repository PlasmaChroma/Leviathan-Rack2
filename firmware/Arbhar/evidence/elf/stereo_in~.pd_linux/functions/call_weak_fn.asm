00002270 <call_weak_fn>:
    2270: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x228c <call_weak_fn+0x1c>
    2274: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x2290 <call_weak_fn+0x20>
    2278: e08f3003     	add	r3, pc, r3
    227c: e7932002     	ldr	r2, [r3, r2]
    2280: e3520000     	cmp	r2, #0
    2284: 012fff1e     	bxeq	lr
    2288: eaffffb3     	b	0x215c <.plt+0xf8>      @ imm = #-0x134
    228c: 80 5d 01 00  	.word	0x00015d80
    2290: cc 00 00 00  	.word	0x000000cc

