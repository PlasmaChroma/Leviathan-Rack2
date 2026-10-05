00000ca8 <shell_bang>:
     ca8: e59f0004     	ldr	r0, [pc, #0x4]          @ 0xcb4 <shell_bang+0xc>  // u32=0xd84; f32?=4.84849269e-42
     cac: e08f0000     	add	r0, pc, r0
     cb0: eaffff83     	b	0xac4 <.plt+0x158>      @ imm = #-0x1f4  // CALL post
     cb4: 84 0d 00 00  	.word	0x00000d84

