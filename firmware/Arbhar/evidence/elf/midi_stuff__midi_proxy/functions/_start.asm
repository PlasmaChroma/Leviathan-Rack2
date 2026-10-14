0001072c <_start>:
   1072c: e3a0b000     	mov	r11, #0
   10730: e3a0e000     	mov	lr, #0
   10734: e49d1004     	ldr	r1, [sp], #4
   10738: e1a0200d     	mov	r2, sp
   1073c: e52d2004     	str	r2, [sp, #-0x4]!
   10740: e52d0004     	str	r0, [sp, #-0x4]!
   10744: e59fc010     	ldr	r12, [pc, #0x10]        @ 0x1075c <_start+0x30>
   10748: e52dc004     	str	r12, [sp, #-0x4]!
   1074c: e59f000c     	ldr	r0, [pc, #0xc]          @ 0x10760 <_start+0x34>
   10750: e59f300c     	ldr	r3, [pc, #0xc]          @ 0x10764 <_start+0x38>
   10754: ebffffc1     	bl	0x10660 <.plt+0x5c>     @ imm = #-0xfc
   10758: ebffffe7     	bl	0x106fc <.plt+0xf8>     @ imm = #-0x64
   1075c: 64 12 01 00  	.word	0x00011264
   10760: 78 0a 01 00  	.word	0x00010a78
   10764: 04 12 01 00  	.word	0x00011204

