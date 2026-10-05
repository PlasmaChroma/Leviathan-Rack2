00000da0 <terminal_setup>:
     da0: e59f0084     	ldr	r0, [pc, #0x84]         @ 0xe2c <terminal_setup+0x8c>
     da4: e92d40f0     	push	{r4, r5, r6, r7, lr}
     da8: e08f0000     	add	r0, pc, r0
     dac: e24dd00c     	sub	sp, sp, #12
     db0: e59f4078     	ldr	r4, [pc, #0x78]         @ 0xe30 <terminal_setup+0x90>
     db4: ebfffe06     	bl	0x5d4 <.plt+0x14>       @ imm = #-0x7e8
     db8: e59f1074     	ldr	r1, [pc, #0x74]         @ 0xe34 <terminal_setup+0x94>
     dbc: e08f4004     	add	r4, pc, r4
     dc0: e3a05000     	mov	r5, #0
     dc4: e1a02005     	mov	r2, r5
     dc8: e3013020     	movw	r3, #0x1020
     dcc: e7941001     	ldr	r1, [r4, r1]
     dd0: e58d5004     	str	r5, [sp, #0x4]
     dd4: e58d5000     	str	r5, [sp]
     dd8: ebfffe27     	bl	0x67c <.plt+0xbc>       @ imm = #-0x764
     ddc: e59f3054     	ldr	r3, [pc, #0x54]         @ 0xe38 <terminal_setup+0x98>
     de0: e59f2054     	ldr	r2, [pc, #0x54]         @ 0xe3c <terminal_setup+0x9c>
     de4: e7946003     	ldr	r6, [r4, r3]
     de8: e1a07000     	mov	r7, r0
     dec: e08f0002     	add	r0, pc, r2
     df0: e5867000     	str	r7, [r6]
     df4: ebfffdf6     	bl	0x5d4 <.plt+0x14>       @ imm = #-0x828
     df8: e59fc040     	ldr	r12, [pc, #0x40]        @ 0xe40 <terminal_setup+0xa0>
     dfc: e3a0300a     	mov	r3, #10
     e00: e794100c     	ldr	r1, [r4, r12]
     e04: e58d5000     	str	r5, [sp]
     e08: e59f5034     	ldr	r5, [pc, #0x34]         @ 0xe44 <terminal_setup+0xa4>
     e0c: e1a02000     	mov	r2, r0
     e10: e1a00007     	mov	r0, r7
     e14: ebfffe21     	bl	0x6a0 <.plt+0xe0>       @ imm = #-0x77c
     e18: e5960000     	ldr	r0, [r6]
     e1c: e7941005     	ldr	r1, [r4, r5]
     e20: e28dd00c     	add	sp, sp, #12
     e24: e8bd40f0     	pop	{r4, r5, r6, r7, lr}
     e28: eafffdf5     	b	0x604 <.plt+0x44>       @ imm = #-0x82c
     e2c: b4 00 00 00  	.word	0x000000b4
     e30: 3c 02 01 00  	.word	0x0001023c
     e34: 6c 00 00 00  	.word	0x0000006c
     e38: 60 00 00 00  	.word	0x00000060
     e3c: 60 00 00 00  	.word	0x00000060
     e40: 64 00 00 00  	.word	0x00000064
     e44: 74 00 00 00  	.word	0x00000074

Disassembly of section .fini:

