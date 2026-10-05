00000ef4 <midi_fifo_setup>:
     ef4: e59f00c4     	ldr	r0, [pc, #0xc4]         @ 0xfc0 <midi_fifo_setup+0xcc>
     ef8: e92d40f0     	push	{r4, r5, r6, r7, lr}
     efc: e08f0000     	add	r0, pc, r0
     f00: e24dd00c     	sub	sp, sp, #12
     f04: e59f40b8     	ldr	r4, [pc, #0xb8]         @ 0xfc4 <midi_fifo_setup+0xd0>
     f08: ebfffdf2     	bl	0x6d8 <.plt+0x14>       @ imm = #-0x838
     f0c: e59f20b4     	ldr	r2, [pc, #0xb4]         @ 0xfc8 <midi_fifo_setup+0xd4>
     f10: e59f10b4     	ldr	r1, [pc, #0xb4]         @ 0xfcc <midi_fifo_setup+0xd8>
     f14: e08f4004     	add	r4, pc, r4
     f18: e3a05000     	mov	r5, #0
     f1c: e3a03028     	mov	r3, #40
     f20: e7942002     	ldr	r2, [r4, r2]
     f24: e7941001     	ldr	r1, [r4, r1]
     f28: e58d5004     	str	r5, [sp, #0x4]
     f2c: e58d5000     	str	r5, [sp]
     f30: ebfffe12     	bl	0x780 <.plt+0xbc>       @ imm = #-0x7b8
     f34: e59f6094     	ldr	r6, [pc, #0x94]         @ 0xfd0 <midi_fifo_setup+0xdc>
     f38: e59f3094     	ldr	r3, [pc, #0x94]         @ 0xfd4 <midi_fifo_setup+0xe0>
     f3c: e08f6006     	add	r6, pc, r6
     f40: e1a07000     	mov	r7, r0
     f44: e08f0003     	add	r0, pc, r3
     f48: e5867000     	str	r7, [r6]
     f4c: ebfffde1     	bl	0x6d8 <.plt+0x14>       @ imm = #-0x87c
     f50: e59fc080     	ldr	r12, [pc, #0x80]        @ 0xfd8 <midi_fifo_setup+0xe4>
     f54: e1a03005     	mov	r3, r5
     f58: e794100c     	ldr	r1, [r4, r12]
     f5c: e1a02000     	mov	r2, r0
     f60: e1a00007     	mov	r0, r7
     f64: ebfffe0e     	bl	0x7a4 <.plt+0xe0>       @ imm = #-0x7c8
     f68: e59f006c     	ldr	r0, [pc, #0x6c]         @ 0xfdc <midi_fifo_setup+0xe8>
     f6c: e5967000     	ldr	r7, [r6]
     f70: e08f0000     	add	r0, pc, r0
     f74: ebfffdd7     	bl	0x6d8 <.plt+0x14>       @ imm = #-0x8a4
     f78: e59f2060     	ldr	r2, [pc, #0x60]         @ 0xfe0 <midi_fifo_setup+0xec>
     f7c: e1a03005     	mov	r3, r5
     f80: e7941002     	ldr	r1, [r4, r2]
     f84: e1a02000     	mov	r2, r0
     f88: e1a00007     	mov	r0, r7
     f8c: ebfffe04     	bl	0x7a4 <.plt+0xe0>       @ imm = #-0x7f0
     f90: e59f104c     	ldr	r1, [pc, #0x4c]         @ 0xfe4 <midi_fifo_setup+0xf0>
     f94: e5966000     	ldr	r6, [r6]
     f98: e08f0001     	add	r0, pc, r1
     f9c: ebfffdcd     	bl	0x6d8 <.plt+0x14>       @ imm = #-0x8cc
     fa0: e59fc040     	ldr	r12, [pc, #0x40]        @ 0xfe8 <midi_fifo_setup+0xf4>
     fa4: e1a03005     	mov	r3, r5
     fa8: e794100c     	ldr	r1, [r4, r12]
     fac: e1a02000     	mov	r2, r0
     fb0: e1a00006     	mov	r0, r6
     fb4: e28dd00c     	add	sp, sp, #12
     fb8: e8bd40f0     	pop	{r4, r5, r6, r7, lr}
     fbc: eafffdf8     	b	0x7a4 <.plt+0xe0>       @ imm = #-0x820
     fc0: 84 01 00 00  	.word	0x00000184
     fc4: e4 10 01 00  	.word	0x000110e4
     fc8: 84 00 00 00  	.word	0x00000084
     fcc: 6c 00 00 00  	.word	0x0000006c
     fd0: 54 11 01 00  	.word	0x00011154
     fd4: 48 01 00 00  	.word	0x00000148
     fd8: 80 00 00 00  	.word	0x00000080
     fdc: 24 01 00 00  	.word	0x00000124
     fe0: 88 00 00 00  	.word	0x00000088
     fe4: 04 01 00 00  	.word	0x00000104
     fe8: 74 00 00 00  	.word	0x00000074

Disassembly of section .fini:

