00002150 <call_weak_fn>:
    2150: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x216c <call_weak_fn+0x1c>
    2154: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x2170 <call_weak_fn+0x20>
    2158: e08f3003     	add	r3, pc, r3
    215c: e7932002     	ldr	r2, [r3, r2]
    2160: e3520000     	cmp	r2, #0
    2164: 012fff1e     	bxeq	lr
    2168: eaffffb8     	b	0x2050 <.plt+0xf8>      @ imm = #-0x120
    216c: a0 5e 01 00  	.word	0x00015ea0
    2170: bc 00 00 00  	.word	0x000000bc

