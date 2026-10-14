00002458 <stereo_in_tilde_new>:
    2458: e59f30a8     	ldr	r3, [pc, #0xa8]         @ 0x2508 <stereo_in_tilde_new+0xb0>
    245c: e92d4070     	push	{r4, r5, r6, lr}
    2460: e08f0003     	add	r0, pc, r3
    2464: e59f50a0     	ldr	r5, [pc, #0xa0]         @ 0x250c <stereo_in_tilde_new+0xb4>
    2468: e5900000     	ldr	r0, [r0]
    246c: ebffff1f     	bl	0x20f0 <.plt+0x8c>      @ imm = #-0x384
    2470: e59f2098     	ldr	r2, [pc, #0x98]         @ 0x2510 <stereo_in_tilde_new+0xb8>
    2474: e08f5005     	add	r5, pc, r5
    2478: e7955002     	ldr	r5, [r5, r2]
    247c: e1a03005     	mov	r3, r5
    2480: e1a02005     	mov	r2, r5
    2484: e1a01000     	mov	r1, r0
    2488: e1a04000     	mov	r4, r0
    248c: ebffff20     	bl	0x2114 <.plt+0xb0>      @ imm = #-0x380
    2490: e1a01005     	mov	r1, r5
    2494: e5840024     	str	r0, [r4, #0x24]
    2498: e1a00004     	mov	r0, r4
    249c: ebffff52     	bl	0x21ec <.plt+0x188>     @ imm = #-0x2b8
    24a0: e1a01005     	mov	r1, r5
    24a4: e3005a3d     	movw	r5, #0xa3d
    24a8: e3435ed7     	movt	r5, #0x3ed7
    24ac: e5840028     	str	r0, [r4, #0x28]
    24b0: e1a00004     	mov	r0, r4
    24b4: ebffff4c     	bl	0x21ec <.plt+0x188>     @ imm = #-0x2d0
    24b8: e3a035fe     	mov	r3, #1065353216
    24bc: e3a01000     	mov	r1, #0
    24c0: e30ae3d7     	movw	lr, #0xa3d7
    24c4: e5841020     	str	r1, [r4, #0x20]
    24c8: e34beef0     	movt	lr, #0xbef0
    24cc: e30cc28f     	movw	r12, #0xc28f
    24d0: e30d170a     	movw	r1, #0xd70a
    24d4: e34bccf5     	movt	r12, #0xbcf5
    24d8: e34b1d23     	movt	r1, #0xbd23
    24dc: e309299a     	movw	r2, #0x999a
    24e0: e5845040     	str	r5, [r4, #0x40]
    24e4: e3432e99     	movt	r2, #0x3e99
    24e8: e584e048     	str	lr, [r4, #0x48]
    24ec: e584c04c     	str	r12, [r4, #0x4c]
    24f0: e5841050     	str	r1, [r4, #0x50]
    24f4: e5842054     	str	r2, [r4, #0x54]
    24f8: e5843044     	str	r3, [r4, #0x44]
    24fc: e584002c     	str	r0, [r4, #0x2c]
    2500: e1a00004     	mov	r0, r4
    2504: e8bd8070     	pop	{r4, r5, r6, pc}
    2508: c4 5c 01 00  	.word	0x00015cc4
    250c: 84 5b 01 00  	.word	0x00015b84
    2510: d8 00 00 00  	.word	0x000000d8

