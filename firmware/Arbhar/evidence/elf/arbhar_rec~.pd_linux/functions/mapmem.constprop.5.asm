00005060 <mapmem.constprop.5>:
    5060: e92d4030     	push	{r4, r5, lr}
    5064: e24dd00c     	sub	sp, sp, #12
    5068: e3a03001     	mov	r3, #1
    506c: e88d0006     	stm	sp, {r1, r2}
    5070: e1a01000     	mov	r1, r0
    5074: e3a02003     	mov	r2, #3
    5078: e3a00000     	mov	r0, #0
    507c: ebfff599     	bl	0x26e8 <.plt+0x1e8>     @ imm = #-0x299c
    5080: e3700001     	cmn	r0, #1
    5084: e1a04000     	mov	r4, r0
    5088: 0a000002     	beq	0x5098 <mapmem.constprop.5+0x38> @ imm = #0x8
    508c: e1a00004     	mov	r0, r4
    5090: e28dd00c     	add	sp, sp, #12
    5094: e8bd8030     	pop	{r4, r5, pc}
    5098: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x50cc <mapmem.constprop.5+0x6c>
    509c: e5935000     	ldr	r5, [r3]
    50a0: ebfff59f     	bl	0x2724 <.plt+0x224>     @ imm = #-0x2984
    50a4: e5900000     	ldr	r0, [r0]
    50a8: ebfff576     	bl	0x2688 <.plt+0x188>     @ imm = #-0x2a28
    50ac: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x50d0 <mapmem.constprop.5+0x70>
    50b0: e59f101c     	ldr	r1, [pc, #0x1c]         @ 0x50d4 <mapmem.constprop.5+0x74>
    50b4: e1a03000     	mov	r3, r0
    50b8: e1a00005     	mov	r0, r5
    50bc: ebfff58f     	bl	0x2700 <.plt+0x200>     @ imm = #-0x29c4
    50c0: e1a00004     	mov	r0, r4
    50c4: e28dd00c     	add	sp, sp, #12
    50c8: e8bd8030     	pop	{r4, r5, pc}
    50cc: 00 00 00 00  	.word	0x00000000
    50d0: 04 98 00 00  	.word	0x00009804
    50d4: 0c 98 00 00  	.word	0x0000980c

