00000c18 <call_weak_fn>:
     c18: e59f3014     	ldr	r3, [pc, #0x14]         @ 0xc34 <call_weak_fn+0x1c>
     c1c: e59f2014     	ldr	r2, [pc, #0x14]         @ 0xc38 <call_weak_fn+0x20>
     c20: e08f3003     	add	r3, pc, r3
     c24: e7932002     	ldr	r2, [r3, r2]
     c28: e3520000     	cmp	r2, #0
     c2c: 012fff1e     	bxeq	lr
     c30: eaffffa9     	b	0xadc <.plt+0xd4>       @ imm = #-0x15c
     c34: d8 43 01 00  	.word	0x000143d8
     c38: c0 00 00 00  	.word	0x000000c0

