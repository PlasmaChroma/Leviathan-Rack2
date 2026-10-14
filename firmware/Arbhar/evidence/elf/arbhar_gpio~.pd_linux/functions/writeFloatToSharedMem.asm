00008768 <writeFloatToSharedMem>:
    8768: e2800a01     	add	r0, r0, #4096
    876c: e5903dc0     	ldr	r3, [r0, #0xdc0]
    8770: e3530000     	cmp	r3, #0
    8774: 0a000002     	beq	0x8784 <writeFloatToSharedMem+0x1c> @ imm = #0x8
    8778: e0831101     	add	r1, r3, r1, lsl #2
    877c: ed810a00     	vstr	s0, [r1]
    8780: e12fff1e     	bx	lr
    8784: e59f2004     	ldr	r2, [pc, #0x4]          @ 0x8790 <writeFloatToSharedMem+0x28>
    8788: e08f0002     	add	r0, pc, r2
    878c: eaffec8a     	b	0x39bc <.plt+0x2c0>     @ imm = #-0x4dd8
    8790: 90 c6 00 00  	.word	0x0000c690

