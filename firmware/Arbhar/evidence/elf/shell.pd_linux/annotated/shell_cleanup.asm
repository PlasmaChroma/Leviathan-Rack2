000013bc <shell_cleanup>:
    13bc: e92d4010     	push	{r4, lr}
    13c0: e1a04000     	mov	r4, r0
    13c4: e5900030     	ldr	r0, [r0, #0x30]
    13c8: ebfffd9f     	bl	0xa4c <.plt+0xe0>       @ imm = #-0x984  // CALL sys_rmpollfn
    13cc: e5940030     	ldr	r0, [r4, #0x30]
    13d0: e3500000     	cmp	r0, #0
    13d4: da000000     	ble	0x13dc <shell_cleanup+0x20> @ imm = #0x0
    13d8: ebfffdda     	bl	0xb48 <.plt+0x1dc>      @ imm = #-0x898  // CALL close
    13dc: e5940034     	ldr	r0, [r4, #0x34]
    13e0: e3500000     	cmp	r0, #0
    13e4: da000000     	ble	0x13ec <shell_cleanup+0x30> @ imm = #0x0
    13e8: ebfffdd6     	bl	0xb48 <.plt+0x1dc>      @ imm = #-0x8a8  // CALL close
    13ec: e5940038     	ldr	r0, [r4, #0x38]
    13f0: e3500000     	cmp	r0, #0
    13f4: da000000     	ble	0x13fc <shell_cleanup+0x40> @ imm = #0x0
    13f8: ebfffdd2     	bl	0xb48 <.plt+0x1dc>      @ imm = #-0x8b8  // CALL close
    13fc: e594003c     	ldr	r0, [r4, #0x3c]
    1400: e3500000     	cmp	r0, #0
    1404: da000000     	ble	0x140c <shell_cleanup+0x50> @ imm = #0x0
    1408: ebfffdce     	bl	0xb48 <.plt+0x1dc>      @ imm = #-0x8c8  // CALL close
    140c: e3e03000     	mvn	r3, #0
    1410: e594004c     	ldr	r0, [r4, #0x4c]
    1414: e5843030     	str	r3, [r4, #0x30]
    1418: e5843034     	str	r3, [r4, #0x34]
    141c: e5843038     	str	r3, [r4, #0x38]
    1420: e584303c     	str	r3, [r4, #0x3c]
    1424: e8bd4010     	pop	{r4, lr}
    1428: eafffd99     	b	0xa94 <.plt+0x128>      @ imm = #-0x99c  // CALL clock_unset

