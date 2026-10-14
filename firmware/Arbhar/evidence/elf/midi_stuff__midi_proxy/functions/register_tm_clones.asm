000107b8 <register_tm_clones>:
   107b8: e59f0024     	ldr	r0, [pc, #0x24]         @ 0x107e4 <register_tm_clones+0x2c>
   107bc: e59f1024     	ldr	r1, [pc, #0x24]         @ 0x107e8 <register_tm_clones+0x30>
   107c0: e0411000     	sub	r1, r1, r0
   107c4: e1a01141     	asr	r1, r1, #2
   107c8: e0811fa1     	add	r1, r1, r1, lsr #31
   107cc: e1b010c1     	asrs	r1, r1, #1
   107d0: 012fff1e     	bxeq	lr
   107d4: e59f3010     	ldr	r3, [pc, #0x10]         @ 0x107ec <register_tm_clones+0x34>
   107d8: e3530000     	cmp	r3, #0
   107dc: 012fff1e     	bxeq	lr
   107e0: e12fff13     	bx	r3
   107e4: 74 20 02 00  	.word	0x00022074
   107e8: 74 20 02 00  	.word	0x00022074
   107ec: 00 00 00 00  	.word	0x00000000

