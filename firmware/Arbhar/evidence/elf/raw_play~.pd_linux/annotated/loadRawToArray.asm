000026d4 <loadRawToArray>:
    26d4: e92d4070     	push	{r4, r5, r6, lr}
    26d8: e24dd068     	sub	sp, sp, #104
    26dc: e28d4004     	add	r4, sp, #4
    26e0: e3a02064     	mov	r2, #100
    26e4: e1a06000     	mov	r6, r0
    26e8: e1a00003     	mov	r0, r3
    26ec: e1a01004     	mov	r1, r4
    26f0: ebfffea9     	bl	0x219c <.plt+0x20c>     @ imm = #-0x55c  // CALL atom_string
    26f4: e59f10c0     	ldr	r1, [pc, #0xc0]         @ 0x27bc <loadRawToArray+0xe8>  // u32=0x4e3c; f32?=2.80652056e-41
    26f8: e1a00004     	mov	r0, r4
    26fc: e08f1001     	add	r1, pc, r1
    2700: ebfffe39     	bl	0x1fec <.plt+0x5c>      @ imm = #-0x71c  // CALL fopen
    2704: e2504000     	subs	r4, r0, #0
    2708: 0a000026     	beq	0x27a8 <loadRawToArray+0xd4> @ imm = #0x98
    270c: e59f00ac     	ldr	r0, [pc, #0xac]         @ 0x27c0 <loadRawToArray+0xec>  // u32=0x4e3c; f32?=2.80652056e-41
    2710: e08f0000     	add	r0, pc, r0
    2714: ebfffe79     	bl	0x2100 <.plt+0x170>     @ imm = #-0x61c  // CALL post
    2718: e3a02002     	mov	r2, #2
    271c: e3a01000     	mov	r1, #0
    2720: e1a00004     	mov	r0, r4
    2724: ebfffe93     	bl	0x2178 <.plt+0x1e8>     @ imm = #-0x5b4  // CALL fseek
    2728: e1a00004     	mov	r0, r4
    272c: ebfffe43     	bl	0x2040 <.plt+0xb0>      @ imm = #-0x6f4  // CALL ftell
    2730: e59f208c     	ldr	r2, [pc, #0x8c]         @ 0x27c4 <loadRawToArray+0xf0>  // u32=0x4e1c; f32?=2.80203641e-41
    2734: e1a01000     	mov	r1, r0
    2738: e1a05000     	mov	r5, r0
    273c: e08f0002     	add	r0, pc, r2
    2740: ebfffe6e     	bl	0x2100 <.plt+0x170>     @ imm = #-0x648  // CALL post
    2744: e3a02000     	mov	r2, #0
    2748: e1a01002     	mov	r1, r2
    274c: e1a00004     	mov	r0, r4
    2750: ebfffe88     	bl	0x2178 <.plt+0x1e8>     @ imm = #-0x5e0  // CALL fseek
    2754: e3500000     	cmp	r0, #0
    2758: 0a000003     	beq	0x276c <loadRawToArray+0x98> @ imm = #0xc
    275c: e1a00004     	mov	r0, r4
    2760: ebfffe6c     	bl	0x2118 <.plt+0x188>     @ imm = #-0x650  // CALL fclose
    2764: e28dd068     	add	sp, sp, #104
    2768: e8bd8070     	pop	{r4, r5, r6, pc}
    276c: e59f3054     	ldr	r3, [pc, #0x54]         @ 0x27c8 <loadRawToArray+0xf4>  // u32=0x4df8; f32?=2.79699173e-41
    2770: e08f0003     	add	r0, pc, r3
    2774: ebfffe61     	bl	0x2100 <.plt+0x170>     @ imm = #-0x67c  // CALL post
    2778: e3550000     	cmp	r5, #0
    277c: da000005     	ble	0x2798 <loadRawToArray+0xc4> @ imm = #0x14
    2780: e1a02005     	mov	r2, r5
    2784: e2860028     	add	r0, r6, #40
    2788: e1a03004     	mov	r3, r4
    278c: e3a01004     	mov	r1, #4
    2790: ebfffe36     	bl	0x2070 <.plt+0xe0>      @ imm = #-0x728  // CALL fread
    2794: eafffff0     	b	0x275c <loadRawToArray+0x88> @ imm = #-0x40
    2798: e59fc02c     	ldr	r12, [pc, #0x2c]        @ 0x27cc <loadRawToArray+0xf8>  // u32=0x4de4; f32?=2.79418914e-41
    279c: e08f000c     	add	r0, pc, r12
    27a0: ebfffe56     	bl	0x2100 <.plt+0x170>     @ imm = #-0x6a8  // CALL post
    27a4: eaffffec     	b	0x275c <loadRawToArray+0x88> @ imm = #-0x50
    27a8: e59fe020     	ldr	lr, [pc, #0x20]         @ 0x27d0 <loadRawToArray+0xfc>  // u32=0x4d90; f32?=2.78241823e-41
    27ac: e08f000e     	add	r0, pc, lr
    27b0: ebfffe52     	bl	0x2100 <.plt+0x170>     @ imm = #-0x6b8  // CALL post
    27b4: e28dd068     	add	sp, sp, #104
    27b8: e8bd8070     	pop	{r4, r5, r6, pc}
    27bc: 3c 4e 00 00  	.word	0x00004e3c
    27c0: 3c 4e 00 00  	.word	0x00004e3c
    27c4: 1c 4e 00 00  	.word	0x00004e1c
    27c8: f8 4d 00 00  	.word	0x00004df8
    27cc: e4 4d 00 00  	.word	0x00004de4
    27d0: 90 4d 00 00  	.word	0x00004d90

