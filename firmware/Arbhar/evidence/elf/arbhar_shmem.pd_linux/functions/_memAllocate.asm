000017f0 <_memAllocate>:
    17f0: eefd7ac0     	vcvt.s32.f32	s15, s0
    17f4: e92d4030     	push	{r4, r5, lr}
    17f8: e24dd05c     	sub	sp, sp, #92
    17fc: ee175a90     	vmov	r5, s15
    1800: e3550000     	cmp	r5, #0
    1804: da000039     	ble	0x18f0 <_memAllocate+0x100> @ imm = #0xe4
    1808: eefd0ae0     	vcvt.s32.f32	s1, s1
    180c: e0804101     	add	r4, r0, r1, lsl #2
    1810: e5940058     	ldr	r0, [r4, #0x58]
    1814: e3500000     	cmp	r0, #0
    1818: edc40a23     	vstr	s1, [r4, #140]
    181c: 0a000002     	beq	0x182c <_memAllocate+0x3c> @ imm = #0x8
    1820: ebfffcbd     	bl	0xb1c <.plt+0x128>      @ imm = #-0xd0c
    1824: e3a03000     	mov	r3, #0
    1828: e5843058     	str	r3, [r4, #0x58]
    182c: e594001c     	ldr	r0, [r4, #0x1c]
    1830: e3700001     	cmn	r0, #1
    1834: 1a000015     	bne	0x1890 <_memAllocate+0xa0> @ imm = #0x54
    1838: e594108c     	ldr	r1, [r4, #0x8c]
    183c: e3002386     	movw	r2, #0x386
    1840: e1a00005     	mov	r0, r5
    1844: e1a01101     	lsl	r1, r1, #2
    1848: ebfffc7a     	bl	0xa38 <.plt+0x44>       @ imm = #-0xe18
    184c: e3700001     	cmn	r0, #1
    1850: e584001c     	str	r0, [r4, #0x1c]
    1854: 0a000013     	beq	0x18a8 <_memAllocate+0xb8> @ imm = #0x4c
    1858: e3a02000     	mov	r2, #0
    185c: e1a01002     	mov	r1, r2
    1860: ebfffc80     	bl	0xa68 <.plt+0x74>       @ imm = #-0xe00
    1864: e28d2004     	add	r2, sp, #4
    1868: e3a01002     	mov	r1, #2
    186c: e5840058     	str	r0, [r4, #0x58]
    1870: e594001c     	ldr	r0, [r4, #0x1c]
    1874: ebfffc84     	bl	0xa8c <.plt+0x98>       @ imm = #-0xdf0
    1878: e594208c     	ldr	r2, [r4, #0x8c]
    187c: e59d3028     	ldr	r3, [sp, #0x28]
    1880: e1530102     	cmp	r3, r2, lsl #2
    1884: ba000010     	blt	0x18cc <_memAllocate+0xdc> @ imm = #0x40
    1888: e28dd05c     	add	sp, sp, #92
    188c: e8bd8030     	pop	{r4, r5, pc}
    1890: e3a02000     	mov	r2, #0
    1894: e1a01002     	mov	r1, r2
    1898: ebfffc7b     	bl	0xa8c <.plt+0x98>       @ imm = #-0xe14
    189c: e3e00000     	mvn	r0, #0
    18a0: e584001c     	str	r0, [r4, #0x1c]
    18a4: eaffffe3     	b	0x1838 <_memAllocate+0x48> @ imm = #-0x74
    18a8: e59fc054     	ldr	r12, [pc, #0x54]        @ 0x1904 <_memAllocate+0x114>
    18ac: e1a01005     	mov	r1, r5
    18b0: e594208c     	ldr	r2, [r4, #0x8c]
    18b4: e3a05000     	mov	r5, #0
    18b8: e08f000c     	add	r0, pc, r12
    18bc: ebfffc75     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xe2c
    18c0: e584508c     	str	r5, [r4, #0x8c]
    18c4: e28dd05c     	add	sp, sp, #92
    18c8: e8bd8030     	pop	{r4, r5, pc}
    18cc: e59fe034     	ldr	lr, [pc, #0x34]         @ 0x1908 <_memAllocate+0x118>
    18d0: e1a01005     	mov	r1, r5
    18d4: e08f000e     	add	r0, pc, lr
    18d8: ebfffc6e     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xe48
    18dc: e3a00000     	mov	r0, #0
    18e0: e584008c     	str	r0, [r4, #0x8c]
    18e4: e5840058     	str	r0, [r4, #0x58]
    18e8: e28dd05c     	add	sp, sp, #92
    18ec: e8bd8030     	pop	{r4, r5, pc}
    18f0: e59f4014     	ldr	r4, [pc, #0x14]         @ 0x190c <_memAllocate+0x11c>
    18f4: e08f0004     	add	r0, pc, r4
    18f8: ebfffc66     	bl	0xa98 <.plt+0xa4>       @ imm = #-0xe68
    18fc: e28dd05c     	add	sp, sp, #92
    1900: e8bd8030     	pop	{r4, r5, pc}
    1904: 90 13 00 00  	.word	0x00001390
    1908: 74 13 00 00  	.word	0x00001374
    190c: 40 13 00 00  	.word	0x00001340

