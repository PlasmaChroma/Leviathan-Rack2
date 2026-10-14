000031f8 <mapmem.constprop.5>:
    31f8: e92d4030     	push	{r4, r5, lr}
    31fc: e24dd00c     	sub	sp, sp, #12
    3200: e3a03001     	mov	r3, #1
    3204: e88d0006     	stm	sp, {r1, r2}
    3208: e1a01000     	mov	r1, r0
    320c: e3a02003     	mov	r2, #3
    3210: e3a00000     	mov	r0, #0
    3214: ebfffba4     	bl	0x20ac <.plt+0x11c>     @ imm = #-0x1170  // CALL mmap
    3218: e3700001     	cmn	r0, #1
    321c: e1a04000     	mov	r4, r0
    3220: 0a000002     	beq	0x3230 <mapmem.constprop.5+0x38> @ imm = #0x8
    3224: e1a00004     	mov	r0, r4
    3228: e28dd00c     	add	sp, sp, #12
    322c: e8bd8030     	pop	{r4, r5, pc}
    3230: e59f302c     	ldr	r3, [pc, #0x2c]         @ 0x3264 <mapmem.constprop.5+0x6c>  // u32=0x0; f32?=0
    3234: e5935000     	ldr	r5, [r3]
    3238: ebfffba7     	bl	0x20dc <.plt+0x14c>     @ imm = #-0x1164  // CALL __errno_location
    323c: e5900000     	ldr	r0, [r0]
    3240: ebfffb8d     	bl	0x207c <.plt+0xec>      @ imm = #-0x11cc  // CALL strerror
    3244: e59f201c     	ldr	r2, [pc, #0x1c]         @ 0x3268 <mapmem.constprop.5+0x70>  // u32=0x77a0; f32?=4.29133642e-41
    3248: e59f101c     	ldr	r1, [pc, #0x1c]         @ 0x326c <mapmem.constprop.5+0x74>  // u32=0x77a8; f32?=4.29245746e-41
    324c: e1a03000     	mov	r3, r0
    3250: e1a00005     	mov	r0, r5
    3254: ebfffb97     	bl	0x20b8 <.plt+0x128>     @ imm = #-0x11a4  // CALL fprintf
    3258: e1a00004     	mov	r0, r4
    325c: e28dd00c     	add	sp, sp, #12
    3260: e8bd8030     	pop	{r4, r5, pc}
    3264: 00 00 00 00  	.word	0x00000000
    3268: a0 77 00 00  	.word	0x000077a0
    326c: a8 77 00 00  	.word	0x000077a8

