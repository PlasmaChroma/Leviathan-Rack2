00003780 <_setChordPitches>:
    3780: e3520008     	cmp	r2, #8
    3784: e92d47f0     	push	{r4, r5, r6, r7, r8, r9, r10, lr}
    3788: b1a07002     	movlt	r7, r2
    378c: a3a07008     	movge	r7, #8
    3790: e3520000     	cmp	r2, #0
    3794: e1a08000     	mov	r8, r0
    3798: da000042     	ble	0x38a8 <_setChordPitches+0x128> @ imm = #0x108
    379c: e2806d9d     	add	r6, r0, #10048
    37a0: e1a09002     	mov	r9, r2
    37a4: e2866010     	add	r6, r6, #16
    37a8: e1a04003     	mov	r4, r3
    37ac: e3a05000     	mov	r5, #0
    37b0: e1a00004     	mov	r0, r4
    37b4: e2855001     	add	r5, r5, #1
    37b8: ebfffb55     	bl	0x2514 <.plt+0x134>     @ imm = #-0x12ac
    37bc: e1570005     	cmp	r7, r5
    37c0: e1a03006     	mov	r3, r6
    37c4: e2844008     	add	r4, r4, #8
    37c8: e286600c     	add	r6, r6, #12
    37cc: ed830a00     	vstr	s0, [r3]
    37d0: cafffff6     	bgt	0x37b0 <_setChordPitches+0x30> @ imm = #-0x28
    37d4: e3590007     	cmp	r9, #7
    37d8: e3a0e001     	mov	lr, #1
    37dc: ca00002e     	bgt	0x389c <_setChordPitches+0x11c> @ imm = #0xb8
    37e0: e3a0100c     	mov	r1, #12
    37e4: e2870001     	add	r0, r7, #1
    37e8: e02c8791     	mla	r12, r1, r7, r8
    37ec: e3022750     	movw	r2, #0x2750
    37f0: e3500008     	cmp	r0, #8
    37f4: e3a03000     	mov	r3, #0
    37f8: e08cc002     	add	r12, r12, r2
    37fc: e58c3000     	str	r3, [r12]
    3800: 0a000025     	beq	0x389c <_setChordPitches+0x11c> @ imm = #0x94
    3804: e0208091     	mla	r0, r1, r0, r8
    3808: e287c002     	add	r12, r7, #2
    380c: e35c0008     	cmp	r12, #8
    3810: e0800002     	add	r0, r0, r2
    3814: e5803000     	str	r3, [r0]
    3818: 0a00001f     	beq	0x389c <_setChordPitches+0x11c> @ imm = #0x7c
    381c: e02c8c91     	mla	r12, r1, r12, r8
    3820: e2870003     	add	r0, r7, #3
    3824: e3500008     	cmp	r0, #8
    3828: e08cc002     	add	r12, r12, r2
    382c: e58c3000     	str	r3, [r12]
    3830: 0a000019     	beq	0x389c <_setChordPitches+0x11c> @ imm = #0x64
    3834: e0208091     	mla	r0, r1, r0, r8
    3838: e287c004     	add	r12, r7, #4
    383c: e35c0008     	cmp	r12, #8
    3840: e0800002     	add	r0, r0, r2
    3844: e5803000     	str	r3, [r0]
    3848: 0a000013     	beq	0x389c <_setChordPitches+0x11c> @ imm = #0x4c
    384c: e02c8c91     	mla	r12, r1, r12, r8
    3850: e2870005     	add	r0, r7, #5
    3854: e3500008     	cmp	r0, #8
    3858: e08cc002     	add	r12, r12, r2
    385c: e58c3000     	str	r3, [r12]
    3860: 0a00000d     	beq	0x389c <_setChordPitches+0x11c> @ imm = #0x34
    3864: e0208091     	mla	r0, r1, r0, r8
    3868: e287c006     	add	r12, r7, #6
    386c: e35c0008     	cmp	r12, #8
    3870: e0800002     	add	r0, r0, r2
    3874: e5803000     	str	r3, [r0]
    3878: 0a000007     	beq	0x389c <_setChordPitches+0x11c> @ imm = #0x1c
    387c: e02c8c91     	mla	r12, r1, r12, r8
    3880: e2877007     	add	r7, r7, #7
    3884: e3570008     	cmp	r7, #8
    3888: e08c0002     	add	r0, r12, r2
    388c: e5803000     	str	r3, [r0]
    3890: 10278791     	mlane	r7, r1, r7, r8
    3894: 10877002     	addne	r7, r7, r2
    3898: 15873000     	strne	r3, [r7]
    389c: e2888a02     	add	r8, r8, #8192
    38a0: e588e6e8     	str	lr, [r8, #0x6e8]
    38a4: e8bd87f0     	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
    38a8: e3a0e000     	mov	lr, #0
    38ac: eaffffcb     	b	0x37e0 <_setChordPitches+0x60> @ imm = #-0xd4

