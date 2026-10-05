00002890 <call_weak_fn>:
    2890: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x28ac <call_weak_fn+0x1c>
    2894: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x28b0 <call_weak_fn+0x20>
    2898: e08f3003     	add	r3, pc, r3
    289c: e7932002     	ldr	r2, [r3, r2]
    28a0: e3520000     	cmp	r2, #0
    28a4: 012fff1e     	bxeq	lr
    28a8: eaffff7f     	b	0x26ac <.plt+0x1ac>     @ imm = #-0x204
    28ac: 60 77 01 00  	.word	0x00017760
    28b0: 50 01 00 00  	.word	0x00000150

