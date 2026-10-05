000033e8 <_memAllocate>:
    33e8: eefd7ac0     	vcvt.s32.f32	s15, s0
    33ec: e92d4030     	push	{r4, r5, lr}
    33f0: e24dd05c     	sub	sp, sp, #92
    33f4: ee175a90     	vmov	r5, s15
    33f8: e3550000     	cmp	r5, #0
    33fc: da000039     	ble	0x34e8 <_memAllocate+0x100> @ imm = #0xe4
    3400: eefd0ae0     	vcvt.s32.f32	s1, s1
    3404: e0804101     	add	r4, r0, r1, lsl #2
    3408: e59400c8     	ldr	r0, [r4, #0xc8]
    340c: e3500000     	cmp	r0, #0
    3410: edc40a3f     	vstr	s1, [r4, #252]
    3414: 0a000002     	beq	0x3424 <_memAllocate+0x3c> @ imm = #0x8
    3418: ebfffc43     	bl	0x252c <.plt+0x248>     @ imm = #-0xef4  // CALL shmdt
    341c: e3a03000     	mov	r3, #0
    3420: e58430c8     	str	r3, [r4, #0xc8]
    3424: e5940094     	ldr	r0, [r4, #0x94]
    3428: e3700001     	cmn	r0, #1
    342c: 1a000015     	bne	0x3488 <_memAllocate+0xa0> @ imm = #0x54
    3430: e59410fc     	ldr	r1, [r4, #0xfc]
    3434: e3002386     	movw	r2, #0x386
    3438: e1a00005     	mov	r0, r5
    343c: e1a01101     	lsl	r1, r1, #2
    3440: ebfffbd6     	bl	0x23a0 <.plt+0xbc>      @ imm = #-0x10a8  // CALL shmget
    3444: e3700001     	cmn	r0, #1
    3448: e5840094     	str	r0, [r4, #0x94]
    344c: 0a000013     	beq	0x34a0 <_memAllocate+0xb8> @ imm = #0x4c
    3450: e3a02000     	mov	r2, #0
    3454: e1a01002     	mov	r1, r2
    3458: ebfffbdf     	bl	0x23dc <.plt+0xf8>      @ imm = #-0x1084  // CALL shmat
    345c: e28d2004     	add	r2, sp, #4
    3460: e3a01002     	mov	r1, #2
    3464: e58400c8     	str	r0, [r4, #0xc8]
    3468: e5940094     	ldr	r0, [r4, #0x94]
    346c: ebfffbef     	bl	0x2430 <.plt+0x14c>     @ imm = #-0x1044  // CALL shmctl
    3470: e59420fc     	ldr	r2, [r4, #0xfc]
    3474: e59d3028     	ldr	r3, [sp, #0x28]
    3478: e1530102     	cmp	r3, r2, lsl #2
    347c: ba000010     	blt	0x34c4 <_memAllocate+0xdc> @ imm = #0x40
    3480: e28dd05c     	add	sp, sp, #92
    3484: e8bd8030     	pop	{r4, r5, pc}
    3488: e3a02000     	mov	r2, #0
    348c: e1a01002     	mov	r1, r2
    3490: ebfffbe6     	bl	0x2430 <.plt+0x14c>     @ imm = #-0x1068  // CALL shmctl
    3494: e3e00000     	mvn	r0, #0
    3498: e5840094     	str	r0, [r4, #0x94]
    349c: eaffffe3     	b	0x3430 <_memAllocate+0x48> @ imm = #-0x74
    34a0: e59fc054     	ldr	r12, [pc, #0x54]        @ 0x34fc <_memAllocate+0x114>  // u32=0x5140; f32?=2.91470081e-41
    34a4: e1a01005     	mov	r1, r5
    34a8: e59420fc     	ldr	r2, [r4, #0xfc]
    34ac: e3a05000     	mov	r5, #0
    34b0: e08f000c     	add	r0, pc, r12
    34b4: ebfffbe0     	bl	0x243c <.plt+0x158>     @ imm = #-0x1080  // CALL error
    34b8: e58450fc     	str	r5, [r4, #0xfc]
    34bc: e28dd05c     	add	sp, sp, #92
    34c0: e8bd8030     	pop	{r4, r5, pc}
    34c4: e59fe034     	ldr	lr, [pc, #0x34]         @ 0x3500 <_memAllocate+0x118>  // u32=0x5124; f32?=2.91077717e-41
    34c8: e1a01005     	mov	r1, r5
    34cc: e08f000e     	add	r0, pc, lr
    34d0: ebfffbd9     	bl	0x243c <.plt+0x158>     @ imm = #-0x109c  // CALL error
    34d4: e3a00000     	mov	r0, #0
    34d8: e58400fc     	str	r0, [r4, #0xfc]
    34dc: e58400c8     	str	r0, [r4, #0xc8]
    34e0: e28dd05c     	add	sp, sp, #92
    34e4: e8bd8030     	pop	{r4, r5, pc}
    34e8: e59f4014     	ldr	r4, [pc, #0x14]         @ 0x3504 <_memAllocate+0x11c>  // u32=0x50f0; f32?=2.90349042e-41
    34ec: e08f0004     	add	r0, pc, r4
    34f0: ebfffbd1     	bl	0x243c <.plt+0x158>     @ imm = #-0x10bc  // CALL error
    34f4: e28dd05c     	add	sp, sp, #92
    34f8: e8bd8030     	pop	{r4, r5, pc}
    34fc: 40 51 00 00  	.word	0x00005140
    3500: 24 51 00 00  	.word	0x00005124
    3504: f0 50 00 00  	.word	0x000050f0

