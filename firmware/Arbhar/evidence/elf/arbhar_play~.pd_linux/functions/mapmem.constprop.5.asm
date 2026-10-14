0000829c <mapmem.constprop.5>:
    829c: e92d4030     	push	{r4, r5, lr}
    82a0: e24dd00c     	sub	sp, sp, #12
    82a4: e3a03001     	mov	r3, #1
    82a8: e88d0006     	stm	sp, {r1, r2}
    82ac: e1a01000     	mov	r1, r0
    82b0: e3a02003     	mov	r2, #3
    82b4: e3a00000     	mov	r0, #0
    82b8: ebffe8bc     	bl	0x25b0 <.plt+0x1d0>     @ imm = #-0x5d10
    82bc: e3700001     	cmn	r0, #1
    82c0: e1a04000     	mov	r4, r0
    82c4: 0a000002     	beq	0x82d4 <mapmem.constprop.5+0x38> @ imm = #0x8
    82c8: e1a00004     	mov	r0, r4
    82cc: e28dd00c     	add	sp, sp, #12
    82d0: e8bd8030     	pop	{r4, r5, pc}
    82d4: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x8308 <mapmem.constprop.5+0x6c>
    82d8: e5935000     	ldr	r5, [r3]
    82dc: ebffe8c2     	bl	0x25ec <.plt+0x20c>     @ imm = #-0x5cf8
    82e0: e5900000     	ldr	r0, [r0]
    82e4: ebffe899     	bl	0x2550 <.plt+0x170>     @ imm = #-0x5d9c
    82e8: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x830c <mapmem.constprop.5+0x70>
    82ec: e59f101c     	ldr	r1, [pc, #0x1c]         @ 0x8310 <mapmem.constprop.5+0x74>
    82f0: e1a03000     	mov	r3, r0
    82f4: e1a00005     	mov	r0, r5
    82f8: ebffe8b2     	bl	0x25c8 <.plt+0x1e8>     @ imm = #-0x5d38
    82fc: e1a00004     	mov	r0, r4
    8300: e28dd00c     	add	sp, sp, #12
    8304: e8bd8030     	pop	{r4, r5, pc}
    8308: 00 00 00 00  	.word	0x00000000
    830c: c4 cf 00 00  	.word	0x0000cfc4
    8310: cc cf 00 00  	.word	0x0000cfcc

