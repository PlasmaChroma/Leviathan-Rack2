00000ebc <shell_bang>:
     ebc: e59f0004     	ldr	r0, [pc, #0x4]          @ 0xec8 <shell_bang+0xc>  // u32=0xe70; f32?=5.17919912e-42
     ec0: e08f0000     	add	r0, pc, r0
     ec4: eaffff6c     	b	0xc7c <.plt+0x170>      @ imm = #-0x250  // CALL post
     ec8: 70 0e 00 00  	.word	0x00000e70

