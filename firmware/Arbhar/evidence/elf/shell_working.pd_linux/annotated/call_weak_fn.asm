00000d48 <call_weak_fn>:
     d48: e59f3014     	ldr	r3, [pc, #0x14]         @ 0xd64 <call_weak_fn+0x1c>  // u32=0x112a8; f32?=9.85280976e-41
     d4c: e59f2014     	ldr	r2, [pc, #0x14]         @ 0xd68 <call_weak_fn+0x20>  // u32=0xd8; f32?=3.02680468e-43
     d50: e08f3003     	add	r3, pc, r3
     d54: e7932002     	ldr	r2, [r3, r2]
     d58: e3520000     	cmp	r2, #0
     d5c: 012fff1e     	bxeq	lr
     d60: eaffffb0     	b	0xc28 <.plt+0x11c>      @ imm = #-0x140  // CALL __gmon_start__
     d64: a8 12 01 00  	.word	0x000112a8
     d68: d8 00 00 00  	.word	0x000000d8

