0000a5e8 <malloc_aligned>:
    a5e8: e92d4030     	push	{r4, r5, lr}
    a5ec: e24dd00c     	sub	sp, sp, #12
    a5f0: e1a05000     	mov	r5, r0
    a5f4: ebffdffc     	bl	0x25ec <.plt+0x20c>     @ imm = #-0x8010
    a5f8: e1a02005     	mov	r2, r5
    a5fc: e3a01a01     	mov	r1, #4096
    a600: e1a04000     	mov	r4, r0
    a604: e28d0004     	add	r0, sp, #4
    a608: ebffdff4     	bl	0x25e0 <.plt+0x200>     @ imm = #-0x8030
    a60c: e2503000     	subs	r3, r0, #0
    a610: 13a00000     	movne	r0, #0
    a614: 059d0004     	ldreq	r0, [sp, #0x4]
    a618: e5843000     	str	r3, [r4]
    a61c: e28dd00c     	add	sp, sp, #12
    a620: e8bd8030     	pop	{r4, r5, pc}

