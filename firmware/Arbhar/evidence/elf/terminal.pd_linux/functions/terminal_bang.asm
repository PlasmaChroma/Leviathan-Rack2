000007dc <terminal_bang>:
     7dc: e59f1080     	ldr	r1, [pc, #0x80]         @ 0x864 <terminal_bang+0x88>
     7e0: e92d41f0     	push	{r4, r5, r6, r7, r8, lr}
     7e4: e08f1001     	add	r1, pc, r1
     7e8: e24dd801     	sub	sp, sp, #65536
     7ec: e1a07000     	mov	r7, r0
     7f0: e2800020     	add	r0, r0, #32
     7f4: e3a08000     	mov	r8, #0
     7f8: ebffff9c     	bl	0x670 <.plt+0xb0>       @ imm = #-0x190
     7fc: e1a0400d     	mov	r4, sp
     800: e1a06000     	mov	r6, r0
     804: ea00000a     	b	0x834 <terminal_bang+0x58> @ imm = #0x28
     808: ebffff8c     	bl	0x640 <.plt+0x80>       @ imm = #-0x1d0
     80c: e597501c     	ldr	r5, [r7, #0x1c]
     810: e0403004     	sub	r3, r0, r4
     814: e1a00004     	mov	r0, r4
     818: e7c48003     	strb	r8, [r4, r3]
     81c: ebffff6c     	bl	0x5d4 <.plt+0x14>       @ imm = #-0x250
     820: e3a03000     	mov	r3, #0
     824: e1a02003     	mov	r2, r3
     828: e1a01000     	mov	r1, r0
     82c: e1a00005     	mov	r0, r5
     830: ebffff94     	bl	0x688 <.plt+0xc8>       @ imm = #-0x1b0
     834: e30f1fff     	movw	r1, #0xffff
     838: e1a02006     	mov	r2, r6
     83c: e1a00004     	mov	r0, r4
     840: ebffff6c     	bl	0x5f8 <.plt+0x38>       @ imm = #-0x250
     844: e3a0100a     	mov	r1, #10
     848: e3500000     	cmp	r0, #0
     84c: e1a00004     	mov	r0, r4
     850: 1affffec     	bne	0x808 <terminal_bang+0x2c> @ imm = #-0x50
     854: e1a00006     	mov	r0, r6
     858: ebffff93     	bl	0x6ac <.plt+0xec>       @ imm = #-0x1b4
     85c: e28dd801     	add	sp, sp, #65536
     860: e8bd81f0     	pop	{r4, r5, r6, r7, r8, pc}
     864: 64 06 00 00  	.word	0x00000664

