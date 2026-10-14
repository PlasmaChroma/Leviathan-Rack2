00010768 <call_weak_fn>:
   10768: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x10784 <call_weak_fn+0x1c>
   1076c: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x10788 <call_weak_fn+0x20>
   10770: e08f3003     	add	r3, pc, r3
   10774: e7932002     	ldr	r2, [r3, r2]
   10778: e3520000     	cmp	r2, #0
   1077c: 012fff1e     	bxeq	lr
   10780: eaffffb9     	b	0x1066c <.plt+0x68>     @ imm = #-0x11c
   10784: 88 18 01 00  	.word	0x00011888
   10788: 68 00 00 00  	.word	0x00000068

