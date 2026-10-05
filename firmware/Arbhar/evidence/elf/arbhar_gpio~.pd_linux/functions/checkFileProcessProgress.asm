0000cd38 <checkFileProcessProgress>:
    cd38: e92d4070     	push	{r4, r5, r6, lr}
    cd3c: e1a05000     	mov	r5, r0
    cd40: ed9f0a31     	vldr	s0, [pc, #196]          @ 0xce0c <checkFileProcessProgress+0xd4>
    cd44: ebffdaa7     	bl	0x37e8 <.plt+0xec>      @ imm = #-0x9564
    cd48: e59f20c0     	ldr	r2, [pc, #0xc0]         @ 0xce10 <checkFileProcessProgress+0xd8>
    cd4c: e08fc002     	add	r12, pc, r2
    cd50: e59c30b4     	ldr	r3, [r12, #0xb4]
    cd54: eeb50ac0     	vcmpe.f32	s0, #0
    cd58: eef1fa10     	vmrs	APSR_nzcv, fpscr
    cd5c: c3a04001     	movgt	r4, #1
    cd60: d3a04000     	movle	r4, #0
    cd64: e1530004     	cmp	r3, r4
    cd68: 08bd8070     	popeq	{r4, r5, r6, pc}
    cd6c: eeb50ac0     	vcmpe.f32	s0, #0
    cd70: e2851a01     	add	r1, r5, #4096
    cd74: e5d10d5c     	ldrb	r0, [r1, #0xd5c]
    cd78: eef1fa10     	vmrs	APSR_nzcv, fpscr
    cd7c: 93a03001     	movls	r3, #1
    cd80: 83a03000     	movhi	r3, #0
    cd84: e3500003     	cmp	r0, #3
    cd88: 93a03000     	movls	r3, #0
    cd8c: 82033001     	andhi	r3, r3, #1
    cd90: e3530000     	cmp	r3, #0
    cd94: 1a000003     	bne	0xcda8 <checkFileProcessProgress+0x70> @ imm = #0xc
    cd98: e59f1074     	ldr	r1, [pc, #0x74]         @ 0xce14 <checkFileProcessProgress+0xdc>
    cd9c: e08f3001     	add	r3, pc, r1
    cda0: e58340b4     	str	r4, [r3, #0xb4]
    cda4: e8bd8070     	pop	{r4, r5, r6, pc}
    cda8: eddc7a26     	vldr	s15, [r12, #152]
    cdac: e3a0e000     	mov	lr, #0
    cdb0: e3a02000     	mov	r2, #0
    cdb4: e5c1ed5c     	strb	lr, [r1, #0xd5c]
    cdb8: eef57a40     	vcmp.f32	s15, #0
    cdbc: eef1fa10     	vmrs	APSR_nzcv, fpscr
    cdc0: 0a00000a     	beq	0xcdf0 <checkFileProcessProgress+0xb8> @ imm = #0x28
    cdc4: e5d5003d     	ldrb	r0, [r5, #0x3d]
    cdc8: e5913ddc     	ldr	r3, [r1, #0xddc]
    cdcc: e58c2098     	str	r2, [r12, #0x98]
    cdd0: e050200e     	subs	r2, r0, lr
    cdd4: e5d5c03c     	ldrb	r12, [r5, #0x3c]
    cdd8: 13a02001     	movne	r2, #1
    cddc: e5853040     	str	r3, [r5, #0x40]
    cde0: e5853048     	str	r3, [r5, #0x48]
    cde4: e5c52045     	strb	r2, [r5, #0x45]
    cde8: e5c5c044     	strb	r12, [r5, #0x44]
    cdec: e5c5e03c     	strb	lr, [r5, #0x3c]
    cdf0: e3a0e000     	mov	lr, #0
    cdf4: e1a00005     	mov	r0, r5
    cdf8: e5c1ede1     	strb	lr, [r1, #0xde1]
    cdfc: ebffdb78     	bl	0x3be4 <.plt+0x4e8>     @ imm = #-0x9220
    ce00: e1a00005     	mov	r0, r5
    ce04: ebffda71     	bl	0x37d0 <.plt+0xd4>      @ imm = #-0x963c
    ce08: eaffffe2     	b	0xcd98 <checkFileProcessProgress+0x60> @ imm = #-0x78
    ce0c: 00 00 f6 42  	.word	0x42f60000
    ce10: 64 a6 01 00  	.word	0x0001a664
    ce14: 14 a6 01 00  	.word	0x0001a614

