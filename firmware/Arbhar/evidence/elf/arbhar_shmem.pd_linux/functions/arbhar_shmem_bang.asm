00000d24 <arbhar_shmem_bang>:
     d24: e92d4070     	push	{r4, r5, r6, lr}
     d28: e2804018     	add	r4, r0, #24
     d2c: e59f6028     	ldr	r6, [pc, #0x28]         @ 0xd5c <arbhar_shmem_bang+0x38>
     d30: e280504c     	add	r5, r0, #76
     d34: e08f6006     	add	r6, pc, r6
     d38: e2844004     	add	r4, r4, #4
     d3c: e1a00006     	mov	r0, r6
     d40: e594303c     	ldr	r3, [r4, #0x3c]
     d44: e5942070     	ldr	r2, [r4, #0x70]
     d48: e5941000     	ldr	r1, [r4]
     d4c: ebffff6f     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x244
     d50: e1540005     	cmp	r4, r5
     d54: 1afffff7     	bne	0xd38 <arbhar_shmem_bang+0x14> @ imm = #-0x24
     d58: e8bd8070     	pop	{r4, r5, r6, pc}
     d5c: 80 1d 00 00  	.word	0x00001d80

