00006464 <malloc_aligned>:
    6464: e92d4030     	push	{r4, r5, lr}
    6468: e24dd00c     	sub	sp, sp, #12
    646c: e1a05000     	mov	r5, r0
    6470: ebfff01e     	bl	0x24f0 <.plt+0x20c>     @ imm = #-0x3f88  // CALL __errno_location
    6474: e1a02005     	mov	r2, r5
    6478: e3a01a01     	mov	r1, #4096
    647c: e1a04000     	mov	r4, r0
    6480: e28d0004     	add	r0, sp, #4
    6484: ebfff016     	bl	0x24e4 <.plt+0x200>     @ imm = #-0x3fa8  // CALL posix_memalign
    6488: e2503000     	subs	r3, r0, #0
    648c: 13a00000     	movne	r0, #0
    6490: 059d0004     	ldreq	r0, [sp, #0x4]
    6494: e5843000     	str	r3, [r4]
    6498: e28dd00c     	add	sp, sp, #12
    649c: e8bd8030     	pop	{r4, r5, pc}

