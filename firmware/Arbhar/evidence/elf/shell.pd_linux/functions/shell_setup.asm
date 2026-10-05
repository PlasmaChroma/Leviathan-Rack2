000019a4 <shell_setup>:
    19a4: e59f0068     	ldr	r0, [pc, #0x68]         @ 0x1a14 <shell_setup+0x70>
    19a8: e92d4030     	push	{r4, r5, lr}
    19ac: e08f0000     	add	r0, pc, r0
    19b0: e24dd00c     	sub	sp, sp, #12
    19b4: e59f405c     	ldr	r4, [pc, #0x5c]         @ 0x1a18 <shell_setup+0x74>
    19b8: ebfffbf0     	bl	0x980 <.plt+0x14>       @ imm = #-0x1040
    19bc: e59f2058     	ldr	r2, [pc, #0x58]         @ 0x1a1c <shell_setup+0x78>
    19c0: e08f4004     	add	r4, pc, r4
    19c4: e59f1054     	ldr	r1, [pc, #0x54]         @ 0x1a20 <shell_setup+0x7c>
    19c8: e3a0c000     	mov	r12, #0
    19cc: e3a03050     	mov	r3, #80
    19d0: e7942002     	ldr	r2, [r4, r2]
    19d4: e08f1001     	add	r1, pc, r1
    19d8: e58dc004     	str	r12, [sp, #0x4]
    19dc: e58dc000     	str	r12, [sp]
    19e0: ebfffc46     	bl	0xb00 <.plt+0x194>      @ imm = #-0xee8
    19e4: e59f5038     	ldr	r5, [pc, #0x38]         @ 0x1a24 <shell_setup+0x80>
    19e8: e59f3038     	ldr	r3, [pc, #0x38]         @ 0x1a28 <shell_setup+0x84>
    19ec: e08f5005     	add	r5, pc, r5
    19f0: e5850000     	str	r0, [r5]
    19f4: e7941003     	ldr	r1, [r4, r3]
    19f8: ebfffc01     	bl	0xa04 <.plt+0x98>       @ imm = #-0xffc
    19fc: e59f4028     	ldr	r4, [pc, #0x28]         @ 0x1a2c <shell_setup+0x88>
    1a00: e5950000     	ldr	r0, [r5]
    1a04: e08f1004     	add	r1, pc, r4
    1a08: e28dd00c     	add	sp, sp, #12
    1a0c: e8bd4030     	pop	{r4, r5, lr}
    1a10: eafffc49     	b	0xb3c <.plt+0x1d0>      @ imm = #-0xedc
    1a14: 54 01 00 00  	.word	0x00000154
    1a18: 38 06 01 00  	.word	0x00010638
    1a1c: dc 00 00 00  	.word	0x000000dc
    1a20: 18 f9 ff ff  	.word	0xfffff918
    1a24: f4 06 01 00  	.word	0x000106f4
    1a28: c8 00 00 00  	.word	0x000000c8
    1a2c: a8 fa ff ff  	.word	0xfffffaa8

Disassembly of section .fini:

