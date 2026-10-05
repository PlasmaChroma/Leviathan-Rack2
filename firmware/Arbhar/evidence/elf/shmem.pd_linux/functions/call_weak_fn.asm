00000898 <call_weak_fn>:
     898: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x8b4 <call_weak_fn+0x1c>
     89c: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x8b8 <call_weak_fn+0x20>
     8a0: e08f3003     	add	r3, pc, r3
     8a4: e7932002     	ldr	r2, [r3, r2]
     8a8: e3520000     	cmp	r2, #0
     8ac: 012fff1e     	bxeq	lr
     8b0: eaffffd1     	b	0x7fc <.plt+0x8c>       @ imm = #-0xbc
     8b4: 58 17 01 00  	.word	0x00011758
     8b8: 80 00 00 00  	.word	0x00000080

