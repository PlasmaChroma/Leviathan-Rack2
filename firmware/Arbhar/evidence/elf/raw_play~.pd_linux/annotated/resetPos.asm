000026b8 <resetPos>:
    26b8: e2803923     	add	r3, r0, #573440
    26bc: e59f000c     	ldr	r0, [pc, #0xc]          @ 0x26d0 <resetPos+0x18>  // u32=0x4e68; f32?=2.81268628e-41
    26c0: e3a02000     	mov	r2, #0
    26c4: e08f0000     	add	r0, pc, r0
    26c8: e5832a28     	str	r2, [r3, #0xa28]
    26cc: eafffe8b     	b	0x2100 <.plt+0x170>     @ imm = #-0x5d4  // CALL post
    26d0: 68 4e 00 00  	.word	0x00004e68

