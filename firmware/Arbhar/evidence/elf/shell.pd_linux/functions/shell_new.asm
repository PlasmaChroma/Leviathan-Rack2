000012f4 <shell_new>:
    12f4: e59f30a8     	ldr	r3, [pc, #0xa8]         @ 0x13a4 <shell_new+0xb0>
    12f8: e92d4070     	push	{r4, r5, r6, lr}
    12fc: e08f0003     	add	r0, pc, r3
    1300: e59f50a0     	ldr	r5, [pc, #0xa0]         @ 0x13a8 <shell_new+0xb4>
    1304: e5900000     	ldr	r0, [r0]
    1308: ebfffdae     	bl	0x9c8 <.plt+0x5c>       @ imm = #-0x948
    130c: e3e01000     	mvn	r1, #0
    1310: e3a02000     	mov	r2, #0
    1314: e08f5005     	add	r5, pc, r5
    1318: e1a04000     	mov	r4, r0
    131c: e580201c     	str	r2, [r0, #0x1c]
    1320: e3a00b02     	mov	r0, #2048
    1324: e5842028     	str	r2, [r4, #0x28]
    1328: e5842024     	str	r2, [r4, #0x24]
    132c: e5841030     	str	r1, [r4, #0x30]
    1330: e5841034     	str	r1, [r4, #0x34]
    1334: e5841038     	str	r1, [r4, #0x38]
    1338: e584103c     	str	r1, [r4, #0x3c]
    133c: ebfffdc5     	bl	0xa58 <.plt+0xec>       @ imm = #-0x8ec
    1340: e3500000     	cmp	r0, #0
    1344: e5840020     	str	r0, [r4, #0x20]
    1348: 0a000011     	beq	0x1394 <shell_new+0xa0> @ imm = #0x44
    134c: ebfffdaf     	bl	0xa10 <.plt+0xa4>       @ imm = #-0x944
    1350: e59f3054     	ldr	r3, [pc, #0x54]         @ 0x13ac <shell_new+0xb8>
    1354: e584002c     	str	r0, [r4, #0x2c]
    1358: e1a00004     	mov	r0, r4
    135c: e7951003     	ldr	r1, [r5, r3]
    1360: ebfffddd     	bl	0xadc <.plt+0x170>      @ imm = #-0x88c
    1364: e59f2044     	ldr	r2, [pc, #0x44]         @ 0x13b0 <shell_new+0xbc>
    1368: e1a00004     	mov	r0, r4
    136c: e7951002     	ldr	r1, [r5, r2]
    1370: ebfffdd9     	bl	0xadc <.plt+0x170>      @ imm = #-0x89c
    1374: e59f1038     	ldr	r1, [pc, #0x38]         @ 0x13b4 <shell_new+0xc0>
    1378: e5840048     	str	r0, [r4, #0x48]
    137c: e1a00004     	mov	r0, r4
    1380: e7951001     	ldr	r1, [r5, r1]
    1384: ebfffdb6     	bl	0xa64 <.plt+0xf8>       @ imm = #-0x928
    1388: e584004c     	str	r0, [r4, #0x4c]
    138c: e1a00004     	mov	r0, r4
    1390: e8bd8070     	pop	{r4, r5, r6, pc}
    1394: e59fc01c     	ldr	r12, [pc, #0x1c]        @ 0x13b8 <shell_new+0xc4>
    1398: e08f000c     	add	r0, pc, r12
    139c: ebfffd8c     	bl	0x9d4 <.plt+0x68>       @ imm = #-0x9d0
    13a0: eaffffe9     	b	0x134c <shell_new+0x58> @ imm = #-0x5c
    13a4: e4 0d 01 00  	.word	0x00010de4
    13a8: e4 0c 01 00  	.word	0x00010ce4
    13ac: b8 00 00 00  	.word	0x000000b8
    13b0: d4 00 00 00  	.word	0x000000d4
    13b4: c4 00 00 00  	.word	0x000000c4
    13b8: f4 06 00 00  	.word	0x000006f4

