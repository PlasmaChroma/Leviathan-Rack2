00001500 <shell_new>:
    1500: e59f30a8     	ldr	r3, [pc, #0xa8]         @ 0x15b0 <shell_new+0xb0>
    1504: e92d4070     	push	{r4, r5, r6, lr}
    1508: e08f0003     	add	r0, pc, r3
    150c: e59f50a0     	ldr	r5, [pc, #0xa0]         @ 0x15b4 <shell_new+0xb4>
    1510: e5900000     	ldr	r0, [r0]
    1514: ebfffd93     	bl	0xb68 <.plt+0x5c>       @ imm = #-0x9b4
    1518: e3e01000     	mvn	r1, #0
    151c: e3a02000     	mov	r2, #0
    1520: e08f5005     	add	r5, pc, r5
    1524: e1a04000     	mov	r4, r0
    1528: e580201c     	str	r2, [r0, #0x1c]
    152c: e3a00b01     	mov	r0, #1024
    1530: e5842028     	str	r2, [r4, #0x28]
    1534: e5842024     	str	r2, [r4, #0x24]
    1538: e5841030     	str	r1, [r4, #0x30]
    153c: e5841034     	str	r1, [r4, #0x34]
    1540: e5841038     	str	r1, [r4, #0x38]
    1544: e584103c     	str	r1, [r4, #0x3c]
    1548: ebfffdad     	bl	0xc04 <.plt+0xf8>       @ imm = #-0x94c
    154c: e3500000     	cmp	r0, #0
    1550: e5840020     	str	r0, [r4, #0x20]
    1554: 0a000011     	beq	0x15a0 <shell_new+0xa0> @ imm = #0x44
    1558: ebfffd91     	bl	0xba4 <.plt+0x98>       @ imm = #-0x9bc
    155c: e59f3054     	ldr	r3, [pc, #0x54]         @ 0x15b8 <shell_new+0xb8>
    1560: e584002c     	str	r0, [r4, #0x2c]
    1564: e1a00004     	mov	r0, r4
    1568: e7951003     	ldr	r1, [r5, r3]
    156c: ebfffdc8     	bl	0xc94 <.plt+0x188>      @ imm = #-0x8e0
    1570: e59f2044     	ldr	r2, [pc, #0x44]         @ 0x15bc <shell_new+0xbc>
    1574: e1a00004     	mov	r0, r4
    1578: e7951002     	ldr	r1, [r5, r2]
    157c: ebfffdc4     	bl	0xc94 <.plt+0x188>      @ imm = #-0x8f0
    1580: e59f1038     	ldr	r1, [pc, #0x38]         @ 0x15c0 <shell_new+0xc0>
    1584: e5840048     	str	r0, [r4, #0x48]
    1588: e1a00004     	mov	r0, r4
    158c: e7951001     	ldr	r1, [r5, r1]
    1590: ebfffda1     	bl	0xc1c <.plt+0x110>      @ imm = #-0x97c
    1594: e584004c     	str	r0, [r4, #0x4c]
    1598: e1a00004     	mov	r0, r4
    159c: e8bd8070     	pop	{r4, r5, r6, pc}
    15a0: e59fc01c     	ldr	r12, [pc, #0x1c]        @ 0x15c4 <shell_new+0xc4>
    15a4: e08f000c     	add	r0, pc, r12
    15a8: ebfffd71     	bl	0xb74 <.plt+0x68>       @ imm = #-0xa3c
    15ac: eaffffe9     	b	0x1558 <shell_new+0x58> @ imm = #-0x5c
    15b0: e8 0b 01 00  	.word	0x00010be8
    15b4: d8 0a 01 00  	.word	0x00010ad8
    15b8: c4 00 00 00  	.word	0x000000c4
    15bc: e4 00 00 00  	.word	0x000000e4
    15c0: d0 00 00 00  	.word	0x000000d0
    15c4: e8 07 00 00  	.word	0x000007e8

