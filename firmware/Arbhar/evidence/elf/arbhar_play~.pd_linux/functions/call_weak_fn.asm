00002718 <call_weak_fn>:
    2718: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x2734 <call_weak_fn+0x1c>
    271c: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x2738 <call_weak_fn+0x20>
    2720: e08f3003     	add	r3, pc, r3
    2724: e7932002     	ldr	r2, [r3, r2]
    2728: e3520000     	cmp	r2, #0
    272c: 012fff1e     	bxeq	lr
    2730: eaffff8f     	b	0x2574 <.plt+0x194>     @ imm = #-0x1c4
    2734: d8 b8 01 00  	.word	0x0001b8d8
    2738: 30 01 00 00  	.word	0x00000130

