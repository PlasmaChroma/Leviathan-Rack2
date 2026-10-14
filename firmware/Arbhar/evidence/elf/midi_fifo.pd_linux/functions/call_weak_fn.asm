000007d4 <call_weak_fn>:
     7d4: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x7f0 <call_weak_fn+0x1c>
     7d8: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x7f4 <call_weak_fn+0x20>
     7dc: e08f3003     	add	r3, pc, r3
     7e0: e7932002     	ldr	r2, [r3, r2]
     7e4: e3520000     	cmp	r2, #0
     7e8: 012fff1e     	bxeq	lr
     7ec: eaffffd1     	b	0x738 <.plt+0x74>       @ imm = #-0xbc
     7f0: 1c 18 01 00  	.word	0x0001181c
     7f4: 78 00 00 00  	.word	0x00000078

