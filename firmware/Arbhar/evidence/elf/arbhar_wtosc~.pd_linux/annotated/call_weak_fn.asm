000025d8 <call_weak_fn>:
    25d8: e59f3014     	ldr	r3, [pc, #0x14]         @ 0x25f4 <call_weak_fn+0x1c>  // u32=0x16a18; f32?=1.29894762e-40
    25dc: e59f2014     	ldr	r2, [pc, #0x14]         @ 0x25f8 <call_weak_fn+0x20>  // u32=0x108; f32?=3.69942795e-43
    25e0: e08f3003     	add	r3, pc, r3
    25e4: e7932002     	ldr	r2, [r3, r2]
    25e8: e3520000     	cmp	r2, #0
    25ec: 012fff1e     	bxeq	lr
    25f0: eaffffa0     	b	0x2478 <.plt+0x194>     @ imm = #-0x180  // CALL __gmon_start__
    25f4: 18 6a 01 00  	.word	0x00016a18
    25f8: 08 01 00 00  	.word	0x00000108

