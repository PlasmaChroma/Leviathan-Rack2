000006b8 <call_weak_fn>:
     6b8: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x6d4 <call_weak_fn+0x1c>
     6bc: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x6d8 <call_weak_fn+0x20>
     6c0: e08f3003     	add	r3, pc, r3
     6c4: e7932002     	ldr	r2, [r3, r2]
     6c8: e3520000     	cmp	r2, #0
     6cc: 012fff1e     	bxeq	lr
     6d0: eaffffd4     	b	0x628 <.plt+0x68>       @ imm = #-0xb0
     6d4: 38 09 01 00  	.word	0x00010938
     6d8: 68 00 00 00  	.word	0x00000068

