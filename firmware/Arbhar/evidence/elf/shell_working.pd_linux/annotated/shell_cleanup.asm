0000180c <shell_cleanup>:
    180c: e92d4010     	push	{r4, lr}
    1810: e1a04000     	mov	r4, r0
    1814: e5900030     	ldr	r0, [r0, #0x30]
    1818: ebfffcf3     	bl	0xbec <.plt+0xe0>       @ imm = #-0xc34  // CALL sys_rmpollfn
    181c: e5940030     	ldr	r0, [r4, #0x30]
    1820: e3500000     	cmp	r0, #0
    1824: da000000     	ble	0x182c <shell_cleanup+0x20> @ imm = #0x0
    1828: ebfffd37     	bl	0xd0c <.plt+0x200>      @ imm = #-0xb24  // CALL close
    182c: e5940034     	ldr	r0, [r4, #0x34]
    1830: e3500000     	cmp	r0, #0
    1834: da000000     	ble	0x183c <shell_cleanup+0x30> @ imm = #0x0
    1838: ebfffd33     	bl	0xd0c <.plt+0x200>      @ imm = #-0xb34  // CALL close
    183c: e5940038     	ldr	r0, [r4, #0x38]
    1840: e3500000     	cmp	r0, #0
    1844: da000000     	ble	0x184c <shell_cleanup+0x40> @ imm = #0x0
    1848: ebfffd2f     	bl	0xd0c <.plt+0x200>      @ imm = #-0xb44  // CALL close
    184c: e594003c     	ldr	r0, [r4, #0x3c]
    1850: e3500000     	cmp	r0, #0
    1854: da000000     	ble	0x185c <shell_cleanup+0x50> @ imm = #0x0
    1858: ebfffd2b     	bl	0xd0c <.plt+0x200>      @ imm = #-0xb54  // CALL close
    185c: e3e03000     	mvn	r3, #0
    1860: e594004c     	ldr	r0, [r4, #0x4c]
    1864: e5843030     	str	r3, [r4, #0x30]
    1868: e5843034     	str	r3, [r4, #0x34]
    186c: e5843038     	str	r3, [r4, #0x38]
    1870: e584303c     	str	r3, [r4, #0x3c]
    1874: e8bd4010     	pop	{r4, lr}
    1878: eafffcf3     	b	0xc4c <.plt+0x140>      @ imm = #-0xc34  // CALL clock_unset

