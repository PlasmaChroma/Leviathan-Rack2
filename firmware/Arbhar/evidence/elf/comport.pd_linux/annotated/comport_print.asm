00002660 <comport_print>:
    2660: e92d4ff0     	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
    2664: e1a06003     	mov	r6, r3
    2668: e59f312c     	ldr	r3, [pc, #0x12c]        @ 0x279c <comport_print+0x13c>  // u32=0x12a60; f32?=1.07036782e-40
    266c: e2427001     	sub	r7, r2, #1
    2670: e59fa128     	ldr	r10, [pc, #0x128]       @ 0x27a0 <comport_print+0x140>  // u32=0x12a40; f32?=1.0699194e-40
    2674: e08fb003     	add	r11, pc, r3
    2678: e59f2124     	ldr	r2, [pc, #0x124]        @ 0x27a4 <comport_print+0x144>  // u32=0x1f68; f32?=1.12664397e-41
    267c: e59f4124     	ldr	r4, [pc, #0x124]        @ 0x27a8 <comport_print+0x148>  // u32=0x1f84; f32?=1.1305676e-41
    2680: e59f3124     	ldr	r3, [pc, #0x124]        @ 0x27ac <comport_print+0x14c>  // u32=0x1f5c; f32?=1.12496241e-41
    2684: e24dd014     	sub	sp, sp, #20
    2688: e59f8120     	ldr	r8, [pc, #0x120]        @ 0x27b0 <comport_print+0x150>  // u32=0x1f74; f32?=1.12832552e-41
    268c: e1a05000     	mov	r5, r0
    2690: e08f1002     	add	r1, pc, r2
    2694: e08f000a     	add	r0, pc, r10
    2698: e08fc004     	add	r12, pc, r4
    269c: e08fa003     	add	r10, pc, r3
    26a0: e3770001     	cmn	r7, #1
    26a4: e3a090ff     	mov	r9, #255
    26a8: e08f8008     	add	r8, pc, r8
    26ac: e58d1004     	str	r1, [sp, #0x4]
    26b0: e58dc008     	str	r12, [sp, #0x8]
    26b4: e58da00c     	str	r10, [sp, #0xc]
    26b8: e58d0000     	str	r0, [sp]
    26bc: 0a00002b     	beq	0x2770 <comport_print+0x110> @ imm = #0xac
    26c0: e1a00006     	mov	r0, r6
    26c4: e1a02009     	mov	r2, r9
    26c8: e59d1000     	ldr	r1, [sp]
    26cc: ebfff94d     	bl	0xc08 <.plt+0x200>      @ imm = #-0x1acc  // CALL atom_string
    26d0: e1a0400b     	mov	r4, r11
    26d4: e2866008     	add	r6, r6, #8
    26d8: e285aa01     	add	r10, r5, #4096
    26dc: e5d4e000     	ldrb	lr, [r4]
    26e0: e1a0b004     	mov	r11, r4
    26e4: e2844001     	add	r4, r4, #1
    26e8: e35e0000     	cmp	lr, #0
    26ec: 0a00000d     	beq	0x2728 <comport_print+0xc8> @ imm = #0x34
    26f0: e595b020     	ldr	r11, [r5, #0x20]
    26f4: e37b0001     	cmn	r11, #1
    26f8: 0a00001e     	beq	0x2778 <comport_print+0x118> @ imm = #0x78
    26fc: e59a20fc     	ldr	r2, [r10, #0xfc]
    2700: e59a10f8     	ldr	r1, [r10, #0xf8]
    2704: e2820001     	add	r0, r2, #1
    2708: e1520001     	cmp	r2, r1
    270c: b59ac0f0     	ldrlt	r12, [r10, #0xf0]
    2710: b58a00fc     	strlt	r0, [r10, #0xfc]
    2714: b7cce002     	strblt	lr, [r12, r2]
    2718: baffffef     	blt	0x26dc <comport_print+0x7c> @ imm = #-0x44
    271c: e1a00008     	mov	r0, r8
    2720: ebfff905     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1bec  // CALL post
    2724: eaffffec     	b	0x26dc <comport_print+0x7c> @ imm = #-0x50
    2728: e3570000     	cmp	r7, #0
    272c: da00000c     	ble	0x2764 <comport_print+0x104> @ imm = #0x30
    2730: e5950020     	ldr	r0, [r5, #0x20]
    2734: e3700001     	cmn	r0, #1
    2738: 0a000014     	beq	0x2790 <comport_print+0x130> @ imm = #0x50
    273c: e2852a01     	add	r2, r5, #4096
    2740: e59210fc     	ldr	r1, [r2, #0xfc]
    2744: e592c0f8     	ldr	r12, [r2, #0xf8]
    2748: e151000c     	cmp	r1, r12
    274c: aa00000c     	bge	0x2784 <comport_print+0x124> @ imm = #0x30
    2750: e59230f0     	ldr	r3, [r2, #0xf0]
    2754: e281a001     	add	r10, r1, #1
    2758: e3a0e020     	mov	lr, #32
    275c: e582a0fc     	str	r10, [r2, #0xfc]
    2760: e7c3e001     	strb	lr, [r3, r1]
    2764: e2477001     	sub	r7, r7, #1
    2768: e3770001     	cmn	r7, #1
    276c: 1affffd3     	bne	0x26c0 <comport_print+0x60> @ imm = #-0xb4
    2770: e28dd014     	add	sp, sp, #20
    2774: e8bd8ff0     	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
    2778: e59d0004     	ldr	r0, [sp, #0x4]
    277c: ebfff8ee     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1c48  // CALL post
    2780: eaffffd5     	b	0x26dc <comport_print+0x7c> @ imm = #-0xac
    2784: e59d0008     	ldr	r0, [sp, #0x8]
    2788: ebfff8eb     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1c54  // CALL post
    278c: eafffff4     	b	0x2764 <comport_print+0x104> @ imm = #-0x30
    2790: e59d000c     	ldr	r0, [sp, #0xc]
    2794: ebfff8e8     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1c60  // CALL post
    2798: eafffff1     	b	0x2764 <comport_print+0x104> @ imm = #-0x3c
    279c: 60 2a 01 00  	.word	0x00012a60
    27a0: 40 2a 01 00  	.word	0x00012a40
    27a4: 68 1f 00 00  	.word	0x00001f68
    27a8: 84 1f 00 00  	.word	0x00001f84
    27ac: 5c 1f 00 00  	.word	0x00001f5c
    27b0: 74 1f 00 00  	.word	0x00001f74

