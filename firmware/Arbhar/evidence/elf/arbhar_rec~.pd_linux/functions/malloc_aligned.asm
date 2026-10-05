000073ac <malloc_aligned>:
    73ac: e92d4030     	push	{r4, r5, lr}
    73b0: e24dd00c     	sub	sp, sp, #12
    73b4: e1a05000     	mov	r5, r0
    73b8: ebffecd9     	bl	0x2724 <.plt+0x224>     @ imm = #-0x4c9c
    73bc: e1a02005     	mov	r2, r5
    73c0: e3a01a01     	mov	r1, #4096
    73c4: e1a04000     	mov	r4, r0
    73c8: e28d0004     	add	r0, sp, #4
    73cc: ebffecd1     	bl	0x2718 <.plt+0x218>     @ imm = #-0x4cbc
    73d0: e2503000     	subs	r3, r0, #0
    73d4: 13a00000     	movne	r0, #0
    73d8: 059d0004     	ldreq	r0, [sp, #0x4]
    73dc: e5843000     	str	r3, [r4]
    73e0: e28dd00c     	add	sp, sp, #12
    73e4: e8bd8030     	pop	{r4, r5, pc}

