00005e24 <checkInternalClock>:
    5e24: e2803a01     	add	r3, r0, #4096
    5e28: e92d4010     	push	{r4, lr}
    5e2c: e1a04000     	mov	r4, r0
    5e30: e5d31df5     	ldrb	r1, [r3, #0xdf5]
    5e34: e59000fc     	ldr	r0, [r0, #0xfc]
    5e38: e3510000     	cmp	r1, #0
    5e3c: 0a000005     	beq	0x5e58 <checkInternalClock+0x34> @ imm = #0x14
    5e40: ed9f0b08     	vldr	d0, [pc, #32]           @ 0x5e68 <checkInternalClock+0x44>
    5e44: ebfff6b8     	bl	0x392c <.plt+0x230>     @ imm = #-0x2520
    5e48: e5940100     	ldr	r0, [r4, #0x100]
    5e4c: e8bd4010     	pop	{r4, lr}
    5e50: ed9f0b04     	vldr	d0, [pc, #16]           @ 0x5e68 <checkInternalClock+0x44>
    5e54: eafff6b4     	b	0x392c <.plt+0x230>     @ imm = #-0x2530
    5e58: ebfff710     	bl	0x3aa0 <.plt+0x3a4>     @ imm = #-0x23c0
    5e5c: e5940100     	ldr	r0, [r4, #0x100]
    5e60: e8bd4010     	pop	{r4, lr}
    5e64: eafff70d     	b	0x3aa0 <.plt+0x3a4>     @ imm = #-0x23cc
    5e68: 00 00 00 00  	.word	0x00000000
    5e6c: 00 40 8f 40  	.word	0x408f4000

