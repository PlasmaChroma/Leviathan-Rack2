00001638 <shmem_setup>:
    1638: e59f0168     	ldr	r0, [pc, #0x168]        @ 0x17a8 <shmem_setup+0x170>  // u32=0x3d8; f32?=1.37887769e-42
    163c: e92d43f0     	push	{r4, r5, r6, r7, r8, r9, lr}
    1640: e08f0000     	add	r0, pc, r0
    1644: e24dd014     	sub	sp, sp, #20
    1648: e59f415c     	ldr	r4, [pc, #0x15c]        @ 0x17ac <shmem_setup+0x174>  // u32=0x109a0; f32?=9.52882956e-41
    164c: ebfffc4c     	bl	0x784 <.plt+0x14>       @ imm = #-0xed0  // CALL gensym
    1650: e59f2158     	ldr	r2, [pc, #0x158]        @ 0x17b0 <shmem_setup+0x178>  // u32=0x90; f32?=2.01786979e-43
    1654: e59f1158     	ldr	r1, [pc, #0x158]        @ 0x17b4 <shmem_setup+0x17c>  // u32=0x8c; f32?=1.96181785e-43
    1658: e08f4004     	add	r4, pc, r4
    165c: e3a05000     	mov	r5, #0
    1660: e3a08006     	mov	r8, #6
    1664: e7942002     	ldr	r2, [r4, r2]
    1668: e3a0302c     	mov	r3, #44
    166c: e7941001     	ldr	r1, [r4, r1]
    1670: e3a07001     	mov	r7, #1
    1674: e58d8008     	str	r8, [sp, #0x8]
    1678: e58d8004     	str	r8, [sp, #0x4]
    167c: e58d500c     	str	r5, [sp, #0xc]
    1680: e58d5000     	str	r5, [sp]
    1684: ebfffc71     	bl	0x850 <.plt+0xe0>       @ imm = #-0xe3c  // CALL class_new
    1688: e59f3128     	ldr	r3, [pc, #0x128]        @ 0x17b8 <shmem_setup+0x180>  // u32=0x84; f32?=1.84971397e-43
    168c: e59f6128     	ldr	r6, [pc, #0x128]        @ 0x17bc <shmem_setup+0x184>  // u32=0x38c; f32?=1.27237901e-42
    1690: e1a09000     	mov	r9, r0
    1694: e08f0006     	add	r0, pc, r6
    1698: e7946003     	ldr	r6, [r4, r3]
    169c: e5869000     	str	r9, [r6]
    16a0: ebfffc37     	bl	0x784 <.plt+0x14>       @ imm = #-0xf24  // CALL gensym
    16a4: e59fc114     	ldr	r12, [pc, #0x114]       @ 0x17c0 <shmem_setup+0x188>  // u32=0x94; f32?=2.07392173e-43
    16a8: e3a0300a     	mov	r3, #10
    16ac: e794100c     	ldr	r1, [r4, r12]
    16b0: e58d5000     	str	r5, [sp]
    16b4: e1a02000     	mov	r2, r0
    16b8: e1a00009     	mov	r0, r9
    16bc: ebfffc6c     	bl	0x874 <.plt+0x104>      @ imm = #-0xe50  // CALL class_addmethod
    16c0: e59f00fc     	ldr	r0, [pc, #0xfc]         @ 0x17c4 <shmem_setup+0x18c>  // u32=0x360; f32?=1.21072187e-42
    16c4: e5969000     	ldr	r9, [r6]
    16c8: e08f0000     	add	r0, pc, r0
    16cc: ebfffc2c     	bl	0x784 <.plt+0x14>       @ imm = #-0xf50  // CALL gensym
    16d0: e59f20f0     	ldr	r2, [pc, #0xf0]         @ 0x17c8 <shmem_setup+0x190>  // u32=0x78; f32?=1.68155816e-43
    16d4: e1a03007     	mov	r3, r7
    16d8: e7941002     	ldr	r1, [r4, r2]
    16dc: e58d5004     	str	r5, [sp, #0x4]
    16e0: e58d7000     	str	r7, [sp]
    16e4: e1a02000     	mov	r2, r0
    16e8: e1a00009     	mov	r0, r9
    16ec: ebfffc60     	bl	0x874 <.plt+0x104>      @ imm = #-0xe80  // CALL class_addmethod
    16f0: e59f10d4     	ldr	r1, [pc, #0xd4]         @ 0x17cc <shmem_setup+0x194>  // u32=0x33c; f32?=1.16027513e-42
    16f4: e5969000     	ldr	r9, [r6]
    16f8: e08f0001     	add	r0, pc, r1
    16fc: ebfffc20     	bl	0x784 <.plt+0x14>       @ imm = #-0xf80  // CALL gensym
    1700: e59fc0c8     	ldr	r12, [pc, #0xc8]        @ 0x17d0 <shmem_setup+0x198>  // u32=0x7c; f32?=1.7376101e-43
    1704: e3a0300a     	mov	r3, #10
    1708: e794100c     	ldr	r1, [r4, r12]
    170c: e58d5000     	str	r5, [sp]
    1710: e1a02000     	mov	r2, r0
    1714: e1a00009     	mov	r0, r9
    1718: ebfffc55     	bl	0x874 <.plt+0x104>      @ imm = #-0xeac  // CALL class_addmethod
    171c: e59f30b0     	ldr	r3, [pc, #0xb0]         @ 0x17d4 <shmem_setup+0x19c>  // u32=0x318; f32?=1.10982838e-42
    1720: e5969000     	ldr	r9, [r6]
    1724: e08f0003     	add	r0, pc, r3
    1728: ebfffc15     	bl	0x784 <.plt+0x14>       @ imm = #-0xfac  // CALL gensym
    172c: e59f20a4     	ldr	r2, [pc, #0xa4]         @ 0x17d8 <shmem_setup+0x1a0>  // u32=0x70; f32?=1.56945428e-43
    1730: e1a03005     	mov	r3, r5
    1734: e7941002     	ldr	r1, [r4, r2]
    1738: e1a02000     	mov	r2, r0
    173c: e1a00009     	mov	r0, r9
    1740: ebfffc4b     	bl	0x874 <.plt+0x104>      @ imm = #-0xed4  // CALL class_addmethod
    1744: e59f0090     	ldr	r0, [pc, #0x90]         @ 0x17dc <shmem_setup+0x1a4>  // u32=0x2fc; f32?=1.07059203e-42
    1748: e5969000     	ldr	r9, [r6]
    174c: e08f0000     	add	r0, pc, r0
    1750: ebfffc0b     	bl	0x784 <.plt+0x14>       @ imm = #-0xfd4  // CALL gensym
    1754: e59f1084     	ldr	r1, [pc, #0x84]         @ 0x17e0 <shmem_setup+0x1a8>  // u32=0x74; f32?=1.62550622e-43
    1758: e1a03008     	mov	r3, r8
    175c: e59f8080     	ldr	r8, [pc, #0x80]         @ 0x17e4 <shmem_setup+0x1ac>  // u32=0x2dc; f32?=1.02575048e-42
    1760: e7941001     	ldr	r1, [r4, r1]
    1764: e58d5000     	str	r5, [sp]
    1768: e1a02000     	mov	r2, r0
    176c: e1a00009     	mov	r0, r9
    1770: ebfffc3f     	bl	0x874 <.plt+0x104>      @ imm = #-0xf04  // CALL class_addmethod
    1774: e08f0008     	add	r0, pc, r8
    1778: e5966000     	ldr	r6, [r6]
    177c: ebfffc00     	bl	0x784 <.plt+0x14>       @ imm = #-0x1000  // CALL gensym
    1780: e59fc060     	ldr	r12, [pc, #0x60]        @ 0x17e8 <shmem_setup+0x1b0>  // u32=0x98; f32?=2.12997367e-43
    1784: e1a03007     	mov	r3, r7
    1788: e794100c     	ldr	r1, [r4, r12]
    178c: e58d5004     	str	r5, [sp, #0x4]
    1790: e58d7000     	str	r7, [sp]
    1794: e1a02000     	mov	r2, r0
    1798: e1a00006     	mov	r0, r6
    179c: ebfffc34     	bl	0x874 <.plt+0x104>      @ imm = #-0xf30  // CALL class_addmethod
    17a0: e28dd014     	add	sp, sp, #20
    17a4: e8bd83f0     	pop	{r4, r5, r6, r7, r8, r9, pc}
    17a8: d8 03 00 00  	.word	0x000003d8
    17ac: a0 09 01 00  	.word	0x000109a0
    17b0: 90 00 00 00  	.word	0x00000090
    17b4: 8c 00 00 00  	.word	0x0000008c
    17b8: 84 00 00 00  	.word	0x00000084
    17bc: 8c 03 00 00  	.word	0x0000038c
    17c0: 94 00 00 00  	.word	0x00000094
    17c4: 60 03 00 00  	.word	0x00000360
    17c8: 78 00 00 00  	.word	0x00000078
    17cc: 3c 03 00 00  	.word	0x0000033c
    17d0: 7c 00 00 00  	.word	0x0000007c
    17d4: 18 03 00 00  	.word	0x00000318
    17d8: 70 00 00 00  	.word	0x00000070
    17dc: fc 02 00 00  	.word	0x000002fc
    17e0: 74 00 00 00  	.word	0x00000074
    17e4: dc 02 00 00  	.word	0x000002dc
    17e8: 98 00 00 00  	.word	0x00000098

Disassembly of section .fini:

