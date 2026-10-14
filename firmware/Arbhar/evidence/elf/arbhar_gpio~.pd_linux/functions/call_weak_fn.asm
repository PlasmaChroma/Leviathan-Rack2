00003de8 <call_weak_fn>:
    3de8: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x3e04 <call_weak_fn+0x1c>
    3dec: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x3e08 <call_weak_fn+0x20>
    3df0: e08f3003     	add	r3, pc, r3
    3df4: e7932002     	ldr	r2, [r3, r2]
    3df8: e3520000     	cmp	r2, #0
    3dfc: 012fff1e     	bxeq	lr
    3e00: eaffff17     	b	0x3a64 <.plt+0x368>     @ imm = #-0x3a4
    3e04: 08 32 02 00  	.word	0x00023208
    3e08: a4 02 00 00  	.word	0x000002a4

