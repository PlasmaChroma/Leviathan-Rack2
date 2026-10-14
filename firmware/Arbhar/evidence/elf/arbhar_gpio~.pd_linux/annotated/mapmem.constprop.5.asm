00010504 <mapmem.constprop.5>:
   10504: e92d4030     	push	{r4, r5, lr}
   10508: e24dd00c     	sub	sp, sp, #12
   1050c: e3a03001     	mov	r3, #1
   10510: e88d0006     	stm	sp, {r1, r2}
   10514: e1a01000     	mov	r1, r0
   10518: e3a02003     	mov	r2, #3
   1051c: e3a00000     	mov	r0, #0
   10520: ebffcd64     	bl	0x3ab8 <.plt+0x3bc>     @ imm = #-0xca70  // CALL mmap
   10524: e3700001     	cmn	r0, #1
   10528: e1a04000     	mov	r4, r0
   1052c: 0a000002     	beq	0x1053c <mapmem.constprop.5+0x38> @ imm = #0x8
   10530: e1a00004     	mov	r0, r4
   10534: e28dd00c     	add	sp, sp, #12
   10538: e8bd8030     	pop	{r4, r5, pc}
   1053c: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x10570 <mapmem.constprop.5+0x6c>  // u32=0x0; f32?=0
   10540: e5935000     	ldr	r5, [r3]
   10544: ebffcd73     	bl	0x3b18 <.plt+0x41c>     @ imm = #-0xca34  // CALL __errno_location
   10548: e5900000     	ldr	r0, [r0]
   1054c: ebffcd35     	bl	0x3a28 <.plt+0x32c>     @ imm = #-0xcb2c  // CALL strerror
   10550: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x10574 <mapmem.constprop.5+0x70>  // u32=0x15d94; f32?=1.25405002e-40
   10554: e59f101c     	ldr	r1, [pc, #0x1c]         @ 0x10578 <mapmem.constprop.5+0x74>  // u32=0x15d9c; f32?=1.25416213e-40
   10558: e1a03000     	mov	r3, r0
   1055c: e1a00005     	mov	r0, r5
   10560: ebffcd63     	bl	0x3af4 <.plt+0x3f8>     @ imm = #-0xca74  // CALL fprintf
   10564: e1a00004     	mov	r0, r4
   10568: e28dd00c     	add	sp, sp, #12
   1056c: e8bd8030     	pop	{r4, r5, pc}
   10570: 00 00 00 00  	.word	0x00000000
   10574: 94 5d 01 00  	.word	0x00015d94
   10578: 9c 5d 01 00  	.word	0x00015d9c

