0000282c <comport_close>:
    282c: e92d4070     	push	{r4, r5, r6, lr}
    2830: e2804a01     	add	r4, r0, #4096
    2834: e1a05000     	mov	r5, r0
    2838: e59400c4     	ldr	r0, [r4, #0xc4]
    283c: ebfff8ac     	bl	0xaf4 <.plt+0xec>       @ imm = #-0x1d50
    2840: e5956020     	ldr	r6, [r5, #0x20]
    2844: e3a03001     	mov	r3, #1
    2848: e3760001     	cmn	r6, #1
    284c: e58430c8     	str	r3, [r4, #0xc8]
    2850: 0a00000b     	beq	0x2884 <comport_close+0x58> @ imm = #0x2c
    2854: e2852060     	add	r2, r5, #96
    2858: e3a01000     	mov	r1, #0
    285c: e1a00006     	mov	r0, r6
    2860: ebfff891     	bl	0xaac <.plt+0xa4>       @ imm = #-0x1dbc
    2864: e1a00006     	mov	r0, r6
    2868: ebfff8da     	bl	0xbd8 <.plt+0x1d0>      @ imm = #-0x1c98
    286c: e595209c     	ldr	r2, [r5, #0x9c]
    2870: e59f0034     	ldr	r0, [pc, #0x34]         @ 0x28ac <comport_close+0x80>
    2874: e1d41af0     	ldrsh	r1, [r4, #160]
    2878: e08f0000     	add	r0, pc, r0
    287c: e5922000     	ldr	r2, [r2]
    2880: ebfff8ad     	bl	0xb3c <.plt+0x134>      @ imm = #-0x1d4c
    2884: e59400e8     	ldr	r0, [r4, #0xe8]
    2888: e3e01000     	mvn	r1, #0
    288c: e3500000     	cmp	r0, #0
    2890: e5851020     	str	r1, [r5, #0x20]
    2894: e1c41ab0     	strh	r1, [r4, #160]
    2898: 08bd8070     	popeq	{r4, r5, r6, pc}
    289c: ed9f0a01     	vldr	s0, [pc, #4]            @ 0x28a8 <comport_close+0x7c>
    28a0: e8bd4070     	pop	{r4, r5, r6, lr}
    28a4: eafff8b3     	b	0xb78 <.plt+0x170>      @ imm = #-0x1d34
    28a8: 00 00 80 bf  	.word	0xbf800000
    28ac: dc 1d 00 00  	.word	0x00001ddc

