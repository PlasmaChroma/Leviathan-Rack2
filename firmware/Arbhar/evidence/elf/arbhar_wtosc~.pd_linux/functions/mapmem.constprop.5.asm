00004118 <mapmem.constprop.5>:
    4118: e92d4030     	push	{r4, r5, lr}
    411c: e24dd00c     	sub	sp, sp, #12
    4120: e3a03001     	mov	r3, #1
    4124: e88d0006     	stm	sp, {r1, r2}
    4128: e1a01000     	mov	r1, r0
    412c: e3a02003     	mov	r2, #3
    4130: e3a00000     	mov	r0, #0
    4134: ebfff8e1     	bl	0x24c0 <.plt+0x1dc>     @ imm = #-0x1c7c
    4138: e3700001     	cmn	r0, #1
    413c: e1a04000     	mov	r4, r0
    4140: 0a000002     	beq	0x4150 <mapmem.constprop.5+0x38> @ imm = #0x8
    4144: e1a00004     	mov	r0, r4
    4148: e28dd00c     	add	sp, sp, #12
    414c: e8bd8030     	pop	{r4, r5, pc}
    4150: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x4184 <mapmem.constprop.5+0x6c>
    4154: e5935000     	ldr	r5, [r3]
    4158: ebfff8e4     	bl	0x24f0 <.plt+0x20c>     @ imm = #-0x1c70
    415c: e5900000     	ldr	r0, [r0]
    4160: ebfff8b8     	bl	0x2448 <.plt+0x164>     @ imm = #-0x1d20
    4164: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x4188 <mapmem.constprop.5+0x70>
    4168: e59f101c     	ldr	r1, [pc, #0x1c]         @ 0x418c <mapmem.constprop.5+0x74>
    416c: e1a03000     	mov	r3, r0
    4170: e1a00005     	mov	r0, r5
    4174: ebfff8d4     	bl	0x24cc <.plt+0x1e8>     @ imm = #-0x1cb0
    4178: e1a00004     	mov	r0, r4
    417c: e28dd00c     	add	sp, sp, #12
    4180: e8bd8030     	pop	{r4, r5, pc}
    4184: 00 00 00 00  	.word	0x00000000
    4188: cc 87 00 00  	.word	0x000087cc
    418c: d4 87 00 00  	.word	0x000087d4

