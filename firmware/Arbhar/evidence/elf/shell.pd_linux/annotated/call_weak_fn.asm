00000b84 <call_weak_fn>:
     b84: e59f3014     	ldr	r3, [pc, #0x14]         @ 0xba0 <call_weak_fn+0x1c>  // u32=0x1146c; f32?=9.91614845e-41
     b88: e59f2014     	ldr	r2, [pc, #0x14]         @ 0xba4 <call_weak_fn+0x20>  // u32=0xcc; f32?=2.85864887e-43
     b8c: e08f3003     	add	r3, pc, r3
     b90: e7932002     	ldr	r2, [r3, r2]
     b94: e3520000     	cmp	r2, #0
     b98: 012fff1e     	bxeq	lr
     b9c: eaffffb3     	b	0xa70 <.plt+0x104>      @ imm = #-0x134  // CALL __gmon_start__
     ba0: 6c 14 01 00  	.word	0x0001146c
     ba4: cc 00 00 00  	.word	0x000000cc

