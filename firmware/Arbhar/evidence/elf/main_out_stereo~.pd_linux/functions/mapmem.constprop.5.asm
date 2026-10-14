00002e3c <mapmem.constprop.5>:
    2e3c: e92d4030     	push	{r4, r5, lr}
    2e40: e24dd00c     	sub	sp, sp, #12
    2e44: e3a03001     	mov	r3, #1
    2e48: e88d0006     	stm	sp, {r1, r2}
    2e4c: e1a01000     	mov	r1, r0
    2e50: e3a02003     	mov	r2, #3
    2e54: e3a00000     	mov	r0, #0
    2e58: ebfffc85     	bl	0x2074 <.plt+0x11c>     @ imm = #-0xdec
    2e5c: e3700001     	cmn	r0, #1
    2e60: e1a04000     	mov	r4, r0
    2e64: 0a000002     	beq	0x2e74 <mapmem.constprop.5+0x38> @ imm = #0x8
    2e68: e1a00004     	mov	r0, r4
    2e6c: e28dd00c     	add	sp, sp, #12
    2e70: e8bd8030     	pop	{r4, r5, pc}
    2e74: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x2ea8 <mapmem.constprop.5+0x6c>
    2e78: e5935000     	ldr	r5, [r3]
    2e7c: ebfffc88     	bl	0x20a4 <.plt+0x14c>     @ imm = #-0xde0
    2e80: e5900000     	ldr	r0, [r0]
    2e84: ebfffc6e     	bl	0x2044 <.plt+0xec>      @ imm = #-0xe48
    2e88: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x2eac <mapmem.constprop.5+0x70>
    2e8c: e59f101c     	ldr	r1, [pc, #0x1c]         @ 0x2eb0 <mapmem.constprop.5+0x74>
    2e90: e1a03000     	mov	r3, r0
    2e94: e1a00005     	mov	r0, r5
    2e98: ebfffc78     	bl	0x2080 <.plt+0x128>     @ imm = #-0xe20
    2e9c: e1a00004     	mov	r0, r4
    2ea0: e28dd00c     	add	sp, sp, #12
    2ea4: e8bd8030     	pop	{r4, r5, pc}
    2ea8: 00 00 00 00  	.word	0x00000000
    2eac: 54 73 00 00  	.word	0x00007354
    2eb0: 5c 73 00 00  	.word	0x0000735c

