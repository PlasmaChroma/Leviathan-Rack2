00006af8 <led_shiftMode>:
    6af8: eef07a08     	vmov.f32	s15, #3.000000e+00
    6afc: e3510001     	cmp	r1, #1
    6b00: ed9f7a15     	vldr	s14, [pc, #84]          @ 0x6b5c <led_shiftMode+0x64>
    6b04: e2802a01     	add	r2, r0, #4096
    6b08: e59f0050     	ldr	r0, [pc, #0x50]         @ 0x6b60 <led_shiftMode+0x68>
    6b0c: e3a03001     	mov	r3, #1
    6b10: e92d4010     	push	{r4, lr}
    6b14: e24dd010     	sub	sp, sp, #16
    6b18: e08f0000     	add	r0, pc, r0
    6b1c: e5924dac     	ldr	r4, [r2, #0xdac]
    6b20: e58d3000     	str	r3, [sp]
    6b24: e3a01000     	mov	r1, #0
    6b28: e58d3008     	str	r3, [sp, #0x8]
    6b2c: e3441316     	movt	r1, #0x4316
    6b30: e58d100c     	str	r1, [sp, #0xc]
    6b34: 1ef07a47     	vmovne.f32	s15, s14
    6b38: edcd7a01     	vstr	s15, [sp, #4]
    6b3c: ebfff2f9     	bl	0x3728 <.plt+0x2c>      @ imm = #-0x341c
    6b40: e1a0300d     	mov	r3, sp
    6b44: e3a02002     	mov	r2, #2
    6b48: e1a01000     	mov	r1, r0
    6b4c: e1a00004     	mov	r0, r4
    6b50: ebfff447     	bl	0x3c74 <.plt+0x578>     @ imm = #-0x2ee4
    6b54: e28dd010     	add	sp, sp, #16
    6b58: e8bd8010     	pop	{r4, pc}
    6b5c: 00 00 00 00  	.word	0x00000000
    6b60: 9c df 00 00  	.word	0x0000df9c

