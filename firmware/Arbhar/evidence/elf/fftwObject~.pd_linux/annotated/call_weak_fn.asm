00000738 <call_weak_fn>:
     738: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x754 <call_weak_fn+0x1c>  // u32=0x148b8; f32?=1.17922068e-40
     73c: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x758 <call_weak_fn+0x20>  // u32=0x6c; f32?=1.51340234e-43
     740: e08f3003     	add	r3, pc, r3
     744: e7932002     	ldr	r2, [r3, r2]
     748: e3520000     	cmp	r2, #0
     74c: 012fff1e     	bxeq	lr
     750: eaffffd3     	b	0x6a4 <.plt+0x8c>       @ imm = #-0xb4  // CALL __gmon_start__
     754: b8 48 01 00  	.word	0x000148b8
     758: 6c 00 00 00  	.word	0x0000006c

