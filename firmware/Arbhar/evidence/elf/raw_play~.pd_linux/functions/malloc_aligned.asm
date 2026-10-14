00005544 <malloc_aligned>:
    5544: e92d4030     	push	{r4, r5, lr}
    5548: e24dd00c     	sub	sp, sp, #12
    554c: e1a05000     	mov	r5, r0
    5550: ebfff2e1     	bl	0x20dc <.plt+0x14c>     @ imm = #-0x347c
    5554: e1a02005     	mov	r2, r5
    5558: e3a01a01     	mov	r1, #4096
    555c: e1a04000     	mov	r4, r0
    5560: e28d0004     	add	r0, sp, #4
    5564: ebfff2d9     	bl	0x20d0 <.plt+0x140>     @ imm = #-0x349c
    5568: e2503000     	subs	r3, r0, #0
    556c: 13a00000     	movne	r0, #0
    5570: 059d0004     	ldreq	r0, [sp, #0x4]
    5574: e5843000     	str	r3, [r4]
    5578: e28dd00c     	add	sp, sp, #12
    557c: e8bd8030     	pop	{r4, r5, pc}

