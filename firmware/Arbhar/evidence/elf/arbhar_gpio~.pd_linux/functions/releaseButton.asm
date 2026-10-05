0000ddf8 <releaseButton>:
    ddf8: e3a03000     	mov	r3, #0
    ddfc: e3a02001     	mov	r2, #1
    de00: e5c03001     	strb	r3, [r0, #0x1]
    de04: e1a01003     	mov	r1, r3
    de08: e5c02007     	strb	r2, [r0, #0x7]
    de0c: e5c03002     	strb	r3, [r0, #0x2]
    de10: e5c03005     	strb	r3, [r0, #0x5]
    de14: e59f0004     	ldr	r0, [pc, #0x4]          @ 0xde20 <releaseButton+0x28>
    de18: e08f0000     	add	r0, pc, r0
    de1c: eaffd755     	b	0x3b78 <.plt+0x47c>     @ imm = #-0xa2ac
    de20: 88 7b 00 00  	.word	0x00007b88

