00005538 <malloc_aligned>:
    5538: e92d4030     	push	{r4, r5, lr}
    553c: e24dd00c     	sub	sp, sp, #12
    5540: e1a05000     	mov	r5, r0
    5544: ebfff31c     	bl	0x21bc <.plt+0x158>     @ imm = #-0x3390  // CALL __errno_location
    5548: e1a02005     	mov	r2, r5
    554c: e3a01a01     	mov	r1, #4096
    5550: e1a04000     	mov	r4, r0
    5554: e28d0004     	add	r0, sp, #4
    5558: ebfff314     	bl	0x21b0 <.plt+0x14c>     @ imm = #-0x33b0  // CALL posix_memalign
    555c: e2503000     	subs	r3, r0, #0
    5560: 13a00000     	movne	r0, #0
    5564: 059d0004     	ldreq	r0, [sp, #0x4]
    5568: e5843000     	str	r3, [r4]
    556c: e28dd00c     	add	sp, sp, #12
    5570: e8bd8030     	pop	{r4, r5, pc}

