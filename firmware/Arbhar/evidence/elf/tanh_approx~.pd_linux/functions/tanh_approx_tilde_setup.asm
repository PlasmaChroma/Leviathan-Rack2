00002d90 <tanh_approx_tilde_setup>:
    2d90: e59f0084     	ldr	r0, [pc, #0x84]         @ 0x2e1c <tanh_approx_tilde_setup+0x8c>
    2d94: e92d40f0     	push	{r4, r5, r6, r7, lr}
    2d98: e08f0000     	add	r0, pc, r0
    2d9c: e24dd014     	sub	sp, sp, #20
    2da0: e59f4078     	ldr	r4, [pc, #0x78]         @ 0x2e20 <tanh_approx_tilde_setup+0x90>
    2da4: ebfffc6c     	bl	0x1f5c <.plt+0x14>      @ imm = #-0xe50
    2da8: e59f2074     	ldr	r2, [pc, #0x74]         @ 0x2e24 <tanh_approx_tilde_setup+0x94>
    2dac: e59f1074     	ldr	r1, [pc, #0x74]         @ 0x2e28 <tanh_approx_tilde_setup+0x98>
    2db0: e08f4004     	add	r4, pc, r4
    2db4: e3a06000     	mov	r6, #0
    2db8: e3a0c006     	mov	r12, #6
    2dbc: e7942002     	ldr	r2, [r4, r2]
    2dc0: e3a03030     	mov	r3, #48
    2dc4: e7941001     	ldr	r1, [r4, r1]
    2dc8: e88d1040     	stm	sp, {r6, r12}
    2dcc: e58d6008     	str	r6, [sp, #0x8]
    2dd0: ebfffcc4     	bl	0x20e8 <.plt+0x1a0>     @ imm = #-0xcf0
    2dd4: e59f5050     	ldr	r5, [pc, #0x50]         @ 0x2e2c <tanh_approx_tilde_setup+0x9c>
    2dd8: e59f3050     	ldr	r3, [pc, #0x50]         @ 0x2e30 <tanh_approx_tilde_setup+0xa0>
    2ddc: e08f5005     	add	r5, pc, r5
    2de0: e1a07000     	mov	r7, r0
    2de4: e08f0003     	add	r0, pc, r3
    2de8: e5857000     	str	r7, [r5]
    2dec: ebfffc5a     	bl	0x1f5c <.plt+0x14>      @ imm = #-0xe98
    2df0: e59fc03c     	ldr	r12, [pc, #0x3c]        @ 0x2e34 <tanh_approx_tilde_setup+0xa4>
    2df4: e1a03006     	mov	r3, r6
    2df8: e794100c     	ldr	r1, [r4, r12]
    2dfc: e1a02000     	mov	r2, r0
    2e00: e1a00007     	mov	r0, r7
    2e04: ebfffcc3     	bl	0x2118 <.plt+0x1d0>     @ imm = #-0xcf4
    2e08: e5950000     	ldr	r0, [r5]
    2e0c: e3a01020     	mov	r1, #32
    2e10: e28dd014     	add	sp, sp, #20
    2e14: e8bd40f0     	pop	{r4, r5, r6, r7, lr}
    2e18: eafffcb5     	b	0x20f4 <.plt+0x1ac>     @ imm = #-0xd2c
    2e1c: dc 44 00 00  	.word	0x000044dc
    2e20: 48 52 01 00  	.word	0x00015248
    2e24: cc 00 00 00  	.word	0x000000cc
    2e28: c8 00 00 00  	.word	0x000000c8
    2e2c: 34 53 01 00  	.word	0x00015334
    2e30: a0 44 00 00  	.word	0x000044a0
    2e34: b4 00 00 00  	.word	0x000000b4

