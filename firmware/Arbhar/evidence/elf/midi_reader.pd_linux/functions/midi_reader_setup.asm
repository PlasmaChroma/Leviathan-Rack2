00000c20 <midi_reader_setup>:
     c20: e59f00d4     	ldr	r0, [pc, #0xd4]         @ 0xcfc <midi_reader_setup+0xdc>
     c24: e92d40f0     	push	{r4, r5, r6, r7, lr}
     c28: e08f0000     	add	r0, pc, r0
     c2c: e24dd00c     	sub	sp, sp, #12
     c30: e59f40c8     	ldr	r4, [pc, #0xc8]         @ 0xd00 <midi_reader_setup+0xe0>
     c34: ebfffeaf     	bl	0x6f8 <.plt+0x14>       @ imm = #-0x544
     c38: e59f20c4     	ldr	r2, [pc, #0xc4]         @ 0xd04 <midi_reader_setup+0xe4>
     c3c: e59f10c4     	ldr	r1, [pc, #0xc4]         @ 0xd08 <midi_reader_setup+0xe8>
     c40: e08f4004     	add	r4, pc, r4
     c44: e3a06000     	mov	r6, #0
     c48: e3a03f4a     	mov	r3, #296
     c4c: e7942002     	ldr	r2, [r4, r2]
     c50: e7941001     	ldr	r1, [r4, r1]
     c54: e58d6004     	str	r6, [sp, #0x4]
     c58: e58d6000     	str	r6, [sp]
     c5c: ebfffed2     	bl	0x7ac <.plt+0xc8>       @ imm = #-0x4b8
     c60: e59f50a4     	ldr	r5, [pc, #0xa4]         @ 0xd0c <midi_reader_setup+0xec>
     c64: e59f30a4     	ldr	r3, [pc, #0xa4]         @ 0xd10 <midi_reader_setup+0xf0>
     c68: e08f5005     	add	r5, pc, r5
     c6c: e59f70a0     	ldr	r7, [pc, #0xa0]         @ 0xd14 <midi_reader_setup+0xf4>
     c70: e5850000     	str	r0, [r5]
     c74: e7941003     	ldr	r1, [r4, r3]
     c78: ebfffeb0     	bl	0x740 <.plt+0x5c>       @ imm = #-0x540
     c7c: e08f0007     	add	r0, pc, r7
     c80: e5957000     	ldr	r7, [r5]
     c84: ebfffe9b     	bl	0x6f8 <.plt+0x14>       @ imm = #-0x594
     c88: e59fc088     	ldr	r12, [pc, #0x88]        @ 0xd18 <midi_reader_setup+0xf8>
     c8c: e1a03006     	mov	r3, r6
     c90: e794100c     	ldr	r1, [r4, r12]
     c94: e1a02000     	mov	r2, r0
     c98: e1a00007     	mov	r0, r7
     c9c: ebfffec5     	bl	0x7b8 <.plt+0xd4>       @ imm = #-0x4ec
     ca0: e59f0074     	ldr	r0, [pc, #0x74]         @ 0xd1c <midi_reader_setup+0xfc>
     ca4: e5957000     	ldr	r7, [r5]
     ca8: e08f0000     	add	r0, pc, r0
     cac: ebfffe91     	bl	0x6f8 <.plt+0x14>       @ imm = #-0x5bc
     cb0: e59f2068     	ldr	r2, [pc, #0x68]         @ 0xd20 <midi_reader_setup+0x100>
     cb4: e1a03006     	mov	r3, r6
     cb8: e7941002     	ldr	r1, [r4, r2]
     cbc: e1a02000     	mov	r2, r0
     cc0: e1a00007     	mov	r0, r7
     cc4: ebfffebb     	bl	0x7b8 <.plt+0xd4>       @ imm = #-0x514
     cc8: e59f1054     	ldr	r1, [pc, #0x54]         @ 0xd24 <midi_reader_setup+0x104>
     ccc: e5955000     	ldr	r5, [r5]
     cd0: e08f0001     	add	r0, pc, r1
     cd4: ebfffe87     	bl	0x6f8 <.plt+0x14>       @ imm = #-0x5e4
     cd8: e59fc048     	ldr	r12, [pc, #0x48]        @ 0xd28 <midi_reader_setup+0x108>
     cdc: e3a0300a     	mov	r3, #10
     ce0: e794100c     	ldr	r1, [r4, r12]
     ce4: e58d6000     	str	r6, [sp]
     ce8: e1a02000     	mov	r2, r0
     cec: e1a00005     	mov	r0, r5
     cf0: ebfffeb0     	bl	0x7b8 <.plt+0xd4>       @ imm = #-0x540
     cf4: e28dd00c     	add	sp, sp, #12
     cf8: e8bd80f0     	pop	{r4, r5, r6, r7, pc}
     cfc: e4 01 00 00  	.word	0x000001e4
     d00: b8 03 01 00  	.word	0x000103b8
     d04: 78 00 00 00  	.word	0x00000078
     d08: 7c 00 00 00  	.word	0x0000007c
     d0c: 24 04 01 00  	.word	0x00010424
     d10: 70 00 00 00  	.word	0x00000070
     d14: 9c 01 00 00  	.word	0x0000019c
     d18: 68 00 00 00  	.word	0x00000068
     d1c: 78 01 00 00  	.word	0x00000178
     d20: 60 00 00 00  	.word	0x00000060
     d24: 5c 01 00 00  	.word	0x0000015c
     d28: 84 00 00 00  	.word	0x00000084

Disassembly of section .fini:

