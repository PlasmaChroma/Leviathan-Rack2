000051d8 <malloc_aligned>:
    51d8: e92d4030     	push	{r4, r5, lr}
    51dc: e24dd00c     	sub	sp, sp, #12
    51e0: e1a05000     	mov	r5, r0
    51e4: ebfff3a7     	bl	0x2088 <.plt+0x140>     @ imm = #-0x3164
    51e8: e1a02005     	mov	r2, r5
    51ec: e3a01a01     	mov	r1, #4096
    51f0: e1a04000     	mov	r4, r0
    51f4: e28d0004     	add	r0, sp, #4
    51f8: ebfff39f     	bl	0x207c <.plt+0x134>     @ imm = #-0x3184
    51fc: e2503000     	subs	r3, r0, #0
    5200: 13a00000     	movne	r0, #0
    5204: 059d0004     	ldreq	r0, [sp, #0x4]
    5208: e5843000     	str	r3, [r4]
    520c: e28dd00c     	add	sp, sp, #12
    5210: e8bd8030     	pop	{r4, r5, pc}

