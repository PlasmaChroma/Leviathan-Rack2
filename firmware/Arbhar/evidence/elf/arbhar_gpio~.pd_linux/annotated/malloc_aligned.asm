00012850 <malloc_aligned>:
   12850: e92d4030     	push	{r4, r5, lr}
   12854: e24dd00c     	sub	sp, sp, #12
   12858: e1a05000     	mov	r5, r0
   1285c: ebffc4ad     	bl	0x3b18 <.plt+0x41c>     @ imm = #-0xed4c  // CALL __errno_location
   12860: e1a02005     	mov	r2, r5
   12864: e3a01a01     	mov	r1, #4096
   12868: e1a04000     	mov	r4, r0
   1286c: e28d0004     	add	r0, sp, #4
   12870: ebffc4a5     	bl	0x3b0c <.plt+0x410>     @ imm = #-0xed6c  // CALL posix_memalign
   12874: e2503000     	subs	r3, r0, #0
   12878: 13a00000     	movne	r0, #0
   1287c: 059d0004     	ldreq	r0, [sp, #0x4]
   12880: e5843000     	str	r3, [r4]
   12884: e28dd00c     	add	sp, sp, #12
   12888: e8bd8030     	pop	{r4, r5, pc}

