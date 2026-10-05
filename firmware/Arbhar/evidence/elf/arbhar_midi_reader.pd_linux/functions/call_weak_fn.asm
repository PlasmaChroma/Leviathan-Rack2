0000040c <call_weak_fn>:
     40c: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x428 <call_weak_fn+0x1c>
     410: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x42c <call_weak_fn+0x20>
     414: e08f3003     	add	r3, pc, r3
     418: e7932002     	ldr	r2, [r3, r2]
     41c: e3520000     	cmp	r2, #0
     420: 012fff1e     	bxeq	lr
     424: eaffffef     	b	0x3e8 <.plt+0x38>       @ imm = #-0x44
     428: e4 0b 01 00  	.word	0x00010be4
     42c: 34 00 00 00  	.word	0x00000034

