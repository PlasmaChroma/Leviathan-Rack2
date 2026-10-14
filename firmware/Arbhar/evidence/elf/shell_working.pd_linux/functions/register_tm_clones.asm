00000db8 <register_tm_clones>:
     db8: e59f1038     	ldr	r1, [pc, #0x38]         @ 0xdf8 <register_tm_clones+0x40>
     dbc: e59f0038     	ldr	r0, [pc, #0x38]         @ 0xdfc <register_tm_clones+0x44>
     dc0: e08f1001     	add	r1, pc, r1
     dc4: e08f0000     	add	r0, pc, r0
     dc8: e0411000     	sub	r1, r1, r0
     dcc: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0xe00 <register_tm_clones+0x48>
     dd0: e1a01141     	asr	r1, r1, #2
     dd4: e08f3003     	add	r3, pc, r3
     dd8: e0811fa1     	add	r1, r1, r1, lsr #31
     ddc: e1b010c1     	asrs	r1, r1, #1
     de0: 012fff1e     	bxeq	lr
     de4: e59f2018     	ldr	r2, [pc, #0x18]         @ 0xe04 <register_tm_clones+0x4c>
     de8: e7933002     	ldr	r3, [r3, r2]
     dec: e3530000     	cmp	r3, #0
     df0: 012fff1e     	bxeq	lr
     df4: e12fff13     	bx	r3
     df8: 2c 13 01 00  	.word	0x0001132c
     dfc: 28 13 01 00  	.word	0x00011328
     e00: 24 12 01 00  	.word	0x00011224
     e04: e8 00 00 00  	.word	0x000000e8

