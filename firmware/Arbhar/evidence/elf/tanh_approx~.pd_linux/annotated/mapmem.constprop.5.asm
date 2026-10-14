00002e8c <mapmem.constprop.5>:
    2e8c: e92d4030     	push	{r4, r5, lr}
    2e90: e24dd00c     	sub	sp, sp, #12
    2e94: e3a03001     	mov	r3, #1
    2e98: e88d0006     	stm	sp, {r1, r2}
    2e9c: e1a01000     	mov	r1, r0
    2ea0: e3a02003     	mov	r2, #3
    2ea4: e3a00000     	mov	r0, #0
    2ea8: ebfffc6a     	bl	0x2058 <.plt+0x110>     @ imm = #-0xe58  // CALL mmap
    2eac: e3700001     	cmn	r0, #1
    2eb0: e1a04000     	mov	r4, r0
    2eb4: 0a000002     	beq	0x2ec4 <mapmem.constprop.5+0x38> @ imm = #0x8
    2eb8: e1a00004     	mov	r0, r4
    2ebc: e28dd00c     	add	sp, sp, #12
    2ec0: e8bd8030     	pop	{r4, r5, pc}
    2ec4: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x2ef8 <mapmem.constprop.5+0x6c>  // u32=0x0; f32?=0
    2ec8: e5935000     	ldr	r5, [r3]
    2ecc: ebfffc6d     	bl	0x2088 <.plt+0x140>     @ imm = #-0xe4c  // CALL __errno_location
    2ed0: e5900000     	ldr	r0, [r0]
    2ed4: ebfffc53     	bl	0x2028 <.plt+0xe0>      @ imm = #-0xeb4  // CALL strerror
    2ed8: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x2efc <mapmem.constprop.5+0x70>  // u32=0x73a0; f32?=4.14784345e-41
    2edc: e59f101c     	ldr	r1, [pc, #0x1c]         @ 0x2f00 <mapmem.constprop.5+0x74>  // u32=0x73a8; f32?=4.14896449e-41
    2ee0: e1a03000     	mov	r3, r0
    2ee4: e1a00005     	mov	r0, r5
    2ee8: ebfffc5d     	bl	0x2064 <.plt+0x11c>     @ imm = #-0xe8c  // CALL fprintf
    2eec: e1a00004     	mov	r0, r4
    2ef0: e28dd00c     	add	sp, sp, #12
    2ef4: e8bd8030     	pop	{r4, r5, pc}
    2ef8: 00 00 00 00  	.word	0x00000000
    2efc: a0 73 00 00  	.word	0x000073a0
    2f00: a8 73 00 00  	.word	0x000073a8

