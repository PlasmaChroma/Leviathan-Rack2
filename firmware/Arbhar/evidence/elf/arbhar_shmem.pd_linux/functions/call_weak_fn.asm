00000bb8 <call_weak_fn>:
     bb8: e59f3014     	ldr	r3, [pc, #0x14]         @ 0xbd4 <call_weak_fn+0x1c>
     bbc: e59f2014     	ldr	r2, [pc, #0x14]         @ 0xbd8 <call_weak_fn+0x20>
     bc0: e08f3003     	add	r3, pc, r3
     bc4: e7932002     	ldr	r2, [r3, r2]
     bc8: e3520000     	cmp	r2, #0
     bcc: 012fff1e     	bxeq	lr
     bd0: eaffffbf     	b	0xad4 <.plt+0xe0>       @ imm = #-0x104
     bd4: 38 24 01 00  	.word	0x00012438
     bd8: bc 00 00 00  	.word	0x000000bc

