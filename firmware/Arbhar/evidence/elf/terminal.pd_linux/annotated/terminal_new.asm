00000868 <terminal_new>:
     868: e59f303c     	ldr	r3, [pc, #0x3c]         @ 0x8ac <terminal_new+0x44>  // u32=0x10788; f32?=9.45371996e-41
     86c: e59f203c     	ldr	r2, [pc, #0x3c]         @ 0x8b0 <terminal_new+0x48>  // u32=0x60; f32?=1.34524653e-43
     870: e08f3003     	add	r3, pc, r3
     874: e92d4010     	push	{r4, lr}
     878: e7930002     	ldr	r0, [r3, r2]
     87c: e5900000     	ldr	r0, [r0]
     880: ebffff59     	bl	0x5ec <.plt+0x2c>       @ imm = #-0x29c  // CALL pd_new
     884: e59f1028     	ldr	r1, [pc, #0x28]         @ 0x8b4 <terminal_new+0x4c>  // u32=0x5c0; f32?=2.06271134e-42
     888: e1a04000     	mov	r4, r0
     88c: e08f0001     	add	r0, pc, r1
     890: ebffff4f     	bl	0x5d4 <.plt+0x14>       @ imm = #-0x2c4  // CALL gensym
     894: e1a01000     	mov	r1, r0
     898: e1a00004     	mov	r0, r4
     89c: ebffff70     	bl	0x664 <.plt+0xa4>       @ imm = #-0x240  // CALL outlet_new
     8a0: e584001c     	str	r0, [r4, #0x1c]
     8a4: e1a00004     	mov	r0, r4
     8a8: e8bd8010     	pop	{r4, pc}
     8ac: 88 07 01 00  	.word	0x00010788
     8b0: 60 00 00 00  	.word	0x00000060
     8b4: c0 05 00 00  	.word	0x000005c0

