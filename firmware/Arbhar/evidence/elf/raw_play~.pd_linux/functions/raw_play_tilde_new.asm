00002644 <raw_play_tilde_new>:
    2644: e59f3060     	ldr	r3, [pc, #0x60]         @ 0x26ac <raw_play_tilde_new+0x68>
    2648: e92d4070     	push	{r4, r5, r6, lr}
    264c: e08f0003     	add	r0, pc, r3
    2650: e59f6058     	ldr	r6, [pc, #0x58]         @ 0x26b0 <raw_play_tilde_new+0x6c>
    2654: e5900000     	ldr	r0, [r0]
    2658: ebfffe6f     	bl	0x201c <.plt+0x8c>      @ imm = #-0x644
    265c: e59fc050     	ldr	r12, [pc, #0x50]        @ 0x26b4 <raw_play_tilde_new+0x70>
    2660: e08f6006     	add	r6, pc, r6
    2664: e3a02000     	mov	r2, #0
    2668: e3a015fe     	mov	r1, #1065353216
    266c: e2805923     	add	r5, r0, #573440
    2670: e580101c     	str	r1, [r0, #0x1c]
    2674: e5801020     	str	r1, [r0, #0x20]
    2678: e1a04000     	mov	r4, r0
    267c: e5852a28     	str	r2, [r5, #0xa28]
    2680: e5852a2c     	str	r2, [r5, #0xa2c]
    2684: e796600c     	ldr	r6, [r6, r12]
    2688: e1a01006     	mov	r1, r6
    268c: ebfffea4     	bl	0x2124 <.plt+0x194>     @ imm = #-0x570
    2690: e1a01006     	mov	r1, r6
    2694: e5850a30     	str	r0, [r5, #0xa30]
    2698: e1a00004     	mov	r0, r4
    269c: ebfffea0     	bl	0x2124 <.plt+0x194>     @ imm = #-0x580
    26a0: e5850a34     	str	r0, [r5, #0xa34]
    26a4: e1a00004     	mov	r0, r4
    26a8: e8bd8070     	pop	{r4, r5, r6, pc}
    26ac: d0 5a 01 00  	.word	0x00015ad0
    26b0: 98 59 01 00  	.word	0x00015998
    26b4: d4 00 00 00  	.word	0x000000d4

