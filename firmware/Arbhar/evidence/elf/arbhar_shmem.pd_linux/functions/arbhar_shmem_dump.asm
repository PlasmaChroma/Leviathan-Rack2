00002608 <arbhar_shmem_dump>:
    2608: e3520001     	cmp	r2, #1
    260c: da000026     	ble	0x26ac <arbhar_shmem_dump+0xa4> @ imm = #0x98
    2610: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    2614: e24dd00c     	sub	sp, sp, #12
    2618: e5936000     	ldr	r6, [r3]
    261c: e3560001     	cmp	r6, #1
    2620: 1a00001c     	bne	0x2698 <arbhar_shmem_dump+0x90> @ imm = #0x70
    2624: e1a05002     	mov	r5, r2
    2628: e1a08000     	mov	r8, r0
    262c: e1a02003     	mov	r2, r3
    2630: e1a01005     	mov	r1, r5
    2634: e3a00000     	mov	r0, #0
    2638: e1a04003     	mov	r4, r3
    263c: ebfff954     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1ab0
    2640: e59f0194     	ldr	r0, [pc, #0x194]        @ 0x27dc <arbhar_shmem_dump+0x1d4>
    2644: e08f0000     	add	r0, pc, r0
    2648: eefd7ac0     	vcvt.s32.f32	s15, s0
    264c: ee179a90     	vmov	r9, s15
    2650: ee171a90     	vmov	r1, s15
    2654: ebfff92d     	bl	0xb10 <.plt+0x11c>      @ imm = #-0x1b4c
    2658: e088a109     	add	r10, r8, r9, lsl #2
    265c: e59a3058     	ldr	r3, [r10, #0x58]
    2660: e3530000     	cmp	r3, #0
    2664: 0a00002a     	beq	0x2714 <arbhar_shmem_dump+0x10c> @ imm = #0xa8
    2668: e5942008     	ldr	r2, [r4, #0x8]
    266c: e3520002     	cmp	r2, #2
    2670: 0a000010     	beq	0x26b8 <arbhar_shmem_dump+0xb0> @ imm = #0x40
    2674: e2421001     	sub	r1, r2, #1
    2678: e3550002     	cmp	r5, #2
    267c: e16f7f11     	clz	r7, r1
    2680: e1a0b2a7     	lsr	r11, r7, #5
    2684: 03a0b000     	moveq	r11, #0
    2688: e35b0000     	cmp	r11, #0
    268c: 1a000023     	bne	0x2720 <arbhar_shmem_dump+0x118> @ imm = #0x8c
    2690: e28dd00c     	add	sp, sp, #12
    2694: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2698: e59f4140     	ldr	r4, [pc, #0x140]        @ 0x27e0 <arbhar_shmem_dump+0x1d8>
    269c: e08f0004     	add	r0, pc, r4
    26a0: e28dd00c     	add	sp, sp, #12
    26a4: e8bd4ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    26a8: eafff8fa     	b	0xa98 <.plt+0xa4>       @ imm = #-0x1c18
    26ac: e59fc130     	ldr	r12, [pc, #0x130]       @ 0x27e4 <arbhar_shmem_dump+0x1dc>
    26b0: e08f100c     	add	r1, pc, r12
    26b4: eafff939     	b	0xba0 <.plt+0x1ac>      @ imm = #-0x1b1c
    26b8: e3a07000     	mov	r7, #0
    26bc: e284b008     	add	r11, r4, #8
    26c0: e1a06007     	mov	r6, r7
    26c4: e2866001     	add	r6, r6, #1
    26c8: e1a02004     	mov	r2, r4
    26cc: e1a01005     	mov	r1, r5
    26d0: e1a00006     	mov	r0, r6
    26d4: ebfff922     	bl	0xb64 <.plt+0x170>      @ imm = #-0x1b78
    26d8: e59a208c     	ldr	r2, [r10, #0x8c]
    26dc: e3a03000     	mov	r3, #0
    26e0: e1a01009     	mov	r1, r9
    26e4: e58d3000     	str	r3, [sp]
    26e8: e1a03007     	mov	r3, r7
    26ec: e58d2004     	str	r2, [sp, #0x4]
    26f0: e1a02000     	mov	r2, r0
    26f4: e1a00008     	mov	r0, r8
    26f8: ebfff8e9     	bl	0xaa4 <.plt+0xb0>       @ imm = #-0x1c5c
    26fc: e79b1186     	ldr	r1, [r11, r6, lsl #3]
    2700: e3510002     	cmp	r1, #2
    2704: e0877000     	add	r7, r7, r0
    2708: 0affffed     	beq	0x26c4 <arbhar_shmem_dump+0xbc> @ imm = #-0x4c
    270c: e28dd00c     	add	sp, sp, #12
    2710: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2714: e59f80cc     	ldr	r8, [pc, #0xcc]         @ 0x27e8 <arbhar_shmem_dump+0x1e0>
    2718: e08f0008     	add	r0, pc, r8
    271c: eaffffdf     	b	0x26a0 <arbhar_shmem_dump+0x98> @ imm = #-0x84
    2720: e1a00006     	mov	r0, r6
    2724: e1a02004     	mov	r2, r4
    2728: e1a01005     	mov	r1, r5
    272c: ebfff918     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1ba0
    2730: e1a02004     	mov	r2, r4
    2734: e1a01005     	mov	r1, r5
    2738: e3a00002     	mov	r0, #2
    273c: eebd0ac0     	vcvt.s32.f32	s0, s0
    2740: ee10ca10     	vmov	r12, s0
    2744: e1cc6fcc     	bic	r6, r12, r12, asr #31
    2748: ebfff905     	bl	0xb64 <.plt+0x170>      @ imm = #-0x1bec
    274c: e3550003     	cmp	r5, #3
    2750: e59a708c     	ldr	r7, [r10, #0x8c]
    2754: 03a0a000     	moveq	r10, #0
    2758: e1a0b000     	mov	r11, r0
    275c: 0a000008     	beq	0x2784 <arbhar_shmem_dump+0x17c> @ imm = #0x20
    2760: e594e018     	ldr	lr, [r4, #0x18]
    2764: e35e0001     	cmp	lr, #1
    2768: 13a0a000     	movne	r10, #0
    276c: 0a000013     	beq	0x27c0 <arbhar_shmem_dump+0x1b8> @ imm = #0x4c
    2770: e3550004     	cmp	r5, #4
    2774: 0a000002     	beq	0x2784 <arbhar_shmem_dump+0x17c> @ imm = #0x8
    2778: e5940020     	ldr	r0, [r4, #0x20]
    277c: e3500001     	cmp	r0, #1
    2780: 0a000007     	beq	0x27a4 <arbhar_shmem_dump+0x19c> @ imm = #0x1c
    2784: e58d7004     	str	r7, [sp, #0x4]
    2788: e1a03006     	mov	r3, r6
    278c: e58da000     	str	r10, [sp]
    2790: e1a0200b     	mov	r2, r11
    2794: e1a01009     	mov	r1, r9
    2798: e1a00008     	mov	r0, r8
    279c: ebfff8c0     	bl	0xaa4 <.plt+0xb0>       @ imm = #-0x1d00
    27a0: eaffffba     	b	0x2690 <arbhar_shmem_dump+0x88> @ imm = #-0x118
    27a4: e1a02004     	mov	r2, r4
    27a8: e1a01005     	mov	r1, r5
    27ac: e3a00004     	mov	r0, #4
    27b0: ebfff8f7     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1c24
    27b4: eebd1ac0     	vcvt.s32.f32	s2, s0
    27b8: ee117a10     	vmov	r7, s2
    27bc: eafffff0     	b	0x2784 <arbhar_shmem_dump+0x17c> @ imm = #-0x40
    27c0: e1a02004     	mov	r2, r4
    27c4: e1a01005     	mov	r1, r5
    27c8: e3a00003     	mov	r0, #3
    27cc: ebfff8f0     	bl	0xb94 <.plt+0x1a0>      @ imm = #-0x1c40
    27d0: eefd0ac0     	vcvt.s32.f32	s1, s0
    27d4: ee10aa90     	vmov	r10, s1
    27d8: eaffffe4     	b	0x2770 <arbhar_shmem_dump+0x168> @ imm = #-0x70
    27dc: cc 07 00 00  	.word	0x000007cc
    27e0: f0 06 00 00  	.word	0x000006f0
    27e4: 64 06 00 00  	.word	0x00000664
    27e8: 08 07 00 00  	.word	0x00000708

