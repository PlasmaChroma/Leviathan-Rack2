000007f4 <call_weak_fn>:
     7f4: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x810 <call_weak_fn+0x1c>
     7f8: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x814 <call_weak_fn+0x20>
     7fc: e08f3003     	add	r3, pc, r3
     800: e7932002     	ldr	r2, [r3, r2]
     804: e3520000     	cmp	r2, #0
     808: 012fff1e     	bxeq	lr
     80c: eaffffd4     	b	0x764 <.plt+0x80>       @ imm = #-0xb0
     810: fc 07 01 00  	.word	0x000107fc
     814: 80 00 00 00  	.word	0x00000080

