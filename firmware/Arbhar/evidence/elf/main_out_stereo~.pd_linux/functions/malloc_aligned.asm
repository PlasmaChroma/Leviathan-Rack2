00005188 <malloc_aligned>:
    5188: e92d4030     	push	{r4, r5, lr}
    518c: e24dd00c     	sub	sp, sp, #12
    5190: e1a05000     	mov	r5, r0
    5194: ebfff3c2     	bl	0x20a4 <.plt+0x14c>     @ imm = #-0x30f8
    5198: e1a02005     	mov	r2, r5
    519c: e3a01a01     	mov	r1, #4096
    51a0: e1a04000     	mov	r4, r0
    51a4: e28d0004     	add	r0, sp, #4
    51a8: ebfff3ba     	bl	0x2098 <.plt+0x140>     @ imm = #-0x3118
    51ac: e2503000     	subs	r3, r0, #0
    51b0: 13a00000     	movne	r0, #0
    51b4: 059d0004     	ldreq	r0, [sp, #0x4]
    51b8: e5843000     	str	r3, [r4]
    51bc: e28dd00c     	add	sp, sp, #12
    51c0: e8bd8030     	pop	{r4, r5, pc}

